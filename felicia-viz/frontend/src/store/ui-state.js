import { observable, action } from 'mobx';

import MESSAGE_TYPES from 'common/message-type';
import Camera from 'store/camera';
import SUBSCRIBER from 'util/subscriber';

export const UI_TYPES = {
  CameraPanel: { name: 'CameraPanel' },
};

class CameraPanelState {
  @observable topic = '';

  @observable filter = 'jet';

  @observable camera = new Camera();

  constructor(id) {
    this.id = id;
  }

  @action update(message) {
    this.camera.updateFrame(message);
  }

  @action selectTopic(newTopic) {
    this.topic = newTopic;
    SUBSCRIBER.subscribeTopic(this.id, MESSAGE_TYPES.Camera.name, newTopic);
  }

  @action selectFilter(newFilter) {
    this.filter = newFilter;
  }
}

class Window {
  @observable id = null;

  @observable type = null;

  constructor(uiState) {
    this.uiState = uiState;
  }

  @action activate(id, type) {
    this.id = id;
    this.type = type;
  }

  @action deacitvate() {
    this.id = null;
    this.type = null;
  }

  getState() {
    if (this.type === null) return null;

    if (this.type === UI_TYPES.CameraPanel.name) {
      return this.uiState.findCameraPanel(this.id);
    }

    return null;
  }
}

export default class UIState {
  @observable cameraPanelStates = [];

  @observable activeWindow = new Window(this);

  id = 0;

  @action addCameraPanel() {
    this.cameraPanelStates.push(new CameraPanelState(this.id));
    this.activeWindow.activate(this.id, UI_TYPES.CameraPanel.name);
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
    if (message.type === MESSAGE_TYPES.Camera.name) {
      this.cameraPanelStates.forEach(cameraPanelState => {
        if (cameraPanelState.id === message.id) {
          cameraPanelState.update(message);
        }
      });
    }
  }
}
