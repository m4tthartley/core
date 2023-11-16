#
#  build.sh
#  Core Font
#
#  Created by Matt Hartley on 29/09/2023.
#  Copyright 2023 GiantJelly. All rights reserved.
#

gcc font_generator.c -o ./font_generator.exe -g -Wno-incompatible-pointer-types
if [ $? -eq 0 ]; then
	./font_generator.exe;
fi
