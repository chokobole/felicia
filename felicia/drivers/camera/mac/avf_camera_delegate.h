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

#include "felicia/drivers/camera/camera_descriptor.h"
#include "felicia/drivers/camera/camera_format.h"

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
}

// Returns a dictionary of capture devices with friendly name and unique id.
+ (NSDictionary*)deviceNames;

// Retrieve the capture supported formats for a given device |descriptor|.
+ (void)getDevice:(const felicia::CameraDescriptor&)camera_descriptor
    supportedFormats:(felicia::CameraFormats*)camera_formats;

@end

#endif  // FELICIA_DRIVERS_CAMERA_MAC_AVF_CAMERA_DELEGATE_H_