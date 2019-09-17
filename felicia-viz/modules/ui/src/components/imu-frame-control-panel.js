import { inject, observer } from 'mobx-react';
import PropTypes from 'prop-types';
import React, { Component } from 'react';
import { Form, MetricCard, MetricChart } from '@streetscape.gl/monochrome';

import { IMU_FRAME_MESSAGE } from '@felicia-viz/proto/messages/imu-frame-message';

import { FORM_STYLE, METRIC_CARD_STYLE } from '../custom-styles';
import { FeliciaVizStore } from '../store';
import { PanelItemContainer, renderText } from './common/panel-item';
import TopicDropdown from './common/topic-dropdown';

function renderImuGraph(self) {
  const { title, value } = self;
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
export default class ImuFrameControlPanel extends Component {
  static propTypes = {
    store: PropTypes.instanceOf(FeliciaVizStore).isRequired,
  };

  SETTINGS = {
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
          render: self => {
            return <TopicDropdown {...self} typeNames={[IMU_FRAME_MESSAGE]} />;
          },
        },
      },
    },
  };

  _onChange = values => {}; // eslint-disable-line no-unused-vars

  _fetchValues() {
    const { store } = this.props;
    const viewState = store.uiState.getActiveViewState();
    const { frame, angularVelocities, linearAccelerations } = viewState;

    if (frame) {
      const { timestamp } = frame;
      return {
        angularVelocity: angularVelocities.history(),
        linearAcceleration: linearAccelerations.history(),
        timestamp,
      };
    }

    return {
      angularVelocity: angularVelocities.history(),
      linearAcceleration: linearAccelerations.history(),
      timestamp: '',
    };
  }

  render() {
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
