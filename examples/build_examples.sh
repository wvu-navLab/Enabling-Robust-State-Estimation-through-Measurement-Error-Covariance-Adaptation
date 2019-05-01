#!/bin/bash
CURRDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd && cd .. )"
DIR="$(dirname "$CURRDIR")"

mkdir -p build

#-------------------------------------------------------------------------------------------------
# Build example files
#--------------------------------------------------------------------------------------------------

g++ test_gnss_bce.cpp -std=c++11 -lboost_system -lboost_program_options -ltbb -I"$DIR/include/"  -L"$DIR/lib/" -lcluster -lgpstk -Wno-deprecated-declarations -lgtsam -I"$DIR/3rdparty/Eigen/" -fopenmp -o "$DIR/examples/build/test_gnss_bce"

g++ test_gnss_maxmix.cpp -std=c++11 -lboost_system -lboost_program_options -ltbb -I"$DIR/include/"  -L"$DIR/lib/" -lgpstk -Wno-deprecated-declarations -lgtsam -I"$DIR/3rdparty/Eigen/" -o "$DIR/examples/build/test_gnss_maxmix"

g++ test_gnss_dcs.cpp -std=c++11 -lboost_system -lboost_program_options -ltbb -I"$DIR/include/"  -L"$DIR/lib/" -lgpstk -Wno-deprecated-declarations -lgtsam -I"$DIR/3rdparty/Eigen/" -o "$DIR/examples/build/test_gnss_dcs"

g++ rnx_2_gtsam.cpp -std=c++11 -lboost_system  -lboost_program_options -I"$DIR/include/"  -L"$DIR/lib/"  -lgpstk -Wno-deprecated-declarations -o "$DIR/examples/build/rnx_2_gtsam"
