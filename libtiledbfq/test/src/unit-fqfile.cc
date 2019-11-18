/**
 * @file   unit-fqfile.cc
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
 */

#include "catch.hpp"

#include "write/fqfile.h"

#include <cstring>
#include <fstream>
#include <iostream>

using namespace tiledb::fq;

static const std::string input_dir = TILEDB_FASTQ_TEST_INPUT_DIR;

TEST_CASE("TileDB-FastQ: Test FQFile", "[tiledbfq][fqfile]") {
  tiledb::Context ctx;
  tiledb::VFS vfs(ctx);

  std::string dataset_uri = "test_dataset";
  if (vfs.is_dir(dataset_uri))
    vfs.remove_dir(dataset_uri);

  FQFile fq;
  fq.open(input_dir + "/SRR062641.filt.fastq.gz");

  FQFile::FQRecord rec;
  REQUIRE(fq.next_record(&rec));
  REQUIRE(rec.header == "SRR062641.1 HWI-EAS110_103327062:5:1:1091:7885/1");
  REQUIRE(
      rec.sequence ==
      "GAAAGAAAGAAAGTCAACTGTATGCTTAAAAATCCAAGTTGTGGGTGGGAAGCTGATTGAATTTTTTACTAC"
      "GGTTCATAAAAAAACACAAGACTCACAT");
  REQUIRE(rec.description == "");
  REQUIRE(
      rec.qualities ==
      std::vector<uint8_t>{
          31, 22, 25, 21, 10, 8,  28, 15, 20, 22, 22, 6, 6, 18, 28, 30, 12,
          29, 29, 26, 25, 32, 29, 32, 2,  2,  2,  2,  2, 2, 2,  2,  2,  2,
          2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 2, 2,  2,  2,  2,
          2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 2, 2,  2,  2,  2,
          2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 2, 2,  2,  2,  2,
          2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 2, 2,  2});

  REQUIRE(fq.next_record(&rec));
  REQUIRE(rec.header == "SRR062641.2 HWI-EAS110_103327062:5:1:1092:19656/1");
  REQUIRE(
      rec.sequence ==
      "AGACTTTATCAAATTATAACTGGGATTAGATAGACAGTCCAGAATAATTTTAATAATTCATGTACATGCTTC"
      "ATGTATTTTCCTTGCTTACACTTGCCTA");
  REQUIRE(rec.description == "");
  REQUIRE(
      rec.qualities ==
      std::vector<uint8_t>{
          28, 31, 29, 31, 21, 30, 29, 32, 26, 32, 32, 30, 20, 30, 35, 35, 33,
          20, 35, 35, 28, 35, 32, 35, 20, 12, 28, 30, 21, 31, 25, 25, 32, 12,
          32, 35, 35, 34, 35, 30, 34, 20, 20, 11, 34, 33, 25, 32, 35, 30, 33,
          35, 25, 20, 35, 35, 30, 33, 35, 28, 25, 20, 34, 31, 20, 34, 2,  2,
          2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
          2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2});

  size_t num_records = 2;
  while (fq.next_record(&rec))
    num_records++;
  REQUIRE(num_records == 109811);

  if (vfs.is_dir(dataset_uri))
    vfs.remove_dir(dataset_uri);
}