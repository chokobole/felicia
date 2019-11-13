// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/* eslint import/prefer-default-export: "off" */
import { RectMessage, RectMessageProtobuf } from './geometry';
import { Color3uMessage, Color3uMessageProtobuf, Image, ImageProtobuf } from './ui';

export const IMAGE_WITH_BOUNDING_BOXES_MESSAGE = 'felicia.ImageWithBoundingBoxesMessage';

export interface BoundingBoxMessageProtobuf {
  color: Color3uMessageProtobuf;
  box: RectMessageProtobuf;
  label: string;
  score: number;
}

export class BoundingBoxMessage {
  color: Color3uMessage;

  box: RectMessage;

  label: string;

  score: number;

  constructor({ color, box, label, score }: BoundingBoxMessageProtobuf) {
    this.color = new Color3uMessage(color);
    this.box = new RectMessage(box);
    this.label = label;
    this.score = score;
  }
}

export interface ImageWithBoundingBoxesMessageProtobuf {
  image: ImageProtobuf;
  boundingBoxes: Array<BoundingBoxMessageProtobuf>;
}

export class ImageWithBoundingBoxesMessage {
  image: Image;

  boundingBoxes: Array<BoundingBoxMessage>;

  constructor({ image, boundingBoxes }: ImageWithBoundingBoxesMessageProtobuf) {
    this.image = new Image(image);
    this.boundingBoxes = boundingBoxes.map(bb => new BoundingBoxMessage(bb));
  }
}
