import {
  POSED_WITH_TIMESTAMP_MESSAGE,
  POSEF_WITH_TIMESTAMP_MESSAGE,
  PoseWithTimestampMessage,
} from '@felicia-viz/proto/messages/geometry';
// @ts-ignore
import { Form } from '@streetscape.gl/monochrome';
import React, { Component } from 'react';
import { FORM_STYLE } from '../custom-styles';
import { renderText } from './common/panel-item';
import TopicDropdown, { Props as TopicDropdownProps } from './common/topic-dropdown';

export default class PoseWithTimestampControlPanel extends Component<{
  pose?: PoseWithTimestampMessage | null;
}> {
  private SETTINGS = {
    header: { type: 'header', title: 'PoseWithTimestamp Control' },
    sectionSeperator: { type: 'separator' },
    info: {
      type: 'header',
      title: 'Info',
      children: {
        position: { type: 'custom', title: 'position', render: renderText },
        theta: { type: 'custom', title: 'theta', render: renderText },
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
                value={[POSEF_WITH_TIMESTAMP_MESSAGE, POSED_WITH_TIMESTAMP_MESSAGE]}
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
    theta: string;
    timestamp: string;
  } {
    const { pose } = this.props;
    if (pose) {
      const { position, theta, timestamp } = pose;
      return {
        position: position.toShortString(),
        theta: theta.toString(),
        timestamp: timestamp.toString(),
      };
    }
    return {
      position: '',
      theta: '',
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
