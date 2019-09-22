import { CAMERA_FRAME_MESSAGE } from '@felicia-viz/proto/messages/camera-frame-message';
import { DEPTH_CAMERA_FRAME_MESSAGE } from '@felicia-viz/proto/messages/depth-camera-frame-message';
import { PixelFormat } from '@felicia-viz/proto/messages/ui';
// @ts-ignore
import { Form } from '@streetscape.gl/monochrome';
import { inject, observer } from 'mobx-react';
import React, { Component } from 'react';
import { FORM_STYLE } from '../custom-styles';
import { FeliciaVizStore } from '../store';
import DepthCameraFrameViewState from '../store/ui/depth-camera-frame-view-state';
import ColormapDropdown, { Props as ColormapDropdownProps } from './colormap-dropdown';
import { renderText } from './common/panel-item';
import TopicDropdown, { Props as TopicDropdownProps } from './common/topic-dropdown';

@inject('store')
@observer
export default class DepthCameraFrameControlPanel extends Component<{
  store?: FeliciaVizStore;
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
          render: (self: ColormapDropdownProps): JSX.Element => {
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
    const { store } = this.props;
    if (store) {
      const viewState = store.uiState.getActiveViewState();
      const { frame, filter } = viewState as DepthCameraFrameViewState;

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
          filter,
        };
      }
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
