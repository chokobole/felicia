import { IMAGE_WITH_BOUNDING_BOXES_MESSAGE } from '@felicia-viz/proto/messages/bounding-box';
import TopicDropdown, {
  Props as TopicDropdownProps,
} from '@felicia-viz/ui/components/common/topic-dropdown';
import { FORM_STYLE } from '@felicia-viz/ui/custom-styles';
import { FeliciaVizStore } from '@felicia-viz/ui/store';
// @ts-ignore
import { Form } from '@streetscape.gl/monochrome';
import { inject, observer } from 'mobx-react';
import React, { Component } from 'react';
import ImageWithBoundingBoxesViewState from '../store/image-with-bounding-boxes-view-state';

@inject('store')
@observer
export default class ImageWithBoundingBoxesControlPanel extends Component<{
  store?: FeliciaVizStore;
}> {
  private SETTINGS = {
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
          render: (self: TopicDropdownProps): JSX.Element => {
            return <TopicDropdown {...self} typeNames={[IMAGE_WITH_BOUNDING_BOXES_MESSAGE]} />;
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

  private _onChange = (values: { lineWidth: number; threshold: number }): void => {
    const { store } = this.props;
    if (!store) return;
    const viewState = store.uiState.getActiveViewState() as ImageWithBoundingBoxesViewState;
    const { lineWidth, threshold } = values;
    if (lineWidth && viewState.lineWidth !== lineWidth) {
      viewState.setLineWidth(lineWidth);
    }

    if (threshold && viewState.threshold !== threshold) {
      viewState.setThreshold(threshold);
    }
  };

  _fetchValues(): {
    lineWidth: number;
    threshold: number;
  } {
    const { store } = this.props;
    if (store) {
      const viewState = store.uiState.getActiveViewState() as ImageWithBoundingBoxesViewState;
      const { lineWidth, threshold } = viewState;

      return {
        lineWidth,
        threshold,
      };
    }

    return {
      lineWidth: 0,
      threshold: 0,
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
