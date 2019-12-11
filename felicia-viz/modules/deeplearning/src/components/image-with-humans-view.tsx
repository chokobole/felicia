// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import {
  HumanBodyMessageProtobuf,
  HumanBodyModelProtobuf,
  HumanBodyProtobuf,
  ImageWithHumansMessage,
} from '@felicia-viz/proto/messages/human';
import { ActivatableFloatPanel, ResizableCanvas } from '@felicia-viz/ui';
import { PanelState } from '@felicia-viz/ui/components/common/activatable-float-panel';
import { FeliciaVizStore } from '@felicia-viz/ui/store';
import ImageWorker, { OutputEvent } from '@felicia-viz/ui/webworkers/image-webworker';
import { inject, observer } from 'mobx-react';
import React, { Component } from 'react';
import ImageWithHumansViewState from '../store/image-with-humans-view-state';

type Pair = [HumanBodyProtobuf, HumanBodyProtobuf];

const BODY_25_PAIRS: Array<Pair> = [
  [HumanBodyProtobuf.HUMAN_BODY_NOSE, HumanBodyProtobuf.HUMAN_BODY_NECK],
  [HumanBodyProtobuf.HUMAN_BODY_NOSE, HumanBodyProtobuf.HUMAN_BODY_R_EYE],
  [HumanBodyProtobuf.HUMAN_BODY_R_EYE, HumanBodyProtobuf.HUMAN_BODY_R_EAR],
  [HumanBodyProtobuf.HUMAN_BODY_NOSE, HumanBodyProtobuf.HUMAN_BODY_L_EYE],
  [HumanBodyProtobuf.HUMAN_BODY_L_EYE, HumanBodyProtobuf.HUMAN_BODY_L_EAR],
  [HumanBodyProtobuf.HUMAN_BODY_NECK, HumanBodyProtobuf.HUMAN_BODY_R_SHOULDER],
  [HumanBodyProtobuf.HUMAN_BODY_R_SHOULDER, HumanBodyProtobuf.HUMAN_BODY_R_ELBOW],
  [HumanBodyProtobuf.HUMAN_BODY_R_ELBOW, HumanBodyProtobuf.HUMAN_BODY_R_WRIST],
  [HumanBodyProtobuf.HUMAN_BODY_NECK, HumanBodyProtobuf.HUMAN_BODY_L_SHOULDER],
  [HumanBodyProtobuf.HUMAN_BODY_L_SHOULDER, HumanBodyProtobuf.HUMAN_BODY_L_ELBOW],
  [HumanBodyProtobuf.HUMAN_BODY_L_ELBOW, HumanBodyProtobuf.HUMAN_BODY_L_WRIST],
  [HumanBodyProtobuf.HUMAN_BODY_NECK, HumanBodyProtobuf.HUMAN_BODY_M_HIP],
  [HumanBodyProtobuf.HUMAN_BODY_M_HIP, HumanBodyProtobuf.HUMAN_BODY_R_HIP],
  [HumanBodyProtobuf.HUMAN_BODY_R_HIP, HumanBodyProtobuf.HUMAN_BODY_R_KNEE],
  [HumanBodyProtobuf.HUMAN_BODY_R_KNEE, HumanBodyProtobuf.HUMAN_BODY_R_ANKLE],
  [HumanBodyProtobuf.HUMAN_BODY_R_ANKLE, HumanBodyProtobuf.HUMAN_BODY_R_BIG_TOE],
  [HumanBodyProtobuf.HUMAN_BODY_R_BIG_TOE, HumanBodyProtobuf.HUMAN_BODY_R_SMALL_TOE],
  [HumanBodyProtobuf.HUMAN_BODY_R_ANKLE, HumanBodyProtobuf.HUMAN_BODY_R_HEEL],
  [HumanBodyProtobuf.HUMAN_BODY_M_HIP, HumanBodyProtobuf.HUMAN_BODY_L_HIP],
  [HumanBodyProtobuf.HUMAN_BODY_L_HIP, HumanBodyProtobuf.HUMAN_BODY_L_KNEE],
  [HumanBodyProtobuf.HUMAN_BODY_L_KNEE, HumanBodyProtobuf.HUMAN_BODY_L_ANKLE],
  [HumanBodyProtobuf.HUMAN_BODY_L_ANKLE, HumanBodyProtobuf.HUMAN_BODY_L_BIG_TOE],
  [HumanBodyProtobuf.HUMAN_BODY_L_BIG_TOE, HumanBodyProtobuf.HUMAN_BODY_L_SMALL_TOE],
  [HumanBodyProtobuf.HUMAN_BODY_L_ANKLE, HumanBodyProtobuf.HUMAN_BODY_L_HEEL],
];

