import { inject, observer } from 'mobx-react';
import PropTypes from 'prop-types';
import React, { Component } from 'react';
import { Form } from '@streetscape.gl/monochrome';

import { FeliciaVizStore } from '@felicia-viz/ui/store';
import { FORM_STYLE } from '@felicia-viz/ui/custom-styles';

@inject('store')
@observer
export default class CameraControlPanel extends Component {
  static propTypes = {
    store: PropTypes.instanceOf(FeliciaVizStore).isRequired,
  };

  SETTINGS = {
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

  _onChange = values => {
    const { store } = this.props;
    const viewState = store.uiState.getActiveViewState();
    const { camera } = viewState;

    if (camera) {
      if (camera.followPose !== values.followPose) {
        camera.setFollowPose(values.followPose);
      }
    }
  };

  _fetchValues() {
    const { store } = this.props;
    const viewState = store.uiState.getActiveViewState();
    const { camera } = viewState;

    if (camera) {
      const { followPose } = camera;
      return {
        followPose,
      };
    }

    return {
      followPose: false,
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
