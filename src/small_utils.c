#include "small_utils.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VAR2STR(x) #x 
#define VAL2STR(x) VAR2STR(x)

typedef enum{
    PBF_NO_FORMAT,
    PBF_NEW_LINE,
    PBF_BOLD,
    PBF_BOLD_NEW_LINE
}print_bubble_format;

/**
 * Faster memory comparison when expected items are predominantly not the same
 * Found here: https://macosxfilerecovery.com/faster-memory-comparison-in-c/
 * @return 0 - same,  > 0 < - not the same
 **/
long my_cmp( void* cmp1, void* cmp2, unsigned long length ){
    if(length>=4){
        long difference = *(unsigned long*)cmp1 - *(unsigned long*)cmp2;
        if(difference) return difference;
    }
    return memcmp(cmp1, cmp2, length);
}

/**
 * Returns the last occurance of `element` in a string
 *
 * @return pointer to the last seen element, or NULL if not found;
 **/
char* string_from_last_element(char* string, char element){
    char* found_at = NULL;
    
    char* tmp_ptr = string;
    while (*tmp_ptr != '\0'){
        if(*tmp_ptr == element){
            found_at = tmp_ptr;
        }
        tmp_ptr++;
    }

    if(found_at == NULL){
        printf("Element '%c' not found in string `%s` \n", element, string);
        return NULL;
    }

    return found_at;

}


void string_find_all_idx(char* string, char element,unsigned int* count, unsigned int* idxs){
    unsigned int str_len = strlen(string);
    unsigned int idx_array[str_len];
   
    unsigned int counter = 0;
    for(unsigned int i=0; i<str_len; i++){
        if(string[i] == element){
            idx_array[counter] = i;
            counter += 1;
        }
    }

    if(idxs == NULL){
        *count = counter;
        return;
    }

    for(unsigned int i=0; i < *count; i++){
        idxs[i] = idx_array[i];
    }
    // printf("Final Count : %d\n", *count);
    // unsigned int between_size = idx_array[*count-1] - idx_array[0] - 1;
    // char* start_ptr = string + idx_array[0]+1;
    // for(unsigned int i=0; i<between_size;i++){
    //     printf("%c", *(start_ptr+i));
    // }
    // putchar('\n');


    return ;
}

int col_hexStr2Int(const char* inHexStr){
    // Sanetize
    char* hexStr = *(char*)inHexStr == '#' ? (char*)inHexStr + 1 : (char*)inHexStr ;
    int out_RGB = 0;

    int size = strlen(hexStr);
    if( size == 8 ){ // RGBA
        printf("TODO: Case RGBA\n");
    }else if(size == 6){ //RGB
        typedef struct{
            char R[2];
            char G[2];
            char B[2];
        }RGB;
        // Takes more memory but easier to read?
        RGB* channels = (RGB*)hexStr;
        /* ────────────────────── Get R channel ──────────────────────*/
        unsigned char R = ((channels->R[0] & 0xF)+(channels->R[0] >>6) | ((channels->R[0]>>3 )&0x8) )<<4;
        R |= ((channels->R[1] & 0xF)+(channels->R[1] >>6) | ((channels->R[1]>>3 )&0x8) );
        /* ────────────────────── Get G channel ──────────────────────*/
        unsigned char G = ((channels->G[0] & 0xF)+(channels->G[0] >>6) | ((channels->G[0]>>3 )&0x8) )<<4;
        G |= ((channels->G[1] & 0xF)+(channels->G[1] >>6) | ((channels->G[1]>>3 )&0x8) );
        /* ────────────────────── Get B channel ──────────────────────*/
        unsigned char B = ((channels->B[0] & 0xF)+(channels->B[0] >>6) | ((channels->B[0]>>3 )&0x8) )<<4;
        B |= ((channels->B[1] & 0xF)+(channels->B[1] >>6) | ((channels->B[1]>>3 )&0x8) );
        /* ────────────────────── Combine Final ──────────────────────*/
        out_RGB = (R<<24)| (G <<16) | (B<<8);
    }else if(size == 2){ // Grayscale
        
        printf("Case Grayscale\n");
    }
    return out_RGB;
}

unsigned long long hex2u64(const char* hex ){
    unsigned long long res =0;
    char c;

    while(( c = *hex++ )){
        char v = ( (c & 0xF) + ( c >> 6) ) | ((c>>3)&0x8);
        res = (res<<4) | (unsigned long long) v;
    }
    return res;
}


void hex2rbg(const char* hex, unsigned int rgb[static 3]){

    char has_hashtag = hex[0] == 35 ? 1 : 0;
    if(has_hashtag) hex += 1;
    // printf("(%c)%s \n",hex[0],hex);
    char i;
    for(i=0; i<6;){
        // printf(" %c%c ", hex[i],hex[i+1]);
        i+=2;
    }

    rgb[0] = hex2u64((char[3]){hex[0],hex[1],'\0'});
    rgb[1] = hex2u64((char[3]){hex[2],hex[3],'\0'});
    rgb[2] = hex2u64((char[3]){hex[4],hex[5],'\0'});
     
    // printf("\033[48;2;%u;%u;%um Hello Test \033[0m\n", R1, G1, B1);

}

