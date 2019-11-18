#
# test/CMakeLists.txt
#
#
# The MIT License
#
# Copyright (c) 2019 TileDB, Inc.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#

find_package(TileDB_EP REQUIRED)
find_package(Catch_EP REQUIRED)

############################################################
# Unit test executable
############################################################

add_executable(tiledb_fq_unit EXCLUDE_FROM_ALL
  ${CMAKE_CURRENT_SOURCE_DIR}/src/unit-bitmap.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/src/unit-fq-store.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/src/unit-fqfile.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/src/unit.cc
)

target_compile_definitions(tiledb_fq_unit
  PRIVATE
    -DTILEDB_FASTQ_TEST_INPUT_DIR="${CMAKE_CURRENT_SOURCE_DIR}/inputs"
)

target_include_directories(tiledb_fq_unit
  PRIVATE
    ${TILEDB_FASTQ_EXPORT_HEADER_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/
)

target_link_libraries(tiledb_fq_unit
  PUBLIC
    tiledbfq
    Catch::Catch
)

add_test(
  NAME "tiledb_fq_unit"
  COMMAND $<TARGET_FILE:tiledb_fq_unit> --durations=yes
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

# Add custom target 'check'
add_custom_target(
  check COMMAND ${CMAKE_CTEST_COMMAND} -V -C ${CMAKE_BUILD_TYPE}
  DEPENDS tiledb_fq_unit
)