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
            return <TopicDropdown {...self} typeNames={[OCCUPANCY_GRID_MAP_MESSAGE]} />;
          },
        },
      },
    },
  };

  private _onChange = (): void => {};

  private _fetchValues(): {
    size: string;
    resolution: string;
    origin: string;
    timestamp: string;
  } {
    const { store } = this.props;
    if (store) {
      const viewState = store.uiState.getActiveViewState() as any;
      const { map } = viewState;

      if (map && map instanceof OccupancyGridMapMessage) {
        const { size, resolution, origin, timestamp } = map;
        return {
          size: size.toShortString(),
          resolution: resolution.toString(),
          origin: origin.toShortString(),
          timestamp: timestamp.toString(),
        };
      }
    }
    return {
      size: '',
      resolution: '',
      origin: '',
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
