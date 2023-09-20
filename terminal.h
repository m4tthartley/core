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
#define BLKF "\e[0;30m"
#define REDF "\e[0;31m"
#define GRNF "\e[0;32m"
#define YELF "\e[0;33m"
#define BLUF "\e[0;34m"
#define MAGF "\e[0;35m"
#define CYNF "\e[0;36m"
#define WHTF "\e[0;37m"
// Background
#define BLKB "\e[40m"
#define REDB "\e[41m"
#define GRNB "\e[42m"
#define YELB "\e[43m"
#define BLUB "\e[44m"
#define MAGB "\e[45m"
#define CYNB "\e[46m"
#define WHTB "\e[47m"
// Bold
#define BBLK "\e[1;30m"
#define BRED "\e[1;31m"
#define BGRN "\e[1;32m"
#define BYEL "\e[1;33m"
#define BBLU "\e[1;34m"
#define BMAG "\e[1;35m"
#define BCYN "\e[1;36m"
#define BWHT "\e[1;37m"
// Underline
#define UBLK "\e[4;30m"
#define URED "\e[4;31m"
#define UGRN "\e[4;32m"
#define UYEL "\e[4;33m"
#define UBLU "\e[4;34m"
#define UMAG "\e[4;35m"
#define UCYN "\e[4;36m"
#define UWHT "\e[4;37m"
// Misc
#define RESET "\e[0m"
#define INVERTED "\e[7m"

void escape_code(char* code) {
	printf(code);
}

int print(char* str, ...) {
	va_list args;
	va_start(args, str);
	int result = vprintf(str, args);
	va_end(args);
	return result;
}