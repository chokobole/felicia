// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import CameraFrameMessage, {
  CAMERA_FRAME_MESSAGE,
} from '@felicia-viz/proto/messages/camera-frame-message';
import { PixelFormat } from '@felicia-viz/proto/messages/ui';
// @ts-ignore
import { Form } from '@streetscape.gl/monochrome';
import React, { Component } from 'react';
import { FORM_STYLE } from '../custom-styles';
import { renderText } from './common/panel-item';
import TopicDropdown, { Props as TopicDropdownProps } from './common/topic-dropdown';

export default class CameraFrameControlPanel extends Component<{
  frame: CameraFrameMessage | null;
}> {
  private SETTINGS = {
    header: { type: 'header', title: 'CameraFrame Control' },
    sectionSeperator: { type: 'separator' },
    info: {
      type: 'header',
      title: 'Info',
      children: {
        width: { type: 'custom', title: 'width', render: renderText },
        height: { type: 'custom', title: 'height', render: renderText },
        frameRate: { type: 'custom', title: 'frameRate', render: renderText },
        pixelFormat: { type: 'custom', title: 'pixelFormat', render: renderText },
        timestamp: { type: 'custom', title: 'timestamp', render: renderText },
      },
    },
    control: {
      type: 'header',
      title: 'Control',
      children: {
        topic: {
          type: 'custom',
          title: 'topic',
          render: (self: TopicDropdownProps): JSX.Element => {
            return <TopicDropdown {...self} value={[CAMERA_FRAME_MESSAGE]} />;
          },
        },
      },
    },
  };

  private _onChange = (): void => {};

  private _fetchValues(): {
    width: string;
    height: string;
    frameRate: string;
    pixelFormat: string;
    timestamp: string;
  } {
    const { frame } = this.props;
    if (frame) {
      const { cameraFormat, timestamp } = frame;
      const { size, pixelFormat, frameRate } = cameraFormat;
      const { width, height } = size;
      return {
        width: width.toString(),
        height: height.toString(),
        frameRate: frameRate.toString(),
        pixelFormat: PixelFormat.valuesById[pixelFormat],
        timestamp: timestamp.toString(),
      };
    }
    return {
      width: '',
      height: '',
      frameRate: '',
      pixelFormat: '',
      timestamp: '',
    };
  }

  render(): JSX.Element {
    return (
      <Form
        data={this.SETTINGS}
        values={this._fetchValues()}
        style={FORM_STYLE}
        onChange={this._onChange}
      />
    );
  }
}
