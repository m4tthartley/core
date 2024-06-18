#
#  build.sh
#  Core Font
#
#  Created by Matt Hartley on 29/09/2023.
#  Copyright 2023 GiantJelly. All rights reserved.
#

clang fonter.c -o ../bin/fonter -g -I$HOME/code/core -framework OpenGL
# if [ $? -eq 0 ]; then
# 	../bin/fonter;
# fi
