/**
 * @file   unit-fq-store.cc
 *
 * @section LICENSE
 *
 * The MIT License
 *
 * @copyright Copyright (c) 2019 TileDB Inc.
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
 *
 * @section DESCRIPTION
 *
 * Tests for FastQ ingestion.
 */

#include "catch.hpp"

#include "write/writer.h"

#include <cstring>
#include <fstream>
#include <iostream>

using namespace tiledb::fq;

static const std::string input_dir = TILEDB_FASTQ_TEST_INPUT_DIR;

TEST_CASE("TileDB-FastQ: Test create", "[tiledbfq][ingest]") {
  tiledb::Context ctx;
  tiledb::VFS vfs(ctx);

  std::string dataset_uri = "test_dataset";
  if (vfs.is_dir(dataset_uri))
    vfs.remove_dir(dataset_uri);

  IngestionParams params;
  params.input_uri = input_dir + "/SRR062641.filt.fastq.gz";
  params.uri = dataset_uri;
  Writer writer;
  writer.set_all_params(params);
  writer.ingest();

  if (vfs.is_dir(dataset_uri))
    vfs.remove_dir(dataset_uri);
}