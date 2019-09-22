import { CAMERA_FRAME_MESSAGE } from '@felicia-viz/proto/messages/camera-frame-message';
import { PixelFormat } from '@felicia-viz/proto/messages/ui';
// @ts-ignore
import { Form } from '@streetscape.gl/monochrome';
import { inject, observer } from 'mobx-react';
import React, { Component } from 'react';
import { FORM_STYLE } from '../custom-styles';
import { FeliciaVizStore } from '../store';
import CameraFrmaeViewState from '../store/ui/camera-frame-view-state';
import { renderText } from './common/panel-item';
import TopicDropdown, { Props as TopicDropdownProps } from './common/topic-dropdown';

@inject('store')
@observer
export default class CameraFrameControlPanel extends Component<{
  store?: FeliciaVizStore;
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
    const { store } = this.props;
    if (store) {
      const viewState = store.uiState.getActiveViewState();
      const { frame } = viewState as CameraFrmaeViewState;

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
