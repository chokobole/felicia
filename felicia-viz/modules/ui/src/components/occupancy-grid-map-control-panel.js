import { inject, observer } from 'mobx-react';
import PropTypes from 'prop-types';
import React, { Component } from 'react';
import { Form } from '@streetscape.gl/monochrome';

import {
  OCCUPANCY_GRID_MAP_MESSAGE,
  OccupancyGridMapMessage,
} from '@felicia-viz/proto/messages/map-message';

import { FORM_STYLE } from '../custom-styles';
import { FeliciaVizStore } from '../store';
import TopicDropdown from './common/topic-dropdown';
import { renderText } from './common/panel-item';

@inject('store')
@observer
export default class OccupancyGridMapControlPanel extends Component {
  static propTypes = {
    store: PropTypes.instanceOf(FeliciaVizStore).isRequired,
  };

  SETTINGS = {
    header: { type: 'header', title: 'OccupancyGridMap Control' },
    sectionSeperator: { type: 'separator' },
    info: {
      type: 'header',
      title: 'Info',
      children: {
        size: { type: 'custom', title: 'ize', render: renderText },
        resolution: { type: 'custom', title: 'resolution', render: renderText },
        origin: { type: 'custom', title: 'origin', render: renderText },
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
            return <TopicDropdown {...self} typeNames={[OCCUPANCY_GRID_MAP_MESSAGE]} />;
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

    if (map && map instanceof OccupancyGridMapMessage) {
      const { size, resolution, origin, timestamp } = map;
      return {
        size: size.toShortString(),
        resolution,
        origin: origin.toShortString(),
        timestamp,
      };
    }
    return {
      size: '',
      resolution: '',
      origin: '',
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
