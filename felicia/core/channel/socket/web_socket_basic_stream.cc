// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Modified by Wonyong Kim(chokobole33@gmail.com)
// Followings are taken and modified from
// https://github.com/chromium/chromium/blob/5db095c2653f332334d56ad739ae5fe1053308b1/net/websockets/websocket_basic_stream.cc

#include "felicia/core/channel/socket/web_socket_basic_stream.h"

#include "third_party/chromium/base/metrics/histogram_macros.h"
#include "third_party/chromium/net/base/net_errors.h"

namespace felicia {

namespace {

// This uses type uint64_t to match the definition of
// WebSocketFrameHeader::payload_length in websocket_frame.h.
const uint64_t kMaxControlFramePayload = 125;

// The number of bytes to attempt to read at a time.
// TODO(ricea): See if there is a better number or algorithm to fulfill our
// requirements:
//  1. We would like to use minimal memory on low-bandwidth or idle connections
//  2. We would like to read as close to line speed as possible on
//     high-bandwidth connections
//  3. We can't afford to cause jank on the IO thread by copying large buffers
//     around
//  4. We would like to hit any sweet-spots that might exist in terms of network
//     packet sizes / encryption block sizes / IPC alignment issues, etc.
const int kReadBufferSize = 32 * 1024;

// Returns the total serialized size of |frames|. This function assumes that
// |frames| will be serialized with mask field. This function forces the
// masked bit of the frames on.
int CalculateSerializedSize(
    std::vector<std::unique_ptr<net::WebSocketFrame>>* frames) {
  const uint64_t kMaximumTotalSize = std::numeric_limits<int>::max();

  uint64_t total_size = 0;
  for (const auto& frame : *frames) {
    // We enforce flow control so the renderer should never be able to force us
    // to cache anywhere near 2GB of frames.
    uint64_t frame_size = frame->header.payload_length +
                          net::GetWebSocketFrameHeaderSize(frame->header);
    CHECK_LE(frame_size, kMaximumTotalSize - total_size)
        << "Aborting to prevent overflow";
    total_size += frame_size;
  }
  return static_cast<int>(total_size);
}

}  // namespace

WebSocketBasicStream::Adapter::~Adapter() = default;

WebSocketBasicStream::WebSocketBasicStream(std::unique_ptr<Adapter> connection)
    : read_buffer_(
          base::MakeRefCounted<net::IOBufferWithSize>(kReadBufferSize)),
      connection_(std::move(connection)) {}

WebSocketBasicStream::~WebSocketBasicStream() { Close(); }

int WebSocketBasicStream::ReadFrames(
    std::vector<std::unique_ptr<net::WebSocketFrame>>* frames,
    net::CompletionOnceCallback callback) {
  read_callback_ = std::move(callback);

  return ReadEverything(frames);
}

int WebSocketBasicStream::WriteFrames(
    std::vector<std::unique_ptr<net::WebSocketFrame>>* frames,
    net::CompletionOnceCallback callback) {
  // This function always concatenates all frames into a single buffer.
  // TODO(ricea): Investigate whether it would be better in some cases to
  // perform multiple writes with smaller buffers.

  write_callback_ = std::move(callback);

  // First calculate the size of the buffer we need to allocate.
  int total_size = CalculateSerializedSize(frames);
  auto combined_buffer =
      base::MakeRefCounted<net::IOBufferWithSize>(total_size);

  char* dest = combined_buffer->data();
  int remaining_size = total_size;
  for (const auto& frame : *frames) {
    int result = net::WriteWebSocketFrameHeader(frame->header, nullptr, dest,
                                                remaining_size);
    DCHECK_NE(net::ERR_INVALID_ARGUMENT, result)
        << "WriteWebSocketFrameHeader() says that " << remaining_size
        << " is not enough to write the header in. This should not happen.";
    CHECK_GE(result, 0) << "Potentially security-critical check failed";
    dest += result;
    remaining_size -= result;

    CHECK_LE(frame->header.payload_length,
             static_cast<uint64_t>(remaining_size));
    const int frame_size = static_cast<int>(frame->header.payload_length);
    if (frame_size > 0) {
      const char* const frame_data = frame->data->data();
      std::copy(frame_data, frame_data + frame_size, dest);
      dest += frame_size;
      remaining_size -= frame_size;
    }
  }
  DCHECK_EQ(0, remaining_size) << "Buffer size calculation was wrong; "
                               << remaining_size << " bytes left over.";
  auto drainable_buffer = base::MakeRefCounted<net::DrainableIOBuffer>(
      combined_buffer.get(), total_size);
  return WriteEverything(drainable_buffer);
}

void WebSocketBasicStream::Close() { connection_->Disconnect(); }

int WebSocketBasicStream::ReadEverything(
    std::vector<std::unique_ptr<net::WebSocketFrame>>* frames) {
  DCHECK(frames->empty());

  // Run until socket stops giving us data or we get some frames.
  while (true) {
    // base::Unretained(this) here is safe because net::Socket guarantees not to
    // call any callbacks after Disconnect(), which we call from the destructor.
    // The caller of ReadEverything() is required to keep |frames| valid.
    int result = connection_->Read(
        read_buffer_.get(), read_buffer_->size(),
        base::BindOnce(&WebSocketBasicStream::OnReadComplete,
                       base::Unretained(this), base::Unretained(frames)));
    if (result == net::ERR_IO_PENDING) return result;
    result = HandleReadResult(result, frames);
    if (result != net::ERR_IO_PENDING) return result;
    DCHECK(frames->empty());
  }
}

int WebSocketBasicStream::WriteEverything(
    const scoped_refptr<net::DrainableIOBuffer>& buffer) {
  while (buffer->BytesRemaining() > 0) {
    // The use of base::Unretained() here is safe because on destruction we
    // disconnect the socket, preventing any further callbacks.
    int result = connection_->Write(
        buffer.get(), buffer->BytesRemaining(),
        base::BindOnce(&WebSocketBasicStream::OnWriteComplete,
                       base::Unretained(this), buffer));
    if (result > 0) {
      UMA_HISTOGRAM_COUNTS_100000("Net.WebSocket.DataUse.Upstream", result);
      buffer->DidConsume(result);
    } else {
      return result;
    }
  }
  return net::OK;
}

void WebSocketBasicStream::OnWriteComplete(
    const scoped_refptr<net::DrainableIOBuffer>& buffer, int result) {
  if (result < 0) {
    DCHECK_NE(net::ERR_IO_PENDING, result);
    std::move(write_callback_).Run(result);
    return;
  }

  DCHECK_NE(0, result);
  UMA_HISTOGRAM_COUNTS_100000("Net.WebSocket.DataUse.Upstream", result);

  buffer->DidConsume(result);
  result = WriteEverything(buffer);
  if (result != net::ERR_IO_PENDING) std::move(write_callback_).Run(result);
}

void WebSocketBasicStream::OnReadComplete(
    std::vector<std::unique_ptr<net::WebSocketFrame>>* frames, int result) {
  result = HandleReadResult(result, frames);
  if (result == net::ERR_IO_PENDING) result = ReadEverything(frames);
  if (result != net::ERR_IO_PENDING) std::move(read_callback_).Run(result);
}

int WebSocketBasicStream::HandleReadResult(
    int result, std::vector<std::unique_ptr<net::WebSocketFrame>>* frames) {
  DCHECK_NE(net::ERR_IO_PENDING, result);
  DCHECK(frames->empty());
  if (result < 0) return result;
  if (result == 0) return net::ERR_CONNECTION_CLOSED;

  UMA_HISTOGRAM_COUNTS_100000("Net.WebSocket.DataUse.Downstream", result);

  std::vector<std::unique_ptr<net::WebSocketFrameChunk>> frame_chunks;
  if (!parser_.Decode(read_buffer_->data(), result, &frame_chunks))
    return WebSocketErrorToNetError(parser_.websocket_error());
  if (frame_chunks.empty()) return net::ERR_IO_PENDING;
  return ConvertChunksToFrames(&frame_chunks, frames);
}

int WebSocketBasicStream::ConvertChunksToFrames(
    std::vector<std::unique_ptr<net::WebSocketFrameChunk>>* frame_chunks,
    std::vector<std::unique_ptr<net::WebSocketFrame>>* frames) {
  for (size_t i = 0; i < frame_chunks->size(); ++i) {
    std::unique_ptr<net::WebSocketFrame> frame;
    int result = ConvertChunkToFrame(std::move((*frame_chunks)[i]), &frame);
    if (result != net::OK) return result;
    if (frame) frames->push_back(std::move(frame));
  }
  frame_chunks->clear();
  if (frames->empty()) return net::ERR_IO_PENDING;
  return net::OK;
}

int WebSocketBasicStream::ConvertChunkToFrame(
    std::unique_ptr<net::WebSocketFrameChunk> chunk,
    std::unique_ptr<net::WebSocketFrame>* frame) {
  DCHECK(frame->get() == NULL);
  bool is_first_chunk = false;
  if (chunk->header) {
    DCHECK(current_frame_header_ == NULL)
        << "Received the header for a new frame without notification that "
        << "the previous frame was complete (bug in WebSocketFrameParser?)";
    is_first_chunk = true;
    current_frame_header_.swap(chunk->header);
  }
  const int chunk_size = chunk->data.get() ? chunk->data->size() : 0;
  DCHECK(current_frame_header_)
      << "Unexpected header-less chunk received "
      << "(final_chunk = " << chunk->final_chunk
      << ", data size = " << chunk_size << ") (bug in WebSocketFrameParser?)";
  scoped_refptr<net::IOBufferWithSize> data_buffer;
  data_buffer.swap(chunk->data);
  const bool is_final_chunk = chunk->final_chunk;
  const net::WebSocketFrameHeader::OpCode opcode =
      current_frame_header_->opcode;
  if (net::WebSocketFrameHeader::IsKnownControlOpCode(opcode)) {
    bool protocol_error = false;
    if (!current_frame_header_->final) {
      DVLOG(1) << "WebSocket protocol error. Control frame, opcode=" << opcode
               << " received with FIN bit unset.";
      protocol_error = true;
    }
    if (current_frame_header_->payload_length > kMaxControlFramePayload) {
      DVLOG(1) << "WebSocket protocol error. Control frame, opcode=" << opcode
               << ", payload_length=" << current_frame_header_->payload_length
               << " exceeds maximum payload length for a control message.";
      protocol_error = true;
    }
    if (protocol_error) {
      current_frame_header_.reset();
      return net::ERR_WS_PROTOCOL_ERROR;
    }
    if (!is_final_chunk) {
      DVLOG(2) << "Encountered a split control frame, opcode " << opcode;
      if (incomplete_control_frame_body_.get()) {
        DVLOG(3) << "Appending to an existing split control frame.";
        AddToIncompleteControlFrameBody(data_buffer);
      } else {
        DVLOG(3) << "Creating new storage for an incomplete control frame.";
        incomplete_control_frame_body_ =
            base::MakeRefCounted<net::GrowableIOBuffer>();
        // This method checks for oversize control frames above, so as long as
        // the frame parser is working correctly, this won't overflow. If a bug
        // does cause it to overflow, it will CHECK() in
        // AddToIncompleteControlFrameBody() without writing outside the buffer.
        incomplete_control_frame_body_->SetCapacity(kMaxControlFramePayload);
        AddToIncompleteControlFrameBody(data_buffer);
      }
      return net::OK;
    }
    if (incomplete_control_frame_body_.get()) {
      DVLOG(2) << "Rejoining a split control frame, opcode " << opcode;
      AddToIncompleteControlFrameBody(data_buffer);
      const int body_size = incomplete_control_frame_body_->offset();
      DCHECK_EQ(body_size,
                static_cast<int>(current_frame_header_->payload_length));
      auto body = base::MakeRefCounted<net::IOBufferWithSize>(body_size);
      memcpy(body->data(), incomplete_control_frame_body_->StartOfBuffer(),
             body_size);
      incomplete_control_frame_body_ = NULL;  // Frame now complete.
      DCHECK(is_final_chunk);
      *frame = CreateFrame(is_final_chunk, body);
      return net::OK;
    }
  }

  // Apply basic sanity checks to the |payload_length| field from the frame
  // header. A check for exact equality can only be used when the whole frame
  // arrives in one chunk.
  DCHECK_GE(current_frame_header_->payload_length,
            base::checked_cast<uint64_t>(chunk_size));
  DCHECK(!is_first_chunk || !is_final_chunk ||
         current_frame_header_->payload_length ==
             base::checked_cast<uint64_t>(chunk_size));

  // Convert the chunk to a complete frame.
  *frame = CreateFrame(is_final_chunk, data_buffer);
  return net::OK;
}

std::unique_ptr<net::WebSocketFrame> WebSocketBasicStream::CreateFrame(
    bool is_final_chunk, const scoped_refptr<net::IOBufferWithSize>& data) {
  std::unique_ptr<net::WebSocketFrame> result_frame;
  const bool is_final_chunk_in_message =
      is_final_chunk && current_frame_header_->final;
  const int data_size = data.get() ? data->size() : 0;
  const net::WebSocketFrameHeader::OpCode opcode =
      current_frame_header_->opcode;
  // Empty frames convey no useful information unless they are the first frame
  // (containing the type and flags) or have the "final" bit set.
  if (is_final_chunk_in_message || data_size > 0 ||
      current_frame_header_->opcode !=
          net::WebSocketFrameHeader::kOpCodeContinuation) {
    result_frame = std::make_unique<net::WebSocketFrame>(opcode);
    result_frame->header.CopyFrom(*current_frame_header_);
    result_frame->header.final = is_final_chunk_in_message;
    result_frame->header.payload_length = data_size;
    result_frame->data = data;
    // Ensure that opcodes Text and Binary are only used for the first frame in
    // the message. Also clear the reserved bits.
    // TODO(ricea): If a future extension requires the reserved bits to be
    // retained on continuation frames, make this behaviour conditional on a
    // flag set at construction time.
    if (!is_final_chunk &&
        net::WebSocketFrameHeader::IsKnownDataOpCode(opcode)) {
      current_frame_header_->opcode =
          net::WebSocketFrameHeader::kOpCodeContinuation;
      current_frame_header_->reserved1 = false;
      current_frame_header_->reserved2 = false;
      current_frame_header_->reserved3 = false;
    }
  }
  // Make sure that a frame header is not applied to any chunks that do not
  // belong to it.
  if (is_final_chunk) current_frame_header_.reset();
  return result_frame;
}

void WebSocketBasicStream::AddToIncompleteControlFrameBody(
    const scoped_refptr<net::IOBufferWithSize>& data_buffer) {
  if (!data_buffer.get()) return;
  const int new_offset =
      incomplete_control_frame_body_->offset() + data_buffer->size();
  CHECK_GE(incomplete_control_frame_body_->capacity(), new_offset)
      << "Control frame body larger than frame header indicates; frame parser "
         "bug?";
  memcpy(incomplete_control_frame_body_->data(), data_buffer->data(),
         data_buffer->size());
  incomplete_control_frame_body_->set_offset(new_offset);
}

}  // namespace felicia