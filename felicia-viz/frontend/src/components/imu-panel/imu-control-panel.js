import React, { Component } from 'react';
import PropTypes from 'prop-types';
import { inject, observer } from 'mobx-react';
import { Form, MetricCard, MetricChart } from '@streetscape.gl/monochrome';

import { IMU_FRAME_MESSAGE } from '@felicia-viz/communication';
import { TopicDropdown, PanelItemContainer, renderText } from '@felicia-viz/ui';

import { FORM_STYLE, METRIC_CARD_STYLE } from 'custom-styles';

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

class History {
  constructor(size) {
    this.size = size;
    this.data = {
      x: [{ x: 0, y: 0 }],
      y: [{ x: 0, y: 0 }],
      z: [{ x: 0, y: 0 }],
    };
  }

  push(vec, timestamp) {
    const { x, y, z } = vec;

    if (x === 0 && y === 0 && z === 0) return;

    if (this.data.x.length === this.size) {
      this.data.x.shift();
      this.data.y.shift();
      this.data.z.shift();
    }

    this.data.x.push({ x: timestamp, y: x });
    this.data.y.push({ x: timestamp, y });
    this.data.z.push({ x: timestamp, y: z });
  }

  history() {
    const { x, y, z } = this.data;
    return {
      x,
      y,
      z,
    };
  }
}

@inject('store')
@observer
export default class ImuControlPanel extends Component {
  static propTypes = {
    store: PropTypes.object.isRequired,
  };

  angularVelocities = new History(100);

  linearAccelerations = new History(100);

  SETTINGS = {
    header: { type: 'header', title: 'Imu Control' },
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
            return <TopicDropdown {...self} typeName={IMU_FRAME_MESSAGE} />;
          },
        },
      },
    },
  };

  _onChange = values => {}; // eslint-disable-line no-unused-vars

  _fetchValues() {
    const { store } = this.props;
    const viewState = store.uiState.activeViewState.getState();
    const { frame } = viewState;

    if (frame) {
      const { angularVelocity, linearAcceleration, timestamp } = frame;
      this.angularVelocities.push(angularVelocity, timestamp);
      this.linearAccelerations.push(linearAcceleration, timestamp);
    }

    return {
      angularVelocity: this.angularVelocities.history(),
      linearAcceleration: this.linearAccelerations.history(),
      timestamp: frame ? frame.timestamp : '',
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
