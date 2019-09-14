import { inject, observer } from 'mobx-react';
import PropTypes from 'prop-types';
import React, { Component } from 'react';
import { Form } from '@streetscape.gl/monochrome';

import { IMAGE_WITH_BOUNDING_BOXES_MESSAGE } from '@felicia-viz/proto/messages/bounding-box';
import { TopicDropdown } from '@felicia-viz/ui';
import { FORM_STYLE } from '@felicia-viz/ui/custom-styles';

@inject('store')
@observer
export default class ImageWithBoundingBoxesControlPanel extends Component {
  static propTypes = {
    store: PropTypes.object.isRequired,
  };

  SETTINGS = {
    header: { type: 'header', title: 'ImageWithBoundingBoxes Control' },
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
            return <TopicDropdown {...self} typeName={IMAGE_WITH_BOUNDING_BOXES_MESSAGE} />;
          },
        },
        lineWidth: {
          type: 'range',
          title: 'lineWidth',
          min: 1,
          max: 20,
          step: 0.5,
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
    const { lineWidth, threshold } = values;
    if (lineWidth && viewState.lineWidth !== lineWidth) {
      viewState.setLineWidth(lineWidth);
    }

    if (threshold && viewState.threshold !== threshold) {
      viewState.setThreshold(threshold);
    }
  };

  _fetchValues() {
    const { store } = this.props;
    const viewState = store.uiState.activeViewState.getState();
    const { lineWidth, threshold } = viewState;

    return {
      lineWidth,
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
