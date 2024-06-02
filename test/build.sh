#
#  build.sh
#  Core Test
#
#  Created by Matt Hartley on 12/11/2023.
#  Copyright 2023 GiantJelly. All rights reserved.
#

mkdir -p build

core_dir="${USERPROFILE}/code/core"

options="-std=c99 -g -I$core_dir -L$core_dir/build"
gcc $options $core_dir/test/invaders.c -o build/invaders.exe -I.. -lgdi32 -lopengl32
