#!/bin/sh

# create output folders
mkdir -p demo/wasm
mkdir -p release

# clean demo web app
rm ./demo/bundle.js
rm ./demo/wasm/SPASS.wasm

# clean release build
rm ./release/SPASSWrapper.js

cd ./src
make clean

# build
emmake make

cd ..
cp ./src_js/SPASSWrapper.js ./release/SPASSWrapper.js

# optionally build demo web app
if [ -n "$1" -a "$1" = "demo" ]
then
	echo "Building demo web app"
	cp ./src_js/demo.js ./release/demo.js
	browserify ./release/demo.js -o ./demo/bundle.js
	cp ./release/SPASS.wasm ./demo/wasm/SPASS.wasm
	rm ./release/demo.js
fi
