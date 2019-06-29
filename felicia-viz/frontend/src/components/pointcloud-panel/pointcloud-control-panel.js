import React, { Component } from 'react';
import PropTypes from 'prop-types';
import { inject, observer } from 'mobx-react';
import { Form } from '@streetscape.gl/monochrome';

import { POINTCLOUD_FRAME_MESSAGE } from '@felicia-viz/communication';
import { TopicDropdown, renderText } from '@felicia-viz/ui';

import { FORM_STYLE } from 'custom-styles';

@inject('store')
@observer
export default class PointcloudControlPanel extends Component {
  static propTypes = {
    store: PropTypes.object.isRequired,
  };

  SETTINGS = {
    header: { type: 'header', title: 'Pointcloud Control' },
    sectionSeperator: { type: 'separator' },
    info: {
      type: 'header',
      title: 'Info',
      children: {
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
            return <TopicDropdown {...self} typeName={POINTCLOUD_FRAME_MESSAGE} />;
          },
        },
      },
    },
  };

  _onChange = values => {}; // eslint-disable-line no-unused-vars

  _fetchValues() {
    const { store } = this.props;
    const viewState = store.uiState.activeViewState.getState();
    const { topic, frame } = viewState;

    if (frame) {
      const { timestamp } = frame;
      return {
        timestamp,
        topic,
      };
    }
    return {
      timestamp: '',
      topic,
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
