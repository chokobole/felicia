import STORE from '@felicia-viz/ui/store';

const COMMANDS = [
  {
    name: 'Show OccupancyGridMapControlPanel',
    action: () => {
      STORE.uiState.markActive(0);
      STORE.uiState.setControlPanel('OccupancyGridMapControlPanel');
    },
  },
  {
    name: 'Show PointcloudControlPanel',
    action: () => {
      STORE.uiState.markActive(0);
      STORE.uiState.setControlPanel('PointcloudControlPanel');
    },
  },
  {
    name: 'Show PoseWithTimestampControlPanel',
    action: () => {
      STORE.uiState.markActive(0);
      STORE.uiState.setControlPanel('PoseWithTimestampControlPanel');
    },
  },
  {
    name: 'Show Pose3WithTimestampControlPanel',
    action: () => {
      STORE.uiState.markActive(0);
      STORE.uiState.setControlPanel('Pose3WithTimestampControlPanel');
    },
  },
  {
    name: 'Add CameraFrameView',
    action: () => {
      STORE.uiState.addView('CameraFrameView');
    },
  },
  {
    name: 'Add DepthCameraFrameView',
    action: () => {
      STORE.uiState.addView('DepthCameraFrameView');
    },
  },
  {
    name: 'Add ImuFrameView',
    action: () => {
      STORE.uiState.addView('ImuFrameView');
    },
  },
  {
    name: 'Add LidarFrameView',
    action: () => {
      STORE.uiState.addView('LidarFrameView');
    },
  },
];

export function matchCommand(query, commands) {
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

export function runAction(name) {
  const command = COMMANDS.find(c => name === c.name);
  if (command) {
    command.action();
  }
}

export default COMMANDS;
