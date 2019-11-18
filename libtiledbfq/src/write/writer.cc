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

#include <future>
#include <tiledb/tiledb>

#include "write/fqfile.h"
#include "write/writer.h"

namespace tiledb {
namespace fq {

Writer::Writer() {
}

void Writer::set_all_params(const IngestionParams& args) {
  args_ = args;
}

void Writer::init_tiledb() {
  if (ctx_ == nullptr)
    ctx_.reset(new tiledb::Context);
}

void Writer::ingest() {
  init_tiledb();
  create_array();

  FQFile fq;
  fq.open(args_.input_uri);

  std::vector<char> header;
  std::vector<uint64_t> header_offsets;
  std::vector<char> sequence;
  std::vector<char> description;
  std::vector<uint64_t> description_offsets;
  std::vector<uint8_t> quality;

  FQFile::FQRecord rec;
  uint64_t header_off = 0, desc_off = 0;
  while (fq.next_record(&rec)) {
    header_offsets.push_back(header_off);
    header.insert(header.end(), rec.header.begin(), rec.header.end());
    header_off += rec.header.size();

    sequence.insert(sequence.end(), rec.sequence.begin(), rec.sequence.end());

    if (rec.description.empty())
      rec.description = "-";
    description_offsets.push_back(desc_off);
    description.insert(
        description.end(), rec.description.begin(), rec.description.end());
    desc_off += rec.description.size();

    quality.insert(quality.end(), rec.qualities.begin(), rec.qualities.end());
  }

  tiledb::Array array(*ctx_, args_.uri, TILEDB_WRITE);
  tiledb::Query query(*ctx_, array);
  query.set_subarray(std::array<uint64_t, 2>{0, header_offsets.size() - 1});
  query.set_buffer("header", header_offsets, header);
  query.set_buffer("sequence", sequence);
  query.set_buffer("description", description_offsets, description);
  query.set_buffer("quality", quality);
  query.submit();
}

void Writer::create_array() {
  const uint64_t tile_extent = 100000;
  const uint64_t dom_min = 0, dom_max = std::numeric_limits<uint64_t>::max() -
                                        tile_extent - 1;
  auto dim =
      tiledb::Dimension::create(*ctx_, "d1", {dom_min, dom_max}, tile_extent);
  tiledb::Domain dom(*ctx_);
  dom.add_dimension(dim);

  const int cell_val_num = 100;

  auto header = tiledb::Attribute::create<std::vector<char>>(
      *ctx_, "header", make_filters({TILEDB_FILTER_BZIP2}));
  auto sequence = tiledb::Attribute::create<char>(
      *ctx_, "sequence", make_filters({TILEDB_FILTER_BZIP2}));
  sequence.set_cell_val_num(cell_val_num);
  auto description = tiledb::Attribute::create<std::vector<char>>(
      *ctx_, "description", make_filters({TILEDB_FILTER_BZIP2}));
  auto quality = tiledb::Attribute::create<uint8_t>(
      *ctx_, "quality", make_filters({TILEDB_FILTER_BZIP2}));
  quality.set_cell_val_num(cell_val_num);

  tiledb::ArraySchema schema(*ctx_, TILEDB_DENSE);
  schema.set_domain(dom);
  schema.add_attribute(header)
      .add_attribute(sequence)
      .add_attribute(description)
      .add_attribute(quality);

  tiledb::Array::create(args_.uri, schema);
}

tiledb::FilterList Writer::make_filters(
    const std::initializer_list<tiledb_filter_type_t>& list) const {
  FilterList filters(*ctx_);
  for (auto t : list)
    filters.add_filter(Filter(*ctx_, t));
  return filters;
}

}  // namespace fq
}  // namespace tiledb
