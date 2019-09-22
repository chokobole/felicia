import { IMAGE_WITH_HUMANS_MESSAGE } from '@felicia-viz/proto/messages/human';
import TopicDropdown, {
  Props as TopicDropdownProps,
} from '@felicia-viz/ui/components/common/topic-dropdown';
import { FORM_STYLE } from '@felicia-viz/ui/custom-styles';
import { FeliciaVizStore } from '@felicia-viz/ui/store';
// @ts-ignore
import { Form } from '@streetscape.gl/monochrome';
import { inject, observer } from 'mobx-react';
import React, { Component } from 'react';
import ImageWithHumansViewState from '../store/image-with-humans-view-state';

@inject('store')
@observer
export default class ImageWithHumansControlPanel extends Component<{
  store?: FeliciaVizStore;
}> {
  private SETTINGS = {
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
          render: (self: TopicDropdownProps): JSX.Element => {
            return <TopicDropdown {...self} value={[IMAGE_WITH_HUMANS_MESSAGE]} />;
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

  private _onChange = (values: { threshold: number }): void => {
    const { store } = this.props;
    if (!store) return;
    const viewState = store.uiState.getActiveViewState() as ImageWithHumansViewState;
    const { threshold } = values;
    if (threshold && viewState.threshold !== threshold) {
      viewState.setThreshold(threshold);
    }
  };

  private _fetchValues(): {
    threshold: number;
  } {
    const { store } = this.props;
    if (store) {
      const viewState = store.uiState.getActiveViewState() as ImageWithHumansViewState;
      const { threshold } = viewState;

      return {
        threshold,
      };
    }
    return {
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
