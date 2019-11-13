// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

declare interface NodeModule {
  hot: {
    accept(path: string, fn: () => void, callback?: () => void): void;
  };
}
