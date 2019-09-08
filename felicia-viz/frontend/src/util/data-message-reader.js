/* eslint no-bitwise: ["off"] */
import { DataElementType, DataChannelType } from '@felicia-viz/communication';

export function getDataView(data) {
  const { buffer, byteOffset, byteLength } = data;
  return new DataView(buffer, byteOffset, byteLength);
}

export function element1Size(elementType) {
  switch (elementType) {
    case DataElementType.values.ELEMENT_TYPE_8U:
    case DataElementType.values.ELEMENT_TYPE_8S:
      return 1;
    case DataElementType.values.ELEMENT_TYPE_16U:
    case DataElementType.values.ELEMENT_TYPE_16S:
      return 2;
    case DataElementType.values.ELEMENT_TYPE_32U:
    case DataElementType.values.ELEMENT_TYPE_32S:
    case DataElementType.values.ELEMENT_TYPE_32F:
      return 4;
    case DataElementType.values.ELEMENT_TYPE_64U:
    case DataElementType.values.ELEMENT_TYPE_64S:
    case DataElementType.values.ELEMENT_TYPE_64F:
      return 8;
    case DataElementType.values.ELEMENT_TYPE_CUSTOM:
      return 1;
    default:
      throw new Error('Unknown element size');
  }
}

export function channelSize(channelType) {
  switch (channelType) {
    case DataChannelType.values.CHANNEL_TYPE_C1:
      return 1;
    case DataChannelType.values.CHANNEL_TYPE_C2:
      return 2;
    case DataChannelType.values.CHANNEL_TYPE_C3:
      return 3;
    case DataChannelType.values.CHANNEL_TYPE_C4:
      return 4;
    case DataChannelType.values.CHANNEL_TYPE_CUSTOM:
      return 1;
    default:
      throw new Error('Unknown channel size');
  }
}

export function getElementAndChannelType(type) {
  return {
    elementType: (type >> 16) & 65535,
    channelType: type & 65535,
  };
}

export default class DataMessageReader {
  constructor(message) {
    this.message = message;
    const { type, data } = this.message;
    const { elementType, channelType } = getElementAndChannelType(type);
    this.elementType = elementType;
    this.channelType = channelType;
    this.dataView = getDataView(data);
    this.readFunc = this._bindReadFunc();
  }

  _bindReadFunc() {
    switch (this.elementType) {
      case DataElementType.values.ELEMENT_TYPE_8U:
        return this.dataView.getUint8.bind(this.dataView);
      case DataElementType.values.ELEMENT_TYPE_8S:
        return this.dataView.getInt8.bind(this.dataView);
      case DataElementType.values.ELEMENT_TYPE_16U:
        return this.dataView.getUint16.bind(this.dataView);
      case DataElementType.values.ELEMENT_TYPE_16S:
        return this.dataView.getInt16.bind(this.dataView);
      case DataElementType.values.ELEMENT_TYPE_32U:
        return this.dataView.getUint32.bind(this.dataView);
      case DataElementType.values.ELEMENT_TYPE_32S:
        return this.dataView.getInt32.bind(this.dataView);
      case DataElementType.values.ELEMENT_TYPE_32F:
        return this.dataView.getFloat32.bind(this.dataView);
      case DataElementType.values.ELEMENT_TYPE_64U:
        return this.dataView.getBigUint64.bind(this.dataView);
      case DataElementType.values.ELEMENT_TYPE_64S:
        return this.dataView.getBigInt64.bind(this.dataView);
      case DataElementType.values.ELEMENT_TYPE_64F:
        return this.dataView.getFloat64.bind(this.dataView);
      default:
        return null;
    }
  }

  hasData(idx) {
    return idx < this.length();
  }

  nextData(idx) {
    const v = [];
    const from = this.elementSize() * idx;
    const to = this.elementSize() * (idx + 1);
    for (let i = from; i < to; i += this.element1Size()) {
      v.push(this.readFunc(i, true));
    }
    return v;
  }

  length() {
    if (!this.length_) this.length_ = this.dataView.byteLength / this.elementSize();
    return this.length_;
  }

  elementSize() {
    if (!this.elementSize_) this.elementSize_ = this.element1Size() * this.channelSize();
    return this.elementSize_;
  }

  element1Size() {
    if (!this.element1Size_) this.element1Size_ = element1Size(this.elementType);
    return this.element1Size_;
  }

  channelSize() {
    if (!this.channelSize_) this.channelSize_ = channelSize(this.channelType);
    return this.channelSize_;
  }
}

export class PointReader extends DataMessageReader {
  constructor(message) {
    super(message);
    switch (this.channelSize()) {
      case DataChannelType.values.CHANNEL_TYPE_C2: {
        this.nextPoint = idx => {
          return this.nextData(idx);
        };
        this.nextPoint3 = idx => {
          const ret = this.nextData(idx);
          ret.push(0);
          return ret;
        };
        break;
      }
      case DataChannelType.values.CHANNEL_TYPE_C3: {
        this.nextPoint = idx => {
          const ret = this.nextData(idx);
          ret.pop();
          return ret;
        };
        this.nextPoint3 = idx => {
          return this.nextData(idx);
        };
        break;
      }
      default:
        break;
    }
  }
}

export class ColorReader extends DataMessageReader {
  constructor(message) {
    super(message);
    if (this.elementType === DataElementType.values.ELEMENT_TYPE_8U) {
      switch (this.channelSize()) {
        case DataChannelType.values.CHANNEL_TYPE_C3: {
          this.nextColor3u = idx => {
            return this.nextData(idx);
          };
          this.nextColor3f = idx => {
            return this.nextData(idx).map(x => x / 255);
          };
          this.nextColor4u = idx => {
            const ret = this.nextData(idx);
            ret.push(255);
            return ret;
          };
          this.nextColor4f = idx => {
            return this.nextColor4u(idx).map(x => x / 255);
          };
          break;
        }
        case DataChannelType.values.CHANNEL_TYPE_C4: {
          this.nextColor3u = idx => {
            const ret = this.nextData(idx);
            ret.pop();
            return ret;
          };
          this.nextColor3f = idx => {
            return this.nextColor3u(idx).map(x => x / 255);
          };
          this.nextColor4u = idx => {
            return this.nextData(idx);
          };
          this.nextColor4f = idx => {
            return this.nextData(idx).map(x => x / 255);
          };
          break;
        }
        default:
          break;
      }
    } else if (this.elementType === DataElementType.values.ELEMENT_TYPE_32F) {
      switch (this.channelSize()) {
        case DataChannelType.values.CHANNEL_TYPE_C3: {
          this.nextColor3u = idx => {
            return this.nextData(idx).map(x => Math.round(x * 255));
          };
          this.nextColor3f = idx => {
            return this.nextData(idx);
          };
          this.nextColor4u = idx => {
            return this.nextColor4f(idx).map(x => Math.round(x * 255));
          };
          this.nextColor4f = idx => {
            const ret = this.nextData(idx);
            ret.push(1);
            return ret;
          };
          break;
        }
        case DataChannelType.values.CHANNEL_TYPE_C4: {
          this.nextColor3u = idx => {
            return this.nextColor3f(idx).map(x => Math.round(x * 255));
          };
          this.nextColor3f = idx => {
            const ret = this.nextData(idx);
            ret.pop();
            return ret;
          };
          this.nextColor4u = idx => {
            return this.nextData(idx).map(x => Math.round(x * 255));
          };
          this.nextColor4f = idx => {
            return this.nextData(idx);
          };
          break;
        }
        default:
          break;
      }
    }
  }
}
