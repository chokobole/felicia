import { inject, observer } from 'mobx-react';
import PropTypes from 'prop-types';
import React, { Component } from 'react';
import { Form } from '@streetscape.gl/monochrome';

import { LIDAR_FRAME_MESSAGE } from '@felicia-viz/proto/messages/lidar-frame-message';
import { TopicDropdown, renderText } from '@felicia-viz/ui';

import { FORM_STYLE } from 'custom-styles';

@inject('store')
@observer
export default class LidarControlPanel extends Component {
  static propTypes = {
    store: PropTypes.object.isRequired,
  };

  SETTINGS = {
    header: { type: 'header', title: 'Lidar Control' },
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
          render: self => {
            return <TopicDropdown {...self} typeName={LIDAR_FRAME_MESSAGE} />;
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
        angleStart,
        angleEnd,
        angleDelta,
        timeDelta,
        scanTime,
        rangeMin,
        rangeMax,
        timestamp,
      };
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
