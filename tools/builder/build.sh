#
#  build.sh
#  Core Watch
#
#  Created by Matt Hartley on 02/09/2023.
#  Copyright 2023 GiantJelly. All rights reserved.
#

mkdir -p ../bin
# pushd build

# core="${USERPROFILE}/code/core/core"

# warnings="-Wno-incompatible-pointer-types"
# gcc main.c $core/core.c $core/timer.c -o build/watch_debug.exe -I.. -g $warnings -std=c99

# result=$?

# if [ $result -eq 0 ]; then
# 	echo "Build successful"
# else
# 	echo "Build failed"
# fi

options="-std=c99 -g -I$HOME/code/core"
clang macos.c -o ../bin/builder $options -framework CoreServices


# popd
