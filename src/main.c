#include "defines.inl"
#include "memory_simulator.h"
#include "instruction_functions.h"
#include "debug_utils.h"
#include <stdlib.h>
#include <string.h>

#ifdef __linux
    #include <limits.h>
    #define MAX_PATH PATH_MAX
#endif



int main() {
    set_color_console();
    asm_host host = {0};
    asm_host* pHost = &host;
    createRegisters(pHost);
    printRegFull(pHost);
    /* return 0;  */


    FILE* f;
    char* rFP = "././resources/listing_0045_challenge_register_movs";
    char fFP[MAX_PATH];
    #ifdef _WIN32
        _fullpath(fFP,rFP,MAX_PATH);
    #else
        realpath(rFP, fFP);
    #endif

    i32 open_result = 0;
    #ifdef _WIN32
        open_result = fopen_s(&f, fFP, "rb");
    #else
        f=fopen(fFP,"rb");
    #endif

    if(open_result != 0){
        #ifdef _WIN32
            char error_buf[512];
            strerror_s(error_buf, 512, open_result);
            printf("Error:\n\t%s\n\t%s",fFP, error_buf);
        #else
            printf("Error:\n\t%s",strerror(open_result));
        #endif
    }
    /* THIS_SPOT("%d", open_result); */
    fseek(f, 0,SEEK_END);
    long flen = ftell(f);
    rewind(f);
    u8* main_buffer = calloc(flen + 1,1);
    fread(main_buffer,1,flen,f);

    fclose(f);
    
    decodeBinary(pHost, main_buffer, flen);

    printRegFull(pHost);

    return 0;
   
}



