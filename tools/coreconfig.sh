#!/bin/sh

include="-I$HOME/code/core"
libs="-framework CoreFoundation -framework Cocoa -framework QuartzCore -framework Metal -framework OpenGL"
echo $include $libs