void col_ihex2vfloat(int in_hex, float* out_col){
    out_col[0] = (float)((in_hex >> 24) & 0xFF)/255.0; 
    out_col[1] = (float)((in_hex >> 16) & 0xFF)/255.0; 
    out_col[2] = (float)((in_hex >> 8) & 0xFF)/255.0; 
    out_col[3] = (float)(in_hex & 0xFF)/255.0; 
}

void col_vfloat2ihex(float* in_col, int* out_hex){
    *out_hex |= (unsigned char)(in_col[0]*255.0)<<24;
    *out_hex |= (unsigned char)(in_col[1]*255.0)<<16;
    *out_hex |= (unsigned char)(in_col[2]*255.0)<<8;
    *out_hex |= (unsigned char)(in_col[3]*255.0);
}

// Create a ASCII color code, for terminal printing
//      in format /033[38;2;R1;G1;B1m/033[48;2;R2;G2;B2m
//      where R1G1B1 is fg_hex - forground(text) color in hexidecimal
//      and   R2G2B2 is bg_hex - background color in hexidecimal
//      !Note keep in mind the string size type it will overwrite stack memory if using regular int
char* col_t(int text_hex, int bg_hex, short bold){
    char* bld = bold ? "\033[1m" : "\033[22m";
    char* out_buffer;

    unsigned char R;
    unsigned char G;
    unsigned char B;
    unsigned char A;

    char _case = 0;
        
    /* ━━ Text Color ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━*/
    char fg_col[255] = {0};
    if( text_hex != 0 ) 
    {
        R = (text_hex&(255<<24))>>24; 
        G = (text_hex&(255<<16))>>16; 
        B = (text_hex&(255<<8)) >>8; 
        A =  text_hex&(255);
        (void)A;
        sprintf(fg_col,"%d;%d;%d", R,G,B);
        _case |= 1;
    }
    /* ━━ Background Color ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━*/
    char bg_col[255] = {0};
    if(bg_hex != 0)
    {
        R = (bg_hex&(255<<24))>>24; 
        G = (bg_hex&(255<<16))>>16; 
        B = (bg_hex&(255<<8)) >>8; 
        A =  bg_hex&(255); 
        sprintf(bg_col,"%d;%d;%d", R,G,B);
        _case |= 2;
    }
    
    long long string_size;
    switch(_case){
        case 0:
            return NULL;
            break;

        case 1:
            string_size = snprintf(NULL, 0, "\e[38;2;%sm%s", fg_col,bld);
            out_buffer = malloc(string_size+1);
            snprintf(out_buffer, string_size+1, "\e[38;2;%sm%s", fg_col,bld);
            break;

        case 2:
            string_size = snprintf(NULL, 0, "\e[48;2;%sm%s",  bg_col, bld );
            out_buffer = malloc(string_size+1);
            snprintf(out_buffer, string_size+1, "\e[48;2;%sm%s",  bg_col, bld );
            break;

        case 3:
            string_size = snprintf(NULL, 0, "\e[38;2;%sm%s\e[48;2;%sm", fg_col,bld, 
                                                                        bg_col);
            out_buffer = malloc(string_size+1);
            snprintf(out_buffer, string_size+1, "\e[38;2;%sm%s\e[48;2;%sm", fg_col, bld, 
                                                                            bg_col);
            break;
        default:
            return NULL;
    }
    
    out_buffer[string_size]='\0';
    return out_buffer;
}

// Invert two ASCII color codes so that output is newcolor -> fg = col1_bg, bg->col2_bg
char* col_t_inv(char* col1, char* col2){
    unsigned int col1_len = strlen(col1);
    unsigned int col2_len = 0;
    if (col2 != NULL)
    { col2_len = strlen(col2); }
    
    char col1_bg[24] = {0}; 
    char col2_bg[24] = {0}; 

    unsigned int s_pos = 0,e_pos = 0;

    for(unsigned int i=0; i < col1_len; ++i){
        if(col1[i]==27){
            unsigned char num = (col1[i+2] - '0')*10 + (col1[i+3]-'0');
            if(num == 48){
                s_pos = i+2;
                for(unsigned int j =s_pos; j < col1_len; ++j){
                    if(col1[j] == 109) {
                        e_pos = j;
                        break;
                    }
                }
                break;
            }
            
        }else{}
    }
    unsigned int copy_size = e_pos - s_pos +1;
    memcpy_s(col1_bg, 24, col1+s_pos , copy_size );
    col1_bg[0]='3';
    col1_bg[copy_size] = '\0';
    // printf("\n%s - \n\t %s = %u\n\t %s = %u\n",col1_bg, VAR2STR(e_pos), e_pos, VAR2STR(s_pos), s_pos);
    if(col2 != NULL)    
    {
        for(unsigned int i=0; i < col2_len; ++i){
            if(col2[i]==27){
                unsigned char num = (col2[i+2] - '0')*10 + (col2[i+3]-'0');
                if(num == 48){
                    s_pos = i+2;
                    for(unsigned int j = s_pos; j < col2_len; ++j){ 
                        if(col2[j] == 'm') { 
                            e_pos = j;
                            break;
                        }
                    }
                    break;
                }
            }else{}
        }
        copy_size = e_pos - s_pos + 1;
        memcpy_s(col2_bg, 24, col2 + s_pos , copy_size );
        col2_bg[copy_size] = '\0';
    }
    // printf("\n%s - \n\t %s = %u\n\t %s = %u\n",col2_bg, VAR2STR(e_pos), e_pos, VAR2STR(s_pos), s_pos);
    
    char temp_char_array[256] = {0};

    if(col2 != NULL){
        unsigned int temp_col_size = snprintf(NULL, 0 , "\033[%s\033[%s", col1_bg, col2_bg);
        snprintf(temp_char_array, temp_col_size + 1 , "\033[%s\033[%s", col1_bg, col2_bg);
        temp_char_array[temp_col_size]='\0';
    } else {
        unsigned int temp_col_size = snprintf(NULL, 0 , "\033[%s", col1_bg);
        snprintf(temp_char_array, temp_col_size + 1 , "\033[%s", col1_bg);
        temp_char_array[temp_col_size]='\0';
    }
    
    // printf("%s 󰝥 \033[0m\n", temp_char_array);
    
    printf("WARNING, `INT` CHANGE NOT IMPLEMENTD!!!!");
    return NULL;
}

