/* global self */
/* eslint no-restricted-globals: ["off"] */
import { fillPointcloud } from 'util/pointcloud';

self.onmessage = event => {
  const { colors, positions, frame } = event.data;
  fillPointcloud(colors, positions, frame);

  self.postMessage({
    colors,
    positions,
  });
};
