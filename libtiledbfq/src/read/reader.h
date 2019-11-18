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

#ifndef TILEDB_FASTQ_READER_H
#define TILEDB_FASTQ_READER_H

#include <future>
#include <map>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include <tiledb/tiledb>

namespace tiledb {
namespace fq {

/* ********************************* */
/*       AUXILIARY DATATYPES         */
/* ********************************* */

/** Arguments/params for export. */
struct ExportParams {
  std::string uri;
  std::string output_uri;
  unsigned memory_budget_mb = 2 * 1024;
  bool verbose = false;
};

/* ********************************* */
/*              READER               */
/* ********************************* */

class Reader {
 public:
  /** Constructor. */
  Reader();

  /** Unimplemented rule-of-5. */
  Reader(Reader&&) = delete;
  Reader(const Reader&) = delete;
  Reader& operator=(Reader&&) = delete;
  Reader& operator=(const Reader&) = delete;

  /** Sets all parameters. */
  void set_all_params(const ExportParams& args);

 private:
  ExportParams args_;
};

}  // namespace fq
}  // namespace tiledb

#endif  // TILEDB_FASTQ_READER_H
