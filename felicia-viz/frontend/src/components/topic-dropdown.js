import React, { Component } from 'react';
import PropTypes from 'prop-types';
import { inject, observer } from 'mobx-react';
import { Dropdown, Label } from '@streetscape.gl/monochrome';

import PanelItemContainer from 'components/panel-item-container';
import { failedToFindActiveState } from 'util/error';
import { FeliciaVizStore } from 'store';

function fetchTopics() {
  const topics = ['color', 'depth'];

  return topics.reduce((obj, v) => {
    obj[v] = v; // eslint-disable-line no-param-reassign
    return obj;
  }, {});
}

@inject('store')
@observer
export default class TopicDropdown extends Component {
  static propTypes = {
    title: PropTypes.string.isRequired,
    store: PropTypes.instanceOf(FeliciaVizStore).isRequired,
  };

  _onTopicChange = value => {
    const { store } = this.props;
    const state = store.uiState.activeWindow.getState();
    if (state) {
      state.selectTopic(value);
    } else {
      failedToFindActiveState();
    }
  };

  render() {
    const { title, store } = this.props;
    const state = store.uiState.activeWindow.getState();
    let value = '';
    if (state) {
      value = state.topic;
    } else {
      failedToFindActiveState();
    }

    return (
      <PanelItemContainer>
        <Label>{title}</Label>
        <Dropdown value={value} data={fetchTopics()} onChange={this._onTopicChange} />
      </PanelItemContainer>
    );
  }
}
