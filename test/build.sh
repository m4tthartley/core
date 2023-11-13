
mkdir -p build

core_dir="${USERPROFILE}/code/core"

options="-fdiagnostics-color=always -Wno-incompatible-pointer-types -g"
gcc $options $core_dir/test/memory_test.c $core_dir/core/core.c -o build/test.exe -I..
