// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Modified by Wonyong Kim(chokobole33@gmail.com)
// Followings are taken and modified from
// https://github.com/chromium/chromium/blob/5db095c2653f332334d56ad739ae5fe1053308b1/net/websockets/websocket_deflate_stream.cc

#include "felicia/core/channel/socket/web_socket_deflate_stream.h"

#include "third_party/chromium/net/base/net_errors.h"

namespace felicia {

namespace {

const int kWindowBits = 15;
const size_t kChunkSize = 4 * 1024;

}  // namespace

WebSocketDeflateStream::WebSocketDeflateStream(
    std::unique_ptr<WebSocketStream> stream,
    PermessageDeflate* permessage_deflate)
    : stream_(std::move(stream)),
      deflater_(permessage_deflate->server_context_take_over_mode()),
      inflater_(kChunkSize, kChunkSize),
      reading_state_(NOT_READING),
      writing_state_(NOT_WRITING),
      current_reading_opcode_(net::WebSocketFrameHeader::kOpCodeText),
      current_writing_opcode_(net::WebSocketFrameHeader::kOpCodeText) {
  DCHECK(stream_);
  deflater_.Initialize(permessage_deflate->server_max_window_bits());
  inflater_.Initialize(permessage_deflate->client_max_window_bits());
}

WebSocketDeflateStream::~WebSocketDeflateStream() = default;

int WebSocketDeflateStream::ReadFrames(
    std::vector<std::unique_ptr<net::WebSocketFrame>>* frames,
    net::CompletionOnceCallback callback) {
  read_callback_ = std::move(callback);
  int result = stream_->ReadFrames(
      frames, base::BindOnce(&WebSocketDeflateStream::OnReadComplete,
                             base::Unretained(this), base::Unretained(frames)));
  if (result < 0) return result;
  DCHECK_EQ(net::OK, result);
  DCHECK(!frames->empty());

  return InflateAndReadIfNecessary(frames);
}

int WebSocketDeflateStream::WriteFrames(
    std::vector<std::unique_ptr<net::WebSocketFrame>>* frames,
    net::CompletionOnceCallback callback) {
  int result = Deflate(frames);
  if (result != net::OK) return result;
  if (frames->empty()) return net::OK;
  return stream_->WriteFrames(frames, std::move(callback));
}

void WebSocketDeflateStream::Close() { stream_->Close(); }

void WebSocketDeflateStream::OnReadComplete(
    std::vector<std::unique_ptr<net::WebSocketFrame>>* frames, int result) {
  if (result != net::OK) {
    frames->clear();
    std::move(read_callback_).Run(result);
    return;
  }

  int r = InflateAndReadIfNecessary(frames);
  if (r != net::ERR_IO_PENDING) std::move(read_callback_).Run(r);
}

int WebSocketDeflateStream::Deflate(
    std::vector<std::unique_ptr<net::WebSocketFrame>>* frames) {
  std::vector<std::unique_ptr<net::WebSocketFrame>> frames_to_write;
  // Store frames of the currently processed message if writing_state_ equals to
  // WRITING_POSSIBLY_COMPRESSED_MESSAGE.
  std::vector<std::unique_ptr<net::WebSocketFrame>> frames_of_message;
  for (size_t i = 0; i < frames->size(); ++i) {
    DCHECK(!(*frames)[i]->header.reserved1);
    if (!net::WebSocketFrameHeader::IsKnownDataOpCode(
            (*frames)[i]->header.opcode)) {
      frames_to_write.push_back(std::move((*frames)[i]));
      continue;
    }
    if (writing_state_ == NOT_WRITING) OnMessageStart(*frames, i);

    std::unique_ptr<net::WebSocketFrame> frame(std::move((*frames)[i]));

    if (writing_state_ == WRITING_UNCOMPRESSED_MESSAGE) {
      if (frame->header.final) writing_state_ = NOT_WRITING;
      frames_to_write.push_back(std::move(frame));
      current_writing_opcode_ = net::WebSocketFrameHeader::kOpCodeContinuation;
    } else {
      if (frame->data.get() &&
          !deflater_.AddBytes(
              frame->data->data(),
              static_cast<size_t>(frame->header.payload_length))) {
        DVLOG(1) << "WebSocket protocol error. "
                 << "deflater_.AddBytes() returns an error.";
        return net::ERR_WS_PROTOCOL_ERROR;
      }
      if (frame->header.final && !deflater_.Finish()) {
        DVLOG(1) << "WebSocket protocol error. "
                 << "deflater_.Finish() returns an error.";
        return net::ERR_WS_PROTOCOL_ERROR;
      }

      if (writing_state_ == WRITING_COMPRESSED_MESSAGE) {
        if (deflater_.CurrentOutputSize() >= kChunkSize ||
            frame->header.final) {
          int result = AppendCompressedFrame(frame->header, &frames_to_write);
          if (result != net::OK) return result;
        }
        if (frame->header.final) writing_state_ = NOT_WRITING;
      } else {
        DCHECK_EQ(WRITING_POSSIBLY_COMPRESSED_MESSAGE, writing_state_);
        bool final = frame->header.final;
        frames_of_message.push_back(std::move(frame));
        if (final) {
          int result = AppendPossiblyCompressedMessage(&frames_of_message,
                                                       &frames_to_write);
          if (result != net::OK) return result;
          frames_of_message.clear();
          writing_state_ = NOT_WRITING;
        }
      }
    }
  }
  DCHECK_NE(WRITING_POSSIBLY_COMPRESSED_MESSAGE, writing_state_);
  frames->swap(frames_to_write);
  return net::OK;
}

void WebSocketDeflateStream::OnMessageStart(
    const std::vector<std::unique_ptr<net::WebSocketFrame>>& frames,
    size_t index) {
  net::WebSocketFrame* frame = frames[index].get();
  current_writing_opcode_ = frame->header.opcode;
  DCHECK(current_writing_opcode_ == net::WebSocketFrameHeader::kOpCodeText ||
         current_writing_opcode_ == net::WebSocketFrameHeader::kOpCodeBinary);

  writing_state_ = WRITING_COMPRESSED_MESSAGE;
}

int WebSocketDeflateStream::AppendCompressedFrame(
    const net::WebSocketFrameHeader& header,
    std::vector<std::unique_ptr<net::WebSocketFrame>>* frames_to_write) {
  const net::WebSocketFrameHeader::OpCode opcode = current_writing_opcode_;
  scoped_refptr<net::IOBufferWithSize> compressed_payload =
      deflater_.GetOutput(deflater_.CurrentOutputSize());
  if (!compressed_payload.get()) {
    DVLOG(1) << "WebSocket protocol error. "
             << "deflater_.GetOutput() returns an error.";
    return net::ERR_WS_PROTOCOL_ERROR;
  }
  auto compressed = std::make_unique<net::WebSocketFrame>(opcode);
  compressed->header.CopyFrom(header);
  compressed->header.opcode = opcode;
  compressed->header.final = header.final;
  compressed->header.reserved1 =
      (opcode != net::WebSocketFrameHeader::kOpCodeContinuation);
  compressed->data = compressed_payload;
  compressed->header.payload_length = compressed_payload->size();

  current_writing_opcode_ = net::WebSocketFrameHeader::kOpCodeContinuation;
  frames_to_write->push_back(std::move(compressed));
  return net::OK;
}

int WebSocketDeflateStream::AppendPossiblyCompressedMessage(
    std::vector<std::unique_ptr<net::WebSocketFrame>>* frames,
    std::vector<std::unique_ptr<net::WebSocketFrame>>* frames_to_write) {
  DCHECK(!frames->empty());

  const net::WebSocketFrameHeader::OpCode opcode = current_writing_opcode_;
  scoped_refptr<net::IOBufferWithSize> compressed_payload =
      deflater_.GetOutput(deflater_.CurrentOutputSize());
  if (!compressed_payload.get()) {
    DVLOG(1) << "WebSocket protocol error. "
             << "deflater_.GetOutput() returns an error.";
    return net::ERR_WS_PROTOCOL_ERROR;
  }

  uint64_t original_payload_length = 0;
  for (size_t i = 0; i < frames->size(); ++i) {
    net::WebSocketFrame* frame = (*frames)[i].get();
    // Asserts checking that frames represent one whole data message.
    DCHECK(net::WebSocketFrameHeader::IsKnownDataOpCode(frame->header.opcode));
    DCHECK_EQ(i == 0, net::WebSocketFrameHeader::kOpCodeContinuation !=
                          frame->header.opcode);
    DCHECK_EQ(i == frames->size() - 1, frame->header.final);
    original_payload_length += frame->header.payload_length;
  }
  if (original_payload_length <=
      static_cast<uint64_t>(compressed_payload->size())) {
    // Compression is not effective. Use the original frames.
    for (size_t i = 0; i < frames->size(); ++i) {
      std::unique_ptr<net::WebSocketFrame> frame = std::move((*frames)[i]);
      frames_to_write->push_back(std::move(frame));
    }
    frames->clear();
    return net::OK;
  }
  auto compressed = std::make_unique<net::WebSocketFrame>(opcode);
  compressed->header.CopyFrom((*frames)[0]->header);
  compressed->header.opcode = opcode;
  compressed->header.final = true;
  compressed->header.reserved1 = true;
  compressed->data = compressed_payload;
  compressed->header.payload_length = compressed_payload->size();

  frames_to_write->push_back(std::move(compressed));
  return net::OK;
}

int WebSocketDeflateStream::Inflate(
    std::vector<std::unique_ptr<net::WebSocketFrame>>* frames) {
  std::vector<std::unique_ptr<net::WebSocketFrame>> frames_to_output;
  std::vector<std::unique_ptr<net::WebSocketFrame>> frames_passed;
  frames->swap(frames_passed);
  for (size_t i = 0; i < frames_passed.size(); ++i) {
    std::unique_ptr<net::WebSocketFrame> frame(std::move(frames_passed[i]));
    frames_passed[i] = NULL;
    DVLOG(3) << "Input frame: opcode=" << frame->header.opcode
             << " final=" << frame->header.final
             << " reserved1=" << frame->header.reserved1
             << " payload_length=" << frame->header.payload_length;

    if (!net::WebSocketFrameHeader::IsKnownDataOpCode(frame->header.opcode)) {
      frames_to_output.push_back(std::move(frame));
      continue;
    }

    if (reading_state_ == NOT_READING) {
      if (frame->header.reserved1)
        reading_state_ = READING_COMPRESSED_MESSAGE;
      else
        reading_state_ = READING_UNCOMPRESSED_MESSAGE;
      current_reading_opcode_ = frame->header.opcode;
    } else {
      if (frame->header.reserved1) {
        DVLOG(1) << "WebSocket protocol error. "
                 << "Receiving a non-first frame with RSV1 flag set.";
        return net::ERR_WS_PROTOCOL_ERROR;
      }
    }

    if (reading_state_ == READING_UNCOMPRESSED_MESSAGE) {
      if (frame->header.final) reading_state_ = NOT_READING;
      current_reading_opcode_ = net::WebSocketFrameHeader::kOpCodeContinuation;
      frames_to_output.push_back(std::move(frame));
    } else {
      DCHECK_EQ(reading_state_, READING_COMPRESSED_MESSAGE);
      if (frame->data.get() &&
          !inflater_.AddBytes(
              frame->data->data(),
              static_cast<size_t>(frame->header.payload_length))) {
        DVLOG(1) << "WebSocket protocol error. "
                 << "inflater_.AddBytes() returns an error.";
        return net::ERR_WS_PROTOCOL_ERROR;
      }
      if (frame->header.final) {
        if (!inflater_.Finish()) {
          DVLOG(1) << "WebSocket protocol error. "
                   << "inflater_.Finish() returns an error.";
          return net::ERR_WS_PROTOCOL_ERROR;
        }
      }
      // TODO(yhirano): Many frames can be generated by the inflater and
      // memory consumption can grow.
      // We could avoid it, but avoiding it makes this class much more
      // complicated.
      while (inflater_.CurrentOutputSize() >= kChunkSize ||
             frame->header.final) {
        size_t size = std::min(kChunkSize, inflater_.CurrentOutputSize());
        auto inflated = std::make_unique<net::WebSocketFrame>(
            net::WebSocketFrameHeader::kOpCodeText);
        scoped_refptr<net::IOBufferWithSize> data = inflater_.GetOutput(size);
        bool is_final = !inflater_.CurrentOutputSize() && frame->header.final;
        if (!data.get()) {
          DVLOG(1) << "WebSocket protocol error. "
                   << "inflater_.GetOutput() returns an error.";
          return net::ERR_WS_PROTOCOL_ERROR;
        }
        inflated->header.CopyFrom(frame->header);
        inflated->header.opcode = current_reading_opcode_;
        inflated->header.final = is_final;
        inflated->header.reserved1 = false;
        inflated->data = data;
        inflated->header.payload_length = data->size();
        DVLOG(3) << "Inflated frame: opcode=" << inflated->header.opcode
                 << " final=" << inflated->header.final
                 << " reserved1=" << inflated->header.reserved1
                 << " payload_length=" << inflated->header.payload_length;
        frames_to_output.push_back(std::move(inflated));
        current_reading_opcode_ =
            net::WebSocketFrameHeader::kOpCodeContinuation;
        if (is_final) break;
      }
      if (frame->header.final) reading_state_ = NOT_READING;
    }
  }
  frames->swap(frames_to_output);
  return frames->empty() ? net::ERR_IO_PENDING : net::OK;
}

int WebSocketDeflateStream::InflateAndReadIfNecessary(
    std::vector<std::unique_ptr<net::WebSocketFrame>>* frames) {
  int result = Inflate(frames);
  while (result == net::ERR_IO_PENDING) {
    DCHECK(frames->empty());

    result = stream_->ReadFrames(
        frames,
        base::BindOnce(&WebSocketDeflateStream::OnReadComplete,
                       base::Unretained(this), base::Unretained(frames)));
    if (result < 0) break;
    DCHECK_EQ(net::OK, result);
    DCHECK(!frames->empty());

    result = Inflate(frames);
  }
  if (result < 0) frames->clear();
  return result;
}

}  // namespace felicia