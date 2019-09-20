import FeliciaProtoRoot from '../felicia-proto-root';

export const DataChannelType = FeliciaProtoRoot.lookupEnum('felicia.DataMessage.ChannelType');
export const DataElementType = FeliciaProtoRoot.lookupEnum('felicia.DataMessage.ElementType');

export enum DataMessageElementTypeProtobuf {
  ELEMENT_TYPE_CUSTOM = 0,
  ELEMENT_TYPE_8U = 1,
  ELEMENT_TYPE_8S = 2,
  ELEMENT_TYPE_16U = 3,
  ELEMENT_TYPE_16S = 4,
  ELEMENT_TYPE_32U = 5,
  ELEMENT_TYPE_32S = 6,
  ELEMENT_TYPE_64U = 7,
  ELEMENT_TYPE_64S = 8,
  ELEMENT_TYPE_32F = 9,
  ELEMENT_TYPE_64F = 10,
}

export enum DataMessageChannelTypeProtobuf {
  CHANNEL_TYPE_CUSTOM = 0,
  CHANNEL_TYPE_C1 = 1,
  CHANNEL_TYPE_C2 = 2,
  CHANNEL_TYPE_C3 = 3,
  CHANNEL_TYPE_C4 = 4,
}

export function getDataView(data: Uint8Array): DataView {
  const { buffer, byteOffset, byteLength } = data;
  return new DataView(buffer, byteOffset, byteLength);
}

