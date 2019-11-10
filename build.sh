#!/bin/sh

# create output folders
echo "### Cleaning..."
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
echo "\n### Building release build..."
emmake make

cd ..
cp ./src_js/SPASSWrapper.js ./release/SPASSWrapper.js

# optionally build and run demo web app
if [ -n "$1" -a "$1" = "demo" ]
then
	echo "\n### Building demo web app..."
	cp ./release/SPASS.wasm ./demo/wasm/SPASS.wasm
	browserify ./src_js/demo.js -o ./demo/bundle.js

	echo "\n### Running demo web app..."
	http-server ./demo
fi
