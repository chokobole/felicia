import {
  OccupancyGridMapMessage,
  OCCUPANCY_GRID_MAP_MESSAGE,
} from '@felicia-viz/proto/messages/map-message';
// @ts-ignore
import { Form } from '@streetscape.gl/monochrome';
import { inject, observer } from 'mobx-react';
import React, { Component } from 'react';
import { FORM_STYLE } from '../custom-styles';
import { FeliciaVizStore } from '../store';
import { renderText } from './common/panel-item';
import TopicDropdown, { Props as TopicDropdownProps } from './common/topic-dropdown';

@inject('store')
@observer
export default class OccupancyGridMapControlPanel extends Component<{
  store?: FeliciaVizStore;
}> {
  private SETTINGS = {
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
          render: (self: TopicDropdownProps): JSX.Element => {
            return <TopicDropdown {...self} value={[OCCUPANCY_GRID_MAP_MESSAGE]} />;
          },
        },
        enabled: {
          type: 'toggle',
          title: 'enabled',
        },
      },
    },
  };

  private _onChange = (values: { enabled: boolean }): void => {
    const { store } = this.props;
    if (!store) return;
    const viewState = store.uiState.getActiveViewState() as any;
    const topic = viewState.getTopic(OCCUPANCY_GRID_MAP_MESSAGE);
    if (topic && !values.enabled) {
      viewState.unsetTopic(OCCUPANCY_GRID_MAP_MESSAGE, topic);
    }
  };

  private _fetchValues(): {
    size: string;
    resolution: string;
    origin: string;
    timestamp: string;
    enabled: boolean;
  } {
    let enabled = false;
    const { store } = this.props;
    if (store) {
      const viewState = store.uiState.getActiveViewState() as any;
      enabled = viewState.hasTopic(OCCUPANCY_GRID_MAP_MESSAGE);
      const { map } = viewState;

      if (map && map instanceof OccupancyGridMapMessage) {
        const { size, resolution, origin, timestamp } = map;
        return {
          size: size.toShortString(),
          resolution: resolution.toString(),
          origin: origin.toShortString(),
          timestamp: timestamp.toString(),
          enabled,
        };
      }
    }
    return {
      size: '',
      resolution: '',
      origin: '',
      timestamp: '',
      enabled,
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
