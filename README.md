# TileDB-FastQ

## Introduction

TileDB-FastQ is a tool to import and model FastQ data as 1D dense TileDB arrays. This
repository provides a command line tool to import FastQ data, as well as functionality to re-export TileDB-FastQ arrays back to FastQ format.

## Build

TileDB-FastQ must currently be built from source, and is tested on macOS and Linux.

```bash
git clone https://github.com/TileDB-Inc/TileDB-FastQ
cd TileDB-FastQ/libtiledbfq
mkdir build && cd build
cmake -DCMAKE_PREFIX_PATH=/path/to/TileDB .. && make -j16
```

CLI usage:

### Help
```bash
libtiledbfq/build/libtiledbfq/src/tiledbfq --help
```

### Import 

```
tiledbfq store -i <FastQ source uri> -u <TileDB target uri> 
```

### Export

```
tiledbfq export -u <TileDB-FastQ array uri> [-o <output FastQ path>]
```


## Code of Conduct

All participants in TileDB spaces are expected to adhere to high standards of
professionalism in all interactions. This repository is governed by the
specific standards and reporting procedures detailed in depth in the
[TileDB core repository Code Of Conduct](
https://github.com/TileDB-Inc/TileDB/blob/dev/CODE_OF_CONDUCT.md).