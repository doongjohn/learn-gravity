set -e

mkdir -p build
cd build

cmake \
	-DCMAKE_C_COMPILER=gcc \
	-DCMAKE_C_FLAGS='' \
  ..

make
