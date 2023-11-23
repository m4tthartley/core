
mkdir -p build

include=$USERPROFILE/code/core
core=$USERPROFILE/code/core/core

core_implementations="$core/core.c "

options="-g -static -I$include -Wno-incompatible-pointer-types"
gcc reloader.c $core_implementations -o ./build/reloader.exe $options

# gcc test.c ../core/audio.cpp -g -static -o ./build/test.dll -shared -I../core -Wno-incompatible-pointer-types -lgdi32 -lopengl32 -lwinmm -lole32

# gcc test.c -g -static -o ./build/test.exe -I../core -Wno-incompatible-pointer-types -lgdi32 -lopengl32

