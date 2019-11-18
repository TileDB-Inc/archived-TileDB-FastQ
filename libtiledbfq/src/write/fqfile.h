/**
 * @section LICENSE
 *
 * The MIT License
 *
 * @copyright Copyright (c) 2019 TileDB, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef TILEDB_FASTQ_FQ_FILE_H
#define TILEDB_FASTQ_FQ_FILE_H

#include <tiledb/context.h>
#include <tiledb/vfs.h>
#include <string>
#include <vector>

#include "utils/buffer.h"

namespace tiledb {
namespace fq {

class FQFile {
 public:
  /** A single "record" in a FastQ file. */
  struct FQRecord {
    std::string header;
    std::string sequence;
    std::string description;
    std::vector<uint8_t> qualities;
  };

  /** Constructor. */
  FQFile();

  /** Unimplemented rule-of-5. */
  FQFile(FQFile&&) = delete;
  FQFile(const FQFile&) = delete;
  FQFile& operator=(FQFile&&) = delete;
  FQFile& operator=(const FQFile&) = delete;

  void open(const std::string& uri);

  bool next_record(FQRecord* record);

 private:
  const size_t file_buffer_bytes_ = 1024;

  std::string uri_;

  size_t file_size_;

  Buffer buffer_;

  size_t buffer_offset_;

  std::unique_ptr<tiledb::Context> ctx_;

  std::unique_ptr<tiledb::VFS> vfs_;

  void parse_record(FQRecord* record);

  bool read_fq_chunk();

  void init_tiledb();

  void decompress_buffer();

  void copy_to_delim(const char* src, char delim, std::string* dest) const;

  void parse_quality_string(
      const std::string& quality_string, std::vector<uint8_t>* result) const;
};

}  // namespace fq
}  // namespace tiledb

#endif  // TILEDB_FASTQ_FQ_FILE_H
