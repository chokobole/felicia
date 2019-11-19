// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_CORE_CHANNEL_CHANNEL_H_
#define FELICIA_CORE_CHANNEL_CHANNEL_H_

#include "third_party/chromium/net/base/address_list.h"
#include "third_party/chromium/net/base/io_buffer.h"

#include "felicia/core/channel/channel_buffer.h"
#include "felicia/core/channel/channel_impl.h"
#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/error/statusor.h"
#include "felicia/core/lib/unit/bytes.h"
#include "felicia/core/protobuf/channel.pb.h"

namespace felicia {

class ShmChannel;
class TCPChannel;
class UDPChannel;
class UDSChannel;
class WSChannel;

class FEL_EXPORT Channel {
 public:
  virtual ~Channel();

  virtual bool IsShmChannel() const;
  virtual bool IsTCPChannel() const;
  virtual bool IsUDPChannel() const;
  virtual bool IsUDSChannel() const;
  virtual bool IsWSChannel() const;

  ShmChannel* ToShmChannel();
  TCPChannel* ToTCPChannel();
  UDPChannel* ToUDPChannel();
  UDSChannel* ToUDSChannel();
  WSChannel* ToWSChannel();

  virtual ChannelDef::Type type() const = 0;

  virtual bool HasReceivers() const;

  // Default false, it means it reads header first and then reads message
  // later. If it returns true, it reads message and header at the same time.
  // UDPChannel and ShmChannel returns true.
  virtual bool ShouldReceiveMessageWithHeader() const;

  // Default false, Websocket has its own header indicates message size.
  // So for websocket channel, it sends message as it is and inside websocekt
  // implementation will attach header.
  virtual bool HasNativeHeader() const;

  bool IsSending() const;
  bool IsReceiving() const;

  virtual void Connect(const ChannelDef& channel_def,
                       StatusOnceCallback callback) = 0;

  bool IsConnected() const;

  void Send(const std::string& text, StatusOnceCallback callback);
  void Receive(std::string* text, StatusOnceCallback callback);

  virtual void SetSendBufferSize(Bytes bytes);
  virtual void SetReceiveBufferSize(Bytes bytes);

  void SetDynamicSendBuffer(bool is_dynamic);
  void SetDynamicReceiveBuffer(bool is_dynamic);

 protected:
  friend class ChannelFactory;
  template <typename T>
  friend class MessageSender;
  template <typename T>
  friend class MessageReceiver;
  template <typename T>
  friend class Publisher;
  template <typename T>
  friend class Subscriber;

  void SetSendBuffer(const ChannelBuffer& send_buffer);
  void SetReceiveBuffer(const ChannelBuffer& receive_buffer);

  void SendInternalBuffer(int size, StatusOnceCallback callback);
  void ReceiveInternalBuffer(int size, StatusOnceCallback callback);

  void OnSend(Status s);
  void OnReceive(Status s);

  virtual bool TrySetEnoughReceiveBufferSize(int capacity);

  Channel();

  std::string* text_ = nullptr;

  std::unique_ptr<ChannelImpl> channel_impl_;
  ChannelBuffer send_buffer_;
  StatusOnceCallback send_callback_;
  ChannelBuffer receive_buffer_;
  StatusOnceCallback receive_callback_;

  DISALLOW_COPY_AND_ASSIGN(Channel);
};

// Convert |ip_endpoint()| of |channel_def| to net::AdressList,
// Returns Status::OK() if succeeded.
FEL_EXPORT Status ToNetAddressList(const ChannelDef& channel_def,
                                   net::AddressList* addrlist);

// Convert EndPoint of |channel_def| to std::string
FEL_EXPORT std::string EndPointToString(const ChannelDef& channel_def);

// Check if |channel_def| is a valid. Returns true if so.
FEL_EXPORT bool IsValidChannelDef(const ChannelDef& channel_def);

// Check if |channel_source| is a valid. Returns true if so.
FEL_EXPORT bool IsValidChannelSource(const ChannelSource& channel_source);

// Check if |c| and |c2| are same. Returns true if so. Return false if
// either |c| or |c2| is invalid or they are not same.
FEL_EXPORT bool IsSameChannelDef(const ChannelDef& c, const ChannelDef& c2);

// Check if |c| and |c2| are same. Returns true if so. Return false if
// either |c| or |c2| is invalid or they are not same.
FEL_EXPORT bool IsSameChannelSource(const ChannelSource& c,
                                    const ChannelSource& c2);

FEL_EXPORT int AllChannelTypes();

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_CHANNEL_H_