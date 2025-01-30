#pragma once
#include <stdio.h>
#ifdef _WIN32
    #include <corecrt.h>
    #include <windows.h>
#endif


//#include <consoleapi.h>
#ifdef _WIN32
    #include <processenv.h>
#endif


#pragma region Colors and formatting for debug Prints

//Red
#define ERR     "\x1b[48;2;235;59;90m\x1b[38;2;50;50;50m\x1b[1m"
#define ERR2    "\x1b[38;2;235;59;90m"
#define ERRCOL  0xFF3D00FF
    // Red to another color
#define ERR2LINF "\x1b[38;2;235;59;90m\x1b[48;2;255;255;43m"
#define ERR2VINF "\x1b[38;2;235;59;90m\x1b[48;2;193;30;220m"
#define ERR2PINF "\x1b[38;2;235;59;90m\x1b[48;2;255;115;175m"
//green
#define SUCC    "\x1b[48;2;102;249;82m\x1b[38;2;50;50;50m\x1b[1m"
#define SUCC2   "\x1b[38;2;102;249;82m"
#define SUCCCOL 0x2ECC71FF
// Yellow
#define INF     "\x1b[48;2;252;202;0m\x1b[38;2;50;50;50m\x1b[1m"
#define INF2    "\x1b[38;2;252;202;0m"
#define INFCOL  0xFFD600FF
    // Yellow to another color
#define INF2SUCC  "\x1b[38;2;252;202;0m\x1b[48;2;102;249;82m"
#define INF2ERR   "\x1b[38;2;252;202;0m\x1b[48;2;235;59;90m"
#define INF2LINF  "\x1b[38;2;252;202;0m\x1b[48;2;255;255;43m"
#define INF2OINF  "\x1b[38;2;252;202;0m\x1b[48;2;255;111;22m"
#define INF2VINF  "\x1b[38;2;252;202;0m\x1b[48;2;193;30;220m"
#define INF2VDINF "\x1b[38;2;252;202;0m\x1b[48;2;131;23;75m"
#define INF2PINF  "\x1b[38;2;252;202;0m\x1b[48;2;255;115;175m"
#define INF2GBINF "\x1b[38;2;252;202;0m\x1b[48;2;45;42;46m"
#define INF2BINF  "\x1b[38;2;252;202;0m\x1b[48;2;75;123;236m"
// Light Yellow
#define LINF    "\x1b[48;2;255;255;43m\x1b[38;2;50;50;50m\x1b[1m"
#define LINF2   "\x1b[38;2;255;255;43m"
// Blue
#define BINF    "\033[48;2;75;123;236m\x1b[38;2;50;50;50m\x1b[1m"
#define BINF2   "\033[38;2;75;123;236m"
    // Blue to Other Colors
#define BINF2LINF "\033[38;2;75;123;236m\x1b[48;2;255;255;43m"
#define BINF2VINF "\033[38;2;75;123;236m\033[48;2;193;30;220m"
#define BINF2PINF "\033[38;2;75;123;236m\033[48;2;255;115;175m"
#define BINF2GBINF "\033[38;2;75;123;236m\033[48;2;45;42;46m"
// Cian
#define BLINF   "\033[48;2;80;214;241m\x1b[38;2;50;50;50m\x1b[1m"
#define BLINF2  "\033[38;2;80;214;2416m"
// BlueGreen
#define BGINF   "\033[48;2;0;181;140m\x1b[38;2;50;50;50m\x1b[1m"
#define BGINF2  "\033[38;2;0;181;140m"
// Orange
#define OINF    "\033[48;2;255;111;22m\033[38;2;50;50;50m\x1b[1m"
#define OINF2   "\033[38;2;255;111;22m"
#define WARNCOL 0xFF8F00FF
    // Orange to Other Colors
#define OINF2LINF "\033[38;2;255;111;22m\x1b[48;2;255;255;43m"
#define OINF2VINF "\033[38;2;255;111;22m\033[48;2;193;30;220m"
#define OINF2PINF "\033[38;2;255;111;22m\033[48;2;255;115;175m"
//gray
#define GINF    "\033[48;2;165;177;194m\x1b[38;2;50;50;50m\x1b[1m"
#define GINF2   "\033[38;2;165;177;194m"
#define GBINF   "\033[48;2;45;42;46m\x1b[38;2;114;112;114m\x1b[1m"
#define GBINF2  "\033[38;2;45;42;46m"
//pink
#define PINF    "\033[48;2;255;115;175m\x1b[38;2;50;50;50m\x1b[1m"
#define PINF2   "\033[38;2;255;115;175m"
//violet
#define VINF    "\033[48;2;193;30;220m\x1b[38;2;50;50;50m\x1b[1m"
#define VINF2   "\x1b[38;2;193;30;220m"
#define VDINF   "\033[48;2;131;23;75m\x1b[38;2;42;254;120m\x1b[1m"
#define VDINF2GBINF "\x1b[38;2;131;23;75m\033[48;2;45;42;46m"
#define VDINF2   "\033[38;2;131;23;75m"

#define INV     "\x1b[7m"
#define INV2    "\x1b[27m"
#define RS      "\x1b[0m"
#define BLD     "\033[1m"

#define WARNICO ""
#define INFICO  ""
#define ERRICO  ""
#define SUCCICO ""
#pragma endregion

#define VAR2STR(x) #x 
#define VAL2STR(x) VAR2STR(x)
#define OBJ2BIN(x) dbg_obj2bin(&x, sizeof(x))

void set_color_console(void);

void print_this_spot(char* const fname, const char fxname[], long long line, const char* msg, ...);
#define THIS_SPOT_MSG(msg, ...) print_this_spot(__FILE__, __FUNCTION__, __LINE__, msg , __VA_ARGS__)
#define THIS_SPOT_EMPTY() print_this_spot(__FILE__, __FUNCTION__, __LINE__, NULL)

#define THIS_SPOT(msg, ...) \
    (msg != NULL ?\
    THIS_SPOT_MSG((msg), __VA_ARGS__) : \
    THIS_SPOT_EMPTY()\
    )

int get_console_width(void);
int get_console_height(void);

void debug_full_console_widht_print(char* str, char* color, char fill_character);
char* dbg_obj2bin(void *object, size_t osize);
