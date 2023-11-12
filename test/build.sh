
mkdir -p build

options="-fdiagnostics-color=always -Wno-incompatible-pointer-types -g"
gcc $options memory_test.c -o build/test.exe -I..
