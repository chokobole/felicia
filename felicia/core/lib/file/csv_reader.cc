// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/lib/file/csv_reader.h"

#include "third_party/chromium/base/strings/string_tokenizer.h"

namespace felicia {

CsvReader::CsvReader() : reader_(BufferedReader::REMOVE_CR_OR_LF) {}

Status CsvReader::Open(const base::FilePath& path, const std::string& delimiter,
                       int skip_head) {
  reader_.Close();
  delimiter_ = delimiter;
  Status s = reader_.Open(path);
  if (!s.ok()) return s;
  for (int i = 0; i < skip_head; ++i) {
    reader_.ReadLine();
  }
  return Status::OK();
}

void CsvReader::Close() { reader_.Close(); }

bool CsvReader::IsOpened() const { return reader_.IsOpened(); }

bool CsvReader::ReadRows(std::vector<std::string>* rows) {
  if (rows->size() > 0) return false;
  std::string line;
  reader_.ReadLine(&line);
  base::StringTokenizer t(line, delimiter_);
  while (t.GetNext()) {
    rows->push_back(t.token());
  }
  return true;
}

bool CsvReader::eof() const { return reader_.eof(); }

}  // namespace felicia