int lerp_hex(int col1, int col2, float lerp_pos){
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wincompatible-pointer-types"
    
    if(lerp_pos == 0){
        return col1;
    }else if (lerp_pos == 1) {
        return col2;
    }
    
    unsigned char A[4];
    A[0] = (col1&(255<<24))>>24; 
    A[1] = (col1&(255<<16))>>16; 
    A[2] = (col1&(255<<8)) >>8; 
    A[3] =  col1&(255);

    unsigned char B[4];
    B[0] = (col2&(255<<24))>>24; 
    B[1] = (col2&(255<<16))>>16; 
    B[2] = (col2&(255<<8)) >>8; 
    B[3] =  col2&(255);

    unsigned char C[4];
    C[0] = A[0] + lerp_pos*(B[0]-A[0]);
    C[1] = A[1] + lerp_pos*(B[1]-A[1]);
    C[2] = A[2] + lerp_pos*(B[2]-A[2]);
    C[3] = A[3] + lerp_pos*(B[3]-A[3]);
    
    int out_color=0;
    out_color |= C[0]<<24;
    out_color |= C[1]<<16;
    out_color |= C[2]<<8;
    out_color |= C[3];
    
    return out_color;
    #pragma clang diagnostic pop
}

int gradient_hex(int* colors, int no_colors,  float position){

    float segment_length = 1.0/(no_colors-1);
    float position_ratio = position/segment_length;
    int   colorIdx       = position_ratio;

    if(colorIdx == no_colors-1){
    
    }
    
    float pos_fraction = fmod(position_ratio, 1.0f);
    
    int out_color = lerp_hex(colors[colorIdx], colors[colorIdx+1], pos_fraction);
    
    return out_color;
}

// Flips terminal color (\e[38;2;(A)m..\e[48;2(B)m) 
//      FG(A)BG(B) -> FG(B)BG(A)
// flipped : should be a char buffer of the same size as t_col
void col_t_FGBG_flip(const char* t_col, char* flipped){
    // TODO: when there is no FG(38) or BG(48) instruction
    //       in the string. Just replace the number 3<->4.
    //       Thus only flipping the color to FG or BG respectively

    char* p = (char*)t_col; // Search pointer
    
    long long fg_size=0;
    long long bg_size=0;
    char* fg_pos;
    char* bg_pos;
    while(*p != '\0'){
        if(*p == '3'){
            char* b = p;
            if(*(b+1) == '8' && *(b+2)==';' && *(b+3)=='2' ){
                b = (b+5);
                fg_pos = b;
                while(*b!='m'){
                    fg_size++;
                    b++;
                }
            }
        }
        p++;
    }
    // Reset search pointer. Maybe useless double search?
    p = (char*)t_col;
    while(*p != '\0'){
        if(*p == '4'){
            char* b = p;
            if(*(b+1) == '8' && *(b+2)==';' && *(b+3)=='2' ){
                b = (b+5);
                bg_pos = b;
                while(*b!='m'){
                    bg_size++;
                    b++;
                }
            }
        }
        p++;
    }
    
    if(fg_size == 0){printf("TODO: NO FG COLOR"); return;}
    if(bg_size == 0){printf("TODO: NO BG COLOR"); return;}

    long long t_size = strlen(t_col);
    /* ╭──────────────────────────────────────────────────────────╮
       │             Fill Foreground escape sequence              │
       ╰──────────────────────────────────────────────────────────╯*/
    long long n1 = (long long)fg_pos - (long long)t_col; // n1 = size of copy
    char* wp = flipped;                                  // wp = work pointer for copying
    memcpy(wp, t_col, n1);                               // Copy region to wp of suze n1
    wp += n1;                                            // Increment work pointer
    /*╭──────────────────────────────────────────────────────────╮
      │          Set Foreground to Background position           │
      ╰──────────────────────────────────────────────────────────╯*/
    memcpy(wp, bg_pos,bg_size);
    wp += bg_size;
    /* ╭──────────────────────────────────────────────────────────╮
       │                        Set Middle                        │
       ╰──────────────────────────────────────────────────────────╯*/
    n1 = (long long)bg_pos - (long long)(fg_pos + fg_size);
    memcpy(wp, fg_pos + fg_size,n1);
    wp += n1;
    /* ╭──────────────────────────────────────────────────────────╮
       │               Set Background to Foreground               │
       ╰──────────────────────────────────────────────────────────╯*/
    memcpy(wp, fg_pos,fg_size);
    wp += fg_size;
    /* ╭──────────────────────────────────────────────────────────╮
       │              Fill anything after Foreground              │
       ╰──────────────────────────────────────────────────────────╯*/
    p = (char*) (t_col + (( (long long)bg_pos - (long long)t_col )+bg_size));
    n1 = (long long)(t_col + t_size) - (long long)p;
    memcpy(wp, p,n1);
    wp += n1;

    *wp = '\0';
}



