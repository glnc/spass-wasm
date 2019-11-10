# spass-wasm
Compile SPASS to WebAssembly using emscripten.

This is a fork of [Classic SPASS: An Automated Theorem Prover for First-Order Logic with Equality](https://www.mpi-inf.mpg.de/departments/automation-of-logic/software/spass-workbench/classic-spass-theorem-prover/) v3.9 by Max Planck Institute for Informatics.

Tested on Ubuntu 16.04 LTS.

## Prerequisites
### emscripten
Follow the instructions on https://kripken.github.io/emscripten-site/docs/getting_started/downloads.html in order to install emscripten and all of its dependencies.

### For SPASS
* bison
* flex
* make

### For Demo Web App
* [browserify](https://www.npmjs.com/package/browserify)
* [http-server](https://www.npmjs.com/package/http-server)

## Building
Run `build.sh` to compile SPASS using emscripten as well as to build the JavaScript modules (see *Usage*).

Run `build.sh demo` to additionally build and run a demo web app listening on http://localhost:8080. 

## Usage
emscripten generates the CommonJS module `release/SPASS.js`. Additionally there is a wrapper module `release/SPASSWrapper.js` that is recommended to use, since it does some optimization regarding memory consumption.

```js
const SPASS = require("./SPASSWrapper.js");

SPASS.setWASMPATH("SPASS.wasm");

SPASS.run("begin_problem(Sokrates1). [...] end_problem.", ["-DocProof"]).then(result => {
	console.log(result);
}).catch(reason => {
	console.error(reason);
});
```

## Original License
Copyright 1996-2010 Max Planck Institute for Informatics. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED "AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE MAX PLANCK INSTITUTE FOR INFORMATICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those of the authors and should not be interpreted as representing official policies, either expressed or implied, of the Max Planck Institute for Informatics.