const COCO_PAIRS: Array<Pair> = [
  [HumanBodyProtobuf.HUMAN_BODY_NOSE, HumanBodyProtobuf.HUMAN_BODY_NECK],
  [HumanBodyProtobuf.HUMAN_BODY_NOSE, HumanBodyProtobuf.HUMAN_BODY_R_EYE],
  [HumanBodyProtobuf.HUMAN_BODY_R_EYE, HumanBodyProtobuf.HUMAN_BODY_R_EAR],
  [HumanBodyProtobuf.HUMAN_BODY_NOSE, HumanBodyProtobuf.HUMAN_BODY_L_EYE],
  [HumanBodyProtobuf.HUMAN_BODY_L_EYE, HumanBodyProtobuf.HUMAN_BODY_L_EAR],
  [HumanBodyProtobuf.HUMAN_BODY_NECK, HumanBodyProtobuf.HUMAN_BODY_R_SHOULDER],
  [HumanBodyProtobuf.HUMAN_BODY_R_SHOULDER, HumanBodyProtobuf.HUMAN_BODY_R_ELBOW],
  [HumanBodyProtobuf.HUMAN_BODY_R_ELBOW, HumanBodyProtobuf.HUMAN_BODY_R_WRIST],
  [HumanBodyProtobuf.HUMAN_BODY_NECK, HumanBodyProtobuf.HUMAN_BODY_L_SHOULDER],
  [HumanBodyProtobuf.HUMAN_BODY_L_SHOULDER, HumanBodyProtobuf.HUMAN_BODY_L_ELBOW],
  [HumanBodyProtobuf.HUMAN_BODY_L_ELBOW, HumanBodyProtobuf.HUMAN_BODY_L_WRIST],
  [HumanBodyProtobuf.HUMAN_BODY_NECK, HumanBodyProtobuf.HUMAN_BODY_R_HIP],
  [HumanBodyProtobuf.HUMAN_BODY_R_HIP, HumanBodyProtobuf.HUMAN_BODY_R_KNEE],
  [HumanBodyProtobuf.HUMAN_BODY_R_KNEE, HumanBodyProtobuf.HUMAN_BODY_R_ANKLE],
  [HumanBodyProtobuf.HUMAN_BODY_NECK, HumanBodyProtobuf.HUMAN_BODY_L_HIP],
  [HumanBodyProtobuf.HUMAN_BODY_L_HIP, HumanBodyProtobuf.HUMAN_BODY_L_KNEE],
  [HumanBodyProtobuf.HUMAN_BODY_L_KNEE, HumanBodyProtobuf.HUMAN_BODY_L_ANKLE],
];

const MPI_PAIRS: Array<Pair> = [
  [HumanBodyProtobuf.HUMAN_BODY_HEAD, HumanBodyProtobuf.HUMAN_BODY_NECK],
  [HumanBodyProtobuf.HUMAN_BODY_NECK, HumanBodyProtobuf.HUMAN_BODY_R_SHOULDER],
  [HumanBodyProtobuf.HUMAN_BODY_R_SHOULDER, HumanBodyProtobuf.HUMAN_BODY_R_ELBOW],
  [HumanBodyProtobuf.HUMAN_BODY_R_ELBOW, HumanBodyProtobuf.HUMAN_BODY_R_WRIST],
  [HumanBodyProtobuf.HUMAN_BODY_NECK, HumanBodyProtobuf.HUMAN_BODY_L_SHOULDER],
  [HumanBodyProtobuf.HUMAN_BODY_L_SHOULDER, HumanBodyProtobuf.HUMAN_BODY_L_ELBOW],
  [HumanBodyProtobuf.HUMAN_BODY_L_ELBOW, HumanBodyProtobuf.HUMAN_BODY_L_WRIST],
  [HumanBodyProtobuf.HUMAN_BODY_NECK, HumanBodyProtobuf.HUMAN_BODY_CHEST],
  [HumanBodyProtobuf.HUMAN_BODY_CHEST, HumanBodyProtobuf.HUMAN_BODY_R_HIP],
  [HumanBodyProtobuf.HUMAN_BODY_R_HIP, HumanBodyProtobuf.HUMAN_BODY_R_KNEE],
  [HumanBodyProtobuf.HUMAN_BODY_R_KNEE, HumanBodyProtobuf.HUMAN_BODY_R_ANKLE],
  [HumanBodyProtobuf.HUMAN_BODY_CHEST, HumanBodyProtobuf.HUMAN_BODY_L_HIP],
  [HumanBodyProtobuf.HUMAN_BODY_L_HIP, HumanBodyProtobuf.HUMAN_BODY_L_KNEE],
  [HumanBodyProtobuf.HUMAN_BODY_L_KNEE, HumanBodyProtobuf.HUMAN_BODY_L_ANKLE],
];

