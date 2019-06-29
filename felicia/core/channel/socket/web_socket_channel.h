// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Modified by Wonyong Kim(chokobole33@gmail.com)
// Followings are taken and modified from
// https://github.com/chromium/chromium/blob/5db095c2653f332334d56ad739ae5fe1053308b1/net/websockets/websocket_channel.h

#ifndef FELICIA_CORE_SOCKET_WEB_SOCKET_CHANNEL_H_
#define FELICIA_CORE_SOCKET_WEB_SOCKET_CHANNEL_H_

#include "third_party/chromium/base/containers/queue.h"
#include "third_party/chromium/base/timer/timer.h"
#include "third_party/chromium/net/websockets/websocket_frame.h"

#include "felicia/core/channel/socket/web_socket_stream.h"

namespace felicia {

class WebSocketChannel {
 public:
  // Methods which return a value of type ChannelState may delete |this|. If the
  // return value is CHANNEL_DELETED, then the caller must return without making
  // any further access to member variables or methods.
  enum ChannelState { CHANNEL_ALIVE, CHANNEL_DELETED };

  WebSocketChannel(std::unique_ptr<WebSocketStream> stream);
  ~WebSocketChannel();

  // Sends a data frame to the remote side. It is the responsibility of the
  // caller to ensure that they have sufficient send quota to send this data,
  // otherwise the connection will be closed without sending. |fin| indicates
  // the last frame in a message, equivalent to "FIN" as specified in section
  // 5.2 of RFC6455. |buffer->data()| is the "Payload Data". If |op_code| is
  // kOpCodeText, or it is kOpCodeContinuation and the type the message is
  // Text, then |buffer->data()| must be a chunk of a valid UTF-8 message,
  // however there is no requirement for |buffer->data()| to be split on
  // character boundaries. Calling SendFrame may result in synchronous calls to
  // |event_interface_| which may result in this object being deleted. In that
  // case, the return value will be CHANNEL_DELETED.
  ChannelState SendFrame(bool fin, net::WebSocketFrameHeader::OpCode op_code,
                         scoped_refptr<net::IOBuffer> buffer,
                         size_t buffer_size,
                         net::CompletionOnceCallback callback);

  // Starts the closing handshake for a client-initiated shutdown of the
  // connection. There is no API to close the connection without a closing
  // handshake, but destroying the WebSocketChannel object while connected will
  // effectively do that. |code| must be in the range 1000-4999. |reason| should
  // be a valid UTF-8 string or empty.
  //
  // Calling this function may result in synchronous calls to |event_interface_|
  // which may result in this object being deleted. In that case, the return
  // value will be CHANNEL_DELETED.
  ChannelState StartClosingHandshake(uint16_t code, const std::string& reason)
      WARN_UNUSED_RESULT;

  bool IsClosedState() const;

 private:
  class PendingReceivedFrame {
   public:
    PendingReceivedFrame(bool final, net::WebSocketFrameHeader::OpCode opcode,
                         scoped_refptr<net::IOBuffer> data, uint64_t offset,
                         uint64_t size)
        : final_(final),
          opcode_(opcode),
          data_(std::move(data)),
          offset_(offset),
          size_(size) {}
    PendingReceivedFrame(const PendingReceivedFrame& other) = default;
    PendingReceivedFrame(PendingReceivedFrame&& other) = default;
    ~PendingReceivedFrame() = default;

    // PendingReceivedFrame is placed in a base::queue and so needs to be
    // copyable and movable.
    PendingReceivedFrame& operator=(const PendingReceivedFrame& other) =
        default;
    PendingReceivedFrame& operator=(PendingReceivedFrame&& other) = default;

    bool final() const { return final_; }
    net::WebSocketFrameHeader::OpCode opcode() const { return opcode_; }

    // ResetOpcode() to Continuation.
    void ResetOpcode() {
      DCHECK(net::WebSocketFrameHeader::IsKnownDataOpCode(opcode_));
      opcode_ = net::WebSocketFrameHeader::kOpCodeContinuation;
    }
    const scoped_refptr<net::IOBuffer>& data() const { return data_; }
    uint64_t offset() const { return offset_; }
    uint64_t size() const { return size_; }

    // Increase |offset_| by |bytes|.
    void DidConsume(uint64_t bytes) {
      DCHECK_LE(offset_, size_);
      DCHECK_LE(bytes, size_ - offset_);
      offset_ += bytes;
    }

