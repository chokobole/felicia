import { IMU_FRAME_MESSAGE } from '@felicia-viz/proto/messages/imu-frame-message';
// @ts-ignore
import { Form, MetricCard, MetricChart } from '@streetscape.gl/monochrome';
import { inject, observer } from 'mobx-react';
import React, { Component } from 'react';
import { FORM_STYLE, METRIC_CARD_STYLE } from '../custom-styles';
import { FeliciaVizStore } from '../store';
import ImuFrameViewState, { Data } from '../store/ui/imu-frame-view-state';
import { PanelItemContainer, renderText, FormProps } from './common/panel-item';
import TopicDropdown, { Props as TopicDropdownProps } from './common/topic-dropdown';

function renderImuGraph({ title, value }: FormProps<Data>): JSX.Element {
  return (
    <PanelItemContainer>
      <MetricCard title={title} className='metric-container' style={METRIC_CARD_STYLE}>
        <MetricChart
          data={value}
          height={200}
          xTicks={0}
          getColor={{
            x: 'red',
            y: 'green',
            z: 'blue',
          }}
        />
      </MetricCard>
    </PanelItemContainer>
  );
}

@inject('store')
@observer
export default class ImuFrameControlPanel extends Component<{
  store?: FeliciaVizStore;
}> {
  private SETTINGS = {
    header: { type: 'header', title: 'ImuFrame Control' },
    sectionSeperator: { type: 'separator' },
    info: {
      type: 'header',
      title: 'Info',
      children: {
        angularVelocity: { type: 'custom', title: 'angularVelocity', render: renderImuGraph },
        linearAcceleration: { type: 'custom', title: 'linearAcceleration', render: renderImuGraph },
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
            return <TopicDropdown {...self} value={[IMU_FRAME_MESSAGE]} />;
          },
        },
      },
    },
  };

  private _onChange = (): void => {};

  private _fetchValues(): {
    angularVelocity: Data;
    linearAcceleration: Data;
    timestamp: string;
  } {
    const { store } = this.props;
    if (store) {
      const viewState = store.uiState.getActiveViewState() as ImuFrameViewState;
      const { frame, angularVelocities, linearAccelerations } = viewState;

      if (frame) {
        const { timestamp } = frame;
        return {
          angularVelocity: angularVelocities.history(),
          linearAcceleration: linearAccelerations.history(),
          timestamp: timestamp.toString(),
        };
      }
    }

    return {
      angularVelocity: { x: [], y: [], z: [] },
      linearAcceleration: { x: [], y: [], z: [] },
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
