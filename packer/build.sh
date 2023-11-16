#
#  build.sh
#  Core Packer
#
#  Created by Matt Hartley on 28/09/2023.
#  Copyright 2023 GiantJelly. All rights reserved.
#

mkdir -p build

gcc packer.c -g -o build/packer.exe -I../core -Wno-incompatible-pointer-types
