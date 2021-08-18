#!/bin/bash

builddir="/b/build"
includes="-Iinclude -lpthread -Llib -lSDL2 -lSDL2main"

mkdir -p $builddir
# -DDEBUG
g++ code/posix_asteroids.cpp -g -DDEBUG $includes -o $builddir/asteroids
exit $?