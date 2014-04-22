


Build
======

using clang to avoid some linking problem::

   mkdir build;cd build;
   export CC=clang
   export CXX=clang++
   cmake .. -DCMAKE_BUILD_TYPE=Debug
   make