const LEFT_HAND_PAIRS: Array<Pair> = [
  [HumanBodyProtobuf.HUMAN_BODY_L_HAND, HumanBodyProtobuf.HUMAN_BODY_L_THUMB_1_CMC],
  [HumanBodyProtobuf.HUMAN_BODY_L_THUMB_1_CMC, HumanBodyProtobuf.HUMAN_BODY_L_THUMB_2_KNUCKLES],
  [HumanBodyProtobuf.HUMAN_BODY_L_THUMB_2_KNUCKLES, HumanBodyProtobuf.HUMAN_BODY_L_THUMB_3_IP],
  [HumanBodyProtobuf.HUMAN_BODY_L_THUMB_3_IP, HumanBodyProtobuf.HUMAN_BODY_L_THUMB_4_FINGER_TIP],
  [HumanBodyProtobuf.HUMAN_BODY_L_HAND, HumanBodyProtobuf.HUMAN_BODY_L_INDEX_1_KNUCKLES],
  [HumanBodyProtobuf.HUMAN_BODY_L_INDEX_1_KNUCKLES, HumanBodyProtobuf.HUMAN_BODY_L_INDEX_2_PIP],
  [HumanBodyProtobuf.HUMAN_BODY_L_INDEX_2_PIP, HumanBodyProtobuf.HUMAN_BODY_L_INDEX_3_DIP],
  [HumanBodyProtobuf.HUMAN_BODY_L_INDEX_3_DIP, HumanBodyProtobuf.HUMAN_BODY_L_INDEX_4_FINGER_TIP],
  [HumanBodyProtobuf.HUMAN_BODY_L_HAND, HumanBodyProtobuf.HUMAN_BODY_L_MIDDLE_1_KNUCKLES],
  [HumanBodyProtobuf.HUMAN_BODY_L_MIDDLE_1_KNUCKLES, HumanBodyProtobuf.HUMAN_BODY_L_MIDDLE_2_PIP],
  [HumanBodyProtobuf.HUMAN_BODY_L_MIDDLE_2_PIP, HumanBodyProtobuf.HUMAN_BODY_L_MIDDLE_3_DIP],
  [HumanBodyProtobuf.HUMAN_BODY_L_MIDDLE_3_DIP, HumanBodyProtobuf.HUMAN_BODY_L_MIDDLE_4_FINGER_TIP],
  [HumanBodyProtobuf.HUMAN_BODY_L_HAND, HumanBodyProtobuf.HUMAN_BODY_L_RING_1_KNUCKLES],
  [HumanBodyProtobuf.HUMAN_BODY_L_RING_1_KNUCKLES, HumanBodyProtobuf.HUMAN_BODY_L_RING_2_PIP],
  [HumanBodyProtobuf.HUMAN_BODY_L_RING_2_PIP, HumanBodyProtobuf.HUMAN_BODY_L_RING_3_DIP],
  [HumanBodyProtobuf.HUMAN_BODY_L_RING_3_DIP, HumanBodyProtobuf.HUMAN_BODY_L_RING_4_FINGER_TIP],
  [HumanBodyProtobuf.HUMAN_BODY_L_HAND, HumanBodyProtobuf.HUMAN_BODY_L_PINKY_1_KNUCKLES],
  [HumanBodyProtobuf.HUMAN_BODY_L_PINKY_1_KNUCKLES, HumanBodyProtobuf.HUMAN_BODY_L_PINKY_2_PIP],
  [HumanBodyProtobuf.HUMAN_BODY_L_PINKY_2_PIP, HumanBodyProtobuf.HUMAN_BODY_L_PINKY_3_DIP],
  [HumanBodyProtobuf.HUMAN_BODY_L_PINKY_3_DIP, HumanBodyProtobuf.HUMAN_BODY_L_PINKY_4_FINGER_TIP],
];