void print_bubble(int fg, int bg, int offset, void* item, const char* item_type, int format){
    char* rs  = "\033[0m";
    char* nl  = (format&1)==1 ? "\n" : "\0";
    char* bld = (format&2)==2 ? "\033[1m" : "\033[22m";
    /* ───────────────────────── Color 1  ─────────────────────────*/
    unsigned char R = (fg&(255<<24))>>24; 
    unsigned char G = (fg&(255<<16))>>16; 
    unsigned char B = (fg&(255<<8))>>8; 
    unsigned char A =  fg&(255);
    (void)A;
    char fg_col[255];
    sprintf(fg_col,"%d;%d;%d", R,G,B);

    char bg_col[255];
    R = (bg&(255<<24))>>24; 
    G = (bg&(255<<16))>>16; 
    B = (bg&(255<<8))>>8; 
    A =  bg&(255); 
    sprintf(bg_col,"%d;%d;%d", R,G,B);

    int type_len = strlen(item_type);
    if(item_type[0] == 'u'){
        if(type_len > 1 && item_type[1] == 'i'){
            if(offset > 0)
            {
                printf("%*c\033[38;2;%sm%s\033[48;2;%sm\033[38;2;%sm%s %u %s\033[38;2;%sm%s%s",offset,' ',
                       bg_col,rs,
                       bg_col,fg_col,bld,
                       *(unsigned int*)item,rs,
                       bg_col,rs,nl);
            }else if(offset<0){
                printf("\033[%dD\033[48;2;%sm\033[38;2;%sm%s %u %s\033[38;2;%sm%s%s",( offset*-1 ),
                       bg_col,fg_col,bld,
                       *(unsigned int*)item,rs,
                       bg_col,rs,nl);
            }else{
                printf("\033[38;2;%sm%s\033[48;2;%sm\033[38;2;%sm%s %u %s\033[38;2;%sm%s%s",
                       bg_col,rs,
                       bg_col,fg_col,bld,
                       *(unsigned int*)item,rs,
                       bg_col,rs,nl);
            }
        }
        else if ( type_len >1 && item_type[1] == 'l') {
            if(type_len > 2 && item_type[2] == 'l'){
                if(offset>0)
                {
                    printf("%*c\033[38;2;%sm%s\033[48;2;%sm\033[38;2;%sm%s %llu %s\033[38;2;%sm%s%s",offset,' ',
                           bg_col,rs,
                           bg_col,fg_col,bld,
                           *(unsigned long long *)item,rs,
                           bg_col,rs,nl);
                }else if(offset<0){
                    printf("\033[%dD\033[48;2;%sm\033[38;2;%sm%s %llu %s\033[38;2;%sm%s%s",( offset*-1 ),
                           bg_col,fg_col,bld,
                           *(unsigned long long*)item,rs,
                           bg_col,rs,nl);
                }else{
                    printf("\033[38;2;%sm%s\033[48;2;%sm\033[38;2;%sm%s %llu %s\033[38;2;%sm%s%s",                           bg_col,rs,
                           bg_col,fg_col,bld,
                           *(unsigned long long *)item,rs,
                           bg_col,rs,nl);
                }

            }
        }
    }

    else if(item_type[0] == 'i'){
        if(offset > 0){
            if(item_type[1] != '\0'){
                if(item_type[1] =='8'){
                    printf("%*c\033[38;2;%sm%s\033[48;2;%sm\033[38;2;%sm%s %d %s\033[38;2;%sm%s%s",offset,' ',
                           bg_col,rs,
                           bg_col,fg_col,bld,
                           *(char*)item,rs,
                           bg_col,rs,nl);
                }else if(item_type[1] == '1'){
                    printf("%*c\033[38;2;%sm%s\033[48;2;%sm\033[38;2;%sm%s %d %s\033[38;2;%sm%s%s",offset,' ',
                           bg_col,rs,
                           bg_col,fg_col,bld,
                           *(short*)item,rs,
                           bg_col,rs,nl);
                }else if(item_type[1] == '3'){
                    printf("%*c\033[38;2;%sm%s\033[48;2;%sm\033[38;2;%sm%s %d %s\033[38;2;%sm%s%s",offset,' ',
                           bg_col,rs,
                           bg_col,fg_col,bld,
                           *(int*)item,rs,
                           bg_col,rs,nl);
                }
            }else{
                printf("%*c\033[38;2;%sm%s\033[48;2;%sm\033[38;2;%sm%s %d %s\033[38;2;%sm%s%s",offset,' ',
                       bg_col,rs,
                       bg_col,fg_col,bld,
                       *(int*)item,rs,
                       bg_col,rs,nl);
            }
        }
        else if(offset < 0){
            if(item_type[1] != '\0'){
                if(item_type[1] =='8'){
                    printf("\033[%dD\033[48;2;%sm\033[38;2;%sm%s %d %s\033[38;2;%sm%s%s",( offset*-1 ),
                           bg_col,fg_col,bld,
                           *(char*)item,rs,
                           bg_col,rs,nl);
                }else if(item_type[1] == '1'){
                    printf("\033[%dD\033[48;2;%sm\033[38;2;%sm%s %d %s\033[38;2;%sm%s%s",( offset*-1 ),
                           bg_col,fg_col,bld,
                           *(short*)item,rs,
                           bg_col,rs,nl);
                }else if(item_type[1] == '3'){
                    printf("\033[%dD\033[48;2;%sm\033[38;2;%sm%s %d %s\033[38;2;%sm%s%s",( offset*-1 ),
                           bg_col,fg_col,bld,
                           *(int*)item,rs,
                           bg_col,rs,nl);    
                }
            }else{
                    printf("\033[%dD\033[48;2;%sm\033[38;2;%sm%s %d %s\033[38;2;%sm%s%s",( offset*-1 ),
                           bg_col,fg_col,bld,
                           *(int*)item,rs,
                           bg_col,rs,nl);
            }
        }else{
            if(item_type[1] != '\0'){
                if(item_type[1] =='8'){
                    printf("\033[38;2;%sm%s\033[48;2;%sm\033[38;2;%sm%s %d %s\033[38;2;%sm%s%s",
                           bg_col,rs,
                           bg_col,fg_col,bld,
                           *(char*)item,rs,
                           bg_col,rs,nl);
                }else if(item_type[1] == '1'){
                    printf("\033[38;2;%sm%s\033[48;2;%sm\033[38;2;%sm%s %d %s\033[38;2;%sm%s%s",
                           bg_col,rs,
                           bg_col,fg_col,bld,
                           *(short*)item,rs,
                           bg_col,rs,nl);
                }else if(item_type[1] == '3'){
                    printf("\033[38;2;%sm%s\033[48;2;%sm\033[38;2;%sm%s %d %s\033[38;2;%sm%s%s",
                           bg_col,rs,
                           bg_col,fg_col,bld,
                           *(int*)item,rs,
                           bg_col,rs,nl);
                }
            }else{
                printf("\033[38;2;%sm%s\033[48;2;%sm\033[38;2;%sm%s %d %s\033[38;2;%sm%s%s",
                       bg_col,rs,
                       bg_col,fg_col,bld,
                       *(int*)item,rs,
                       bg_col,rs,nl);
            }
        }
    }
    else if(item_type[0] == 'l'){
        if (type_len>1 && item_type[1] == 'l') {
            if(offset>0)
            {
                printf("%*c\033[38;2;%sm%s\033[48;2;%sm\033[38;2;%sm%s %lli %s\033[38;2;%sm%s%s",offset,' ',
                       bg_col,rs,
                       bg_col,fg_col,bld,
                       *(long long int*)item,rs,
                       bg_col,rs,nl);
            }else if(offset<0){
                printf("\033[%dD\033[48;2;%sm\033[38;2;%sm%s %lli %s\033[38;2;%sm%s%s",( offset*-1 ),
                       bg_col,fg_col,bld,
                       *(long long int*)item,rs,
                       bg_col,rs,nl);
            }else{
                printf("\033[38;2;%sm%s\033[48;2;%sm\033[38;2;%sm%s %lli %s\033[38;2;%sm%s%s",
                       bg_col,rs,
                       bg_col,fg_col,bld,
                       *(long long int*)item,rs,
                       bg_col,rs,nl);
            }
        }
        else if (type_len>1 && item_type[1] == 'f') {
            if(offset>0)
            {
                printf("%*c\033[38;2;%sm%s\033[48;2;%sm\033[38;2;%sm%s %lf %s\033[38;2;%sm%s%s",offset,' ',
                       bg_col,rs,
                       bg_col,fg_col,bld,
                       *(double*)item,rs,
                       bg_col,rs,nl);
            }else if(offset<0){
                printf("\033[%dD\033[48;2;%sm\033[38;2;%sm%s %lf %s\033[38;2;%sm%s%s",( offset*-1 ),
                       bg_col,fg_col,bld,
                       *(double*)item,rs,
                       bg_col,rs,nl);
            }else{
                printf("\033[38;2;%sm%s\033[48;2;%sm\033[38;2;%sm%s %lf %s\033[38;2;%sm%s%s",                       bg_col,rs,
                       bg_col,fg_col,bld,
                       *(double*)item,rs,
                       bg_col,rs,nl);
            }
        }
    }
    else if(item_type[0] == 'L'){
        if(type_len>1 && item_type[1] == 'f'){
            if(offset>0)
            {
                printf("%*c\033[38;2;%sm%s\033[48;2;%sm\033[38;2;%sm%s %Lf %s\033[38;2;%sm%s%s",offset,' ',
                       bg_col,rs,
                       bg_col,fg_col,bld,
                       *(long double *)item,rs,
                       bg_col,rs,nl);
            }else if(offset<0){
                printf("\033[%dD\033[48;2;%sm\033[38;2;%sm%s %Lf %s\033[38;2;%sm%s%s",( offset*-1 ),
                       bg_col,fg_col,bld,
                       *(long double*)item,rs,
                       bg_col,rs,nl);
            }else{
                printf("\033[38;2;%sm%s\033[48;2;%sm\033[38;2;%sm%s %Lf %s\033[38;2;%sm%s%s",
                       bg_col,rs,
                       bg_col,fg_col,bld,
                       *(long double *)item,rs,
                       bg_col,rs,nl);
            }
        }
    }

    else if(item_type[0] == 'f'){
        if(offset>0)
        {
            printf("%*c\033[38;2;%sm%s\033[48;2;%sm\033[38;2;%sm%s %f %s\033[38;2;%sm%s%s",offset,' ',
                   bg_col,rs,
                   bg_col,fg_col,bld,
                   *(float *)item,rs,
                   bg_col,rs,nl);
        }else if(offset<0){
            printf("\033[%dD\033[48;2;%sm\033[38;2;%sm%s %f %s\033[38;2;%sm%s%s",( offset*-1 ),
                   bg_col,fg_col,bld,
                   *(float*)item,rs,
                   bg_col,rs,nl);
        }else{
            printf("\033[38;2;%sm%s\033[48;2;%sm\033[38;2;%sm%s %f %s\033[38;2;%sm%s%s",
                   bg_col,rs,
                   bg_col,fg_col,bld,
                   *(float *)item,rs,
                   bg_col,rs,nl);
        }
    }
    else if(item_type[0] == 's'){
        if(offset>0)
        {
            printf("%*c\033[38;2;%sm%s\033[48;2;%sm\033[38;2;%sm%s %s %s\033[38;2;%sm%s%s",offset,' ',
                   bg_col,rs,
                   bg_col,fg_col,bld,
                   (char*)item,rs,
                   bg_col,rs,nl);
        }else if(offset<0){
            printf("\033[%dD\033[48;2;%sm\033[38;2;%sm%s %s %s\033[38;2;%sm%s%s",( offset*-1 ),
                   bg_col,fg_col,bld,
                   (char*)item,rs,
                   bg_col,rs,nl);
        }else{
            printf("\033[38;2;%sm%s\033[48;2;%sm\033[38;2;%sm%s %s %s\033[38;2;%sm%s%s",
                   bg_col,rs,
                   bg_col,fg_col,bld,
                   (char*)item,rs,
                   bg_col,rs,nl);
        }
    }
    else if(item_type[0] == 'p'){
        if(offset>0)
        {
            printf("%*c\033[38;2;%sm%s\033[48;2;%sm\033[38;2;%sm%s %p %s\033[38;2;%sm%s%s",offset,' ',
                   bg_col,rs,
                   bg_col,fg_col,bld,
                   (void*)item,rs,
                   bg_col,rs,nl);
        }else if(offset<0){
            printf("\033[%dD\033[48;2;%sm\033[38;2;%sm%s %p %s\033[38;2;%sm%s%s",( offset*-1 ),
                   bg_col,fg_col,bld,
                   (void*)item,rs,
                   bg_col,rs,nl);
        }else{
            printf("\033[38;2;%sm%s\033[48;2;%sm\033[38;2;%sm%s %p %s\033[38;2;%sm%s%s",
                   bg_col,rs,
                   bg_col,fg_col,bld,
                   (void*)item,rs,
                   bg_col,rs,nl);
        }
    }
}

