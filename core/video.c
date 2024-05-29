//
//  video.c
//  Core
//
//  Created by Matt Hartley on 29/05/2024.
//  Copyright 2024 GiantJelly. All rights reserved.
//

#include "video.h"

// #define __LINUX__

#if defined(__SDL__)
#	include "video_sdl.c"
#elif defined(__WIN32__)
#	include "video_win32.c"
#elif defined(__LINUX__)
#	include "video_linux.c"
#elif defined(__MACOS__)
#	include "video_macos.c"
#else
#   error "NO PLATFORM IS DEFINED"
#endif