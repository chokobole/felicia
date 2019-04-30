// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Modified by Wonyong Kim(chokobole33@gmail.com)
// Followings are taken and modified from
// https://github.com/chromium/chromium/blob/5db095c2653f332334d56ad739ae5fe1053308b1/media/capture/video/mac/video_capture_device_avfoundation_mac.mm

#import "felicia/drivers/camera/mac/avf_camera_delegate.h"

#import <CoreMedia/CoreMedia.h>

#include "third_party/chromium/base/strings/sys_string_conversions.h"

@implementation DeviceNameAndTransportType

- (id)initWithName:(NSString*)deviceName transportType:(int32_t)transportType {
  if (self = [super init]) {
    deviceName_.reset([deviceName copy]);
    transportType_ = transportType;
  }
  return self;
}

- (NSString*)deviceName {
  return deviceName_;
}

- (int32_t)transportType {
  return transportType_;
}

@end  // @implementation DeviceNameAndTransportType

@implementation AvfCameraDelegate

#pragma mark Class methods

+ (void)getDeviceNames:(NSMutableDictionary*)deviceNames {
  // At this stage we already know that AVFoundation is supported and the whole
  // library is loaded and initialised, by the device monitoring.
  NSArray* devices = [AVCaptureDevice devices];
  int number_of_suspended_devices = 0;
  for (AVCaptureDevice* device in devices) {
    if ([device hasMediaType:AVMediaTypeVideo] || [device hasMediaType:AVMediaTypeMuxed]) {
      if ([device isSuspended]) {
        ++number_of_suspended_devices;
        continue;
      }
      DeviceNameAndTransportType* nameAndTransportType =
          [[[DeviceNameAndTransportType alloc] initWithName:[device localizedName]
                                              transportType:[device transportType]] autorelease];
      [deviceNames setObject:nameAndTransportType forKey:[device uniqueID]];
    }
  }
}

+ (NSDictionary*)deviceNames {
  NSMutableDictionary* deviceNames = [[[NSMutableDictionary alloc] init] autorelease];
  // The device name retrieval is not going to happen in the main thread, and
  // this might cause instabilities (it did in QTKit), so keep an eye here.
  [self getDeviceNames:deviceNames];
  return deviceNames;
}

+ (void)getDevice:(const felicia::CameraDescriptor&)camera_descriptor
    supportedFormats:(felicia::CameraFormats*)camera_formats {
  NSArray* devices = [AVCaptureDevice devices];
  AVCaptureDevice* device = nil;
  for (device in devices) {
    if (base::SysNSStringToUTF8([device uniqueID]) == camera_descriptor.device_id()) break;
  }
  if (device == nil) return;
  for (AVCaptureDeviceFormat* format in device.formats) {
    // MediaSubType is a CMPixelFormatType but can be used as CVPixelFormatType
    // as well according to CMFormatDescription.h
    const felicia::CameraFormat::PixelFormat pixelFormat =
        felicia::CameraFormat::FromAVFoundationPixelFormat(
            CMFormatDescriptionGetMediaSubType([format formatDescription]));

    CMVideoDimensions dimensions =
        CMVideoFormatDescriptionGetDimensions([format formatDescription]);

    for (AVFrameRateRange* frameRate in [format videoSupportedFrameRateRanges]) {
      felicia::CameraFormat camera_format = felicia::CameraFormat(
          dimensions.width, dimensions.height, pixelFormat, frameRate.maxFrameRate);
      camera_formats->push_back(camera_format);
      DVLOG(2) << camera_descriptor.display_name() << " " << camera_format.ToString();
    }
  }
}

@end