export function element1Size(elementType: DataMessageElementTypeProtobuf): number {
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

export function channelSize(channelType: DataMessageChannelTypeProtobuf): number {
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

export function getElementAndChannelType(
  type: number
): {
  elementType: number;
  channelType: number;
} {
  return {
    elementType: (type >> 16) & 65535,
    channelType: type & 65535,
  };
}

export interface DataMessageProtobuf {
  type: number;
  data: Uint8Array;
}

export default class Data {
  elementType: DataMessageElementTypeProtobuf;

  channelType: DataMessageChannelTypeProtobuf;

  data: Uint8Array;

  dataView: DataView;

  readFunc:
    | ((byteOffset: number, littenEndian?: boolean | undefined) => number)
    | ((byteOffset: number, littleEndian?: boolean | undefined) => bigint)
    | null;

  private length_ = 0;

  private elementSize_ = 0;

  private element1Size_ = 0;

  private channelSize_ = 0;

  constructor({ elementType, channelType, data }: DataMessage) {
    this.elementType = elementType;
    this.channelType = channelType;
    this.data = data;
    this.dataView = getDataView(data);
    this.readFunc = this._bindReadFunc()!;
  }

  _bindReadFunc():
    | ((byteOffset: number, littenEndian?: boolean | undefined) => number)
    | ((byteOffset: number, littleEndian?: boolean | undefined) => bigint)
    | null {
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

  hasData(idx: number): boolean {
    return idx < this.length();
  }

  nextData(idx: number): number[] {
    const v = [];
    const from = this.elementSize() * idx;
    const to = this.elementSize() * (idx + 1);
    for (let i = from; i < to; i += this.element1Size()) {
      v.push(this.readFunc!(i, true));
    }
    return v as number[];
  }

  length(): number {
    if (!this.length_) this.length_ = this.dataView.byteLength / this.elementSize();
    return this.length_;
  }

  elementSize(): number {
    if (!this.elementSize_) {
      this.elementSize_ = this.element1Size() * this.channelSize();
    }
    return this.elementSize_;
  }

  element1Size(): number {
    if (!this.element1Size_) this.element1Size_ = element1Size(this.elementType);
    return this.element1Size_;
  }

  channelSize(): number {
    if (!this.channelSize_) this.channelSize_ = channelSize(this.channelType);
    return this.channelSize_;
  }
}

export class Points extends Data {
  nextPoint: (idx: number) => number[];

  nextPoint3: (idx: number) => number[];

  constructor(message: DataMessage) {
    super(message);
    switch (this.channelSize()) {
      case DataChannelType.values.CHANNEL_TYPE_C2: {
        this.nextPoint = (idx: number): number[] => {
          return this.nextData(idx);
        };
        this.nextPoint3 = (idx: number): number[] => {
          const ret = this.nextData(idx);
          ret.push(0);
          return ret;
        };
        break;
      }
      case DataChannelType.values.CHANNEL_TYPE_C3: {
        this.nextPoint = (idx: number): number[] => {
          const ret = this.nextData(idx);
          ret.pop();
          return ret;
        };
        this.nextPoint3 = (idx: number): number[] => {
          return this.nextData(idx);
        };
        break;
      }
      default:
        throw new Error('Invalid channel size');
    }
  }
}

export class Colors extends Data {
  nextColor3u: (idx: number) => number[];

  nextColor3f: (idx: number) => number[];

  nextColor4u: (idx: number) => number[];

  nextColor4f: (idx: number) => number[];

  constructor(message: DataMessage) {
    super(message);
    if (this.elementType === DataElementType.values.ELEMENT_TYPE_8U) {
      switch (this.channelSize()) {
        case DataChannelType.values.CHANNEL_TYPE_C3: {
          this.nextColor3u = (idx: number): number[] => {
            return this.nextData(idx);
          };
          this.nextColor3f = (idx: number): number[] => {
            return this.nextData(idx).map(x => x / 255);
          };
          this.nextColor4u = (idx: number): number[] => {
            const ret = this.nextData(idx);
            ret.push(255);
            return ret;
          };
          this.nextColor4f = (idx: number): number[] => {
            return this.nextColor4u!(idx).map(x => x / 255);
          };
          break;
        }
        case DataChannelType.values.CHANNEL_TYPE_C4: {
          this.nextColor3u = (idx: number): number[] => {
            const ret = this.nextData(idx);
            ret.pop();
            return ret;
          };
          this.nextColor3f = (idx: number): number[] => {
            return this.nextColor3u!(idx).map(x => x / 255);
          };
          this.nextColor4u = (idx: number): number[] => {
            return this.nextData(idx);
          };
          this.nextColor4f = (idx: number): number[] => {
            return this.nextData(idx).map(x => x / 255);
          };
          break;
        }
        default:
          throw new Error('Invalid channel size');
      }
    } else if (this.elementType === DataElementType.values.ELEMENT_TYPE_32F) {
      switch (this.channelSize()) {
        case DataChannelType.values.CHANNEL_TYPE_C3: {
          this.nextColor3u = (idx: number): number[] => {
            return this.nextData(idx).map(x => Math.round(x * 255));
          };
          this.nextColor3f = (idx: number): number[] => {
            return this.nextData(idx);
          };
          this.nextColor4u = (idx: number): number[] => {
            return this.nextColor4f!(idx).map(x => Math.round(x * 255));
          };
          this.nextColor4f = (idx: number): number[] => {
            const ret = this.nextData(idx);
            ret.push(1);
            return ret;
          };
          break;
        }
        case DataChannelType.values.CHANNEL_TYPE_C4: {
          this.nextColor3u = (idx: number): number[] => {
            return this.nextColor3f!(idx).map(x => Math.round(x * 255));
          };
          this.nextColor3f = (idx: number): number[] => {
            const ret = this.nextData(idx);
            ret.pop();
            return ret;
          };
          this.nextColor4u = (idx: number): number[] => {
            return this.nextData(idx).map(x => Math.round(x * 255));
          };
          this.nextColor4f = (idx: number): number[] => {
            return this.nextData(idx);
          };
          break;
        }
        default:
          throw new Error('Invalid channel size');
      }
    } else {
      throw new Error('Invalid element type');
    }
  }
}

export class DataMessage {
  elementType: DataMessageElementTypeProtobuf;

  channelType: DataMessageChannelTypeProtobuf;

  data: Uint8Array;

  constructor({ type, data }: DataMessageProtobuf) {
    const { elementType, channelType } = getElementAndChannelType(type);
    this.elementType = elementType;
    this.channelType = channelType;
    this.data = data;
  }
}
