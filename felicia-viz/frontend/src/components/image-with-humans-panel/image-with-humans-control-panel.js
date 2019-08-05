import React, { Component } from 'react';
import PropTypes from 'prop-types';
import { inject, observer } from 'mobx-react';
import { Form } from '@streetscape.gl/monochrome';

import { IMAGE_WITH_HUMANS_MESSAGE } from '@felicia-viz/communication';
import { TopicDropdown } from '@felicia-viz/ui';

import { FORM_STYLE } from 'custom-styles';

@inject('store')
@observer
export default class ImageWithHumansControlPanel extends Component {
  static propTypes = {
    store: PropTypes.object.isRequired,
  };

  SETTINGS = {
    header: { type: 'header', title: 'ImageWithHumans Control' },
    sectionSeperator: { type: 'separator' },
    info: {
      type: 'header',
      title: 'Info',
      children: {},
    },
    control: {
      type: 'header',
      title: 'Control',
      children: {
        topic: {
          type: 'custom',
          title: 'topic',
          render: self => {
            return <TopicDropdown {...self} typeName={IMAGE_WITH_HUMANS_MESSAGE} />;
          },
        },
        threshold: {
          type: 'range',
          title: 'threshold',
          min: 0,
          max: 1,
          step: 0.05,
        },
      },
    },
  };

  _onChange = values => {
    const { store } = this.props;
    const viewState = store.uiState.activeViewState.getState();
    const { threshold } = values;
    if (threshold && viewState.threshold !== threshold) {
      viewState.setThreshold(threshold);
    }
  };

  _fetchValues() {
    const { store } = this.props;
    const viewState = store.uiState.activeViewState.getState();
    const { threshold } = viewState;

    return {
      threshold,
    };
  }

  render() {
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
