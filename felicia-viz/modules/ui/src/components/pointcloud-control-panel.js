import { inject, observer } from 'mobx-react';
import PropTypes from 'prop-types';
import React, { Component } from 'react';
import { Form } from '@streetscape.gl/monochrome';

import { Colors, Points } from '@felicia-viz/proto/messages/data-message';
import { POINTCLOUD_MESSAGE, PointcloudMessage } from '@felicia-viz/proto/messages/map-message';

import { FORM_STYLE } from '../custom-styles';
import { FeliciaVizStore } from '../store';
import TopicDropdown from './common/topic-dropdown';
import { renderText } from './common/panel-item';

@inject('store')
@observer
export default class PointcloudControlPanel extends Component {
  static propTypes = {
    store: PropTypes.instanceOf(FeliciaVizStore).isRequired,
  };

  SETTINGS = {
    header: { type: 'header', title: 'Pointcloud Control' },
    sectionSeperator: { type: 'separator' },
    info: {
      type: 'header',
      title: 'Info',
      children: {
        points: { type: 'custom', title: 'points', render: renderText },
        colors: { type: 'custom', title: 'colors', render: renderText },
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
            return <TopicDropdown {...self} typeNames={[POINTCLOUD_MESSAGE]} />;
          },
        },
      },
    },
  };

  _onChange = values => {}; // eslint-disable-line no-unused-vars

  _fetchValues() {
    const { store } = this.props;
    const viewState = store.uiState.getActiveViewState();
    const { map } = viewState;

    if (map && map instanceof PointcloudMessage) {
      const { points, colors, timestamp } = map;
      return {
        points: new Points(points).length(),
        colors: new Colors(colors).length(),
        timestamp,
      };
    }
    return {
      points: '',
      colors: '',
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
