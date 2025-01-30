#include "debug_utils.h"
#include <stdio.h>
#ifdef _WIN32
    #include <corecrt.h>
    #include <windows.h>
#endif

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
