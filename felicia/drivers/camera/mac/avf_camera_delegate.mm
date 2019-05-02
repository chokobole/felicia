// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Modified by Wonyong Kim(chokobole33@gmail.com)
// Followings are taken and modified from
// https://github.com/chromium/chromium/blob/5db095c2653f332334d56ad739ae5fe1053308b1/media/capture/video/mac/video_capture_device_avfoundation_mac.mm

#import "felicia/drivers/camera/mac/avf_camera_delegate.h"

#import <CoreMedia/CoreMedia.h>

#include "third_party/chromium/base/mac/foundation_util.h"
#include "third_party/chromium/base/strings/sys_string_conversions.h"

#include "felicia/drivers/camera/camera_errors.h"
#include "felicia/drivers/camera/timestamp_constants.h"

namespace {

// Extracts |base_address| and |length| out of a SampleBuffer.
void ExtractBaseAddressAndLength(char** base_address, size_t* length,
                                 CMSampleBufferRef sample_buffer) {
  CMBlockBufferRef block_buffer = CMSampleBufferGetDataBuffer(sample_buffer);
  DCHECK(block_buffer);

  size_t length_at_offset;
  const OSStatus status =
      CMBlockBufferGetDataPointer(block_buffer, 0, &length_at_offset, length, base_address);
  DCHECK_EQ(noErr, status);
  // Expect the (M)JPEG data to be available as a contiguous reference, i.e.
  // not covered by multiple memory blocks.
  DCHECK_EQ(length_at_offset, *length);
}

}  // namespace

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

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wthread-safety-analysis"

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

#pragma mark Public methods

- (id)initWithFrameReceiver:(felicia::FrameReceiver*)frameReceiver {
  if ((self = [super init])) {
    DCHECK(main_thread_checker_.CalledOnValidThread());
    DCHECK(frameReceiver);
    [self setFrameReceiver:frameReceiver];
    captureSession_.reset([[AVCaptureSession alloc] init]);
    self->was_set_ = NO;
  }
  return self;
}

- (void)dealloc {
  [self stopCapture];
  [super dealloc];
}

- (void)setFrameReceiver:(felicia::FrameReceiver*)frameReceiver {
  base::AutoLock lock(lock_);
  frameReceiver_ = frameReceiver;
}

- (BOOL)setCaptureDevice:(NSString*)deviceId errorMessage:(NSString**)outMessage {
  DCHECK(captureSession_);
  DCHECK(main_thread_checker_.CalledOnValidThread());

  if (!deviceId) {
    // First stop the capture session, if it's running.
    [self stopCapture];
    // Now remove the input and output from the capture session.
    [captureSession_ removeOutput:captureVideoDataOutput_];
    if (stillImageOutput_) [captureSession_ removeOutput:stillImageOutput_];
    if (captureDeviceInput_) {
      [captureSession_ removeInput:captureDeviceInput_];
      // No need to release |captureDeviceInput_|, is owned by the session.
      captureDeviceInput_ = nil;
    }
    return YES;
  }

  // Look for input device with requested name.
  captureDevice_ = [AVCaptureDevice deviceWithUniqueID:deviceId];
  if (!captureDevice_) {
    *outMessage = [NSString stringWithUTF8String:"Could not open video capture device."];
    return NO;
  }

  // Create the capture input associated with the device. Easy peasy.
  NSError* error = nil;
  captureDeviceInput_ = [AVCaptureDeviceInput deviceInputWithDevice:captureDevice_ error:&error];
  if (!captureDeviceInput_) {
    captureDevice_ = nil;
    *outMessage =
        [NSString stringWithFormat:@"Could not create video capture input (%@): %@",
                                   [error localizedDescription], [error localizedFailureReason]];
    return NO;
  }
  [captureSession_ addInput:captureDeviceInput_];

  // Create and plug the still image capture output. This should happen in
  // advance of the actual picture to allow for the 3A to stabilize.
  stillImageOutput_.reset([[AVCaptureStillImageOutput alloc] init]);
  if (stillImageOutput_ && [captureSession_ canAddOutput:stillImageOutput_])
    [captureSession_ addOutput:stillImageOutput_];

  // Create a new data output for video. The data output is configured to
  // discard late frames by default.
  captureVideoDataOutput_.reset([[AVCaptureVideoDataOutput alloc] init]);
  if (!captureVideoDataOutput_) {
    [captureSession_ removeInput:captureDeviceInput_];
    *outMessage = [NSString stringWithUTF8String:"Could not create video data output."];
    return NO;
  }
  [captureVideoDataOutput_ setAlwaysDiscardsLateVideoFrames:true];
  [captureVideoDataOutput_
      setSampleBufferDelegate:self
                        queue:dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0)];
  [captureSession_ addOutput:captureVideoDataOutput_];

  return YES;
}

