import { LIDAR_FRAME_MESSAGE } from '@felicia-viz/proto/messages/lidar-frame-message';
// @ts-ignore
import { Form } from '@streetscape.gl/monochrome';
import { inject, observer } from 'mobx-react';
import React, { Component } from 'react';
import { FORM_STYLE } from '../custom-styles';
import { FeliciaVizStore } from '../store';
import LidarFrameViewState from '../store/ui/lidar-frame-view-state';
import { renderText } from './common/panel-item';
import TopicDropdown, { Props as TopicDropdownProps } from './common/topic-dropdown';

@inject('store')
@observer
export default class LidarFrameControlPanel extends Component<{
  store?: FeliciaVizStore;
}> {
  private SETTINGS = {
    header: { type: 'header', title: 'LidarFrame Control' },
    sectionSeperator: { type: 'separator' },
    info: {
      type: 'header',
      title: 'Info',
      children: {
        angleStart: { type: 'custom', title: 'angleStart', render: renderText },
        angleEnd: { type: 'custom', title: 'angleEnd', render: renderText },
        angleDelta: { type: 'custom', title: 'angleDelta', render: renderText },
        timeDelta: { type: 'custom', title: 'timeDelta', render: renderText },
        scanTime: { type: 'custom', title: 'scanTime', render: renderText },
        rangeMin: { type: 'custom', title: 'rangeMin', render: renderText },
        rangeMax: { type: 'custom', title: 'rangeMax', render: renderText },
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
            return <TopicDropdown {...self} typeNames={[LIDAR_FRAME_MESSAGE]} />;
          },
        },
      },
    },
  };

  private _onChange = (): void => {};

  _fetchValues(): {
    angleStart: string;
    angleEnd: string;
    angleDelta: string;
    timeDelta: string;
    scanTime: string;
    rangeMin: string;
    rangeMax: string;
    timestamp: string;
  } {
    const { store } = this.props;
    if (store) {
      const viewState = store.uiState.getActiveViewState() as LidarFrameViewState;
      const { frame } = viewState;

      if (frame) {
        const {
          angleStart,
          angleEnd,
          angleDelta,
          timeDelta,
          scanTime,
          rangeMin,
          rangeMax,
          timestamp,
        } = frame;
        return {
          angleStart: angleStart.toString(),
          angleEnd: angleEnd.toString(),
          angleDelta: angleDelta.toString(),
          timeDelta: timeDelta.toString(),
          scanTime: scanTime.toString(),
          rangeMin: rangeMin.toString(),
          rangeMax: rangeMax.toString(),
          timestamp: timestamp.toString(),
        };
      }
    }
    return {
      angleStart: '',
      angleEnd: '',
      angleDelta: '',
      timeDelta: '',
      scanTime: '',
      rangeMin: '',
      rangeMax: '',
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