   private:
    bool final_;
    net::WebSocketFrameHeader::OpCode opcode_;
    scoped_refptr<net::IOBuffer> data_;
    // Where to start reading from data_. Everything prior to offset_ has
    // already been sent to the browser.
    uint64_t offset_;
    // The size of data_.
    uint64_t size_;
  };

  // The object passes through a linear progression of states from
  // FRESHLY_CONSTRUCTED to CLOSED, except that the SEND_CLOSED and RECV_CLOSED
  // states may be skipped in case of error.
  enum State {
    FRESHLY_CONSTRUCTED,
    CONNECTING,
    CONNECTED,
    SEND_CLOSED,  // A Close frame has been sent but not received.
    RECV_CLOSED,  // Used briefly between receiving a Close frame and sending
                  // the response. Once the response is sent, the state changes
                  // to CLOSED.
    CLOSE_WAIT,   // The Closing Handshake has completed, but the remote server
                  // has not yet closed the connection.
    CLOSED,       // The Closing Handshake has completed and the connection
                  // has been closed; or the connection is failed.
  };

  // Sets |state_| to |new_state| and updates UMA if necessary.
  void SetState(State new_state);

  // Returns true if state_ is SEND_CLOSED, CLOSE_WAIT or CLOSED.
  bool InClosingState() const;

  // Calls WebSocketStream::WriteFrames() with the appropriate arguments
  ChannelState WriteFrames() WARN_UNUSED_RESULT;

  // Callback from WebSocketStream::WriteFrames. Sends pending data or adjusts
  // the send quota of the renderer channel as appropriate. |result| is a net
  // error code, usually OK. If |synchronous| is true, then OnWriteDone() is
  // being called from within the WriteFrames() loop and does not need to call
  // WriteFrames() itself.
  ChannelState OnWriteDone(bool synchronous, int result) WARN_UNUSED_RESULT;

  // Calls WebSocketStream::ReadFrames() with the appropriate arguments. Stops
  // calling ReadFrames if current_receive_quota_ is 0.
  ChannelState ReadFrames() WARN_UNUSED_RESULT;

  // Callback from WebSocketStream::ReadFrames. Handles any errors and processes
  // the returned chunks appropriately to their type. |result| is a net error
  // code. If |synchronous| is true, then OnReadDone() is being called from
  // within the ReadFrames() loop and does not need to call ReadFrames() itself.
  ChannelState OnReadDone(bool synchronous, int result) WARN_UNUSED_RESULT;

  // Handles a single frame that the object has received enough of to process.
  // May call |event_interface_| methods, send responses to the server, and
  // change the value of |state_|.
  //
  // This method performs sanity checks on the frame that are needed regardless
  // of the current state. Then, calls the HandleFrameByState() method below
  // which performs the appropriate action(s) depending on the current state.
  ChannelState HandleFrame(std::unique_ptr<net::WebSocketFrame> frame)
      WARN_UNUSED_RESULT;

  // Handles a single frame depending on the current state. It's used by the
  // HandleFrame() method.
  ChannelState HandleFrameByState(
      const net::WebSocketFrameHeader::OpCode opcode, bool final,
      scoped_refptr<net::IOBuffer> data_buffer,
      uint64_t size) WARN_UNUSED_RESULT;

  // Forwards a received data frame to the renderer, if connected. If
  // |expecting_continuation| is not equal to |expecting_to_read_continuation_|,
  // will fail the channel. Also checks the UTF-8 validity of text frames.
  ChannelState HandleDataFrame(net::WebSocketFrameHeader::OpCode opcode,
                               bool final,
                               scoped_refptr<net::IOBuffer> data_buffer,
                               uint64_t size) WARN_UNUSED_RESULT;

  // Handles an incoming close frame with |code| and |reason|.
  ChannelState HandleCloseFrame(uint16_t code,
                                const std::string& reason) WARN_UNUSED_RESULT;

  // Responds to a closing handshake initiated by the server.
  ChannelState RespondToClosingHandshake() WARN_UNUSED_RESULT;

  // Low-level method to send a single frame. Used for both data and control
  // frames. Either sends the frame immediately or buffers it to be scheduled
  // when the current write finishes. |fin| and |op_code| are defined as for
  // SendFrame() above, except that |op_code| may also be a control frame
  // opcode.
  ChannelState SendFrameInternal(bool fin,
                                 net::WebSocketFrameHeader::OpCode op_code,
                                 scoped_refptr<net::IOBuffer> buffer,
                                 uint64_t buffer_size) WARN_UNUSED_RESULT;

