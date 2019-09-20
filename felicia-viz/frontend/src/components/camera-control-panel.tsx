import { FORM_STYLE } from '@felicia-viz/ui/custom-styles';
import { FeliciaVizStore } from '@felicia-viz/ui/store';
// @ts-ignore
import { Form } from '@streetscape.gl/monochrome';
import { inject, observer } from 'mobx-react';
import React, { Component } from 'react';
import MainSceneState from '../store/ui/main-scene-state';

@inject('store')
@observer
export default class CameraControlPanel extends Component<{
  store?: FeliciaVizStore;
}> {
  private SETTINGS = {
    header: { type: 'header', title: 'Camera Control' },
    sectionSeperator: { type: 'separator' },
    control: {
      type: 'header',
      title: 'Control',
      children: {
        followPose: {
          type: 'toggle',
          title: 'followPose',
        },
      },
    },
  };

  private _onChange = (values: { followPose: boolean }): void => {
    const { store } = this.props;
    if (!store) return;
    const viewState = store.uiState.getActiveViewState() as MainSceneState;
    const { camera } = viewState;

    if (camera) {
      if (camera.followPose !== values.followPose) {
        camera.setFollowPose(values.followPose);
      }
    }
  };

  private _fetchValues(): {
    followPose: boolean;
  } {
    const { store } = this.props;
    if (store) {
      const viewState = store.uiState.getActiveViewState() as MainSceneState;
      const { camera } = viewState;

      if (camera) {
        const { followPose } = camera;
        return {
          followPose,
        };
      }
    }
    return {
      followPose: false,
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
