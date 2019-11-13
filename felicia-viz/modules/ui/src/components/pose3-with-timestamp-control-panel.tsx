// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import {
  POSE3D_WITH_TIMESTAMP_MESSAGE,
  POSE3F_WITH_TIMESTAMP_MESSAGE,
  Pose3WithTimestampMessage,
} from '@felicia-viz/proto/messages/geometry';
// @ts-ignore
import { Form } from '@streetscape.gl/monochrome';
import React, { Component } from 'react';
import { FORM_STYLE } from '../custom-styles';
import { renderText } from './common/panel-item';
import TopicDropdown, { Props as TopicDropdownProps } from './common/topic-dropdown';

export default class Pose3WithTimestampControlPanel extends Component<{
  pose: Pose3WithTimestampMessage | null;
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
                value={[POSE3F_WITH_TIMESTAMP_MESSAGE, POSE3D_WITH_TIMESTAMP_MESSAGE]}
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
    const { pose } = this.props;
    if (pose) {
      const { position, orientation, timestamp } = pose;
      return {
        position: position.toShortString(),
        orientation: orientation.toShortString(),
        timestamp: timestamp.toString(),
      };
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
