import { toJS } from 'mobx';
import { inject, observer } from 'mobx-react';
import PropTypes from 'prop-types';
import React, { Component } from 'react';
import { Dropdown, Label } from '@streetscape.gl/monochrome';

import { findWSChannel, hasWSChannel } from '@felicia-viz/proto/messages/master-data';

import { FeliciaVizStore } from '../../store';
import { PanelItemContainer } from './panel-item';

@inject('store')
@observer
export default class TopicDropdown extends Component {
  static propTypes = {
    title: PropTypes.string.isRequired,
    typeNames: PropTypes.arrayOf(PropTypes.string).isRequired,
    store: PropTypes.instanceOf(FeliciaVizStore).isRequired,
    isEnabled: PropTypes.bool.isRequired,
  };

  _onTopicChange = value => {
    const { store } = this.props;
    const topics = toJS(store.topicInfo.topics);
    let found = topics.find(v => {
      return v.topic === value;
    });
    if (!found) {
      console.error(`Failed to find topic ${value}`);
      return;
    }

    const { typeName } = found;
    found = findWSChannel(found);

    if (found) {
      const viewState = store.uiState.getActiveViewState();
      viewState.setTopic(typeName, value, found.ipEndpoint);
    } else {
      console.error(`Failed to find ip endpoint for ${value}`);
    }
  };

  render() {
    const { title, typeNames, store, isEnabled } = this.props;
    const viewState = store.uiState.getActiveViewState();

    let data = {};
    let value = '';
    const topics = toJS(store.topicInfo.topics);
    // eslint-disable-next-line no-restricted-syntax
    for (const typeName of typeNames) {
      if (viewState.topics.has(typeName)) {
        value = viewState.topics.get(typeName);
      }
      data = topics.reduce((obj, v) => {
        const { topic } = v;
        if (hasWSChannel(v) && typeName === v.typeName) {
          obj[topic] = topic; // eslint-disable-line no-param-reassign
        }
        return obj;
      }, data);
    }
    if (value === '') {
      data[''] = '';
    }

    return (
      <PanelItemContainer>
        <Label>{title}</Label>
        <Dropdown value={value} data={data} onChange={this._onTopicChange} isEnabled={isEnabled} />
      </PanelItemContainer>
    );
  }
}