void print_bubble_text(my_print_context context, const char* text, char* icon, int format){
    char* rs  = "\033[0m";
    char* nl  = (format&1)==1 ? "\n" : "";
    char* bld = (format&2)==2 ? "\033[1m" : "\033[22m";
    /* ───────────────────────── Color 1  ─────────────────────────*/
    unsigned char R = (context.col1&(255<<24))>>24; 
    unsigned char G = (context.col1&(255<<16))>>16; 
    unsigned char B = (context.col1&(255<<8))>>8; 
    unsigned char A = context.col1&(255);
    (void)A;
    char col1[255];
    sprintf(col1,"%d;%d;%d", R,G,B);
    char colt1[255];
    R = (context.text1&(255<<24))>>24; 
    G = (context.text1&(255<<16))>>16; 
    B = (context.text1&(255<<8))>>8; 
    A =  context.text1&(255); 
    sprintf(colt1,"%d;%d;%d", R,G,B);
    /* ───────────────────────── Color 2 ─────────────────────────*/
    char col2[255];
    R = (context.col2&(255<<24))>>24; 
    G = (context.col2&(255<<16))>>16; 
    B = (context.col2&(255<<8))>>8; 
    A =  context.col2&(255); 
    sprintf(col2,"%d;%d;%d", R,G,B);
    char colt2[255];
    R = (context.text2&(255<<24))>>24; 
    G = (context.text2&(255<<16))>>16; 
    B = (context.text2&(255<<8))>>8; 
    A =  context.text2&(255); 
    sprintf(colt2,"%d;%d;%d", R,G,B);
    /* ─────────────────────── Final Print ───────────────────────*/
    if(context.offset>0)
    {
        printf("%*c\033[38;2;%sm%s\033[48;2;%sm\033[38;2;%sm%s%s %s\033[48;2;%sm\033[38;2;%sm%s %s %s\033[38;2;%sm%s%s",
               context.offset,' ',
               col1,rs,                    // Cap
               col1, colt1, bld, icon,rs,  // Icon
               col2, colt2, bld, text,rs,  // Text
               col2,rs,nl);                // Cap
    }else{

        printf("\033[38;2;%sm%s\033[48;2;%sm\033[38;2;%sm%s%s %s\033[48;2;%sm\033[38;2;%sm%s %s %s\033[38;2;%sm%s%s",
               col1,rs,                    // Cap
               col1, colt1, bld, icon,rs,  // Icon
               col2, colt2, bld, text,rs,  // Text
               col2,rs,nl);                // Cap
    }
}

