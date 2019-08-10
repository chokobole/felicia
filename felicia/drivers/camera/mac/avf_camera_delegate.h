// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Modified by Wonyong Kim(chokobole33@gmail.com)
// Followings are taken and modified from
// https://github.com/chromium/chromium/blob/5db095c2653f332334d56ad739ae5fe1053308b1/media/capture/video/mac/video_capture_device_avfoundation_mac.h

#ifndef FELICIA_DRIVERS_CAMERA_MAC_AVF_CAMERA_DELEGATE_H_
#define FELICIA_DRIVERS_CAMERA_MAC_AVF_CAMERA_DELEGATE_H_

#import <AVFoundation/AVFoundation.h>
#import <Foundation/Foundation.h>

#include "third_party/chromium/base/mac/scoped_nsobject.h"
#include "third_party/chromium/base/synchronization/lock.h"
#include "third_party/chromium/base/threading/thread_checker.h"

#include "felicia/drivers/camera/camera_descriptor.h"
#include "felicia/drivers/camera/camera_format.h"
#include "felicia/drivers/camera/mac/frame_receiver.h"

// Small class to bundle device name and connection type into a dictionary.
@interface DeviceNameAndTransportType : NSObject {
 @private
  base::scoped_nsobject<NSString> deviceName_;
  // The transport type of the device (USB, PCI, etc), values are defined in
  // <IOKit/audio/IOAudioTypes.h> as kIOAudioDeviceTransportType*.
  int32_t transportType_;
}

- (id)initWithName:(NSString*)name transportType:(int32_t)transportType;

- (NSString*)deviceName;
- (int32_t)transportType;
@end

@interface AvfCameraDelegate : NSObject <AVCaptureVideoDataOutputSampleBufferDelegate> {
 @private
  // The following attributes are set via -setCaptureHeight:width:frameRate:fourcc:.
  int frameWidth_;
  int frameHeight_;
  float frameRate_;
  FourCharCode fourcc_;
  bool was_set_;

  base::Lock lock_;  // Protects concurrent setting and using |frameReceiver_|.
  felicia::drivers::FrameReceiver* frameReceiver_;  // weak.

  base::scoped_nsobject<AVCaptureSession> captureSession_;

  // |captureDevice_| is an object coming from AVFoundation, used only to be
  // plugged in |captureDeviceInput_| and to query for session preset support.
  AVCaptureDevice* captureDevice_;
  // |captureDeviceInput_| is owned by |captureSession_|.
  AVCaptureDeviceInput* captureDeviceInput_;
  base::scoped_nsobject<AVCaptureVideoDataOutput> captureVideoDataOutput_;

  // An AVDataOutput specialized for taking pictures out of |captureSession_|.
  base::scoped_nsobject<AVCaptureStillImageOutput> stillImageOutput_;

  base::ThreadChecker main_thread_checker_;
}

// Returns a dictionary of capture devices with friendly name and unique id.
+ (NSDictionary*)deviceNames;

// Retrieve the capture supported formats for a given device |descriptor|.
+ (void)getDevice:(const felicia::drivers::CameraDescriptor&)camera_descriptor
    supportedFormats:(felicia::drivers::CameraFormats*)camera_formats;

// Initializes the instance and the underlying capture session and registers the
// frame receiver.
- (id)initWithFrameReceiver:(felicia::drivers::FrameReceiver*)frameReceiver;

// Sets the frame receiver.
- (void)setFrameReceiver:(felicia::drivers::FrameReceiver*)frameReceiver;

// Sets which capture device to use by name, retrieved via |deviceNames|. Once
// the deviceId is known, the library objects are created if needed and
// connected for the capture, and a by default resolution is set. If deviceId is
// nil, then the eventual capture is stopped and library objects are
// disconnected. Returns YES on success, NO otherwise. If the return value is
// NO, an error message is assigned to |outMessage|. This method should not be
// called during capture.
- (BOOL)setCaptureDevice:(NSString*)deviceId errorMessage:(NSString**)outMessage;

// Retrieves the capture properties. Return YES on success, else NO.
- (BOOL)getCameraFormat:(felicia::drivers::CameraFormat*)cameraFormat;

// Configures the capture properties for the capture session and the video data
// output; this means it MUST be called after setCaptureDevice:. Return YES on
// success, else NO.
- (BOOL)setCaptureHeight:(int)height
                   width:(int)width
               frameRate:(float)frameRate
                  fourcc:(FourCharCode)fourcc;

// Starts video capturing and register the notification listeners. Must be
// called after setCaptureDevice:, and, eventually, also after
// setCaptureHeight:width:frameRate:. Returns YES on success, NO otherwise.
- (BOOL)startCapture;

// Stops video capturing and stops listening to notifications.
- (void)stopCapture;

@end

#endif  // FELICIA_DRIVERS_CAMERA_MAC_AVF_CAMERA_DELEGATE_H_