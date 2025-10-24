##
##  Created by Matt Hartley on 08/09/2025.
##  Copyright 2025 GiantJelly. All rights reserved.
##

set -e

mkdir -p ../bin

# COPY SCRIPTS
cp ./coreconfig.sh ../bin/coreconfig
chmod +x ../bin/coreconfig
cp ./commit.sh ../bin/commit
chmod +x ../bin/commit

# FONT PACKER
clang $(coreconfig) fontpacker.c -o ../bin/fontpacker

# BUILDER
options="-std=c99 -g -Wno-deprecated-declarations"
clang builder/builder.c -o ../bin/builder $options $(coreconfig)
