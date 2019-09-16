import { shallowEqualArrays } from 'shallow-equal';

export default class KeyBinding {
  constructor(node) {
    this.node = node;
    this.keyMap = new Map();
    this.registeredActions = [];
  }

  _onKeyDownOrUp = event => {
    const isKeyDown = event.type === 'keydown';
    this.keyMap.set(event.key, isKeyDown);
    this._doAction(event, isKeyDown);
  };

  _doAction(event, isKeyDown) {
    let matchedLength = 0;
    this.registeredActions.forEach(registerAction => {
      const { keys, keyDownAction, keyUpAction } = registerAction;
      const { length } = keys;
      if (matchedLength !== 0 && matchedLength > length) return;
      let allMatched = false;
      for (let i = 0; i < length; i += 1) {
        allMatched = this.keyMap.get(keys[i]);
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
    if (keys.length === 0) return;

    this.registeredActions.push({
      keys,
      keyDownAction,
      keyUpAction,
    });
    this.registeredActions.sort((k, k2) => k.length < k2.length);
  }

  unregisterAction(keys, keyDownAction, keyUpAction) {
    for (let i = 0; i < this.registeredActions.length; i += 1) {
      const registeredAction = this.registeredActions[i];
      if (
        shallowEqualArrays(keys, registeredAction.keys) &&
        keyDownAction === registeredAction.keyDownAction &&
        keyUpAction === registeredAction.keyUpAction
      ) {
        this.registeredActions.splice(i, 1);
        return;
      }
    }
  }

  bind() {
    this.node.addEventListener('keydown', this._onKeyDownOrUp);
    this.node.addEventListener('keyup', this._onKeyDownOrUp);
  }

  unbind() {
    this.node.removeEventListener('keydown', this._onKeyDownOrUp);
    this.node.removeEventListener('keyup', this._onKeyDownOrUp);
  }
}
