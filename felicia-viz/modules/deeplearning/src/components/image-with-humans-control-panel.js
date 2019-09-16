import { inject, observer } from 'mobx-react';
import PropTypes from 'prop-types';
import React, { Component } from 'react';
import { Form } from '@streetscape.gl/monochrome';

import { IMAGE_WITH_HUMANS_MESSAGE } from '@felicia-viz/proto/messages/human';
import { TopicDropdown } from '@felicia-viz/ui';
import { FORM_STYLE } from '@felicia-viz/ui/custom-styles';
import { FeliciaVizStore } from '@felicia-viz/ui/store';

@inject('store')
@observer
export default class ImageWithHumansControlPanel extends Component {
  static propTypes = {
    store: PropTypes.instanceOf(FeliciaVizStore).isRequired,
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
            return <TopicDropdown {...self} typeNames={[IMAGE_WITH_HUMANS_MESSAGE]} />;
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
    const viewState = store.uiState.getActiveViewState();
    const { threshold } = values;
    if (threshold && viewState.threshold !== threshold) {
      viewState.setThreshold(threshold);
    }
  };

  _fetchValues() {
    const { store } = this.props;
    const viewState = store.uiState.getActiveViewState();
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
