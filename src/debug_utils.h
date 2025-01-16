#include <stdio.h>
#ifdef _WIN32
    #include <corecrt.h>
    #include <windows.h>
#endif

#ifndef _DBGU_H
    #define _DBGU_H


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
#endif //_DBGU_H



#ifdef DBGU_IMPLEMENTATION

//#include <consoleapi.h>
#ifdef _WIN32
    #include <processenv.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdarg.h>


// Console Color output
void set_color_console(void){
    // Set Mode
#ifdef _WIN32
    DWORD dwMode = 0;
    GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    dwMode |= ENABLE_LVB_GRID_WORLDWIDE;
    SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), dwMode);
    //
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
}




void print_this_spot(char* const fname, const char fxname[], long long line, const char* msg, ...){
    va_list arg_list;

    unsigned char col1[3] = {255, 234, 0};
    unsigned char col2[3] = {93, 90, 88};
    unsigned char col3[3] = {141, 110, 99};
    unsigned char col4[3] = {239, 235, 233};
    char* bg = "\033[48;2;";
    char* fg = "\033[38;2;";
    char* rs = "\033[0m";
    char* bld = "\033[1m";

    char str_buf[1024] = {0};
    if(msg){
        va_start(arg_list, msg);
        /* int va_count = count_fmt_args(msg); */
        vsprintf_s(str_buf,  512, msg, arg_list);
        va_end(arg_list); 
    }
    char* start = str_buf, *end = start;
    while(*end != '\0') end++;
    long long buf_len = end - start;

    printf("%s%d;%d;%dm%s%d;%d;%dm%s 󰃚 %lli %s%s%d;%d;%dm%s%d;%d;%dm%s%s%d;%d;%dm%s%d;%d;%dm %s %s%s%d;%d;%dm%s%d;%d;%dm%s%s%d;%d;%dm%s%d;%d;%dm %s %s%s%d;%d;%dm%s%d;%d;%dm%s%s%d;%d;%dm%s%d;%d;%dm%s  %s %s%s%d;%d;%dm%s\n", 

            bg,col1[0],col1[1],col1[2], fg,col2[0],col2[1],col2[2], bld, line, rs,
            bg,col2[0],col2[1],col2[2], fg,col1[0],col1[1],col1[2], rs,
            bg,col2[0],col2[1],col2[2], fg,col1[0],col1[1],col1[2], fname, rs,
            bg,col3[0],col3[1],col3[2], fg,col2[0],col2[1],col2[2], rs,
            bg,col3[0],col3[1],col3[2], fg,col1[0],col1[1],col1[2], fxname, rs,
            bg,col4[0],col4[1],col4[2], fg,col3[0],col3[1],col3[2], rs,
            bg,col4[0],col4[1],col4[2], fg,col2[0],col2[1],col2[2], bld, buf_len ? str_buf : "...", rs,
            fg,col4[0],col4[1],col4[2], rs

           );
    /* printf("%s%s%s%s %s %s%s%s%s%s%s %s() : %s  %lli ◆ %s\n", INV, LINF, RS, */
    /*                                                                 LINF, fname, INV, RS, */
    /*                                                                 INF, INV, RS, */
    /*                                                                 INF, fxname, INV, line, RS); */
}
    


int get_console_width(void){
    HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO screen_info;
    GetConsoleScreenBufferInfo(console_handle, &screen_info);
    // printf("Console Width:%d and Height:%d\n", screen_info.dwSize.X, screen_info.dwSize.Y);
    int out_result = screen_info.dwSize.X;

    return out_result;
}
int get_console_height(void){
    HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO screen_info;
    GetConsoleScreenBufferInfo(console_handle, &screen_info);
    // printf("Console Width:%d and Height:%d\n", screen_info.dwSize.X, screen_info.dwSize.Y);
    int out_result = screen_info.dwSize.Y;

    return out_result;
}


void debug_full_console_widht_print(char* str, char* color, char fill_character){
    int console_width = get_console_width();
    
    int string_length = snprintf(NULL,0,"%s",str);
    // printf("string len %d\n", string_length);


    // Handle color input
    char print_color[60];
    if(color == NULL){
        print_color[0] = '\0';
    }else{
        int color_len = snprintf(NULL,0,"%s",color);
        memcpy(print_color, color, color_len);
        print_color[color_len]='\0';
    }

    
    int padding_size = (console_width - string_length)/2;
    char padding_buffer[padding_size+1];
    for(int i = 0; i< padding_size; i++){
        padding_buffer[i] = fill_character;
        
    }
    padding_buffer[padding_size]='\0';
    
    // printf("\n %d | %d | %d\n", console_width, string_length, padding_size);
    
    // Final Print
    int print_size = snprintf(NULL, 0, "%s%s%s%s%s\n",print_color,padding_buffer,str,padding_buffer,RS);
    // printf("printsize %d | string_length %d | padding_size %d | console_width %d \n",print_size, string_length,padding_size, console_width);
    char console_buffer[print_size+1];
    snprintf(console_buffer, print_size+1 ,"%s%s%s%s%s\n",print_color,padding_buffer,str,padding_buffer,RS);
    console_buffer[print_size+1] = '\0';
    printf("%s",console_buffer);

}
char* dbg_obj2bin(void *object, size_t osize) {
    const unsigned char *p = (const unsigned char *) object;
    p += osize;
    
    unsigned long long buffer_size = osize * CHAR_BIT + (osize-1) + 1;
    char* s = (char*)calloc(buffer_size, 1);
    // printf("s pos-start: %p\n",s);
    while (osize-- > 0) {
        p--;
        unsigned i = CHAR_BIT;
        while (i-- > 0) {
            *s++ = ((*p >> i) & 1) + '0';
            // printf("%p - %i \n", s, *s);
            if(!i % CHAR_BIT && osize != CHAR_BIT){
                *s++ = ' '; 
            }
        }
    }
    *s = '\0';
    return s-buffer_size;
}
#endif //DBGU_IMPLEMENTATION
