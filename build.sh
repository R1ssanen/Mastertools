#! bin/bash

cmake -DCMAKE_CXX_COMPILER=g++ -GNinja -S engine/ -B engine/build
cmake --build engine/build
