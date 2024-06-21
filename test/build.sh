#
#  build.sh
#  Core Test
#
#  Created by Matt Hartley on 12/11/2023.
#  Copyright 2023 GiantJelly. All rights reserved.
#

mkdir -p build

# core_dir="~/code/core"

cd ../tools/fonter
../bin/fonter
cd ../../test

options="-std=c99 -g -I~/code/core"

clang $options invaders.c -o build/invaders -I.. -D__SDL__ -framework OpenGL

clang $options scoreserver.c -o build/scoreserver -I..
