//
//  video.h
//  Core
//
//  Created by Matt Hartley on 29/05/2024.
//  Copyright 2024 GiantJelly. All rights reserved.
//

#ifndef __CORE_VIDEO_HEADER__
#define __CORE_VIDEO_HEADER__


#ifdef __WIN32__
#	include "video_win32.c"
#endif

#ifdef __LINUX__
#	include "video_linux.c"
#endif

#ifdef __MACOS__
#	include "video_macos.c"
#endif


#endif