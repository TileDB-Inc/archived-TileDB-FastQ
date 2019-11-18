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

#include <clipp.h>
#include <tiledb/version.h>
#include <sstream>
#include <thread>

#include "read/reader.h"
#include "utils/utils.h"
#include "write/writer.h"

using namespace tiledb::fq;

namespace {
/** TileDB-FastQ operation mode */
enum class Mode { Version, Store, Export, UNDEF };

/** Returns TileDB-FastQ and TileDB version information in string form. */
std::string version_info() {
  std::stringstream ss;
  ss << "TileDB-FastQ build " << utils::TILEDB_FASTQ_COMMIT_HASH << "\n";
  auto v = tiledb::version();
  ss << "TileDB version " << std::get<0>(v) << "." << std::get<1>(v) << "."
     << std::get<2>(v);
  return ss.str();
}

/** Returns a help string, displaying the given default value. */
template <typename T>
std::string defaulthelp(const std::string& msg, T default_value) {
  return msg + " [default " + std::to_string(default_value) + "]";
}

/** Prints a formatted help message for a command. */
void print_command_usage(
    const std::string& name, const std::string& desc, const clipp::group& cli) {
  using namespace clipp;
  clipp::doc_formatting fmt{};
  fmt.start_column(4).doc_column(25);
  std::cout << name << "\n\nDESCRIPTION\n    " << desc << "\n\nUSAGE\n"
            << usage_lines(cli, name, fmt) << "\n\nOPTIONS\n"
            << documentation(cli, fmt) << "\n";
}

/** Prints the 'store' mode help message. */
void usage_store(const clipp::group& store_mode) {
  print_command_usage(
      "tiledbfq store",
      "Ingests a FastQ file into a TileDB-FastQ array.",
      store_mode);
}

/** Prints the 'export' mode help message. */
void usage_export(const clipp::group& export_mode) {
  print_command_usage(
      "tiledbfq export",
      "Exports data from a TileDB-FastQ array.",
      export_mode);
}

/** Prints the default help message. */
void usage(
    const clipp::group& cli,
    const clipp::group& store_mode,
    const clipp::group& export_mode) {
  using namespace clipp;
  std::cout
      << "TileDB-FastQ -- efficient FastQ data storage and retrieval.\n\n"
      << "This command-line utility provides an interface to create, store and "
         "efficiently retrieve FastQ data in the TileDB storage format."
      << "\n\n"
      << "More information: TileDB <https://tiledb.com>\n"
      << version_info() << "\n\n";

  std::cout << "Summary:\n" << usage_lines(cli, "tiledbfq") << "\n\n\n";
  usage_store(store_mode);
  std::cout << "\n\n";
  usage_export(export_mode);
  std::cout << "\n";
}

/** Store/ingest. */
void do_store(const IngestionParams& args) {
  Writer writer;
  writer.set_all_params(args);
  writer.ingest();
}

/** Export. */
void do_export(const ExportParams& args) {
  Reader reader;
  reader.set_all_params(args);
}

}  // namespace

int main(int argc, char** argv) {
  using namespace clipp;
  Mode opmode = Mode::UNDEF;

  IngestionParams store_args;
  auto store_mode =
      (required("-u", "--uri") % "TileDB-FastQ array URI" &
           value("uri", store_args.uri),
       required("-i", "--input") % "URI of FastQ file to ingest." &
           value("uri", store_args.input_uri),
       option("-v", "--verbose").set(store_args.verbose) %
           "Enable verbose output",
       option("-b", "--mem-budget-mb") %
               defaulthelp(
                   "The memory budget (MB).", store_args.memory_budget_mb) &
           value("MB", store_args.memory_budget_mb));

  ExportParams export_args;
  auto export_mode =
      (required("-u", "--uri") % "TileDB-FastQ array URI" &
           value("uri", export_args.uri),
       option("-o", "--output-path") % "The URI of output file to create." &
           value("path", export_args.output_uri),
       option("-v", "--verbose").set(export_args.verbose) %
           "Enable verbose output",
       option("-b", "--mem-budget-mb") %
               defaulthelp(
                   "The memory budget (MB).", export_args.memory_budget_mb) &
           value("MB", export_args.memory_budget_mb));

  auto cli =
      (command("--version", "-v", "version").set(opmode, Mode::Version) %
           "Prints the version and exits." |
       (command("store").set(opmode, Mode::Store), store_mode) |
       (command("export").set(opmode, Mode::Export), export_mode));

  if (!parse(argc, argv, cli)) {
    if (argc > 1) {
      // Try to print the right help page.
      if (std::string(argv[1]) == "store") {
        usage_store(store_mode);
      } else if (std::string(argv[1]) == "export") {
        usage_export(export_mode);
      } else {
        usage(cli, store_mode, export_mode);
      }
    } else {
      usage(cli, store_mode, export_mode);
    }
    return 1;
  }

  switch (opmode) {
    case Mode::Version:
      std::cout << version_info() << "\n";
      break;
    case Mode::Store:
      do_store(store_args);
      break;
    case Mode::Export:
      do_export(export_args);
      break;
    default:
      usage(cli, store_mode, export_mode);
      return 1;
  }

  return 0;
}
