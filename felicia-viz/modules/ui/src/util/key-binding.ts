// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

type CallbackFunc = (e: KeyboardEvent) => void;

interface Action {
  id: number;
  keys: string[];
  keyDownAction: CallbackFunc;
  keyUpAction?: CallbackFunc;
}

class KeyBinding {
  private keyMap: Map<string, boolean>;

  private registeredActions: Array<Action>;

  private id: number;

  constructor() {
    this.keyMap = new Map<string, boolean>();
    this.registeredActions = [];
    this.id = 0;
  }

  private _onKeyDownOrUp: EventListener = (event: Event): void => {
    const isKeyDown = event.type === 'keydown';
    const keyboardEvent = event as KeyboardEvent;
    this.keyMap.set(keyboardEvent.code, isKeyDown);
    this._doAction(keyboardEvent, isKeyDown);
  };

  private _hasKey = (key: string): boolean | undefined => {
    if (key === 'Shift' || key === 'Control' || key === 'Alt') {
      const ret = this.keyMap.get(`${key}Left`);
      if (ret) return true;
      return this.keyMap.get(`${key}Right`);
    }
    return this.keyMap.get(key);
  };

  private _doAction(event: KeyboardEvent, isKeyDown: boolean): void {
    let matchedLength = 0;
    this.registeredActions.forEach(registerAction => {
      const { keys, keyDownAction, keyUpAction } = registerAction;
      const { length } = keys;
      if (matchedLength !== 0 && matchedLength > length) return;
      let allMatched = false;
      for (let i = 0; i < length; i += 1) {
        allMatched = this._hasKey(keys[i]) as boolean;
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

  registerAction(keys: string[], keyDownAction: CallbackFunc, keyUpAction?: CallbackFunc): number {
    if (keys.length === 0) return -1;

    this.registeredActions.push({
      id: this.id,
      keys,
      keyDownAction,
      keyUpAction,
    });
    this.id += 1;
    this.registeredActions.sort((a, a2) => a2.keys.length - a.keys.length);
    return this.id - 1;
  }

  unregisterAction(id: number): void {
    const idx = this.registeredActions.findIndex(registerAction => id === registerAction.id);
    if (idx >= 0) this.registeredActions.splice(idx, 1);
  }

  bind(node: Node): void {
    node.addEventListener('keydown', this._onKeyDownOrUp);
    node.addEventListener('keyup', this._onKeyDownOrUp);
  }

  unbind(node: Node): void {
    node.removeEventListener('keydown', this._onKeyDownOrUp);
    node.removeEventListener('keyup', this._onKeyDownOrUp);
  }
}

const KEY_BINDING = new KeyBinding();
export default KEY_BINDING;
