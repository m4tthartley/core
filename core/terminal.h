//
//  terminal.h
//  Core
//
//  Created by Matt Hartley on 15/09/2023.
//  Copyright 2023 GiantJelly. All rights reserved.
//

// ANSI ESCAPE SEQUENCES
// typedef enum {
// 	EC_DEFAULT = 0,
// 	EC_BLACK = 30,
// 	EC_BLACK_BG = 40,
// 	EC_RED = 31,
// 	EC_RED_BG = 41,
// 	EC_GREEN = 32,
// 	EC_GREEN_BG = 42,
// 	EC_YELLOW = 33,
// 	EC_YELLOW_BG = 43,
// 	EC_BLUE = 34,
// 	EC_BLUE_BG = 44,
// 	EC_MAGENTA = 35,
// 	EC_MAGENTA_BG = 45,
// 	EC_CYAN = 36,
// 	EC_CYAN_BG = 46,
// 	EC_DARK_WHITE = 37,
// 	EC_DARK_WHITE_BG = 47,
// 	EC_BRIGHT_BLACK = 90,
// 	EC_BRIGHT_BLACK_BG = 100,
// 	EC_BRIGHT_RED = 91,
// 	EC_BRIGHT_RED_BG = 101,
// 	EC_BRIGHT_GREEN = 92,
// 	EC_BRIGHT_GREEN_BG = 102,
// 	EC_BRIGHT_YELLOW = 93,
// 	EC_BRIGHT_YELLOW_BG = 103,
// 	EC_BRIGHT_BLUE = 94,
// 	EC_BRIGHT_BLUE_BG = 104,
// 	EC_BRIGHT_MAGENTA = 95,
// 	EC_BRIGHT_MAGENTA_BG = 105,
// 	EC_BRIGHT_CYAN = 96,
// 	EC_BRIGHT_CYAN_BG = 106,
// 	CF_WHITE = 97,
// 	CF_WHITE_BG = 107,
// 	EC_BOLD = 1,
// 	EC_UNDERLINE = 4,
// 	EC_NO_UNDERLINE = 24,
// 	EC_REVERSE = 7,
// 	EC_POSITIVE = 27,
// } PRINT_COLOR;

// Foreground
#define TERM_BLACK_FG "\e[0;30m"
#define TERM_RED_FG "\e[0;31m"
#define TERM_GREEN_FG "\e[0;32m"
#define TERM_YELLOW_FG "\e[0;33m"
#define TERM_BLUE_FG "\e[0;34m"
#define TERM_MAGENTA_FG "\e[0;35m"
#define TERM_CYAN_FG "\e[0;36m"
#define TERM_WHITE_FG "\e[0;37m"
// Bright Foreground
#define TERM_BRIGHT_BLACK_FG "\e[0;90m"
#define TERM_BRIGHT_RED_FG "\e[0;91m"
#define TERM_BRIGHT_GREEN_FG "\e[0;92m"
#define TERM_BRIGHT_YELLOW_FG "\e[0;93m"
#define TERM_BRIGHT_BLUE_FG "\e[0;94m"
#define TERM_BRIGHT_MAGENTA_FG "\e[0;95m"
#define TERM_BRIGHT_CYAN_FG "\e[0;96m"
#define TERM_BRIGHT_WHITE_FG "\e[0;97m"
// Background
#define TERM_BLACK_BG "\e[40m"
#define TERM_RED_BG "\e[41m"
#define TERM_GREEN_BG "\e[42m"
#define TERM_YELLOW_BG "\e[43m"
#define TERM_BLUE_BG "\e[44m"
#define TERM_MAGENTA_BG "\e[45m"
#define TERM_CYAN_BG "\e[46m"
#define TERM_WHITE_BG "\e[47m"
// Bright Background
#define TERM_BRIGHT_BLACK_BG "\e[100m"
#define TERM_BRIGHT_RED_BG "\e[101m"
#define TERM_BRIGHT_GREEN_BG "\e[102m"
#define TERM_BRIGHT_YELLOW_BG "\e[103m"
#define TERM_BRIGHT_BLUE_BG "\e[104m"
#define TERM_BRIGHT_MAGENTA_BG "\e[105m"
#define TERM_BRIGHT_CYAN_BG "\e[106m"
#define TERM_BRIGHT_WHITE_BG "\e[107m"
// Bold
#define TERM_BLACK_BOLD "\e[1;30m"
#define TERM_RED_BOLD "\e[1;31m"
#define TERM_GREEN_BOLD "\e[1;32m"
#define TERM_YELLOW_BOLD "\e[1;33m"
#define TERM_BLUE_BOLD "\e[1;34m"
#define TERM_MAGENTA_BOLD "\e[1;35m"
#define TERM_CYAN_BOLD "\e[1;36m"
#define TERM_WHITE_BOLD "\e[1;37m"
// Underline
#define TERM_BLACK_UNDERLINE "\e[4;30m"
#define TERM_RED_UNDERLINE "\e[4;31m"
#define TERM_GREEN_UNDERLINE "\e[4;32m"
#define TERM_YELLOW_UNDERLINE "\e[4;33m"
#define TERM_BLUE_UNDERLINE "\e[4;34m"
#define TERM_MAGENTA_UNDERLINE "\e[4;35m"
#define TERM_CYAN_UNDERLINE "\e[4;36m"
#define TERM_WHITE_UNDERLINE "\e[4;37m"
// Misc
#define TERM_RESET "\e[0m"
#define TERM_INVERTED "\e[7m"
#define TERM_CLEAR "\033c"
