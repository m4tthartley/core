//
//  Created by Matt Hartley on 07/03/2025.
//  Copyright 2025 GiantJelly. All rights reserved.
//

// TARGET PLATFORM CONDITIONALS


#ifndef __TARGETCONDITIONALS_HEADER__
#define __TARGETCONDITIONALS_HEADER__


#ifdef _WIN32
#	undef __WIN32__
#	define __WIN32__
#endif
#ifdef __linux__
#	define __LINUX__
#	define __POSIX__
#endif
#ifdef __APPLE__
#	define __POSIX__
#	include <TargetConditionals.h>
#	if TARGET_OS_OSX
#		define __MACOS__
#	endif
#	if TARGET_OS_IOS
#		define __IOS__
#	endif
#	if TARGET_OS_SIMULATOR
#		define __SIM__
#	endif
#endif


#endif