- (BOOL)getCameraFormat:(felicia::CameraFormat*)cameraFormat {
  if (was_set_) {
    *cameraFormat = felicia::CameraFormat(
        frameWidth_, frameHeight_, felicia::CameraFormat::FromAVFoundationPixelFormat(fourcc_),
        frameRate_);
    return YES;
  }

  const CMFormatDescriptionRef formatDescription =
      [[captureDevice_ activeFormat] formatDescription];
  const FourCharCode fourcc = CMFormatDescriptionGetMediaSubType(formatDescription);
  const CMVideoDimensions dimensions = CMVideoFormatDescriptionGetDimensions(formatDescription);
  const CMTime maxFrameDuration = [captureDevice_ activeVideoMaxFrameDuration];
  const float maxFrameRate = maxFrameDuration.timescale / double(maxFrameDuration.value);
  *cameraFormat = felicia::CameraFormat(
      felicia::Sizei(static_cast<int>(dimensions.width), static_cast<int>(dimensions.height)),
      felicia::CameraFormat::FromAVFoundationPixelFormat(fourcc), maxFrameRate);

  return YES;
}

- (BOOL)setCaptureHeight:(int)height
                   width:(int)width
               frameRate:(float)frameRate
                  fourcc:(FourCharCode)fourcc {
  DCHECK(![captureSession_ isRunning] && main_thread_checker_.CalledOnValidThread());

  frameWidth_ = width;
  frameHeight_ = height;
  frameRate_ = frameRate;
  fourcc_ = fourcc;
  was_set_ = true;

  if (fourcc_ == kCMVideoCodecType_JPEG_OpenDML) {
    [captureSession_ removeOutput:stillImageOutput_];
    stillImageOutput_.reset();
  }

  // The capture output has to be configured, despite Mac documentation
  // detailing that setting the sessionPreset would be enough. The reason for
  // this mismatch is probably because most of the AVFoundation docs are written
  // for iOS and not for MacOsX. AVVideoScalingModeKey() refers to letterboxing
  // yes/no and preserve aspect ratio yes/no when scaling. Currently we set
  // cropping and preservation.
  NSDictionary* videoSettingsDictionary = @{
    (id)kCVPixelBufferWidthKey : @(width),
    (id)kCVPixelBufferHeightKey : @(height),
    (id)kCVPixelBufferPixelFormatTypeKey : @(fourcc),
    AVVideoScalingModeKey : AVVideoScalingModeResizeAspectFill
  };
  [captureVideoDataOutput_ setVideoSettings:videoSettingsDictionary];

  AVCaptureConnection* captureConnection =
      [captureVideoDataOutput_ connectionWithMediaType:AVMediaTypeVideo];
  // Check selector existence, related to bugs http://crbug.com/327532 and
  // http://crbug.com/328096.
  // CMTimeMake accepts integer argumenst but |frameRate| is float, round it.
  if ([captureConnection respondsToSelector:@selector(isVideoMinFrameDurationSupported)] &&
      [captureConnection isVideoMinFrameDurationSupported]) {
    [captureConnection
        setVideoMinFrameDuration:CMTimeMake(felicia::kFrameRatePrecision,
                                            (int)(frameRate * felicia::kFrameRatePrecision))];
  }
  if ([captureConnection respondsToSelector:@selector(isVideoMaxFrameDurationSupported)] &&
      [captureConnection isVideoMaxFrameDurationSupported]) {
    [captureConnection
        setVideoMaxFrameDuration:CMTimeMake(felicia::kFrameRatePrecision,
                                            (int)(frameRate * felicia::kFrameRatePrecision))];
  }
  return YES;
}

- (BOOL)startCapture {
  DCHECK(main_thread_checker_.CalledOnValidThread());
  if (!captureSession_) {
    DLOG(ERROR) << "Video capture session not initialized.";
    return NO;
  }
  // Connect the notifications.
  NSNotificationCenter* nc = [NSNotificationCenter defaultCenter];
  [nc addObserver:self
         selector:@selector(onVideoError:)
             name:AVCaptureSessionRuntimeErrorNotification
           object:captureSession_];
  [captureSession_ startRunning];
  return YES;
}

