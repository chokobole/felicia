/* eslint react/jsx-no-bind: ["off"] */
import React, { Component } from 'react';
import PropTypes from 'prop-types';
import { toJS } from 'mobx';
import { inject, observer } from 'mobx-react';
import styled from '@emotion/styled';

import { hasWSChannel, findWSChannel } from '@felicia-viz/communication';

const LabelContainer = styled.div(() => ({
  display: 'flex',
  flexDirection: 'column',
}));

const Label = styled.label(props => ({
  display: 'flex',
  alignItems: 'center',
  cursor: 'inherit',
  width: '100%',
  color: props.isOn ? props.theme.textColorPrimary : props.theme.textColorDisabled,

  ...props.style,
}));

@inject('store')
@observer
export default class TopicList extends Component {
  static propTypes = {
    typeNames: PropTypes.arrayOf(PropTypes.string).isRequired,
    style: PropTypes.object,
    store: PropTypes.object.isRequired,
    theme: PropTypes.object.isRequired,
  };

  static defaultProps = {
    style: {},
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
      const viewState = store.uiState.activeViewState.getState();
      if (viewState.topics.has(typeName)) {
        viewState.unsetTopic(typeName, value);
      } else {
        viewState.setTopic(typeName, value, found.ipEndpoint);
      }
    } else {
      console.error(`Failed to find ip endpoint for ${value}`);
    }
  };

  render() {
    const { typeNames, store, style, theme } = this.props;
    const viewState = store.uiState.activeViewState.getState();

    let allTopics = {};
    // eslint-disable-next-line no-restricted-syntax
    for (const typeName of typeNames) {
      let value = null;
      if (viewState.topics.has(typeName)) {
        value = viewState.topics.get(typeName);
      }
      const topics = toJS(store.topicInfo.topics);
      allTopics = topics.reduce((obj, v) => {
        const { topic } = v;
        if (hasWSChannel(v) && typeName === v.typeName) {
          obj[topic] = value === topic; // eslint-disable-line no-param-reassign
        }
        return obj;
      }, allTopics);
    }

    const allTopicsJsx = [];
    Object.keys(allTopics).forEach(topic => {
      allTopicsJsx.push(
        <Label
          key={topic}
          isOn={allTopics[topic]}
          theme={theme}
          style={style}
          onPointerDown={this._onTopicChange.bind(this, topic)}>
          {topic}
        </Label>
      );
    });

    return <LabelContainer>{allTopicsJsx}</LabelContainer>;
  }
}
