class KeyBinding {
  constructor() {
    this.keyMap = new Map();
    this.registeredActions = [];
    this.id = 0;
  }

  _onKeyDownOrUp = event => {
    const isKeyDown = event.type === 'keydown';
    this.keyMap.set(event.code, isKeyDown);
    this._doAction(event, isKeyDown);
  };

  _hasKey = key => {
    if (key === 'Shift' || key === 'Control' || key === 'Alt') {
      const ret = this.keyMap.get(`${key}Left`);
      if (ret) return true;
      return this.keyMap.get(`${key}Right`);
    }
    return this.keyMap.get(key);
  };

  _doAction(event, isKeyDown) {
    let matchedLength = 0;
    this.registeredActions.forEach(registerAction => {
      const { keys, keyDownAction, keyUpAction } = registerAction;
      const { length } = keys;
      if (matchedLength !== 0 && matchedLength > length) return;
      let allMatched = false;
      for (let i = 0; i < length; i += 1) {
        allMatched = this._hasKey(keys[i]);
        if (!allMatched) break;
      }
      if (allMatched) {
        matchedLength = length;
        if (isKeyDown) {
          if (keyDownAction) keyDownAction(event);
        } else if (keyUpAction) {
          keyUpAction(event);
        }
      }
    });
  }

  registerAction(keys, keyDownAction, keyUpAction) {
    if (keys.length === 0) return -1;

    this.registeredActions.push({
      id: this.id,
      keys,
      keyDownAction,
      keyUpAction,
    });
    this.id += 1;
    this.registeredActions.sort((k, k2) => k.length < k2.length);
    return this.id - 1;
  }

  unregisterAction(id) {
    const idx = this.registeredActions.findIndex(registerAction => id === registerAction.id);
    if (idx >= 0) this.registeredActions.splice(idx, 1);
  }

  bind(node) {
    node.addEventListener('keydown', this._onKeyDownOrUp);
    node.addEventListener('keyup', this._onKeyDownOrUp);
  }

  unbind(node) {
    node.removeEventListener('keydown', this._onKeyDownOrUp);
    node.removeEventListener('keyup', this._onKeyDownOrUp);
  }
}

const KEY_BINDING = new KeyBinding();
export default KEY_BINDING;
