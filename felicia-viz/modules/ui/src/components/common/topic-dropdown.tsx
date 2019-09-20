import { findWSChannel, hasWSChannel } from '@felicia-viz/proto/messages/master-data';
// @ts-ignore
import { Dropdown, Label } from '@streetscape.gl/monochrome';
import { toJS } from 'mobx';
import { inject, observer } from 'mobx-react';
import React, { Component } from 'react';
import { FeliciaVizStore } from '../../store';
import { PanelItemContainer } from './panel-item';

export interface Props {
  title: string;
  typeNames: string[];
  store?: FeliciaVizStore;
  isEnabled: boolean;
}

@inject('store')
@observer
export default class TopicDropdown extends Component<Props> {
  private _onTopicChange = (value: string): void => {
    const { store } = this.props;
    if (!store) return;
    const topics = toJS(store.topicInfo.topics);
    const found = topics.find(v => {
      return v.topic === value;
    });
    if (!found) {
      console.error(`Failed to find topic ${value}`);
      return;
    }

    const { typeName } = found;
    const channelDef = findWSChannel(found);

    if (channelDef) {
      const viewState = store.uiState.getActiveViewState();
      if (viewState) viewState.setTopic(typeName, value, channelDef.ipEndpoint);
    } else {
      console.error(`Failed to find ip endpoint for ${value}`);
    }
  };

  render(): JSX.Element | null {
    const { title, typeNames, store, isEnabled } = this.props;
    if (!store) return null;
    const viewState = store.uiState.getActiveViewState();
    if (!viewState) return null;

    let data: { [topic: string]: string } = {};
    let value: string | undefined = '';
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
