import {
  POSE3D_WITH_TIMESTAMP_MESSAGE,
  POSE3F_WITH_TIMESTAMP_MESSAGE,
  Pose3WithTimestampMessage,
} from '@felicia-viz/proto/messages/geometry';
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
export default class Pose3WithTimestampControlPanel extends Component<{
  store?: FeliciaVizStore;
}> {
  private SETTINGS = {
    header: { type: 'header', title: 'Pose3WithTimestamp Control' },
    sectionSeperator: { type: 'separator' },
    info: {
      type: 'header',
      title: 'Info',
      children: {
        position: { type: 'custom', title: 'position', render: renderText },
        orientation: { type: 'custom', title: 'orientation', render: renderText },
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
            return (
              <TopicDropdown
                {...self}
                typeNames={[POSE3F_WITH_TIMESTAMP_MESSAGE, POSE3D_WITH_TIMESTAMP_MESSAGE]}
              />
            );
          },
        },
      },
    },
  };

  private _onChange = (): void => {};

  private _fetchValues(): {
    position: string;
    orientation: string;
    timestamp: string;
  } {
    const { store } = this.props;
    if (store) {
      const viewState = store.uiState.getActiveViewState() as any;
      const { pose } = viewState;

      if (pose && pose instanceof Pose3WithTimestampMessage) {
        const { position, orientation, timestamp } = pose;
        return {
          position: position.toShortString(),
          orientation: orientation.toShortString(),
          timestamp: timestamp.toString(),
        };
      }
    }
    return {
      position: '',
      orientation: '',
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
