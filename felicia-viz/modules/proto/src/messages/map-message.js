/* eslint import/prefer-default-export: "off" */
import { DataMessage } from './data-message';
import { SizeMessage, PointMessage } from './geometry';

export const OCCUPANCY_GRID_MAP_MESSAGE = 'felicia.map.OccupancyGridMapMessage';
export const POINTCLOUD_MESSAGE = 'felicia.map.PointcloudMessage';

export class OccupancyGridMapMessage {
  constructor(message) {
    const { data, size, resolution, origin, timestamp } = message;
    this.size = new SizeMessage(size);
    this.resolution = resolution;
    this.origin = new PointMessage(origin);
    this.data = data;
    this.timestamp = timestamp;
  }
}

export class PointcloudMessage {
  constructor(message) {
    const { points, colors, timestamp } = message;
    this.points = new DataMessage(points);
    this.colors = new DataMessage(colors);
    this.timestamp = timestamp;
  }
}
