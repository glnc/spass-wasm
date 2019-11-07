# spass-wasm
Compile SPASS to WebAssembly using emscripten.

This is a fork of [Classic SPASS: An Automated Theorem Prover for First-Order Logic with Equality](https://www.mpi-inf.mpg.de/departments/automation-of-logic/software/spass-workbench/classic-spass-theorem-prover/) v3.9 by Max Planck Institute for Informatics.

Tested on Ubuntu 18.04 LTS (native and within Windows Subsystem for Linux).

## Prerequisites
* bison
* emscripten
* flex
* make
* browserify (only if you want to build the demo web app)

Follow the instructions on https://kripken.github.io/emscripten-site/docs/getting_started/downloads.html in order to install emscripten and all of its dependencies.

## Building
Run `build.sh` to build SPASS using emscripten as well as a JavaScript wrapper.

Run `build.sh demo` to additionally build a demo web app. You should then serve the contents of /demo using a webserver and open the page with your browser. 

## Usage
SPASS is build to the CommonJS module 'SPASS'. To invoke SPASS it's recommended to use the 'SPASSWrapper' module, since it does some optimization regarding the memory consumption.

```js
SPASSWrapper = require("./SPASSWrapper.js");

// Tell the wrapper where to find the binary. If you do not tell the wrapper where to look, it will try and load "SPASS.wasm" by default.
SPASSWrapper.setWasmPath("SPASS.wasm");

// Initialize the wrapper. This will load the binary, compile it and create the needed Memory.
SPASSWrapper.init();

// let data = ...

// Run SPASS and print the output to the console.
// "data" should be a string with the contents of a file you would run SPASS normaly on.
SPASSWrapper.run(data).then(result=>{
   console.log(result);
});

// If SPASS should be invoked with additional parameters, you can pass them too.
SPASSWrapper.run(data, ["-Flotter"]).then(result=>{
   console.log(result);
});

```

## Original License
Copyright 1996-2010 Max Planck Institute for Informatics. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED "AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE MAX PLANCK INSTITUTE FOR INFORMATICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those of the authors and should not be interpreted as representing official policies, either expressed or implied, of the Max Planck Institute for Informatics.
