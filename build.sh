#!/bin/sh 

CONFIGURATION="release"
BUILD_DIRECTORY="build"
NAUTILUS_EXTENSIONS_DIRECTORY="/usr/lib/x86_64-linux-gnu/nautilus/extensions-3.0"
PLUGIN_PATH=$BUILD_DIRECTORY"/lib/libnautilus-test.so"

cmake -S . -B $BUILD_DIRECTORY -G "Unix Makefiles" -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_BUILD_TYPE:STRING=$CONFIGURATION -DCMAKE_C_COMPILER:FILEPATH=/usr/bin/gcc -DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/g++
cmake --build $BUILD_DIRECTORY --config $CONFIGURATION

echo 'Install libnautilus-test.so to '$NAUTILUS_EXTENSIONS_DIRECTORY
#cmake --install $BUILD_DIRECTORY --prefix $NAUTILUS_EXTENSIONS_DIRE
sudo cp $PLUGIN_PATH $NAUTILUS_EXTENSIONS_DIRECTORY