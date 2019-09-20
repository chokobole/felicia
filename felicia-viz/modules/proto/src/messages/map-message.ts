/* eslint import/prefer-default-export: "off" */
import { DataMessage, DataMessageProtobuf } from './data-message';
import { PointMessage, PointMessageProtobuf, SizeMessage, SizeMessageProtobuf } from './geometry';

export const OCCUPANCY_GRID_MAP_MESSAGE = 'felicia.map.OccupancyGridMapMessage';
export const POINTCLOUD_MESSAGE = 'felicia.map.PointcloudMessage';

export interface OccupancyGridMapMessageProtobuf {
  size: SizeMessageProtobuf;
  resolution: number;
  origin: PointMessageProtobuf;
  data: Uint8Array;
  timestamp: number;
}

export class OccupancyGridMapMessage {
  size: SizeMessage;

  resolution: number;

  origin: PointMessage;

  data: Uint8Array;

  timestamp: number;

  constructor({ size, resolution, origin, data, timestamp }: OccupancyGridMapMessageProtobuf) {
    this.size = new SizeMessage(size);
    this.resolution = resolution;
    this.origin = new PointMessage(origin);
    this.data = data;
    this.timestamp = timestamp;
  }
}

export interface PointcloudMessageProtobuf {
  points: DataMessageProtobuf;
  colors: DataMessageProtobuf;
  timestamp: number;
}

export class PointcloudMessage {
  points: DataMessage;

  colors: DataMessage;

  timestamp: number;

  constructor({ points, colors, timestamp }: PointcloudMessageProtobuf) {
    this.points = new DataMessage(points);
    this.colors = new DataMessage(colors);
    this.timestamp = timestamp;
  }
}
