// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import STORE from '@felicia-viz/ui/store';

export interface Command {
  name: string;
  lowerName?: string;
  matches?: number[];
  action: () => void;
}

const COMMANDS: Array<Command> = [
  {
    name: 'Show CameraControlPanel',
    action: (): void => {
      STORE.uiState.markActive(0);
      STORE.uiState.setControlPanel('CameraControlPanel');
    },
  },
  {
    name: 'Show OccupancyGridMapControlPanel',
    action: (): void => {
      STORE.uiState.markActive(0);
      STORE.uiState.setControlPanel('OccupancyGridMapControlPanel');
    },
  },
  {
    name: 'Show PointcloudControlPanel',
    action: (): void => {
      STORE.uiState.markActive(0);
      STORE.uiState.setControlPanel('PointcloudControlPanel');
    },
  },
  {
    name: 'Show PoseWithTimestampControlPanel',
    action: (): void => {
      STORE.uiState.markActive(0);
      STORE.uiState.setControlPanel('PoseWithTimestampControlPanel');
    },
  },
  {
    name: 'Show Pose3WithTimestampControlPanel',
    action: (): void => {
      STORE.uiState.markActive(0);
      STORE.uiState.setControlPanel('Pose3WithTimestampControlPanel');
    },
  },
  {
    name: 'Add CameraFrameView',
    action: (): void => {
      STORE.uiState.addView('CameraFrameView');
    },
  },
  {
    name: 'Add DepthCameraFrameView',
    action: (): void => {
      STORE.uiState.addView('DepthCameraFrameView');
    },
  },
  {
    name: 'Add ImuFrameView',
    action: (): void => {
      STORE.uiState.addView('ImuFrameView');
    },
  },
  {
    name: 'Add LidarFrameView',
    action: (): void => {
      STORE.uiState.addView('LidarFrameView');
    },
  },
];

export function matchCommand(query: string, commands: Array<Command>): Array<Command> {
  const candidates = commands.map(command =>
    Object.assign(command, { lowerName: command.name.toLowerCase(), matches: [] })
  );
  const values = query.split('');
  values.forEach((c, i) => {
    candidates.forEach(candidate => {
      const matchesLen = candidate.matches.length;
      if (i !== matchesLen) return;
      const lastIndex = matchesLen === 0 ? -1 : candidate.matches[matchesLen - 1];
      const index = candidate.lowerName.indexOf(c, lastIndex + 1);
      if (index !== -1) {
        candidate.matches.push(index);
      }
    });
  });
  return candidates.filter(candidate => candidate.matches.length === values.length);
}

export function runAction(name: string): void {
  const command = COMMANDS.find(c => name === c.name);
  if (command) {
    command.action();
  }
}

export default COMMANDS;