const RIGHT_HAND_PAIRS: Array<Pair> = [
  [HumanBodyProtobuf.HUMAN_BODY_R_HAND, HumanBodyProtobuf.HUMAN_BODY_R_THUMB_1_CMC],
  [HumanBodyProtobuf.HUMAN_BODY_R_THUMB_1_CMC, HumanBodyProtobuf.HUMAN_BODY_R_THUMB_2_KNUCKLES],
  [HumanBodyProtobuf.HUMAN_BODY_R_THUMB_2_KNUCKLES, HumanBodyProtobuf.HUMAN_BODY_R_THUMB_3_IP],
  [HumanBodyProtobuf.HUMAN_BODY_R_THUMB_3_IP, HumanBodyProtobuf.HUMAN_BODY_R_THUMB_4_FINGER_TIP],
  [HumanBodyProtobuf.HUMAN_BODY_R_HAND, HumanBodyProtobuf.HUMAN_BODY_R_INDEX_1_KNUCKLES],
  [HumanBodyProtobuf.HUMAN_BODY_R_INDEX_1_KNUCKLES, HumanBodyProtobuf.HUMAN_BODY_R_INDEX_2_PIP],
  [HumanBodyProtobuf.HUMAN_BODY_R_INDEX_2_PIP, HumanBodyProtobuf.HUMAN_BODY_R_INDEX_3_DIP],
  [HumanBodyProtobuf.HUMAN_BODY_R_INDEX_3_DIP, HumanBodyProtobuf.HUMAN_BODY_R_INDEX_4_FINGER_TIP],
  [HumanBodyProtobuf.HUMAN_BODY_R_HAND, HumanBodyProtobuf.HUMAN_BODY_R_MIDDLE_1_KNUCKLES],
  [HumanBodyProtobuf.HUMAN_BODY_R_MIDDLE_1_KNUCKLES, HumanBodyProtobuf.HUMAN_BODY_R_MIDDLE_2_PIP],
  [HumanBodyProtobuf.HUMAN_BODY_R_MIDDLE_2_PIP, HumanBodyProtobuf.HUMAN_BODY_R_MIDDLE_3_DIP],
  [HumanBodyProtobuf.HUMAN_BODY_R_MIDDLE_3_DIP, HumanBodyProtobuf.HUMAN_BODY_R_MIDDLE_4_FINGER_TIP],
  [HumanBodyProtobuf.HUMAN_BODY_R_HAND, HumanBodyProtobuf.HUMAN_BODY_R_RING_1_KNUCKLES],
  [HumanBodyProtobuf.HUMAN_BODY_R_RING_1_KNUCKLES, HumanBodyProtobuf.HUMAN_BODY_R_RING_2_PIP],
  [HumanBodyProtobuf.HUMAN_BODY_R_RING_2_PIP, HumanBodyProtobuf.HUMAN_BODY_R_RING_3_DIP],
  [HumanBodyProtobuf.HUMAN_BODY_R_RING_3_DIP, HumanBodyProtobuf.HUMAN_BODY_R_RING_4_FINGER_TIP],
  [HumanBodyProtobuf.HUMAN_BODY_R_HAND, HumanBodyProtobuf.HUMAN_BODY_R_PINKY_1_KNUCKLES],
  [HumanBodyProtobuf.HUMAN_BODY_R_PINKY_1_KNUCKLES, HumanBodyProtobuf.HUMAN_BODY_R_PINKY_2_PIP],
  [HumanBodyProtobuf.HUMAN_BODY_R_PINKY_2_PIP, HumanBodyProtobuf.HUMAN_BODY_R_PINKY_3_DIP],
  [HumanBodyProtobuf.HUMAN_BODY_R_PINKY_3_DIP, HumanBodyProtobuf.HUMAN_BODY_R_PINKY_4_FINGER_TIP],
];

