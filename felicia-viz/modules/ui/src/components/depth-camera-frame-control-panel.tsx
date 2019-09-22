import { CAMERA_FRAME_MESSAGE } from '@felicia-viz/proto/messages/camera-frame-message';
import DepthCameraFrameMessage, {
  DEPTH_CAMERA_FRAME_MESSAGE,
} from '@felicia-viz/proto/messages/depth-camera-frame-message';
import { PixelFormat } from '@felicia-viz/proto/messages/ui';
// @ts-ignore
import { Form } from '@streetscape.gl/monochrome';
import React, { Component } from 'react';
import { FORM_STYLE } from '../custom-styles';
import ColormapDropdown from './colormap-dropdown';
import { FormProps, renderText } from './common/panel-item';
import TopicDropdown, { Props as TopicDropdownProps } from './common/topic-dropdown';

export default class DepthCameraFrameControlPanel extends Component<{
  frame: DepthCameraFrameMessage | null;
  filter: string | null;
}> {
  private SETTINGS = {
    header: { type: 'header', title: 'DepthCameraFrame Control' },
    sectionSeperator: { type: 'separator' },
    info: {
      type: 'header',
      title: 'Info',
      children: {
        width: { type: 'custom', title: 'width', render: renderText },
        height: { type: 'custom', title: 'height', render: renderText },
        frameRate: { type: 'custom', title: 'frameRate', render: renderText },
        pixelFormat: { type: 'custom', title: 'pixelFormat', render: renderText },
        min: { type: 'custom', title: 'min', render: renderText },
        max: { type: 'custom', title: 'max', render: renderText },
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
            return <TopicDropdown {...self} value={[DEPTH_CAMERA_FRAME_MESSAGE]} />;
          },
        },
        filter: {
          type: 'custom',
          title: 'filter',
          render: (self: FormProps<string>): JSX.Element => {
            return <ColormapDropdown {...self} />;
          },
        },
        topicToAlign: {
          type: 'custom',
          title: 'topicToAlign',
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
    min: string;
    max: string;
    timestamp: string;
    filter: string;
  } {
    const { frame, filter } = this.props;
    if (frame) {
      const { cameraFormat, min, max, timestamp } = frame;
      const { size, pixelFormat, frameRate } = cameraFormat;
      const { width, height } = size;
      return {
        width: width.toString(),
        height: height.toString(),
        frameRate: frameRate.toString(),
        pixelFormat: PixelFormat.valuesById[pixelFormat],
        min: min.toString(),
        max: max.toString(),
        timestamp: timestamp.toString(),
        filter: filter!,
      };
    }
    return {
      width: '',
      height: '',
      frameRate: '',
      pixelFormat: '',
      min: '',
      max: '',
      timestamp: '',
      filter: '',
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
