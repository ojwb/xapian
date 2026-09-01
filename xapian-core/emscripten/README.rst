Building for Emscripten
=======================

Emscripten >= 1.39.10 is required.  Earlier versions had a bug in their
`O_TRUNC` emulation which affects Xapian.  1.39.10 was released in 2020 so
we no longer try to support older Emscripten versions.

Emscripten support has been tested with the glass backend.  The instructions
for building below disable other backends.  They build, but have not been
tested and disabling them reduces the code size.

The following instructions have been tested on Ubuntu 24.04.

Build xapian-core from an unpacked release tarball (or a bootstrapped git
repository):
::

  emconfigure ./configure CPPFLAGS='-DFLINTLOCK_USE_FLOCK' CXXFLAGS='-Oz -s USE_ZLIB=1 -fno-rtti' --disable-shared --disable-backend-honey --disable-backend-inmemory --disable-backend-remote
  emmake make

Test compiling a webassembly binary (from source `xapianjstest.cc`):
::

  em++ -Oz -s USE_ZLIB=1 -s WASM=1 -Iinclude emscripten/xapianjstest.cc .libs/libxapian.a -o emscripten/xapianjstest.js

You can run the resulting binary using nodejs:
::

  cd emscripten
  node xapianjstest.js