- (void)stopCapture {
  DCHECK(main_thread_checker_.CalledOnValidThread());
  if ([captureSession_ isRunning]) [captureSession_ stopRunning];  // Synchronous.
  [[NSNotificationCenter defaultCenter] removeObserver:self];
}

#pragma mark Private methods

// |captureOutput| is called by the capture device to deliver a new frame.
// AVFoundation calls from a number of threads, depending on, at least, if
// Chrome is on foreground or background.
- (void)captureOutput:(AVCaptureOutput*)captureOutput
    didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer
           fromConnection:(AVCaptureConnection*)connection {
  const CMFormatDescriptionRef formatDescription = CMSampleBufferGetFormatDescription(sampleBuffer);
  const FourCharCode fourcc = CMFormatDescriptionGetMediaSubType(formatDescription);
  const CMVideoDimensions dimensions = CMVideoFormatDescriptionGetDimensions(formatDescription);
  const felicia::CameraFormat cameraFormat(
      felicia::Sizei(static_cast<int>(dimensions.width), static_cast<int>(dimensions.height)),
      felicia::CameraFormat::FromAVFoundationPixelFormat(fourcc), frameRate_);

  // We have certain format expectation for capture output:
  // For MJPEG, |sampleBuffer| is expected to always be a CVBlockBuffer.
  // For other formats, |sampleBuffer| may be either CVBlockBuffer or
  // CVImageBuffer. CVBlockBuffer seems to be used in the context of CoreMedia
  // plugins/virtual cameras. In order to find out whether it is CVBlockBuffer
  // or CVImageBuffer we call CMSampleBufferGetImageBuffer() and check if the
  // return value is nil.
  char* baseAddress = 0;
  size_t frameSize = 0;
  CVImageBufferRef videoFrame = nil;
  if (fourcc != kCMVideoCodecType_JPEG_OpenDML) {
    videoFrame = CMSampleBufferGetImageBuffer(sampleBuffer);
    // Lock the frame and calculate frame size.
    if (videoFrame &&
        CVPixelBufferLockBaseAddress(videoFrame, kCVPixelBufferLock_ReadOnly) == kCVReturnSuccess) {
      baseAddress = static_cast<char*>(CVPixelBufferGetBaseAddress(videoFrame));
      frameSize = CVPixelBufferGetHeight(videoFrame) * CVPixelBufferGetBytesPerRow(videoFrame);
    } else {
      videoFrame = nil;
    }
  }
  if (!videoFrame) {
    ExtractBaseAddressAndLength(&baseAddress, &frameSize, sampleBuffer);
  }

  {
    base::AutoLock lock(lock_);
    const CMTime cm_timestamp = CMSampleBufferGetPresentationTimeStamp(sampleBuffer);
    const base::TimeDelta timestamp =
        CMTIME_IS_VALID(cm_timestamp)
            ? base::TimeDelta::FromMicroseconds(cm_timestamp.value *
                                                base::TimeTicks::kMicrosecondsPerSecond /
                                                cm_timestamp.timescale)
            : felicia::kNoTimestamp;

    if (frameReceiver_ && baseAddress) {
      frameReceiver_->ReceiveFrame(reinterpret_cast<uint8_t*>(baseAddress), frameSize, cameraFormat,
                                   0, 0, timestamp);
    }
  }

  if (videoFrame) CVPixelBufferUnlockBaseAddress(videoFrame, kCVPixelBufferLock_ReadOnly);
}

- (void)onVideoError:(NSNotification*)errorNotification {
  NSError* error = base::mac::ObjCCast<NSError>(
      [[errorNotification userInfo] objectForKey:AVCaptureSessionErrorKey]);
  [self sendErrorString:[NSString stringWithFormat:@"%@: %@", [error localizedDescription],
                                                   [error localizedFailureReason]]];
}

- (void)sendErrorString:(NSString*)error {
  DLOG(ERROR) << base::SysNSStringToUTF8(error);
  base::AutoLock lock(lock_);
  if (frameReceiver_)
    frameReceiver_->ReceiveError(
        felicia::Status(felicia::error::Code::UNKNOWN, base::SysNSStringToUTF8(error)));
}

@end

#pragma clang diagnostic pop