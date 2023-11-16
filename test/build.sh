#
#  build.sh
#  Core Test
#
#  Created by Matt Hartley on 12/11/2023.
#  Copyright 2023 GiantJelly. All rights reserved.
#

mkdir -p build

core_dir="${USERPROFILE}/code/core"

options="-fdiagnostics-color=always -Wno-incompatible-pointer-types -g"
gcc $options $core_dir/test/memory_test.c $core_dir/core/core.c $core_dir/core/video_win32.c -o build/test.exe -I.. -lgdi32 -lopengl32
