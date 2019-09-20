import { Colors, Points } from '@felicia-viz/proto/messages/data-message';
import { PointcloudMessage, POINTCLOUD_MESSAGE } from '@felicia-viz/proto/messages/map-message';
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
export default class PointcloudControlPanel extends Component<{
  store?: FeliciaVizStore;
}> {
  private SETTINGS = {
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
          render: (self: TopicDropdownProps): JSX.Element => {
            return <TopicDropdown {...self} typeNames={[POINTCLOUD_MESSAGE]} />;
          },
        },
      },
    },
  };

  private _onChange = (): void => {};

  private _fetchValues(): {
    points: string;
    colors: string;
    timestamp: string;
  } {
    const { store } = this.props;
    if (store) {
      const viewState = store.uiState.getActiveViewState() as any;
      const { map } = viewState;

      if (map && map instanceof PointcloudMessage) {
        const { points, colors, timestamp } = map;
        return {
          points: new Points(points).length().toString(),
          colors: new Colors(colors).length().toString(),
          timestamp: timestamp.toString(),
        };
      }
    }
    return {
      points: '',
      colors: '',
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
