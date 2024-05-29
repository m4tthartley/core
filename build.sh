#
#  build.sh
#  Core
#
#  Created by Matt Hartley on 09/12/2023.
#  Copyright 2023 GiantJelly. All rights reserved.
#

mkdir -p build

files="../core/core.c ../core/video.c ../core/im.c ../core/math.c"
warnings="-wd4047 -wd4024 -wd4133"

pushd build
cl -nologo -Zi -LD $files $warnings -Fecore.dll user32.lib gdi32.lib opengl32.lib -D__SDL__
result=$?
popd

exit $result