void print_bubble_value(my_print_context context, const char* text, void* value, char* value_type, int format){
    char* rs  = "\033[0m";
    char* nl  = (format&1)==1 ? "\n" : " ";
    char* bld = (format&2)==2 ? "\033[1m" : "\033[22m";

    /* ───────────────────────── Color 1  ─────────────────────────*/
    unsigned char R = (context.col1&(255<<24))>>24; 
    unsigned char G = (context.col1&(255<<16))>>16; 
    unsigned char B = (context.col1&(255<<8))>>8; 
    unsigned char A = context.col1&(255);
    (void)A;

    char col1[255];
    sprintf(col1,"%d;%d;%d", R,G,B);
    char colt1[255];
    R = (context.text1&(255<<24))>>24; 
    G = (context.text1&(255<<16))>>16; 
    B = (context.text1&(255<<8))>>8; 
    A =  context.text1&(255); 
    sprintf(colt1,"%d;%d;%d", R,G,B);

    /* ───────────────────────── Color 2 ─────────────────────────*/
    char col2[255];
    R = (context.col2&(255<<24))>>24; 
    G = (context.col2&(255<<16))>>16; 
    B = (context.col2&(255<<8))>>8; 
    A =  context.col2&(255); 
    sprintf(col2,"%d;%d;%d", R,G,B);
    char colt2[255];
    R = (context.text2&(255<<24))>>24; 
    G = (context.text2&(255<<16))>>16; 
    B = (context.text2&(255<<8))>>8; 
    A =  context.text2&(255); 
    sprintf(colt2,"%d;%d;%d", R,G,B);

    /* ─────────────────────── Final Print ───────────────────────*/
    int type_len = strlen(value_type);
    if(value_type[0] == 'u'){
        if(type_len > 1 && value_type[1] == 'i'){
            printf("%*c\033[38;2;%sm%s\033[48;2;%sm\033[38;2;%sm%s%s %s\033[48;2;%sm\033[38;2;%sm%s %u %s\033[38;2;%sm%s%s",
                                                context.offset,' ',
                                                col2, rs,                   // Cap
                                                col2, colt2, bld, text,rs,  // Icon
                                                col1, colt1, bld, *(unsigned int*)value,rs, // Text
                                                col1, rs,nl);               // Cap
        }
        else if ( type_len >1 && value_type[1] == 'l') {
            if(type_len > 2 && value_type[2] == 'l'){
                printf("%*c\033[38;2;%sm%s\033[48;2;%sm\033[38;2;%sm%s%s %s\033[48;2;%sm\033[38;2;%sm%s %llu %s\033[38;2;%sm%s%s",
                                                    context.offset,' ',
                                                    col2, rs,                   // Cap
                                                    col2, colt2, bld, text,rs,  // Icon
                                                    col1, colt1, bld, *(unsigned long long*)value,rs, // Text
                                                    col1, rs,nl);               // Cap
            }
        }
    }

    else if(value_type[0] == 'i'){
        printf("%*c\033[38;2;%sm%s\033[48;2;%sm\033[38;2;%sm%s%s %s\033[48;2;%sm\033[38;2;%sm%s %d %s\033[38;2;%sm%s%s",
                                            context.offset,' ',
                                            col2, rs,                   // Cap
                                            col2, colt2, bld, text,rs,  // Icon
                                            col1, colt1, bld, *(int*)value,rs, // Text
                                            col1, rs,nl);               // Cap
    }

    else if(value_type[0] == 'l'){
        if (type_len>1 && value_type[1] == 'l') {
            printf("%*c\033[38;2;%sm%s\033[48;2;%sm\033[38;2;%sm%s%s %s\033[48;2;%sm\033[38;2;%sm%s %lli %s\033[38;2;%sm%s%s",
                                                context.offset,' ',
                                                col2, rs,                   // Cap
                                                col2, colt2, bld, text,rs,  // Icon
                                                col1, colt1, bld, *(long long int*)value,rs, // Text
                                                col1, rs,nl);               // Cap
        }
        else if (type_len>1 && value_type[1] == 'f') {
            printf("%*c\033[38;2;%sm%s\033[48;2;%sm\033[38;2;%sm%s%s %s\033[48;2;%sm\033[38;2;%sm%s %lf %s\033[38;2;%sm%s%s",
                                                context.offset,' ',
                                                col2, rs,                   // Cap
                                                col2, colt2, bld, text,rs,  // Icon
                                                col1, colt1, bld, *(double*)value,rs, // Text
                                                col1, rs,nl);               // Cap
        }
    }

    else if(value_type[0] == 'L'){
        if(type_len>1 && value_type[1] == 'f'){
            printf("%*c\033[38;2;%sm%s\033[48;2;%sm\033[38;2;%sm%s%s %s\033[48;2;%sm\033[38;2;%sm%s %Lf %s\033[38;2;%sm%s%s",
                                                context.offset,' ',
                                                col2, rs,                   // Cap
                                                col2, colt2, bld, text,rs,  // Icon
                                                col1, colt1, bld, *(long double*)value,rs, // Text
                                                col1, rs,nl);               // Cap
        }
    }

    else if(value_type[0] == 'f'){
        printf("%*c\033[38;2;%sm%s\033[48;2;%sm\033[38;2;%sm%s%s %s\033[48;2;%sm\033[38;2;%sm%s %f %s\033[38;2;%sm%s%s",
                                            context.offset,' ',
                                            col2, rs,                   // Cap
                                            col2, colt2, bld, text,rs,  // Icon
                                            col1, colt1, bld, *(float*)value,rs, // Text
                                            col1, rs,nl);               // Cap
    }
    else if(value_type[0] == 's'){
        printf("%*c\033[38;2;%sm%s\033[48;2;%sm\033[38;2;%sm%s%s %s\033[48;2;%sm\033[38;2;%sm%s %s %s\033[38;2;%sm%s%s",
                                            context.offset,' ',
                                            col2, rs,                   // Cap
                                            col2, colt2, bld, text,rs,  // Icon
                                            col1, colt1, bld, (char*)value,rs, // Text
                                            col1, rs,nl);               // Cap
    }
    else if(value_type[0] == 'p'){
        printf("%*c\033[38;2;%sm%s\033[48;2;%sm\033[38;2;%sm%s%s %s\033[48;2;%sm\033[38;2;%sm%s %p %s\033[38;2;%sm%s%s",
                                            context.offset,' ',
                                            col2, rs,                   // Cap
                                            col2, colt2, bld, text,rs,  // Icon
                                            col1, colt1, bld, value,rs, // Text
                                            col1, rs,nl);               // Cap
    }
}
void print_nvidia_rect(my_print_context context, const char* text1, const char* text2, int format){

    char* rs  = "\033[0m";
    char* nl  = (format&1)==1 ? "\n" : " ";
    char* bld = (format&2)==2 ? "\033[1m" : "\033[22m";
    (void)nl;
    (void)bld;

    /* ───────────────────────── Color 1  ─────────────────────────*/
    unsigned char R = (context.col1&(255<<24))>>24; 
    unsigned char G = (context.col1&(255<<16))>>16; 
    unsigned char B = (context.col1&(255<<8))>>8; 
    unsigned char A = context.col1&(255);
    (void)A;

    char col1[255];
    sprintf(col1,"%d;%d;%d", R,G,B);
    char colt1[255];
    R = (context.text1&(255<<24))>>24; 
    G = (context.text1&(255<<16))>>16; 
    B = (context.text1&(255<<8))>>8; 
    A =  context.text1&(255); 
    sprintf(colt1,"%d;%d;%d", R,G,B);

    /* ───────────────────────── Color 2 ─────────────────────────*/
    char col2[255];
    R = (context.col2&(255<<24))>>24; 
    G = (context.col2&(255<<16))>>16; 
    B = (context.col2&(255<<8))>>8; 
    A =  context.col2&(255); 
    sprintf(col2,"%d;%d;%d", R,G,B);
    char colt2[255];
    R = (context.text2&(255<<24))>>24; 
    G = (context.text2&(255<<16))>>16; 
    B = (context.text2&(255<<8))>>8; 
    A =  context.text2&(255); 
    sprintf(colt2,"%d;%d;%d", R,G,B);

    printf("%*c\033[38;2;%sm%s\033[48;2;%sm\033[38;2;%sm %s %s\033[38;2;%sm\033[48;2;%sm%s\033[48;2;%sm\033[38;2;%sm %s %s\033[48;2;%sm\033[38;2;%sm%s\033[38;2;%sm%s\n",
           context.offset, ' ',
           col1,rs,
           col1,colt1, text1,rs,
           col1,col2,rs,
           col2,colt2,text2,rs,
           col2,col1,rs,col1,rs);
}

