# TileDB-FastQ

## Build

```bash
cd TileDB-FastQ/libtiledbfq
mkdir build && cd build
cmake -DCMAKE_PREFIX_PATH=/path/to/TileDB .. && make -j16
```

CLI help:
```bash
libtiledbfq/build/libtiledbfq/src/tiledbfq --help
```