const FACE_PAIRS: Array<Pair> = [
  [HumanBodyProtobuf.HUMAN_BODY_FACE_COUNTOUR_0, HumanBodyProtobuf.HUMAN_BODY_FACE_COUNTOUR_1],
  [HumanBodyProtobuf.HUMAN_BODY_FACE_COUNTOUR_1, HumanBodyProtobuf.HUMAN_BODY_FACE_COUNTOUR_2],
  [HumanBodyProtobuf.HUMAN_BODY_FACE_COUNTOUR_2, HumanBodyProtobuf.HUMAN_BODY_FACE_COUNTOUR_3],
  [HumanBodyProtobuf.HUMAN_BODY_FACE_COUNTOUR_3, HumanBodyProtobuf.HUMAN_BODY_FACE_COUNTOUR_4],
  [HumanBodyProtobuf.HUMAN_BODY_FACE_COUNTOUR_4, HumanBodyProtobuf.HUMAN_BODY_FACE_COUNTOUR_5],
  [HumanBodyProtobuf.HUMAN_BODY_FACE_COUNTOUR_5, HumanBodyProtobuf.HUMAN_BODY_FACE_COUNTOUR_6],
  [HumanBodyProtobuf.HUMAN_BODY_FACE_COUNTOUR_6, HumanBodyProtobuf.HUMAN_BODY_FACE_COUNTOUR_7],
  [HumanBodyProtobuf.HUMAN_BODY_FACE_COUNTOUR_7, HumanBodyProtobuf.HUMAN_BODY_FACE_COUNTOUR_8],
  [HumanBodyProtobuf.HUMAN_BODY_FACE_COUNTOUR_8, HumanBodyProtobuf.HUMAN_BODY_FACE_COUNTOUR_9],
  [HumanBodyProtobuf.HUMAN_BODY_FACE_COUNTOUR_9, HumanBodyProtobuf.HUMAN_BODY_FACE_COUNTOUR_10],
  [HumanBodyProtobuf.HUMAN_BODY_FACE_COUNTOUR_10, HumanBodyProtobuf.HUMAN_BODY_FACE_COUNTOUR_11],
  [HumanBodyProtobuf.HUMAN_BODY_FACE_COUNTOUR_11, HumanBodyProtobuf.HUMAN_BODY_FACE_COUNTOUR_12],
  [HumanBodyProtobuf.HUMAN_BODY_FACE_COUNTOUR_12, HumanBodyProtobuf.HUMAN_BODY_FACE_COUNTOUR_13],
  [HumanBodyProtobuf.HUMAN_BODY_FACE_COUNTOUR_13, HumanBodyProtobuf.HUMAN_BODY_FACE_COUNTOUR_14],
  [HumanBodyProtobuf.HUMAN_BODY_FACE_COUNTOUR_14, HumanBodyProtobuf.HUMAN_BODY_FACE_COUNTOUR_15],
  [HumanBodyProtobuf.HUMAN_BODY_FACE_COUNTOUR_15, HumanBodyProtobuf.HUMAN_BODY_FACE_COUNTOUR_16],
  [HumanBodyProtobuf.HUMAN_BODY_R_EYE_BROW_0, HumanBodyProtobuf.HUMAN_BODY_R_EYE_BROW_1],
  [HumanBodyProtobuf.HUMAN_BODY_R_EYE_BROW_1, HumanBodyProtobuf.HUMAN_BODY_R_EYE_BROW_2],
  [HumanBodyProtobuf.HUMAN_BODY_R_EYE_BROW_2, HumanBodyProtobuf.HUMAN_BODY_R_EYE_BROW_3],
  [HumanBodyProtobuf.HUMAN_BODY_R_EYE_BROW_3, HumanBodyProtobuf.HUMAN_BODY_R_EYE_BROW_4],
  [HumanBodyProtobuf.HUMAN_BODY_L_EYE_BROW_4, HumanBodyProtobuf.HUMAN_BODY_L_EYE_BROW_3],
  [HumanBodyProtobuf.HUMAN_BODY_L_EYE_BROW_3, HumanBodyProtobuf.HUMAN_BODY_L_EYE_BROW_2],
  [HumanBodyProtobuf.HUMAN_BODY_L_EYE_BROW_2, HumanBodyProtobuf.HUMAN_BODY_L_EYE_BROW_1],
  [HumanBodyProtobuf.HUMAN_BODY_L_EYE_BROW_1, HumanBodyProtobuf.HUMAN_BODY_L_EYE_BROW_0],
  [HumanBodyProtobuf.HUMAN_BODY_NOSE_UPPER_0, HumanBodyProtobuf.HUMAN_BODY_NOSE_UPPER_1],
  [HumanBodyProtobuf.HUMAN_BODY_NOSE_UPPER_1, HumanBodyProtobuf.HUMAN_BODY_NOSE_UPPER_2],
  [HumanBodyProtobuf.HUMAN_BODY_NOSE_UPPER_2, HumanBodyProtobuf.HUMAN_BODY_NOSE_UPPER_3],
  [HumanBodyProtobuf.HUMAN_BODY_NOSE_LOWER_0, HumanBodyProtobuf.HUMAN_BODY_NOSE_LOWER_1],
  [HumanBodyProtobuf.HUMAN_BODY_NOSE_LOWER_1, HumanBodyProtobuf.HUMAN_BODY_NOSE_LOWER_2],
  [HumanBodyProtobuf.HUMAN_BODY_NOSE_LOWER_2, HumanBodyProtobuf.HUMAN_BODY_NOSE_LOWER_3],
  [HumanBodyProtobuf.HUMAN_BODY_NOSE_LOWER_3, HumanBodyProtobuf.HUMAN_BODY_NOSE_LOWER_4],
  [HumanBodyProtobuf.HUMAN_BODY_R_EYE_0, HumanBodyProtobuf.HUMAN_BODY_R_EYE_1],
  [HumanBodyProtobuf.HUMAN_BODY_R_EYE_1, HumanBodyProtobuf.HUMAN_BODY_R_EYE_2],
  [HumanBodyProtobuf.HUMAN_BODY_R_EYE_2, HumanBodyProtobuf.HUMAN_BODY_R_EYE_3],
  [HumanBodyProtobuf.HUMAN_BODY_R_EYE_3, HumanBodyProtobuf.HUMAN_BODY_R_EYE_4],
  [HumanBodyProtobuf.HUMAN_BODY_R_EYE_4, HumanBodyProtobuf.HUMAN_BODY_R_EYE_5],
  [HumanBodyProtobuf.HUMAN_BODY_R_EYE_5, HumanBodyProtobuf.HUMAN_BODY_R_EYE_0],
  [HumanBodyProtobuf.HUMAN_BODY_L_EYE_0, HumanBodyProtobuf.HUMAN_BODY_L_EYE_1],
  [HumanBodyProtobuf.HUMAN_BODY_L_EYE_1, HumanBodyProtobuf.HUMAN_BODY_L_EYE_2],
  [HumanBodyProtobuf.HUMAN_BODY_L_EYE_2, HumanBodyProtobuf.HUMAN_BODY_L_EYE_3],
  [HumanBodyProtobuf.HUMAN_BODY_L_EYE_3, HumanBodyProtobuf.HUMAN_BODY_L_EYE_4],
  [HumanBodyProtobuf.HUMAN_BODY_L_EYE_4, HumanBodyProtobuf.HUMAN_BODY_L_EYE_5],
  [HumanBodyProtobuf.HUMAN_BODY_L_EYE_5, HumanBodyProtobuf.HUMAN_BODY_L_EYE_0],
  [HumanBodyProtobuf.HUMAN_BODY_O_MOUSE_0, HumanBodyProtobuf.HUMAN_BODY_O_MOUSE_1],
  [HumanBodyProtobuf.HUMAN_BODY_O_MOUSE_1, HumanBodyProtobuf.HUMAN_BODY_O_MOUSE_2],
  [HumanBodyProtobuf.HUMAN_BODY_O_MOUSE_2, HumanBodyProtobuf.HUMAN_BODY_O_MOUSE_3],
  [HumanBodyProtobuf.HUMAN_BODY_O_MOUSE_3, HumanBodyProtobuf.HUMAN_BODY_O_MOUSE_4],
  [HumanBodyProtobuf.HUMAN_BODY_O_MOUSE_4, HumanBodyProtobuf.HUMAN_BODY_O_MOUSE_5],
  [HumanBodyProtobuf.HUMAN_BODY_O_MOUSE_5, HumanBodyProtobuf.HUMAN_BODY_O_MOUSE_6],
  [HumanBodyProtobuf.HUMAN_BODY_O_MOUSE_6, HumanBodyProtobuf.HUMAN_BODY_O_MOUSE_7],
  [HumanBodyProtobuf.HUMAN_BODY_O_MOUSE_7, HumanBodyProtobuf.HUMAN_BODY_O_MOUSE_8],
  [HumanBodyProtobuf.HUMAN_BODY_O_MOUSE_8, HumanBodyProtobuf.HUMAN_BODY_O_MOUSE_9],
  [HumanBodyProtobuf.HUMAN_BODY_O_MOUSE_9, HumanBodyProtobuf.HUMAN_BODY_O_MOUSE_10],
  [HumanBodyProtobuf.HUMAN_BODY_O_MOUSE_10, HumanBodyProtobuf.HUMAN_BODY_O_MOUSE_11],
  [HumanBodyProtobuf.HUMAN_BODY_O_MOUSE_11, HumanBodyProtobuf.HUMAN_BODY_O_MOUSE_0],
  [HumanBodyProtobuf.HUMAN_BODY_I_MOUSE_0, HumanBodyProtobuf.HUMAN_BODY_I_MOUSE_1],
  [HumanBodyProtobuf.HUMAN_BODY_I_MOUSE_1, HumanBodyProtobuf.HUMAN_BODY_I_MOUSE_2],
  [HumanBodyProtobuf.HUMAN_BODY_I_MOUSE_2, HumanBodyProtobuf.HUMAN_BODY_I_MOUSE_3],
  [HumanBodyProtobuf.HUMAN_BODY_I_MOUSE_3, HumanBodyProtobuf.HUMAN_BODY_I_MOUSE_4],
  [HumanBodyProtobuf.HUMAN_BODY_I_MOUSE_4, HumanBodyProtobuf.HUMAN_BODY_I_MOUSE_5],
  [HumanBodyProtobuf.HUMAN_BODY_I_MOUSE_5, HumanBodyProtobuf.HUMAN_BODY_I_MOUSE_6],
  [HumanBodyProtobuf.HUMAN_BODY_I_MOUSE_6, HumanBodyProtobuf.HUMAN_BODY_I_MOUSE_7],
  [HumanBodyProtobuf.HUMAN_BODY_I_MOUSE_7, HumanBodyProtobuf.HUMAN_BODY_I_MOUSE_0],
];

