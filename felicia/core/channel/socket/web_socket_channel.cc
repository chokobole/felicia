// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Modified by Wonyong Kim(chokobole33@gmail.com)
// Followings are taken and modified from
// https://github.com/chromium/chromium/blob/5db095c2653f332334d56ad739ae5fe1053308b1/net/websockets/websocket_channel.cc

#include "felicia/core/channel/socket/web_socket_channel.h"

#include "third_party/chromium/base/big_endian.h"
#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/containers/circular_deque.h"
#include "third_party/chromium/base/strings/stringprintf.h"
#include "third_party/chromium/net/websockets/websocket_errors.h"

namespace felicia {

namespace {

const size_t kWebSocketCloseCodeLength = 2;
// Timeout for waiting for the server to acknowledge a closing handshake.
const int kClosingHandshakeTimeoutSeconds = 60;
// We wait for the server to close the underlying connection as recommended in
// https://tools.ietf.org/html/rfc6455#section-7.1.1
// We don't use 2MSL since there're server implementations that don't follow
// the recommendation and wait for the client to close the underlying
// connection. It leads to unnecessarily long time before CloseEvent
// invocation. We want to avoid this rather than strictly following the spec
// recommendation.
const int kUnderlyingConnectionCloseTimeoutSeconds = 2;

using ChannelState = WebSocketChannel::ChannelState;

// Maximum close reason length = max control frame payload -
//                               status code length
//                             = 125 - 2
const size_t kMaximumCloseReasonLength = 125 - kWebSocketCloseCodeLength;

// Check a close status code for strict compliance with RFC6455. This is only
// used for close codes received from a renderer that we are intending to send
// out over the network. See ParseClose() for the restrictions on incoming close
// codes. The |code| parameter is type int for convenience of implementation;
// the real type is uint16_t. Code 1005 is treated specially; it cannot be set
// explicitly by Javascript but the renderer uses it to indicate we should send
// a Close frame with no payload.
bool IsStrictlyValidCloseStatusCode(int code) {
  static const int kInvalidRanges[] = {
      // [BAD, OK)
      0,    1000,   // 1000 is the first valid code
      1006, 1007,   // 1006 MUST NOT be set.
      1014, 3000,   // 1014 unassigned; 1015 up to 2999 are reserved.
      5000, 65536,  // Codes above 5000 are invalid.
  };
  const int* const kInvalidRangesEnd =
      kInvalidRanges + base::size(kInvalidRanges);

  DCHECK_GE(code, 0);
  DCHECK_LT(code, 65536);
  const int* upper = std::upper_bound(kInvalidRanges, kInvalidRangesEnd, code);
  DCHECK_NE(kInvalidRangesEnd, upper);
  DCHECK_GT(upper, kInvalidRanges);
  DCHECK_GT(*upper, code);
  DCHECK_LE(*(upper - 1), code);
  return ((upper - kInvalidRanges) % 2) == 0;
}

// Sets |name| to the name of the frame type for the given |opcode|. Note that
// for all of Text, Binary and Continuation opcode, this method returns
// "Data frame".
void GetFrameTypeForOpcode(net::WebSocketFrameHeader::OpCode opcode,
                           std::string* name) {
  switch (opcode) {
    case net::WebSocketFrameHeader::kOpCodeText:    // fall-thru
    case net::WebSocketFrameHeader::kOpCodeBinary:  // fall-thru
    case net::WebSocketFrameHeader::kOpCodeContinuation:
      *name = "Data frame";
      break;

    case net::WebSocketFrameHeader::kOpCodePing:
      *name = "Ping";
      break;

    case net::WebSocketFrameHeader::kOpCodePong:
      *name = "Pong";
      break;

    case net::WebSocketFrameHeader::kOpCodeClose:
      *name = "Close";
      break;

    default:
      *name = "Unknown frame type";
      break;
  }

  return;
}

}  // namespace

WebSocketChannel::WebSocketChannel(std::unique_ptr<WebSocketStream> stream)
    : stream_(std::move(stream)),
      closing_handshake_timeout_(
          base::TimeDelta::FromSeconds(kClosingHandshakeTimeoutSeconds)),
      underlying_connection_close_timeout_(base::TimeDelta::FromSeconds(
          kUnderlyingConnectionCloseTimeoutSeconds)),
      has_received_close_frame_(false),
      received_close_code_(0),
      state_(CONNECTED) {
  ignore_result(ReadFrames());
}

WebSocketChannel::~WebSocketChannel() {
  // The stream may hold a pointer to read_frames_, and so it needs to be
  // destroyed first.
  stream_.reset();
  // The timer may have a callback pointing back to us, so stop it just in case
  // someone decides to run the event loop from their destructor.
  close_timer_.Stop();
}

void WebSocketChannel::SetState(State new_state) {
  DCHECK_NE(state_, new_state);
  state_ = new_state;
}

bool WebSocketChannel::IsClosedState() const { return state_ == CLOSED; }

bool WebSocketChannel::InClosingState() const {
  // The state RECV_CLOSED is not supported here, because it is only used in one
  // code path and should not leak into the code in general.
  DCHECK_NE(RECV_CLOSED, state_)
      << "InClosingState called with state_ == RECV_CLOSED";
  return state_ == SEND_CLOSED || state_ == CLOSE_WAIT || state_ == CLOSED;
}

ChannelState WebSocketChannel::SendFrame(
    bool fin, net::WebSocketFrameHeader::OpCode op_code,
    scoped_refptr<net::IOBuffer> buffer, size_t buffer_size,
    net::CompletionOnceCallback callback) {
  DCHECK(write_callback_.is_null());
  DCHECK(op_code != net::WebSocketFrameHeader::kOpCodeText &&
         op_code != net::WebSocketFrameHeader::kOpCodeContinuation);
  DCHECK_LE(buffer_size, static_cast<size_t>(INT_MAX));
  DCHECK(stream_) << "Got SendFrame without a connection established; fin="
                  << fin << " op_code=" << op_code
                  << " buffer_size=" << buffer_size;

  if (InClosingState()) {
    DVLOG(1) << "SendFrame called in state " << state_
             << ". This may be a bug, or a harmless race.";
    std::move(callback).Run(net::ERR_CONNECTION_CLOSED);
    return CHANNEL_ALIVE;
  }

  DCHECK(net::WebSocketFrameHeader::IsKnownDataOpCode(op_code))
      << "Got SendFrame with bogus op_code " << op_code << " fin=" << fin
      << " buffer_size=" << buffer_size;
  DCHECK_EQ(state_, CONNECTED);

  write_callback_ = std::move(callback);
  return SendFrameInternal(fin, op_code, std::move(buffer), buffer_size);
}

ChannelState WebSocketChannel::StartClosingHandshake(
    uint16_t code, const std::string& reason) {
  if (InClosingState()) {
    // When the associated renderer process is killed while the channel is in
    // CLOSING state we reach here.
    DVLOG(1) << "StartClosingHandshake called in state " << state_
             << ". This may be a bug, or a harmless race.";
    return CHANNEL_ALIVE;
  }
  if (has_received_close_frame_) {
    // We reach here if the client wants to start a closing handshake while
    // the browser is waiting for the client to consume incoming data frames
    // before responding to a closing handshake initiated by the server.
    // As the client doesn't want the data frames any more, we can respond to
    // the closing handshake initiated by the server.
    return RespondToClosingHandshake();
  }
  DCHECK_EQ(state_, CONNECTED);

  DCHECK(!close_timer_.IsRunning());
  // This use of base::Unretained() is safe because we stop the timer in the
  // destructor.
  close_timer_.Start(
      FROM_HERE, closing_handshake_timeout_,
      base::Bind(&WebSocketChannel::CloseTimeout, base::Unretained(this)));

  // Javascript actually only permits 1000 and 3000-4999, but the implementation
  // itself may produce different codes. The length of |reason| is also checked
  // by Javascript.
  if (!IsStrictlyValidCloseStatusCode(code) ||
      reason.size() > kMaximumCloseReasonLength) {
    // "InternalServerError" is actually used for errors from any endpoint, per
    // errata 3227 to RFC6455. If the renderer is sending us an invalid code or
    // reason it must be malfunctioning in some way, and based on that we
    // interpret this as an internal error.
    if (SendClose(net::kWebSocketErrorInternalServerError, "") ==
        CHANNEL_DELETED)
      return CHANNEL_DELETED;
    DCHECK_EQ(CONNECTED, state_);
    SetState(SEND_CLOSED);
    return CHANNEL_ALIVE;
  }
  if (SendClose(code, reason) == CHANNEL_DELETED) return CHANNEL_DELETED;
  DCHECK_EQ(CONNECTED, state_);
  SetState(SEND_CLOSED);
  return CHANNEL_ALIVE;
}

ChannelState WebSocketChannel::WriteFrames() {
  int result = net::OK;
  do {
    // This use of base::Unretained is safe because this object owns the
    // WebSocketStream and destroying it cancels all callbacks.
    result = stream_->WriteFrames(
        data_being_sent_->frames(),
        base::Bind(base::IgnoreResult(&WebSocketChannel::OnWriteDone),
                   base::Unretained(this), false));
    if (result != net::ERR_IO_PENDING) {
      if (OnWriteDone(true, result) == CHANNEL_DELETED) return CHANNEL_DELETED;
      // OnWriteDone() returns CHANNEL_DELETED on error. Here |state_| is
      // guaranteed to be the same as before OnWriteDone() call.
    }
  } while (result == net::OK && data_being_sent_);
  return CHANNEL_ALIVE;
}

ChannelState WebSocketChannel::OnWriteDone(bool synchronous, int result) {
  DCHECK_NE(FRESHLY_CONSTRUCTED, state_);
  DCHECK_NE(CONNECTING, state_);
  DCHECK_NE(net::ERR_IO_PENDING, result);
  DCHECK(data_being_sent_);
  switch (result) {
    case net::OK:
      if (data_to_send_next_) {
        data_being_sent_ = std::move(data_to_send_next_);
        if (!synchronous) return WriteFrames();
      } else {
        data_being_sent_.reset();
      }
      if (!write_callback_.is_null()) std::move(write_callback_).Run(result);
      return CHANNEL_ALIVE;

      // If a recoverable error condition existed, it would go here.

    default:
      DCHECK_LT(result, 0)
          << "WriteFrames() should only return OK or ERR_ codes";

      stream_->Close();
      SetState(CLOSED);
      DoDropChannel(false, net::kWebSocketErrorAbnormalClosure, "");
      return CHANNEL_DELETED;
  }
}

ChannelState WebSocketChannel::ReadFrames() {
  int result = net::OK;
  while (result == net::OK) {
    // This use of base::Unretained is safe because this object owns the
    // WebSocketStream, and any pending reads will be cancelled when it is
    // destroyed.
    result = stream_->ReadFrames(
        &read_frames_,
        base::Bind(base::IgnoreResult(&WebSocketChannel::OnReadDone),
                   base::Unretained(this), false));
    if (result != net::ERR_IO_PENDING) {
      if (OnReadDone(true, result) == CHANNEL_DELETED) return CHANNEL_DELETED;
    }
    DCHECK_NE(CLOSED, state_);
  }
  return CHANNEL_ALIVE;
}

ChannelState WebSocketChannel::OnReadDone(bool synchronous, int result) {
  DCHECK_NE(FRESHLY_CONSTRUCTED, state_);
  DCHECK_NE(CONNECTING, state_);
  DCHECK_NE(net::ERR_IO_PENDING, result);
  switch (result) {
    case net::OK:
      // ReadFrames() must use ERR_CONNECTION_CLOSED for a closed connection
      // with no data read, not an empty response.
      DCHECK(!read_frames_.empty())
          << "ReadFrames() returned OK, but nothing was read.";
      for (size_t i = 0; i < read_frames_.size(); ++i) {
        if (HandleFrame(std::move(read_frames_[i])) == CHANNEL_DELETED)
          return CHANNEL_DELETED;
      }
      read_frames_.clear();
      // There should always be a call to ReadFrames pending.
      // TODO(ricea): Unless we are out of quota.
      DCHECK_NE(CLOSED, state_);
      if (!synchronous) return ReadFrames();
      return CHANNEL_ALIVE;

    case net::ERR_WS_PROTOCOL_ERROR:
      // This could be kWebSocketErrorProtocolError (specifically, non-minimal
      // encoding of payload length) or kWebSocketErrorMessageTooBig, or an
      // extension-specific error.
      FailChannel("Invalid frame header", net::kWebSocketErrorProtocolError,
                  "WebSocket Protocol Error");
      return CHANNEL_DELETED;

    default:
      DCHECK_LT(result, 0)
          << "ReadFrames() should only return OK or ERR_ codes";

      stream_->Close();
      SetState(CLOSED);

      uint16_t code = net::kWebSocketErrorAbnormalClosure;
      std::string reason = "";
      bool was_clean = false;
      if (has_received_close_frame_) {
        code = received_close_code_;
        reason = received_close_reason_;
        was_clean = (result == net::ERR_CONNECTION_CLOSED);
      }

      DoDropChannel(was_clean, code, reason);
      return CHANNEL_DELETED;
  }
}

ChannelState WebSocketChannel::HandleFrame(
    std::unique_ptr<net::WebSocketFrame> frame) {
  const net::WebSocketFrameHeader::OpCode opcode = frame->header.opcode;
  DCHECK(!net::WebSocketFrameHeader::IsKnownControlOpCode(opcode) ||
         frame->header.final);
  if (frame->header.reserved1 || frame->header.reserved2 ||
      frame->header.reserved3) {
    FailChannel(
        base::StringPrintf("One or more reserved bits are on: reserved1 = %d, "
                           "reserved2 = %d, reserved3 = %d",
                           static_cast<int>(frame->header.reserved1),
                           static_cast<int>(frame->header.reserved2),
                           static_cast<int>(frame->header.reserved3)),
        net::kWebSocketErrorProtocolError, "Invalid reserved bit");
    return CHANNEL_DELETED;
  }

  // Respond to the frame appropriately to its type.
  return HandleFrameByState(opcode, frame->header.final, std::move(frame->data),
                            frame->header.payload_length);
}

ChannelState WebSocketChannel::HandleFrameByState(
    const net::WebSocketFrameHeader::OpCode opcode, bool final,
    scoped_refptr<net::IOBuffer> data_buffer, uint64_t size) {
  DCHECK_NE(RECV_CLOSED, state_)
      << "HandleFrame() does not support being called re-entrantly from within "
         "SendClose()";
  DCHECK_NE(CLOSED, state_);
  if (state_ == CLOSE_WAIT) {
    std::string frame_name;
    GetFrameTypeForOpcode(opcode, &frame_name);

    // FailChannel() won't send another Close frame.
    FailChannel(frame_name + " received after close",
                net::kWebSocketErrorProtocolError, "");
    return CHANNEL_DELETED;
  }
  switch (opcode) {
    case net::WebSocketFrameHeader::kOpCodeText:  // fall-thru
    case net::WebSocketFrameHeader::kOpCodeBinary:
    case net::WebSocketFrameHeader::kOpCodeContinuation:
      return HandleDataFrame(opcode, final, std::move(data_buffer), size);

    case net::WebSocketFrameHeader::kOpCodePing:
      DVLOG(1) << "Got Ping of size " << size;
      if (state_ == CONNECTED)
        return SendFrameInternal(true, net::WebSocketFrameHeader::kOpCodePong,
                                 std::move(data_buffer), size);
      DVLOG(3) << "Ignored ping in state " << state_;
      return CHANNEL_ALIVE;

    case net::WebSocketFrameHeader::kOpCodePong:
      DVLOG(1) << "Got Pong of size " << size;
      // There is no need to do anything with pong messages.
      return CHANNEL_ALIVE;

    case net::WebSocketFrameHeader::kOpCodeClose: {
      uint16_t code = net::kWebSocketNormalClosure;
      std::string reason;
      std::string message;
      if (!ParseClose(std::move(data_buffer), size, &code, &reason, &message)) {
        FailChannel(message, code, reason);
        return CHANNEL_DELETED;
      }
      // TODO(ricea): Find a way to safely log the message from the close
      // message (escape control codes and so on).
      DVLOG(1) << "Got Close with code " << code;
      return HandleCloseFrame(code, reason);
    }

    default:
      FailChannel(base::StringPrintf("Unrecognized frame opcode: %d", opcode),
                  net::kWebSocketErrorProtocolError, "Unknown opcode");
      return CHANNEL_DELETED;
  }
}

ChannelState WebSocketChannel::HandleDataFrame(
    net::WebSocketFrameHeader::OpCode opcode, bool final,
    scoped_refptr<net::IOBuffer> data_buffer, uint64_t size) {
  DLOG(ERROR) << "Not implemented, why should this handle data frame?";
  return CHANNEL_ALIVE;
}

ChannelState WebSocketChannel::HandleCloseFrame(uint16_t code,
                                                const std::string& reason) {
  DVLOG(1) << "Got Close with code " << code;
  switch (state_) {
    case CONNECTED:
      has_received_close_frame_ = true;
      received_close_code_ = code;
      received_close_reason_ = reason;
      if (!pending_received_frames_.empty()) {
        // We have some data to be sent to the renderer before sending this
        // frame.
        return CHANNEL_ALIVE;
      }
      return RespondToClosingHandshake();

    case SEND_CLOSED:
      SetState(CLOSE_WAIT);
      DCHECK(close_timer_.IsRunning());
      close_timer_.Stop();
      // This use of base::Unretained() is safe because we stop the timer
      // in the destructor.
      close_timer_.Start(
          FROM_HERE, underlying_connection_close_timeout_,
          base::Bind(&WebSocketChannel::CloseTimeout, base::Unretained(this)));

      // From RFC6455 section 7.1.5: "Each endpoint
      // will see the status code sent by the other end as _The WebSocket
      // Connection Close Code_."
      has_received_close_frame_ = true;
      received_close_code_ = code;
      received_close_reason_ = reason;
      break;

    default:
      LOG(DFATAL) << "Got Close in unexpected state " << state_;
      break;
  }
  return CHANNEL_ALIVE;
}

ChannelState WebSocketChannel::RespondToClosingHandshake() {
  DCHECK(has_received_close_frame_);
  DCHECK_EQ(CONNECTED, state_);
  SetState(RECV_CLOSED);
  if (SendClose(received_close_code_, received_close_reason_) ==
      CHANNEL_DELETED)
    return CHANNEL_DELETED;
  DCHECK_EQ(RECV_CLOSED, state_);

  SetState(CLOSE_WAIT);
  DCHECK(!close_timer_.IsRunning());
  // This use of base::Unretained() is safe because we stop the timer
  // in the destructor.
  close_timer_.Start(
      FROM_HERE, underlying_connection_close_timeout_,
      base::Bind(&WebSocketChannel::CloseTimeout, base::Unretained(this)));

  // event_interface_->OnClosingHandshake();
  return CHANNEL_ALIVE;
}

ChannelState WebSocketChannel::SendFrameInternal(
    bool fin, net::WebSocketFrameHeader::OpCode op_code,
    scoped_refptr<net::IOBuffer> buffer, uint64_t size) {
  DCHECK(state_ == CONNECTED || state_ == RECV_CLOSED);
  DCHECK(stream_);

  auto frame = std::make_unique<net::WebSocketFrame>(op_code);
  net::WebSocketFrameHeader& header = frame->header;
  header.final = fin;
  header.masked = false;
  header.payload_length = size;
  frame->data = std::move(buffer);

  if (data_being_sent_) {
    // Either the link to the WebSocket server is saturated, or several messages
    // are being sent in a batch.
    // TODO(ricea): Keep some statistics to work out the situation and adjust
    // quota appropriately.
    if (!data_to_send_next_)
      data_to_send_next_ = std::make_unique<SendBuffer>();
    data_to_send_next_->AddFrame(std::move(frame));
    return CHANNEL_ALIVE;
  }

  data_being_sent_ = std::make_unique<SendBuffer>();
  data_being_sent_->AddFrame(std::move(frame));
  return WriteFrames();
}

void WebSocketChannel::FailChannel(const std::string& message, uint16_t code,
                                   const std::string& reason) {
  DCHECK_NE(FRESHLY_CONSTRUCTED, state_);
  DCHECK_NE(CONNECTING, state_);
  DCHECK_NE(CLOSED, state_);

  // TODO(ricea): Logging.
  if (state_ == CONNECTED) {
    if (SendClose(code, reason) == CHANNEL_DELETED) return;
  }

  // Careful study of RFC6455 section 7.1.7 and 7.1.1 indicates the browser
  // should close the connection itself without waiting for the closing
  // handshake.
  stream_->Close();
  SetState(CLOSED);
  // event_interface_->OnFailChannel(message);
  if (!write_callback_.is_null())
    std::move(write_callback_).Run(net::ERR_CONNECTION_RESET);
}

ChannelState WebSocketChannel::SendClose(uint16_t code,
                                         const std::string& reason) {
  DCHECK(state_ == CONNECTED || state_ == RECV_CLOSED);
  DCHECK_LE(reason.size(), kMaximumCloseReasonLength);
  scoped_refptr<net::IOBuffer> body;
  uint64_t size = 0;
  if (code == net::kWebSocketErrorNoStatusReceived) {
    // Special case: translate kWebSocketErrorNoStatusReceived into a Close
    // frame with no payload.
    DCHECK(reason.empty());
    body = base::MakeRefCounted<net::IOBuffer>(0);
  } else {
    const size_t payload_length = kWebSocketCloseCodeLength + reason.length();
    body = base::MakeRefCounted<net::IOBuffer>(payload_length);
    size = payload_length;
    base::WriteBigEndian(body->data(), code);
    static_assert(sizeof(code) == kWebSocketCloseCodeLength,
                  "they should both be two");
    std::copy(reason.begin(), reason.end(),
              body->data() + kWebSocketCloseCodeLength);
  }
  return SendFrameInternal(true, net::WebSocketFrameHeader::kOpCodeClose,
                           std::move(body), size);
}

bool WebSocketChannel::ParseClose(scoped_refptr<net::IOBuffer> buffer,
                                  uint64_t size, uint16_t* code,
                                  std::string* reason, std::string* message) {
  reason->clear();
  if (size < kWebSocketCloseCodeLength) {
    if (size == 0U) {
      *code = net::kWebSocketErrorNoStatusReceived;
      return true;
    }

    DVLOG(1) << "Close frame with payload size " << size << " received "
             << "(the first byte is " << std::hex
             << static_cast<int>(buffer->data()[0]) << ")";
    *code = net::kWebSocketErrorProtocolError;
    *message = "Received a broken close frame containing an invalid size body.";
    return false;
  }

  const char* data = buffer->data();
  uint16_t unchecked_code = 0;
  base::ReadBigEndian(data, &unchecked_code);
  static_assert(sizeof(unchecked_code) == kWebSocketCloseCodeLength,
                "they should both be two bytes");

  switch (unchecked_code) {
    case net::kWebSocketErrorNoStatusReceived:
    case net::kWebSocketErrorAbnormalClosure:
    case net::kWebSocketErrorTlsHandshake:
      *code = net::kWebSocketErrorProtocolError;
      *message =
          "Received a broken close frame containing a reserved status code.";
      return false;

    default:
      *code = unchecked_code;
      break;
  }

  std::string text(data + kWebSocketCloseCodeLength, data + size);
  // if (StreamingUtf8Validator::Validate(text)) {
  //   reason->swap(text);
  //   return true;
  // }

  *code = net::kWebSocketErrorProtocolError;
  *reason = "Invalid UTF-8 in Close frame";
  *message = "Received a broken close frame containing invalid UTF-8.";
  return false;
}

void WebSocketChannel::DoDropChannel(bool was_clean, uint16_t code,
                                     const std::string& reason) {
  // event_interface_->OnDropChannel(was_clean, code, reason);
  if (!write_callback_.is_null())
    std::move(write_callback_).Run(net::ERR_CONNECTION_RESET);

  close_timer_.Stop();
}

void WebSocketChannel::CloseTimeout() {
  stream_->Close();
  SetState(CLOSED);
  DoDropChannel(false, net::kWebSocketErrorAbnormalClosure, "");
  // |this| has been deleted.
}

}  // namespace felicia