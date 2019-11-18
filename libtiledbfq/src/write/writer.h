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

#ifndef TILEDB_FASTQ_WRITER_H
#define TILEDB_FASTQ_WRITER_H

#include <tiledb/tiledb>

namespace tiledb {
namespace fq {

/* ********************************* */
/*       AUXILIARY DATATYPES         */
/* ********************************* */

/** Arguments/params for dataset ingestion. */
struct IngestionParams {
  std::string uri;
  std::string input_uri;
  bool verbose = false;
  unsigned memory_budget_mb = 2 * 1024;
};

/* ********************************* */
/*              WRITER               */
/* ********************************* */

class Writer {
 public:
  /** Constructor. */
  Writer();

  /** Unimplemented rule-of-5. */
  Writer(Writer&&) = delete;
  Writer(const Writer&) = delete;
  Writer& operator=(Writer&&) = delete;
  Writer& operator=(const Writer&) = delete;

  void ingest();

  /** Sets all parameters. */
  void set_all_params(const IngestionParams& args);

 private:
  IngestionParams args_;

  std::unique_ptr<tiledb::Context> ctx_;

  void init_tiledb();

  void create_array();

  tiledb::FilterList make_filters(
      const std::initializer_list<tiledb_filter_type_t>& list) const;
};

}  // namespace fq
}  // namespace tiledb

#endif  // TILEDB_FASTQ_WRITER_H