const BODY_COLORS: Array<[number, number, number]> = new Array(
  HumanBodyProtobuf.HUMAN_BODY_L_PUPIL + 1
);
BODY_COLORS.fill([255, 255, 255]);
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_NOSE] = [255, 0, 85];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_NECK] = [255, 0, 0];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_R_SHOULDER] = [255, 85, 0];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_R_ELBOW] = [255, 170, 0];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_R_WRIST] = [255, 255, 0];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_L_SHOULDER] = [170, 255, 0];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_L_ELBOW] = [85, 255, 0];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_L_WRIST] = [0, 255, 0];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_M_HIP] = [255, 0, 0];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_R_HIP] = [0, 255, 85];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_R_KNEE] = [0, 255, 170];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_R_ANKLE] = [0, 255, 255];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_L_HIP] = [0, 170, 255];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_L_KNEE] = [0, 85, 255];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_L_ANKLE] = [0, 0, 255];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_R_EYE] = [255, 0, 170];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_L_EYE] = [170, 0, 255];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_R_EAR] = [255, 0, 255];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_L_EAR] = [85, 0, 255];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_L_BIG_TOE] = [0, 0, 255];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_L_SMALL_TOE] = [0, 0, 255];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_L_HEEL] = [0, 0, 255];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_R_BIG_TOE] = [0, 255, 255];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_R_SMALL_TOE] = [0, 255, 255];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_R_HEEL] = [0, 255, 255];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_HEAD] = BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_NONE];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_CHEST] = [0, 0, 255];

BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_L_HAND] = [100, 100, 100];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_L_THUMB_1_CMC] = [100, 0, 0];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_L_THUMB_2_KNUCKLES] = [150, 0, 0];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_L_THUMB_3_IP] = [200, 0, 0];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_L_THUMB_4_FINGER_TIP] = [255, 0, 0];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_L_INDEX_1_KNUCKLES] = [100, 100, 0];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_L_INDEX_2_PIP] = [150, 150, 0];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_L_INDEX_3_DIP] = [200, 200, 0];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_L_INDEX_4_FINGER_TIP] = [255, 255, 0];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_L_MIDDLE_1_KNUCKLES] = [0, 100, 50];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_L_MIDDLE_2_PIP] = [0, 150, 75];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_L_MIDDLE_3_DIP] = [0, 200, 100];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_L_MIDDLE_4_FINGER_TIP] = [0, 255, 125];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_L_RING_1_KNUCKLES] = [0, 50, 100];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_L_RING_2_PIP] = [0, 75, 150];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_L_RING_3_DIP] = [0, 100, 200];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_L_RING_4_FINGER_TIP] = [0, 125, 255];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_L_PINKY_1_KNUCKLES] = [100, 0, 100];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_L_PINKY_2_PIP] = [150, 0, 150];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_L_PINKY_3_DIP] = [200, 0, 200];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_L_PINKY_4_FINGER_TIP] = [255, 0, 255];

BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_R_HAND] = BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_L_HAND];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_R_THUMB_1_CMC] =
  BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_L_THUMB_1_CMC];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_R_THUMB_2_KNUCKLES] =
  BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_L_THUMB_2_KNUCKLES];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_R_THUMB_3_IP] =
  BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_L_THUMB_3_IP];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_R_THUMB_4_FINGER_TIP] =
  BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_L_THUMB_4_FINGER_TIP];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_R_INDEX_1_KNUCKLES] =
  BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_L_INDEX_1_KNUCKLES];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_R_INDEX_2_PIP] =
  BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_L_INDEX_2_PIP];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_R_INDEX_3_DIP] =
  BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_L_INDEX_3_DIP];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_R_INDEX_4_FINGER_TIP] =
  BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_L_INDEX_4_FINGER_TIP];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_R_MIDDLE_1_KNUCKLES] =
  BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_L_MIDDLE_1_KNUCKLES];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_R_MIDDLE_2_PIP] =
  BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_L_MIDDLE_2_PIP];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_R_MIDDLE_3_DIP] =
  BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_L_MIDDLE_3_DIP];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_R_MIDDLE_4_FINGER_TIP] =
  BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_L_MIDDLE_4_FINGER_TIP];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_R_RING_1_KNUCKLES] =
  BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_L_RING_1_KNUCKLES];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_R_RING_2_PIP] =
  BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_L_RING_2_PIP];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_R_RING_3_DIP] =
  BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_L_RING_3_DIP];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_R_RING_4_FINGER_TIP] =
  BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_L_RING_4_FINGER_TIP];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_R_PINKY_1_KNUCKLES] =
  BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_L_PINKY_1_KNUCKLES];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_R_PINKY_2_PIP] =
  BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_L_PINKY_2_PIP];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_R_PINKY_3_DIP] =
  BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_L_PINKY_3_DIP];
BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_R_PINKY_4_FINGER_TIP] =
  BODY_COLORS[HumanBodyProtobuf.HUMAN_BODY_L_PINKY_4_FINGER_TIP];

function getKeyPointColor(humanBody: HumanBodyProtobuf): string {
  const [r, g, b] = BODY_COLORS[humanBody];
  return `rgb(${r}, ${g}, ${b})`;
}

interface ImageWithHumansViewImplProps {
  width: string;
  height: string;
  frame: ImageWithHumansMessage | null;
  lineWidth: number;
  radius: number;
  threshold: number;
}

class ImageWithHumansViewImpl extends Component<ImageWithHumansViewImplProps> {
  static defaultProps = {
    frame: null,
    lineWidth: 5,
    radius: 5,
    threshold: 0.5,
  };

  private proxyContext: CanvasRenderingContext2D | null = null;

  componentDidMount(): void {
    this.worker = new ImageWorker();

    this.keypoints = new Array(BODY_COLORS.length);

    this.worker.onmessage = (event: OutputEvent<ImageWithHumansMessage>): void => {
      this._drawImageDataAndHumans(event.data);
    };
  }

  shouldComponentUpdate(nextProps: ImageWithHumansViewImplProps): boolean {
    const { frame, lineWidth, radius, threshold } = this.props;

    if (frame !== nextProps.frame) {
      this._loadImageData(nextProps.frame);
      return true;
    }

    if (
      lineWidth !== nextProps.lineWidth ||
      radius !== nextProps.radius ||
      threshold !== nextProps.threshold
    ) {
      return true;
    }

    return false;
  }

  componentWillUnmount(): void {
    this.worker.terminate();
  }

  private _onCanvasLoad = (
    proxyCavnas: HTMLCanvasElement,
    proxyContext: CanvasRenderingContext2D | null,
    resizableCanvas: ResizableCanvas
  ): void => {
    this.proxyCanvas = proxyCavnas;
    this.proxyContext = proxyContext;
    this.resizableCanvas = resizableCanvas;
  };

  private worker: ImageWorker;

  private keypoints?: Array<HumanBodyMessageProtobuf | null>;

  private proxyCanvas?: HTMLCanvasElement;

  private resizableCanvas?: ResizableCanvas;

