#!/bin/bash

include="-I$HOME/code/core"

if [[ $OSTYPE == "linux-gnu" ]]; then
    libs="-lm"
fi

if [[ $OSTYPE == "darwin" ]]; then
    libs="-framework CoreFoundation -framework CoreServices -framework Cocoa -framework QuartzCore -framework Metal -framework OpenGL"
fi

if [[ $OSTYPE == "msys" ]]; then
    libs=""
fi

echo $include $libs
