// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Modified by Wonyong Kim(chokobole33@gmail.com)
// Followings are taken and modified from
// https://github.com/chromium/chromium/blob/5db095c2653f332334d56ad739ae5fe1053308b1/net/websockets/websocket_deflate_stream.h

#ifndef FELICIA_CORE_CHANNEL_SOCKET_WEB_SOCKET_DEFLATE_STREAM_H_
#define FELICIA_CORE_CHANNEL_SOCKET_WEB_SOCKET_DEFLATE_STREAM_H_

#include "third_party/chromium/net/websockets/websocket_deflater.h"
#include "third_party/chromium/net/websockets/websocket_inflater.h"

#include "felicia/core/channel/socket/permessage_deflate.h"
#include "felicia/core/channel/socket/web_socket_stream.h"

namespace felicia {

// WebSocketDeflateStream is a WebSocketStream subclass.
// WebSocketDeflateStream is for permessage-deflate WebSocket extension[1].
//
// WebSocketDeflateStream::ReadFrames and WriteFrames may change frame
// boundary. In particular, if a control frame is placed in the middle of
// data message frames, the control frame can overtake data frames.
// Say there are frames df1, df2 and cf, df1 and df2 are frames of a
// data message and cf is a control message frame. cf may arrive first and
// data frames may follow cf.
// Note that message boundary will be preserved, i.e. if the last frame of
// a message m1 is read / written before the last frame of a message m2,
// WebSocketDeflateStream will respect the order.
//
// [1]: http://tools.ietf.org/html/draft-ietf-hybi-permessage-compression-12
class WebSocketDeflateStream : public WebSocketStream {
 public:
  WebSocketDeflateStream(std::unique_ptr<WebSocketStream> stream,
                         PermessageDeflate* permessage_deflate);
  ~WebSocketDeflateStream() override;

  // WebSocketStream functions.
  int ReadFrames(std::vector<std::unique_ptr<net::WebSocketFrame>>* frames,
                 net::CompletionOnceCallback callback) override;
  int WriteFrames(std::vector<std::unique_ptr<net::WebSocketFrame>>* frames,
                  net::CompletionOnceCallback callback) override;
  void Close() override;

 private:
  enum ReadingState {
    READING_COMPRESSED_MESSAGE,
    READING_UNCOMPRESSED_MESSAGE,
    NOT_READING,
  };

  enum WritingState {
    WRITING_COMPRESSED_MESSAGE,
    WRITING_UNCOMPRESSED_MESSAGE,
    WRITING_POSSIBLY_COMPRESSED_MESSAGE,
    NOT_WRITING,
  };

  // Handles asynchronous completion of ReadFrames() call on |stream_|.
  void OnReadComplete(std::vector<std::unique_ptr<net::WebSocketFrame>>* frames,
                      int result);

  // This function deflates |frames| and stores the result to |frames| itself.
  int Deflate(std::vector<std::unique_ptr<net::WebSocketFrame>>* frames);
  void OnMessageStart(
      const std::vector<std::unique_ptr<net::WebSocketFrame>>& frames,
      size_t index);
  int AppendCompressedFrame(
      const net::WebSocketFrameHeader& header,
      std::vector<std::unique_ptr<net::WebSocketFrame>>* frames_to_write);
  int AppendPossiblyCompressedMessage(
      std::vector<std::unique_ptr<net::WebSocketFrame>>* frames,
      std::vector<std::unique_ptr<net::WebSocketFrame>>* frames_to_write);

  // This function inflates |frames| and stores the result to |frames| itself.
  int Inflate(std::vector<std::unique_ptr<net::WebSocketFrame>>* frames);

  int InflateAndReadIfNecessary(
      std::vector<std::unique_ptr<net::WebSocketFrame>>* frames);

  const std::unique_ptr<WebSocketStream> stream_;
  net::WebSocketDeflater deflater_;
  net::WebSocketInflater inflater_;
  ReadingState reading_state_;
  WritingState writing_state_;
  net::WebSocketFrameHeader::OpCode current_reading_opcode_;
  net::WebSocketFrameHeader::OpCode current_writing_opcode_;

  // User callback saved for asynchronous reads.
  net::CompletionOnceCallback read_callback_;

  DISALLOW_COPY_AND_ASSIGN(WebSocketDeflateStream);
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_SOCKET_WEB_SOCKET_DEFLATE_STREAM_H_