  private _loadImageData(frame: ImageWithHumansMessage | null): void {
    if (!(this.proxyContext && this.resizableCanvas)) {
      return;
    }

    if (!frame) {
      this.resizableCanvas.clearRect();
      return;
    }

    const { image, model, humans } = frame;
    const { width, height } = image.size;
    const imageData = this.proxyContext.getImageData(0, 0, width, height);

    this.worker.postMessage(
      {
        imageData,
        image,
        data: {
          humans,
          model,
        },
      },
      [imageData.data.buffer, image.data.buffer]
    );
  }

  private _drawImageDataAndHumans(data: {
    imageData: ImageData;
    data: ImageWithHumansMessage;
  }): void {
    if (!(this.proxyContext && this.proxyCanvas && this.resizableCanvas)) return;

    if (!data) {
      this.resizableCanvas.clearRect();
      return;
    }

    if (!this.keypoints) return;

    const { imageData } = data;
    const { humans, model } = data.data;
    const { width, height } = imageData;
    const { radius, threshold } = this.props;

    this.proxyCanvas.width = width;
    this.proxyCanvas.height = height;

    this.proxyContext.putImageData(imageData, 0, 0);
    let hasHand = false;
    let hasFace = false;
    for (let i = 0; i < humans.length; i += 1) {
      this.keypoints.fill(null);
      const { humanBodies } = humans[i];
      for (let j = 0; j < humanBodies.length; j += 1) {
        const { humanBody, position, score } = humanBodies[j];
        if (score >= threshold) {
          this.keypoints[humanBody] = humanBodies[j];
          const { x, y } = position;
          hasHand =
            hasHand ||
            (humanBody >= HumanBodyProtobuf.HUMAN_BODY_L_HAND &&
              humanBody <= HumanBodyProtobuf.HUMAN_BODY_R_PINKY_4_FINGER_TIP);
          hasFace =
            hasFace ||
            (humanBody >= HumanBodyProtobuf.HUMAN_BODY_FACE_COUNTOUR_0 &&
              humanBody <= HumanBodyProtobuf.HUMAN_BODY_L_PUPIL);
          this.proxyContext.beginPath();
          this.proxyContext.fillStyle = getKeyPointColor(humanBody);
          this.proxyContext.arc(x, y, radius, 0, Math.PI * 2, false);
          this.proxyContext.fill();
        }
      }

      let pairs = null;
      if (model === HumanBodyModelProtobuf.HUMAN_BODY_MODEL_BODY_25) {
        pairs = BODY_25_PAIRS;
      } else if (model === HumanBodyModelProtobuf.HUMAN_BODY_MODEL_COCO) {
        pairs = COCO_PAIRS;
      } else if (model === HumanBodyModelProtobuf.HUMAN_BODY_MODEL_MPI) {
        pairs = MPI_PAIRS;
      } else {
        console.error(`Unknown model ${model}`);
      }
      if (pairs) {
        this._connectPairs(pairs);
      }
      if (hasHand) {
        this._connectPairs(LEFT_HAND_PAIRS);
        this._connectPairs(RIGHT_HAND_PAIRS);
      }
      if (hasFace) {
        this._connectPairs(FACE_PAIRS);
      }
    }

    this.resizableCanvas.update();
  }

  private _connectPairs(pairs: Array<Pair>): void {
    if (!this.proxyContext) return;
    if (!this.keypoints) return;

    const { lineWidth } = this.props;

    for (let j = 0; j < pairs.length; j += 1) {
      const [bodyIdx, body2Idx] = pairs[j];
      const body = this.keypoints[bodyIdx];
      const body2 = this.keypoints[body2Idx];
      if (body && body2) {
        this.proxyContext.beginPath();
        this.proxyContext.strokeStyle = getKeyPointColor(body2.humanBody);
        this.proxyContext.lineWidth = lineWidth;
        this.proxyContext.moveTo(body.position.x, body.position.y);
        this.proxyContext.lineTo(body2.position.x, body2.position.y);
        this.proxyContext.stroke();
      }
    }
  }

  render(): JSX.Element {
    const { width, height } = this.props;

    return <ResizableCanvas width={width} height={height} onCanvasLoad={this._onCanvasLoad} />;
  }
}

export interface Props {
  id: number;
  store?: FeliciaVizStore;
}

export interface State {
  width: string;
  height: string;
}

@inject('store')
@observer
export default class ImageWithHumansView extends Component<Props, State> {
  state = {
    width: '100%',
    height: 'auto',
  };

  private _onResize = (panelState: PanelState): void => {
    const { width, height } = this.state;
    const w = `${panelState.width}px`;
    const h = `${panelState.height}px`;
    if (width === w && height === h) return;
    this.setState({ width: w, height: h });
  };

  render(): JSX.Element | null {
    const { id, store } = this.props;
    const { width, height } = this.state;
    if (!store) return null;
    const { uiState } = store;
    const viewState = uiState.findView(id) as ImageWithHumansViewState;
    const { frame, threshold } = viewState;

    return (
      <ActivatableFloatPanel
        id={id}
        type='ImageWithHumansView'
        uiState={uiState}
        onUpdate={this._onResize}>
        <ImageWithHumansViewImpl
          width={width}
          height={height}
          frame={frame}
          threshold={threshold}
        />
      </ActivatableFloatPanel>
    );
  }
}
