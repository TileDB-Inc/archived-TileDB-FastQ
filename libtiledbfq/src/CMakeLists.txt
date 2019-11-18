message(STATUS "Starting TileDB-FastQ build.")

############################################################
# Find required dependencies
############################################################

find_package(Clipp_EP REQUIRED)
find_package(TileDB_EP REQUIRED)
find_package(Zlib_EP REQUIRED)

############################################################
# Get source commit hash
############################################################

find_package(Git REQUIRED)

execute_process(
  COMMAND "${GIT_EXECUTABLE}" describe --dirty=-modified --always
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  RESULT_VARIABLE res
  OUTPUT_VARIABLE BUILD_COMMIT_HASH
  ERROR_QUIET
  OUTPUT_STRIP_TRAILING_WHITESPACE)

set_property(GLOBAL APPEND
  PROPERTY CMAKE_CONFIGURE_DEPENDS
  "${CMAKE_SOURCE_DIR}/.git/index")

message(STATUS "Building with commit hash ${BUILD_COMMIT_HASH}")

############################################################
# Common object library
############################################################

set(TILEDB_FASTQ_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/utils/bitmap.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/utils/buffer.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/utils/utils.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/read/reader.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/write/fqfile.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/write/writer.cc
)

add_library(TILEDB_FASTQ_OBJECTS OBJECT
  ${TILEDB_FASTQ_SOURCES}
)

set_property(TARGET TILEDB_FASTQ_OBJECTS PROPERTY POSITION_INDEPENDENT_CODE ON)

target_include_directories(TILEDB_FASTQ_OBJECTS
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    $<TARGET_PROPERTY:TileDB::tiledb_shared,INTERFACE_INCLUDE_DIRECTORIES>
)

target_compile_definitions(TILEDB_FASTQ_OBJECTS PRIVATE
  -DBUILD_COMMIT_HASH="${BUILD_COMMIT_HASH}"
)

############################################################
# tiledbfq library target
############################################################

add_library(tiledbfq SHARED
  $<TARGET_OBJECTS:TILEDB_FASTQ_OBJECTS>
)

target_link_libraries(tiledbfq
  PUBLIC
    TileDB::tiledb_shared
    Zlib::Zlib
)

# List of API headers (to be installed)
# set(TILEDB_FASTQ_PUBLIC_HEADERS
#   ${CMAKE_CURRENT_SOURCE_DIR}/c_api/tiledbfq.h
#   ${CMAKE_CURRENT_SOURCE_DIR}/c_api/tiledbfq_enum.h
# )

############################################################
# CLI executable target
############################################################

add_executable(tiledbfq-bin
  $<TARGET_OBJECTS:TILEDB_FASTQ_OBJECTS>
  ${CMAKE_CURRENT_SOURCE_DIR}/cli/tiledbfq.cc
)

set_target_properties(tiledbfq-bin PROPERTIES OUTPUT_NAME tiledbfq)

target_link_libraries(tiledbfq-bin
  PUBLIC
    Clipp::Clipp
    TileDB::tiledb_shared
    Zlib::Zlib
)

if (NOT APPLE)
  target_link_libraries(tiledbfq-bin PRIVATE pthread)
endif()

target_include_directories(tiledbfq-bin
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

############################################################
# API symbol exports (and public headers for install)
############################################################

include(GenerateExportHeader)

# Generates the file 'tiledbfq_export.h' suitable for the current compiler.
generate_export_header(TILEDB_FASTQ_OBJECTS
  BASE_NAME tiledbfq
)

# Set variables in the parent scope so the tests can reference it.
set(TILEDB_FASTQ_EXPORT_HEADER_NAME "tiledbfq_export.h" PARENT_SCOPE)
set(TILEDB_FASTQ_EXPORT_HEADER_DIR "${CMAKE_CURRENT_BINARY_DIR}" PARENT_SCOPE)
set(TILEDB_FASTQ_EXPORT_HEADER "${CMAKE_CURRENT_BINARY_DIR}/tiledbfq_export.h")
set(TILEDB_FASTQ_EXPORT_HEADER "${TILEDB_FASTQ_EXPORT_HEADER}" PARENT_SCOPE)

# Set related compiler settings
target_compile_definitions(TILEDB_FASTQ_OBJECTS PRIVATE -DTILEDB_FASTQ_OBJECTS_EXPORTS)
target_include_directories(TILEDB_FASTQ_OBJECTS PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

# Add the generated header to the public headers list
list(APPEND TILEDB_FASTQ_PUBLIC_HEADERS
  "${TILEDB_FASTQ_EXPORT_HEADER}"
)

# Set the public headers, which are the ones that get installed.
set_target_properties(tiledbfq
  PROPERTIES
    PUBLIC_HEADER "${TILEDB_FASTQ_PUBLIC_HEADERS}"
)

############################################################
# Installation
############################################################

include(GNUInstallDirs)

# Set rpath to be relative to the .so.
if (APPLE)
  set_target_properties(tiledbfq PROPERTIES INSTALL_RPATH "@loader_path/")
else()
  set_target_properties(tiledbfq PROPERTIES INSTALL_RPATH "$ORIGIN/")
endif()

set_property(
  TARGET tiledbfq-bin
  PROPERTY INSTALL_RPATH
    "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}"
    "${CMAKE_INSTALL_PREFIX}/lib"
)

install(
  TARGETS tiledbfq tiledbfq-bin
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/tiledbfq
)
