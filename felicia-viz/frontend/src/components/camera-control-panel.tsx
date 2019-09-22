import { Vector3Message } from '@felicia-viz/proto/messages/geometry';
import Vector3Input from '@felicia-viz/ui/components/common/vector3-input';
import { FormProps } from '@felicia-viz/ui/components/common/panel-item';
import { FORM_STYLE } from '@felicia-viz/ui/custom-styles';
import STORE from '@felicia-viz/ui/store';
// @ts-ignore
import { Form } from '@streetscape.gl/monochrome';
import React, { Component } from 'react';
import MainSceneState from '../store/ui/main-scene-state';

export default class CameraControlPanel extends Component<{
  followPose: boolean;
  position: Vector3Message | null;
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
        position: {
          type: 'custom',
          title: 'position',
          render: (self: FormProps<Vector3Message | null>): JSX.Element => {
            return <Vector3Input {...self} />;
          },
        },
      },
    },
  };

  private _onChange = (values: { followPose?: boolean; position?: Vector3Message }): void => {
    const viewState = STORE.uiState.getActiveViewState() as MainSceneState;
    const { camera } = viewState;

    if (camera) {
      if (values.followPose !== undefined && camera.followPose !== values.followPose) {
        camera.setFollowPose(values.followPose);
      }

      if (values.position !== undefined && camera.position !== values.position) {
        camera.setPosition(values.position);
      }
    }
  };

  private _fetchValues(): {
    followPose: boolean;
    position: Vector3Message | null;
  } {
    const { followPose, position } = this.props;
    return {
      followPose,
      position,
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
