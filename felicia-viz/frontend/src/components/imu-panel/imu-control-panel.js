import React, { Component } from 'react';
import PropTypes from 'prop-types';
import { inject, observer } from 'mobx-react';
import { Form, MetricCard, MetricChart } from '@streetscape.gl/monochrome';

import { IMU_MESSAGE } from '@felicia-viz/communication';
import { TopicDropdown, PanelItemContainer } from '@felicia-viz/ui';

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

class ImuHistory {
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

  angularVelocities = new ImuHistory(100);

  linearAccelerations = new ImuHistory(100);

  SETTINGS = {
    userHeader: { type: 'header', title: 'Imu Control' },
    sectionSeperator: { type: 'separator' },
    cameraInfo: {
      type: 'header',
      title: 'Info',
      children: {
        angularVelocity: { type: 'custom', title: 'angularVelocity', render: renderImuGraph },
        linearAcceleration: { type: 'custom', title: 'linearAcceleration', render: renderImuGraph },
      },
    },
    caemraControl: {
      type: 'header',
      title: 'Control',
      children: {
        topic: {
          type: 'custom',
          title: 'topic',
          render: self => {
            return <TopicDropdown {...self} typeName={IMU_MESSAGE} />;
          },
        },
      },
    },
  };

  _onChange = values => {}; // eslint-disable-line no-unused-vars

  _fetchValues() {
    const { store } = this.props;
    const viewState = store.uiState.activeViewState.getState();
    const { topic, imu } = viewState;

    if (imu) {
      const { angularVelocity, linearAcceleration, timestamp } = imu;
      this.angularVelocities.push(angularVelocity, timestamp);
      this.linearAccelerations.push(linearAcceleration, timestamp);
    }

    return {
      topic,
      angularVelocity: this.angularVelocities.history(),
      linearAcceleration: this.linearAccelerations.history(),
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
