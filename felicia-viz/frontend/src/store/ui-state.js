import { observable, action } from 'mobx';

import TYPES from 'common/connection-type';
import Camera from 'store/camera';

export class CameraPanelState {
  @observable topic = '';

  @observable camera = new Camera();

  constructor(id) {
    this.id = id;
  }

  @action update(message) {
    this.camera.updateFrame(message);
  }

  @action selectTopic(newTopic) {
    this.topic = newTopic;
  }
}

export class UIState {
  @observable cameraPanelStates = [];

  id = 0;

  @action addCameraPanel() {
    this.cameraPanelStates.push(new CameraPanelState(this.id));
    this.id += 1;
  }

  @action removeCameraPanel(id) {
    const idx = this.cameraPanelStates.findIndex(cameraPanelState => {
      return cameraPanelState.id === id;
    });
    if (idx > -1) this.cameraPanelStates.splice(idx, 1);
  }

  findCameraPanel(id) {
    return this.cameraPanelStates.find(cameraPanelState => {
      return cameraPanelState.id === id;
    });
  }

  update(message) {
    if (message.type === TYPES.Camera.name) {
      this.cameraPanelStates.forEach(cameraPanelState => {
        if (cameraPanelState.id === message.id) {
          cameraPanelState.update(message);
        }
      });
    }
  }
}
