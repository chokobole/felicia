// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Modified by Wonyong Kim(chokobole33@gmail.com)
// Followings are taken and modified from
// https://github.com/chromium/chromium/blob/5db095c2653f332334d56ad739ae5fe1053308b1/net/websockets/websocket_basic_stream.h

#ifndef FELICIA_CORE_CHANNEL_SOCKET_WEB_SOCKET_BASIC_STREAM_H_
#define FELICIA_CORE_CHANNEL_SOCKET_WEB_SOCKET_BASIC_STREAM_H_

#include "third_party/chromium/net/socket/tcp_socket.h"
#include "third_party/chromium/net/websockets/websocket_frame_parser.h"

#include "felicia/core/channel/socket/web_socket_stream.h"

namespace felicia {

class WebSocketBasicStream : public WebSocketStream {
 public:
  class Adapter {
   public:
    virtual ~Adapter();

    virtual bool IsConnected() = 0;

    virtual int Read(net::IOBuffer* buf, int buf_len,
                     net::CompletionOnceCallback callback) = 0;
    virtual int Write(net::IOBuffer* buf, int buf_len,
                      net::CompletionOnceCallback callback) = 0;
    virtual void Disconnect() = 0;
  };

  WebSocketBasicStream(std::unique_ptr<Adapter> connection);
  ~WebSocketBasicStream();

  // WebSocketStream implementation.
  int ReadFrames(std::vector<std::unique_ptr<net::WebSocketFrame>>* frames,
                 net::CompletionOnceCallback callback) override;

  int WriteFrames(std::vector<std::unique_ptr<net::WebSocketFrame>>* frames,
                  net::CompletionOnceCallback callback) override;

  void Close() override;

 protected:
  // Reads until socket read returns asynchronously or returns error.
  // If returns ERR_IO_PENDING, then |read_callback_| will be called with result
  // later.
  int ReadEverything(std::vector<std::unique_ptr<net::WebSocketFrame>>* frames);

  // Called when a read completes. Parses the result, tries to read more.
  // Might call |read_callback_|.
  void OnReadComplete(std::vector<std::unique_ptr<net::WebSocketFrame>>* frames,
                      int result);

  // Writes until |buffer| is fully drained (in which case returns OK) or a
  // socket write returns asynchronously or returns an error.  If returns
  // ERR_IO_PENDING, then |write_callback_| will be called with result later.
  int WriteEverything(const scoped_refptr<net::DrainableIOBuffer>& buffer);

  // Called when a write completes.  Tries to write more.
  // Might call |write_callback_|.
  void OnWriteComplete(const scoped_refptr<net::DrainableIOBuffer>& buffer,
                       int result);

  // Attempts to parse the output of a read as WebSocket frames. On success,
  // returns OK and places the frame(s) in |frames|.
  int HandleReadResult(
      int result, std::vector<std::unique_ptr<net::WebSocketFrame>>* frames);

  // Converts the chunks in |frame_chunks| into frames and writes them to
  // |frames|. |frame_chunks| is destroyed in the process. Returns
  // ERR_WS_PROTOCOL_ERROR if an invalid chunk was found. If one or more frames
  // was added to |frames|, then returns OK, otherwise returns ERR_IO_PENDING.
  int ConvertChunksToFrames(
      std::vector<std::unique_ptr<net::WebSocketFrameChunk>>* frame_chunks,
      std::vector<std::unique_ptr<net::WebSocketFrame>>* frames);

  // Converts a |chunk| to a |frame|. |*frame| should be NULL on entry to this
  // method. If |chunk| is an incomplete control frame, or an empty middle
  // frame, then |*frame| may still be NULL on exit. If an invalid control frame
  // is found, returns ERR_WS_PROTOCOL_ERROR and the stream is no longer
  // usable. Otherwise returns OK (even if frame is still NULL).
  int ConvertChunkToFrame(std::unique_ptr<net::WebSocketFrameChunk> chunk,
                          std::unique_ptr<net::WebSocketFrame>* frame);

  // Creates a frame based on the value of |is_final_chunk|, |data| and
  // |current_frame_header_|. Clears |current_frame_header_| if |is_final_chunk|
  // is true. |data| may be NULL if the frame has an empty payload. A frame in
  // the middle of a message with no data is not useful; in this case the
  // returned frame will be NULL. Otherwise, |current_frame_header_->opcode| is
  // set to Continuation after use if it was Text or Binary, in accordance with
  // WebSocket RFC6455 section 5.4.
  std::unique_ptr<net::WebSocketFrame> CreateFrame(
      bool is_final_chunk, const scoped_refptr<net::IOBufferWithSize>& data);

  // Adds |data_buffer| to the end of |incomplete_control_frame_body_|, applying
  // bounds checks.
  void AddToIncompleteControlFrameBody(
      const scoped_refptr<net::IOBufferWithSize>& data_buffer);

  // Storage for pending reads. All active WebSockets spend all the time with a
  // call to ReadFrames() pending, so there is no benefit in trying to share
  // this between sockets.
  scoped_refptr<net::IOBufferWithSize> read_buffer_;

  std::unique_ptr<Adapter> connection_;

  // Frame header for the frame currently being received. Only non-NULL while we
  // are processing the frame. If the frame arrives in multiple chunks, it can
  // remain non-NULL until additional chunks arrive. If the header of the frame
  // was invalid, this is set to NULL, the channel is failed, and subsequent
  // chunks of the same frame will be ignored.
  std::unique_ptr<net::WebSocketFrameHeader> current_frame_header_;

  // Although it should rarely happen in practice, a control frame can arrive
  // broken into chunks. This variable provides storage for a partial control
  // frame until the rest arrives. It will be NULL the rest of the time.
  scoped_refptr<net::GrowableIOBuffer> incomplete_control_frame_body_;

  // This keeps the current parse state (including any incomplete headers) and
  // parses frames.
  net::WebSocketFrameParser parser_;

  // User callback saved for asynchronous writes and reads.
  net::CompletionOnceCallback write_callback_;
  net::CompletionOnceCallback read_callback_;
};

class TCPSocketAdapter : public WebSocketBasicStream::Adapter {
 public:
  TCPSocketAdapter(std::unique_ptr<net::TCPSocket> socket)
      : socket_(std::move(socket)) {}
  TCPSocketAdapter(TCPSocketAdapter&& other)
      : socket_(std::move(other.socket_)) {}
  void operator=(TCPSocketAdapter&& other) {
    socket_ = std::move(other.socket_);
  }

  bool IsConnected() override { return socket_->IsConnected(); }

  int Read(net::IOBuffer* buf, int buf_len,
           net::CompletionOnceCallback callback) override {
    return socket_->Read(buf, buf_len, std::move(callback));
  }

  int Write(net::IOBuffer* buf, int buf_len,
            net::CompletionOnceCallback callback) override {
    return socket_->Write(
        buf, buf_len, std::move(callback),
        net::DefineNetworkTrafficAnnotation("web_socket_basic_stream", "Send"));
  }

  void Disconnect() override { return socket_->Close(); }

 private:
  std::unique_ptr<net::TCPSocket> socket_;

  DISALLOW_COPY_AND_ASSIGN(TCPSocketAdapter);
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_SOCKET_WEB_SOCKET_STREAM_H_