/*
 * @breif : Perofrms a check on packets of tests and returns the `index` on the test it matched =>
 * (A,B,C) (if A == NULL) returns idx of `A` regardless if `B` and `C` tests are positive.
 * It checks pointers and integers only since every variable provided is converted to (long long) even pointers
 * @param test_count: Item count of the test array
 * @param test_packets: Array of structures `sanity_check_packet`
 * @returns: the `idx` of the first positive test. If all tests are negative it returns -1
 * */
int sanity_check_i(int test_count, sanity_check_packet* test_packets ){
        int error_idx = -1;

        #pragma clang diagnostic push
        #pragma clang diagnostic ignored "-Wpointer-integer-compare"
        
        for(int i=0; i<test_count; ++i){
            sanity_check_packet packet = test_packets[i];
            long long var = packet.test_type & _SC_DEREF ? (long long)*(int*)(packet.variable):
                            (long long)packet.variable;
            if(packet.test_type & _SC_GREATER ){
                if(packet.test_type & _SC_EQUALS){
                    if(var >= packet.result){
                        error_idx = i;
                        return error_idx;
                    }
                }else{
                    if(var > packet.result){
                        error_idx = i;
                        return error_idx;
                    }
                }
                continue;
            }else if( packet.test_type & _SC_LESSER ){
                if(packet.test_type & _SC_EQUALS){
                    if(var <= packet.result){
                        error_idx = i;
                        return error_idx;
                    }
                }else{
                    if((var < packet.result)){
                        error_idx = i;
                        return error_idx;
                    }
                }
                continue;
            }else if(packet.test_type & _SC_EQUALS){
                if(packet.test_type & _SC_NOT){
                    if((var != packet.result)){
                        error_idx = i;
                        return error_idx;
                    }
                }
                else{
                    if(var == packet.result){
                        error_idx = i;
                        return error_idx;
                    }
                }
            }
    }
    #pragma clang diagnostic pop
        return error_idx;
}





