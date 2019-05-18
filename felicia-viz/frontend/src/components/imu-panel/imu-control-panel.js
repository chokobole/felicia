import React, { Component } from 'react';
import PropTypes from 'prop-types';
import { inject, observer } from 'mobx-react';
import { Form } from '@streetscape.gl/monochrome';

import TopicDropdown from 'components/common/topic-dropdown';

@inject('store')
@observer
export default class ImuControlPanel extends Component {
  static propTypes = {
    store: PropTypes.object.isRequired,
  };

  SETTINGS = {
    userHeader: { type: 'header', title: 'Imu Control' },
    sectionSeperator: { type: 'separator' },
    cameraInfo: {
      type: 'header',
      title: 'Info',
      children: {},
    },
    caemraControl: {
      type: 'header',
      title: 'Control',
      children: {
        topic: {
          type: 'custom',
          title: 'topic',
          render: self => {
            return <TopicDropdown {...self} typeName='' />;
          },
        },
      },
    },
  };

  _onChange = values => {}; // eslint-disable-line no-unused-vars

  _fetchValues() {
    const { store } = this.props;
    const { uiState } = store;
    const viewState = uiState.findView(uiState.activeViewState.id);
    const { topic } = viewState;

    return {
      topic,
    };
  }

  render() {
    return (
      <Form
        data={this.SETTINGS}
        values={this._fetchValues()}
        style={{
          wrapper: {
            padding: 12,
          },
        }}
        onChange={this._onChange}
      />
    );
  }
}
