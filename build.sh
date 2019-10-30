#!/bin/sh

# create needed folders if they dont exist already
mkdir -p release
mkdir -p demo/wasm

# clean demo
rm ./demo/wasm/SPASS.wasm
rm ./demo/bundle.js

# clean
rm ./release/SPASSWrapper.js
cd ./src
make clean

# rebuild
emmake make
cd ..
cp ./src_js/SPASSWrapper.js ./release/SPASSWrapper.js

# if demo, build demo with browserify
if [ -n "$1" -a $1 = "demo" ]
then
    echo "build the demo"
    cp ./src_js/demo.js ./release/demo.js
    # browserify
    browserify ./release/demo.js -o ./demo/bundle.js
    cp ./release/SPASS.wasm  ./demo/wasm/SPASS.wasm
    rm ./release/demo.js
fi