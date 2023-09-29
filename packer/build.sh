
mkdir -p build

gcc packer.c -g -o build/packer.exe -I../core -Wno-incompatible-pointer-types
