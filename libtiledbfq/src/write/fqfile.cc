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

#include <zlib.h>

#include "write/fqfile.h"

namespace tiledb {
namespace fq {

FQFile::FQFile()
    : file_size_(0)
    , buffer_offset_(0) {
}

void FQFile::open(const std::string& uri) {
  init_tiledb();
  if (!vfs_->is_file(uri))
    throw std::runtime_error(
        "Error opening FastQ file '" + uri + "'; file does not exist.");

  uri_ = uri;
  file_size_ = vfs_->file_size(uri);
  buffer_offset_ = 0;

  // TODO: remove:
  read_fq_chunk();
}

bool FQFile::next_record(FQFile::FQRecord* record) {
  if (buffer_offset_ >= buffer_.size())
    return false;

  if (record == nullptr)
    throw std::runtime_error(
        "Error getting next FQ record; output parameter is null");

  parse_record(record);

  return true;
}

void FQFile::parse_record(FQFile::FQRecord* record) {
  size_t offset = buffer_offset_;
  if (buffer_.value<char>(offset) != '@')
    throw std::runtime_error("FastQ parse error; expected '@' to begin record");
  offset += 1;

  record->header.clear();
  copy_to_delim(buffer_.data<char>() + offset, '\n', &record->header);
  offset += record->header.size() + 1;

  record->sequence.clear();
  copy_to_delim(buffer_.data<char>() + offset, '\n', &record->sequence);
  offset += record->sequence.size() + 1;

  if (*(buffer_.data<char>() + offset) != '+')
    throw std::runtime_error("FastQ parse error; expected '+' character");
  offset += 1;
  record->description.clear();
  copy_to_delim(buffer_.data<char>() + offset, '\n', &record->description);
  offset += record->description.size() + 1;

  std::string quality_string;
  copy_to_delim(buffer_.data<char>() + offset, '\n', &quality_string);
  offset += quality_string.size() + 1;

  record->qualities.clear();
  parse_quality_string(quality_string, &record->qualities);

  buffer_offset_ = offset;
}

void FQFile::parse_quality_string(
    const std::string& quality_string, std::vector<uint8_t>* result) const {
  for (char c : quality_string) {
    if (c < '!' || c > '~')
      throw std::runtime_error(
          "FastQ parse error; invalid char '" + std::to_string(c) +
          "' in quality string " + quality_string);
    uint8_t q = (uint8_t)(c - '!');
    result->push_back(q);
  }
}

void FQFile::copy_to_delim(
    const char* src, char delim, std::string* dest) const {
  while (*src != delim && *src != '\0')
    dest->push_back(*src++);
}

bool FQFile::read_fq_chunk() {
  // TODO: read in a chunk that's guaranteed to end on a record boundary.
  //   Currently just buffering the whole file.
  VFS::filebuf filebuf(*vfs_);
  filebuf.open(uri_, std::ios::in);
  std::istream is(&filebuf);
  if (!is.good() || is.fail() || is.bad()) {
    const char* err_c_str = strerror(errno);
    throw std::runtime_error(
        "Cannot read FastQ chunk from '" + uri_ + "'; " +
        std::string(err_c_str));
  }

  buffer_.clear();
  buffer_.resize(file_size_);

  is.read(buffer_.data<char>(), file_size_);
  if (is.bad() || static_cast<uint64_t>(is.gcount()) != file_size_) {
    const char* err_c_str = strerror(errno);
    throw std::runtime_error(
        "Error reading from file '" + uri_ + "'; " + std::string(err_c_str));
  }

  // TODO encapsulate decompression; also go chunk by chunk.
  // Allocate deflate state
  z_stream strm;
  strm.zalloc = Z_NULL;
  strm.zfree = Z_NULL;
  strm.opaque = Z_NULL;
  strm.avail_in = 0;
  strm.next_in = Z_NULL;

  const int window_bits = 32;  // 32 = auto detect header
  if (inflateInit2(&strm, window_bits) != Z_OK)
    throw std::runtime_error("Error decompressing; zlib stream init failed.");

  Buffer output;
  output.reserve(static_cast<size_t>(1.5f * buffer_.size()));

  // Decompress
  const size_t chunk_size = 10 * 1024 * 1024;
  Buffer chunk_buff;
  chunk_buff.resize(chunk_size);

  strm.next_in = buffer_.data<unsigned char>();
  strm.next_out = chunk_buff.data<unsigned char>();
  strm.avail_in = (uInt)buffer_.size();
  strm.avail_out = chunk_size;

  // Repeat until the input stream is exhausted.
  int ret;
  do {
    do {
      strm.next_out = chunk_buff.data<unsigned char>();
      strm.avail_out = chunk_size;

      ret = inflate(&strm, Z_NO_FLUSH);
      switch (ret) {
        case Z_OK:
        case Z_BUF_ERROR:
        case Z_STREAM_END:
          // OK or recoverable errors.
          break;
        default:
          std::string msg(strm.msg);
          inflateEnd(&strm);
          throw std::runtime_error(
              "Error decompressing; zlib inflate failed: " + msg);
      }

      size_t res_bytes = chunk_size - strm.avail_out;
      output.append(chunk_buff.data<void>(), res_bytes);
    } while (strm.avail_out == 0);
  } while (ret != Z_STREAM_END);

  if (inflateEnd(&strm) != Z_OK)
    throw std::runtime_error("Error decompressing; zlib stream deinit failed.");

  output.swap(buffer_);
  buffer_offset_ = 0;

  return true;
}

void FQFile::init_tiledb() {
  if (ctx_ == nullptr)
    ctx_.reset(new tiledb::Context);
  if (vfs_ == nullptr)
    vfs_.reset(new tiledb::VFS(*ctx_));
}

}  // namespace fq
}  // namespace tiledb