  // Performs the "Fail the WebSocket Connection" operation as defined in
  // RFC6455. A NotifyFailure message is sent to the renderer with |message|.
  // The renderer will log the message to the console but not expose it to
  // Javascript. Javascript will see a Close code of AbnormalClosure (1006) with
  // an empty reason string. If state_ is CONNECTED then a Close message is sent
  // to the remote host containing the supplied |code| and |reason|. If the
  // stream is open, closes it and sets state_ to CLOSED. This function deletes
  // |this|.
  void FailChannel(const std::string& message, uint16_t code,
                   const std::string& reason);

  // Sends a Close frame to Start the WebSocket Closing Handshake, or to respond
  // to a Close frame from the server. As a special case, setting |code| to
  // kWebSocketErrorNoStatusReceived will create a Close frame with no payload;
  // this is symmetric with the behaviour of ParseClose.
  ChannelState SendClose(uint16_t code,
                         const std::string& reason) WARN_UNUSED_RESULT;

  // Parses a Close frame payload. If no status code is supplied, then |code| is
  // set to 1005 (No status code) with empty |reason|. If the reason text is not
  // valid UTF-8, then |reason| is set to an empty string. If the payload size
  // is 1, or the supplied code is not permitted to be sent over the network,
  // then false is returned and |message| is set to an appropriate console
  // message.
  bool ParseClose(scoped_refptr<net::IOBuffer> buffer, uint64_t size,
                  uint16_t* code, std::string* reason, std::string* message);

  // Drop this channel.
  // If there are pending opening handshake notifications, notify them
  // before dropping. This function deletes |this|.
  void DoDropChannel(bool was_clean, uint16_t code, const std::string& reason);

  // Called if the closing handshake times out. Closes the connection and
  // informs the |event_interface_| if appropriate.
  void CloseTimeout();

  // The WebSocketStream on which to send and receive data.
  std::unique_ptr<WebSocketStream> stream_;

  // A class to encapsulate a set of frames and information about the size of
  // those frames.
  class SendBuffer {
   public:
    SendBuffer() : total_bytes_(0) {}

    // Add a WebSocketFrame to the buffer and increase total_bytes_.
    void AddFrame(std::unique_ptr<net::WebSocketFrame> chunk) {
      total_bytes_ += chunk->header.payload_length;
      frames_.push_back(std::move(chunk));
    }

    // Return a pointer to the frames_ for write purposes.
    std::vector<std::unique_ptr<net::WebSocketFrame>>* frames() {
      return &frames_;
    }

   private:
    // The frames_ that will be sent in the next call to WriteFrames().
    std::vector<std::unique_ptr<net::WebSocketFrame>> frames_;

    // The total size of the payload data in |frames_|. This will be used to
    // measure the throughput of the link.
    // TODO(ricea): Measure the throughput of the link.
    uint64_t total_bytes_;
  };

  // Data that is currently pending write, or NULL if no write is pending.
  std::unique_ptr<SendBuffer> data_being_sent_;
  // Data that is queued up to write after the current write completes.
  // Only non-NULL when such data actually exists.
  std::unique_ptr<SendBuffer> data_to_send_next_;

  // Destination for the current call to WebSocketStream::ReadFrames
  std::vector<std::unique_ptr<net::WebSocketFrame>> read_frames_;

  // Frames that have been read but not yet forwarded to the renderer due to
  // lack of quota.
  base::queue<PendingReceivedFrame> pending_received_frames_;

  // Timer for the closing handshake.
  base::OneShotTimer close_timer_;

  // Timeout for the closing handshake.
  base::TimeDelta closing_handshake_timeout_;

  // Timeout for the underlying connection close after completion of closing
  // handshake.
  base::TimeDelta underlying_connection_close_timeout_;

  // Storage for the status code and reason from the time the Close frame
  // arrives until the connection is closed and they are passed to
  // OnDropChannel().
  bool has_received_close_frame_;
  uint16_t received_close_code_;
  std::string received_close_reason_;

  // The current state of the channel. Mainly used for sanity checking, but also
  // used to track the close state.
  State state_;

  net::CompletionOnceCallback write_callback_;

  DISALLOW_COPY_AND_ASSIGN(WebSocketChannel);
};

}  // namespace felicia

#endif  // FELICIA_CORE_SOCKET_WEB_SOCKET_CHANNEL_H_