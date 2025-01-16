#pragma once

#ifndef ARRLEN
    #define ARRLEN(x) (sizeof(x)/sizeof(*x))
#endif
#ifndef ABS
    #define ABS(x) ((x)<0?(x)*-1:x)
#endif

typedef struct{
    int offset;
    int col1;
    int text1;
    int col2;
    int text2;
}my_print_context;

long my_cmp( void* cmp1, void* cmp2, unsigned long length );

char* string_from_last_element(char* string, char element);
void string_find_all_idx(char* string, char element,unsigned int* count, unsigned int* idxs);

void col_ihex2vfloat(int in_hex, float* out_col);
void col_vfloat2ihex(float* in_col, int* out_hex);
/*
   Note! Allocates memory on the heap! Remeber to free() when you are done with the color
 */
char* col_t(int text_hex, int bg_hex, short bold);
char* col_t_inv(char* col1, char* col2);
void col_t_FGBG_flip(const char* in_col, char* flipped);
int  lerp_hex(int col1, int col2, float lerp_pos );
int gradient_hex(int* colors, int no_colors,  float position);

/* ───────────────────── Print Functions ─────────────────────*/
void print_bubble(int fg, int bg, int offset, void* item, const char* item_type, int format);
void print_bubble_value(my_print_context context, const char* text, void* value, char* value_type, int format);
void print_bubble_text(my_print_context context, const char* text, char* icon, int format);


typedef enum{
    _SC_GREATER =1,
    _SC_LESSER  =2,
    _SC_EQUALS  =4,
    _SC_NOT     =8,
    _SC_DEREF   =16,
    _SC_NO_ERRORS = -1
}sanity_test_check;

typedef struct sanity_check_packet{
    void* variable;
    sanity_test_check test_type;
    long long result;
}sanity_check_packet;

int sanity_check_i(int test_count, sanity_check_packet* test_packets );

