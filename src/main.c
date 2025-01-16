#include "defines.inl"
#include "helper_tables.h"
#include <string.h>
#ifndef DBGU_IMPLEMENTATION
    #define DBGU_IMPLEMENTATION
    #include "debug_utils.h"
#endif
#include "small_utils.h"

#include <stdio.h>
#include <stdlib.h>
#ifdef __linux
    #include <limits.h>
    #define MAX_PATH PATH_MAX
#endif

//       
// |  1st Byte  |x| 2nd Byte   |x| 3rd Byte |x| 4th Byte |x| 5th Byte |x| 6-th Byte |x|
// ||OPCODE|D|W||x| MOD|REG|R/M|x|          |x|          |x|          |x|           |x|
// |------------|x|------------|x|----------|x|----------|x|----------|x|-----------|x|          
// ||000000|0|0||x| 00 |000|000|x|          |x|          |x|          |x|           |x|
// |------------|x|------------|x|----------|x|----------|x|----------|x|-----------|x|
// __ 1st Byte ________________________________________________________
// OpCode -> describes the operation
// D      -> describes if REG is src=0 or dst=1
// W      -> describes if the operans are 1 byte or 1 word(4bytes)
// __ Secibd Byte _____________________________________________________
// MOD    -> defines if RM is a register or memory8-16 address
// REG    -> Address that is a register
// R/M    -> Address that is either register or memory

typedef struct instrCodeEntry{
    unsigned int hi:4;
    unsigned int lo:4;
}instrCodeEntry;

instrCodeEntry istrCodeTable[16*16];
void initCodeTable(instrCodeEntry* table){
    for(u32 y=0; y<16; ++y){
        for(u32 x=0; x<16; ++x){
            table[(y*16)+x].hi =y;
            table[(y*16)+x].lo =x;
        }
    }
}


u32 getInstructionID(u8* reader_pos){
    u8 opCode = *(u8*)reader_pos;
    u8 opCodeHi = ( opCode & 0xF0 ) >>4;
    u8 opCodeLo =   opCode & 0x0F;
    /* THIS_SPOT("%s", OBJ2BIN(opCodeLo)); */
    u32 instrID =0;
    for(u16 y=0;y<16;++y){
        /* printf("- %02X ",istrCodeTable[y*16].hi); */
        if(opCodeHi == (u8)istrCodeTable[y*16].hi){
            for(u16 x =0; x<16;++x){
                /* printf(" %02x ",istrCodeTable[y*16+x].lo); */
                if(opCodeLo == (u8)istrCodeTable[(y*16)+x].lo){
                    instrID = cmdIDTable[(y*16)+x];
                    break;
                }
            }
            /* putchar('\n'); */
        }
        /* putchar('\n'); */
    }
    return instrID;
}

u32 decodeMOV(u8* instruction_pos);
u32 decodeADD(u8* instruction_pos);
u32 decodeSUB(u8* instruction_pos);
u32 decodeCMP(u8* instruction_pos);
u32 decodeJMP(u8* instruction_pos){
    UNUSED instruction_pos;
    u32 offset = 2;
    u8 hi = (*instruction_pos & 0xF0)>>4;
    u8 lo = *instruction_pos  & 0x0F;

    char memNameBuf[255]={0};
    printf("JMP ");
    switch(hi){
        case 0x0F:
            THIS_SPOT("Indirect");
            u8 byte2 = *(instruction_pos+1);        // quick reference for the second Byte in the instruction( all cases where 8 is the high 4 bits, we have at least 1 byte after )
            u8 mod  = ( byte2 & 192 ) >> 6;         // get the MOD description
            u8 reg  = (byte2 & 56) >> 3;            // get the REG address ID
            u8 rm   = (byte2 & 7);                  // get the REG or MEM addres ID
            u8 modOffsets[] = {2,3,4,2};
            offset = modOffsets[mod];
            switch(reg){
                case 6:
                    THIS_SPOT("Within Segment");
                    break;
                case 7:
                    THIS_SPOT("Intersegment");
                    break;
                default:break;
            }
            break;
        case 0x0E:
            char* loopNames[] = {"LOOPNZ","LOOPZ","LOOP","JECXZ"};
            i8 offset = *(i8*)(void*)(instruction_pos+1);
            printf("%s to (%d)",loopNames[lo],  offset);
            switch(lo){
                case 9:
                    THIS_SPOT("Within Segment");
                    offset = 3;
                    break;
                case 10:
                    THIS_SPOT("Intersegment");
                    offset = 5;
                    break;
                case 11:
                    THIS_SPOT("Within Segment-short");
                    offset = 2;
                    break;

                default:break;
            }
            break;
        case 0x07:
            char* jumpNames[]={"JO","JNO","JB","JNB","JE","JNE/JNZ","JBE",
                               "JA","JS","JNS","JPE","JPO/JNP","JL","JGE/JNL",
                               "JLE","JG"};
            offset = *(i8*)(void*)(instruction_pos+1);
            printf("%s to (%d)",jumpNames[lo],  offset);
            /* THIS_SPOT("Conditional"); */
            break;
        default:break;
    }

    return offset;
}


int main() {
    set_color_console();
    initCodeTable(istrCodeTable);

    FILE* f;
    char* rFP = "././resources/listing_0041_add_sub_cmp_jnz";
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
            printf("Error:\n\t%s", error_buf);
        #else
            printf("Error:\n\t%s",strerror(open_result));
        #endif
    }
    /* THIS_SPOT("%d", open_result); */
    fseek(f, 0,SEEK_END);
    long flen = ftell(f);
    rewind(f);
    int opCodesLen = ARRLEN(opCodes);
    u8* main_buffer = calloc(flen + 1,1);
    fread(main_buffer,1,flen,f);

    fclose(f);
    
    u32 i=0;
    u32 instruction_number = 1;
    u8* reader = main_buffer;
    
    while(i<(u32)flen){
        u8 dbg_byte = *reader;
        u32 instructionID = getInstructionID(reader);
        printf("%03d -(%02X %s)- (%03d) ",instruction_number, dbg_byte, OBJ2BIN(dbg_byte),instructionID);
        
        u32 offset = 0;
        switch(instructionID){
            case op_MOV:
                offset = decodeMOV(reader);
                /* THIS_SPOT("Offset is %d", offset); */
                break;
            case op_ADD:
                offset = decodeADD(reader);
                break;
            case op_SUB:
                offset = decodeSUB(reader);
                break;
            case op_CMP:
                offset = decodeCMP(reader);
                break;
            case op_JMP:
                offset = decodeJMP(reader);
                break;
            case op_LES:
                /* printf("- %s -\n",OBJ2BIN(*(u8*)reader)); */
                offset = decodeMOV(reader);
                break;
            default:
                offset++;
                break;
        }

        reader += offset;
        i += offset;
        instruction_number ++;
        putchar('\n');
    }
    putchar('\n');

    return 0;
   
}





u32 decodeMOV(u8* instruction_pos){
    printf("MOV");
    u32 offset = 2;
    u8 hi = (*instruction_pos & 0xF0)>>4;
    u8 lo = *instruction_pos  & 0x0F;

    char memNameBuf[255]={0};
    
    // proceed based on high bit type
    switch(hi){
        case 8: // Basic MOV
            u8 type = (lo & 12) >> 2;               // get lower 4 bits
            u8 d    = (lo&2) >> 1;                  // get direction/ to or from REG
            u8 w    = (lo&1);                       // get size of REGs
            u8 byte2 = *(instruction_pos+1);        // quick reference for the second Byte in the instruction( all cases where 8 is the high 4 bits, we have at least 1 byte after )
            u8 mod  = ( byte2 & 192 ) >> 6;         // get the MOD description
            u8 reg  = (byte2 & 56) >> 3;            // get the REG address ID
            u8 rm   = (byte2 & 7);                  // get the REG or MEM addres ID

            // Set the correct offset(the return) for the next instruction
            switch(mod){
                case 0: // RM is MEM with no offset bytes [addr]
                    offset = 2;
                    break;
                case 1: // RM is MEM with 1 offset Byte [addr + u8]
                    offset = 3;
                    break;
                case 2: // RM is MEM with 2 offset Bytes [addr + u16] 
                    offset = 4;
                    break;
                case 3: // RM is REG thus no offset
                    offset = 2;
                    break;
                default:
                    break;
            }
            // proceed based on type/lo bits
            switch(type){
                case 2:
                    switch(mod){
                        case 3:
                            printf("to %s from %s ",d?(w?regNamesW1[reg]:regNamesW0[reg]):(w?regNamesW1[rm]:regNamesW0[rm]),
                                                    d?(w?regNamesW1[rm]:regNamesW0[rm]):(w?regNamesW1[reg]:regNamesW0[reg])
                                  
                            );
                            break;
                        case 0:
                            if(rm != 6){
                                snprintf(memNameBuf, 255,"[ %s ]",memNames0[rm]);
                                printf("to %s from %s ",d?(w?regNamesW1[reg]:regNamesW0[reg]): memNameBuf,
                                                    d?(memNameBuf):(w?regNamesW1[reg]:regNamesW0[reg])
                                );
                            }
                            else{
                                // Direct Access uses the only 2 displacement bytes to describe the address
                                i32 displ = *(u16*)(void*)(instruction_pos+2);
                                snprintf(memNameBuf, 255,"[ %d ]",displ);
                                printf("to %s from %s ", w?regNamesW1[reg]:regNamesW0[reg], memNameBuf
                                       );
                                offset = 4;
                            }
                            break;
                        case 1:
                            /* THIS_SPOT("8bit - %s %s %s",OBJ2BIN(*(u8*)instruction_pos), OBJ2BIN(*(u8*)(instruction_pos+1)), OBJ2BIN(*(u8*)(instruction_pos+2)) ); */
                            /* printf("TODO: MEM case displace 8 bit"); */
                            i8 offset8 = *(i8*)(instruction_pos + 2 );
                            if(offset8 != 0)
                                snprintf(memNameBuf, 255,"[ %s %s %d ]",memNames0[rm], offset8 > 0 ?"+":"-", ABS(offset8));
                            else
                                snprintf(memNameBuf, 255,"[ %s ]",memNames0[rm]);
                            printf("to %s from %s ",d?(w?regNamesW1[reg]:regNamesW0[reg]): memNameBuf,
                                                    d?(memNameBuf):(w?regNamesW1[reg]:regNamesW0[reg])
                                   );
                            break;
                        case 2:
                            /* THIS_SPOT("16bit - %s %s %s",OBJ2BIN(*(u8*)instruction_pos), OBJ2BIN(*(u8*)(instruction_pos+1)), OBJ2BIN(*(u16*)(instruction_pos+2)) ); */
                            /* printf("TODO: MEM case displace 16 bit"); */
                            i16 offset16 = *(i16*)(instruction_pos + 2 );
                            if(offset16!=0)
                                snprintf(memNameBuf, 255,"[ %s %s %d ]",memNames0[rm],offset16 > 0 ?"+":"-", ABS(offset16));
                            else
                                snprintf(memNameBuf, 255,"[ %s ]",memNames0[rm]);
                            printf("to %s from %s ",d?(w?regNamesW1[reg]:regNamesW0[reg]): memNameBuf,
                                                    d?(memNameBuf):(w?regNamesW1[reg]:regNamesW0[reg])
                                   );
                            break;
                        default:
                            break;
                    }
                    break;
                case 3:
                    THIS_SPOT("16bit - %s %s",OBJ2BIN(*(u8*)instruction_pos), OBJ2BIN(*(u8*)(instruction_pos+1)));
                    printf("R/M from/to SReg");
                    break;
                default:
                    break;
            }
            break;

        case 10: // MEM TO ACCUMULATOR
            /* THIS_SPOT("!TODO! MOV MEM ACCU %s", OBJ2BIN(lo)); */
            d = ( lo & 0b0010 ) >> 1;
            w = lo & 0b0001;
            i32 displ_10 = w ? *(i16*)(void*)(instruction_pos+1):
                        *(i8*)(void*)(instruction_pos+1);
            snprintf(memNameBuf, 255, "[ %d ]", displ_10);
            printf("to %s from %s ", d ? memNameBuf : "AX",
                                     d ? "AX": memNameBuf);
            offset = w ? 3 : 2;
            break;

        case 11: // MOV IMMEDIATE to REG, data could be 1 or 2 bytes
            printf("IMM ");
            w = (lo&8)>>3;
            reg = (lo&7);
            
            for(u8 i=0; i < 8; i++ ){
                if(reg == regCodes[i]){
                    printf("to %s ", w ? regNamesW1[i] : regNamesW0[i]);
                }
            }
            
            if(w){
                i16 data = *(u16*)(instruction_pos+1);
                printf("the 16 bit value %d ", data);
            }else{
                i8 data = *(instruction_pos+1);
                printf("the 08 bit value %d ", data);
            }
            offset = w ? 3 : 2;
            /* printf("W=%d",w); */
            break;
        case 12:
            w    = (lo&1);                       // get size of REGs
            byte2 = *(instruction_pos+1);        // quick reference for the second Byte in the instruction( all cases where 8 is the high 4 bits, we have at least 1 byte after )
            mod  = ( byte2 & 192 ) >> 6;         // get the MOD description
            // REG is 000 always
            rm   = (byte2 & 7);                  // get the REG or MEM addres ID
            
            // getting the displacement based on MOD
            i32 displ = mod == 1 ? (i32)*( u8*)(void*)( instruction_pos + 2 ):
                        mod == 2 ? (i32)*(u16*)(void*)( instruction_pos + 2 ):
                        mod == 0 ? 0 : -1;
            u8 mod_d_check = mod | (w<<2); // packing mod and w together -> 111 loking only the last 3 bits so we do a single check

            i32 data = mod_d_check == 0  ? (i32)*( u8*)(void*)(instruction_pos + 2) : // 0 Displacement       | Data is 1 byte 
                       mod_d_check == 1  ? (i32)*( u8*)(void*)(instruction_pos + 3) : // 1 Byte displacement  | Data is 1 Byte
                       mod_d_check == 2  ? (i32)*( u8*)(void*)(instruction_pos + 4) : // 2 Bytes displacement | Data is 1 Byte
                       mod_d_check == 3  ? (i32)*( u8*)(void*)(instruction_pos + 2) : // 0 Displacement(REG)  | Data is 1 Byte
                       mod_d_check == 4  ? (i32)*(u16*)(void*)(instruction_pos + 2) : // 0 Displacment        | Data is 2 Bytes
                       mod_d_check == 5  ? (i32)*(u16*)(void*)(instruction_pos + 3) : // 1 Byte displacement  | Data is 2 Bytes
                       mod_d_check == 6  ? (i32)*(u16*)(void*)(instruction_pos + 4) : // 2 Bytes displacement | Data is 2 Bytes
                       mod_d_check == 7  ? (i32)*(u16*)(void*)(instruction_pos + 2) : // 0 Displacement(REG)  | Data is 2 Bytes
                       0xFFFFFF;
            offset = mod_d_check == 0 ? 3 : 
                     mod_d_check == 1 ? 4 :
                     mod_d_check == 2 ? 5 :
                     mod_d_check == 3 ? 3 :
                     mod_d_check == 4 ? 4 :
                     mod_d_check == 5 ? 5 :
                     mod_d_check == 6 ? 6 :
                     mod_d_check == 7 ? 4 : 2;

            /* THIS_SPOT("16bit - %s %s",OBJ2BIN(*(u8*)instruction_pos), OBJ2BIN(*(u8*)(instruction_pos+1))); */
            /* THIS_SPOT("%d %d %d (%s),%d -> %d", w, mod, displ, OBJ2BIN(mod_d_check), data, offset); */
            /* printf("MOV IMM R/M"); */
            if(displ != 0)
                snprintf(memNameBuf, 255,"[ %s %s %d ]",memNames0[rm], displ > 0 ?"+":"-", ABS(displ));
            else
                snprintf(memNameBuf, 255,"[ %s ]",memNames0[rm]);
            printf("to %s the %s value %d",memNameBuf, w? "16bit":"8bit", data);
            break;
        default:
            break;
    }


    return offset;
}



u32 decodeADD(u8* instruction_pos){
    UNUSED instruction_pos;
    u32 offset = 2;
    u8 hi = (*instruction_pos & 0xF0)>>4;
    u8 lo = (*instruction_pos & 0x0F);
    char memNameBuf[255]={0};

    switch(hi){
        case 0:
            printf("ADD ");
            switch(lo>>2){
                case 0:
                    u8 d = (lo & 2)>>1;
                    u8 w =  lo & 1;
                    u8 byte2 = *(instruction_pos + 1);
                    u8 mod   = (byte2 & 192)>>6;
                    u8 reg   = (byte2 & 56 )>>3;
                    u8 rm    = (byte2 & 7);
                    u32 displ= mod == 1 ? *(u8*)(instruction_pos+2):
                               mod == 2 ? *(u16*)(void*)(instruction_pos+2): 0 ;
                    // Set the correct offset(the return) for the next instruction
                    u8 modoffsets[] = {2,3,4,2};
                    offset = modoffsets[mod];
                    /* THIS_SPOT("offset %d", offset); */
                    switch(mod){
                        case 0:
                            snprintf(memNameBuf, 255,"[ %s ]",memNames0[rm]);
                            printf("to %s val in %s ",d?(w?regNamesW1[reg]:regNamesW0[reg]): memNameBuf,
                                                    d?(memNameBuf):(w?regNamesW1[reg]:regNamesW0[reg])
                            );
                            break;
                        case 1:
                            u8 sign = displ & 128;
                            if(displ>0) snprintf(memNameBuf, 255,"[ %s %s %d ]",memNames0[rm],sign > 0 ? "-": "+", displ);
                            else snprintf(memNameBuf, 255,"[ %s ]",memNames0[rm]);
                            printf("to %s val in %s ",d?(w?regNamesW1[reg]:regNamesW0[reg]): memNameBuf,
                                                    d?(memNameBuf):(w?regNamesW1[reg]:regNamesW0[reg])
                            );
                            break;
                        case 2:
                            THIS_SPOT(NULL);
                            break;
                        case 3: 
                            snprintf(memNameBuf, 255,"[ %s ]",w?regNamesW1[rm]:regNamesW0[rm]);
                            printf("to %s val in %s ",d?(w?regNamesW1[reg]:regNamesW0[reg]): memNameBuf,
                                                    d?(memNameBuf):(w?regNamesW1[reg]:regNamesW0[reg])
                            );
                            break;
                        default: break;
                    }
                    break;
                case 1:
                    w = lo & 1;
                    u8 wOffsets[2] = {2,3};
                    offset = wOffsets[w];
                    i32 data = 0;
                    if(w){
                        data = *(i16*)(void*)(instruction_pos+1) & 0x0000FFFF;
                        if( data & 0x00008000){
                            data |= 0xFFFF0000;;
                        }
                    }else{
                        data = *(i8*)(void*)(instruction_pos+1)  & 0x000000FF;
                        if( data & 0x00000080){
                            data |= 0xFFFFFF00;;
                        }
                    }

                    printf("to %s val %d",w ? "AX" : "AL", data);
                    break;
                default:
                    break;
            }
            break;
        case 1:
            THIS_SPOT("ADD with carry RegMem/ACCUMULATOR");
            break;
        case 8:
            u8 s = (lo & 2)>>1;
            u8 w =  lo & 1;
            u8 byte2 = *(instruction_pos + 1);
            u8 mod   = (byte2 & 192)>>6;
            u8 ext   = (byte2 & 56 )>>3;
            u8 rm    = (byte2 & 7);
            i32 displ = mod == 1 ? *(i8*)(void*)(instruction_pos+2):
                        mod == 2 ? *(i16*)(void*)(instruction_pos+2): 
                        (mod == 0 && rm == 6) ? *(i16*)(void*)(instruction_pos+2) : 0;
            // When we have Instructions with S bit, then 0 means the data is 16bits, when it's 1 it means data is 8bit but it's sign bit is extended so it fits 16bits.
            //  This works only when W is 1, because it means that the register is 16 bit. So the value must be properly fitted. This is only for the CPU not us, but it
            //  affects the offset and the size we must retrive from the instruction_pos
            i32 data =  mod == 1 ? ( (w&&s==0) ? *(i16*)(void*)(instruction_pos+3) & 0x00FF: *(i8*)(void*)(instruction_pos+3) & 0x00FF ) :
                        mod == 2 ? ( (w&&s==0) ? *(i16*)(void*)(instruction_pos+4) & 0x00FF: *(i8*)(void*)(instruction_pos+4) & 0x00FF) :
                        (w&&s==0) ? *(i16*)(void*)(instruction_pos+2) & 0x00FF: *(i8*)(void*)(instruction_pos+2) & 0x00FF;
            u8 offsetTable[]={3,4,5,3,
                              4,5,6,4};
            offset = offsetTable[w*4+mod];
            offset -= w ? (s ? 1 : 0) : 0;// Adjusting the offset based on the Signed(S) bit
            /* THIS_SPOT("Offset = %d",offset); */

            /* THIS_SPOT("%d|%d| %d|%d|%d",s, w, mod, ext,rm); */
            
            u8    extTypes[8] = {0,1,2,3,4,5,6,7};
            char* extNames[]  = {"ADD","OR","ADC","SBB","AND","SUB","XOR","CMP"};
            if(mod == 3){
                snprintf(memNameBuf,255, " %s ", w ? regNamesW1[rm]:regNamesW0[rm]);
            }else if(mod == 0){
                u8 sign = displ & 128;
                if(rm !=6){
                    if(displ)
                        snprintf(memNameBuf,255, "[ %s %s %d ]", w ? regNamesW1[rm]:regNamesW0[rm],
                                                             s ? "+":"-", displ);
                    else
                        snprintf(memNameBuf,255, "[ %s ]", memNames0[rm]);
                }else{//Direct Memory Access where Displacement is an address
                    snprintf(memNameBuf,255, "[ %d ]", displ);
                    offset+=2; // we have to increase the offset because in this unique case we took 2 bytes for the direct addressing
                }
            }else{
                u8 sign = displ & 128;
                snprintf(memNameBuf,255, "[ %s %s %d ]", memNames0[rm],
                                                         s ? "+":"-", displ);
            }

            printf("%s", extNames[ext]);
            printf(" to %s , %d ", memNameBuf, data);

            break;
        default:
            break;
    }
    printf(" offset(%d)", offset);
    return offset;
}



u32 decodeSUB(u8* instruction_pos){
    UNUSED instruction_pos;
    u32 offset = 2;
    u8 hi = (*instruction_pos & 0xF0)>>4;
    u8 lo = (*instruction_pos & 0x0F);
    char memNameBuf[255]={0};
    printf("SUB ");
    switch(hi){
        case 1:
            THIS_SPOT(NULL);
            break;
        case 2:
            switch(lo>>2){
                case 0:THIS_SPOT(NULL);break;
                case 1:THIS_SPOT(NULL);break;
                case 2:
                    u8 d = (lo & 2)>>1;
                    u8 w =  lo & 1;
                    u8 byte2 = *(instruction_pos + 1);
                    u8 mod   = (byte2 & 192)>>6;
                    u8 reg   = (byte2 & 56 )>>3;
                    u8 rm    = (byte2 & 7);
                    u32 displ= mod == 1 ? *(u8*)(instruction_pos+2):
                               mod == 2 ? *(u16*)(void*)(instruction_pos+2): 0 ;
                    // Set the correct offset(the return) for the next instruction
                    u8 modoffsets[] = {2,3,4,2};
                    offset = modoffsets[mod];
                    /* THIS_SPOT("offset %d", offset); */
                    switch(mod){
                        case 0:
                            snprintf(memNameBuf, 255,"[ %s ]",memNames0[rm]);
                            printf("to %s val in %s ",d?(w?regNamesW1[reg]:regNamesW0[reg]): memNameBuf,
                                                    d?(memNameBuf):(w?regNamesW1[reg]:regNamesW0[reg])
                            );
                            break;
                        case 1:
                            u8 sign = displ & 128;
                            if(displ>0) snprintf(memNameBuf, 255,"[ %s %s %d ]",memNames0[rm],sign > 0 ? "-": "+", displ);
                            else snprintf(memNameBuf, 255,"[ %s ]",memNames0[rm]);
                            printf("to %s val in %s ",d?(w?regNamesW1[reg]:regNamesW0[reg]): memNameBuf,
                                                    d?(memNameBuf):(w?regNamesW1[reg]:regNamesW0[reg])
                            );
                            break;
                        case 2:
                            THIS_SPOT(NULL);
                            break;
                        case 3: 
                            snprintf(memNameBuf, 255,"[ %s ]",w?regNamesW1[rm]:regNamesW0[rm]);
                            printf("to %s val in %s ",d?(w?regNamesW1[reg]:regNamesW0[reg]): memNameBuf,
                                                    d?(memNameBuf):(w?regNamesW1[reg]:regNamesW0[reg])
                            );
                            break;
                        default: break;
                    }
                    break;
                case 3:
                    w = lo & 1;
                    u8 wOffsets[2] = {2,3};
                    offset = wOffsets[w];
                    i32 data = 0;
                    if(w){
                        data = *(i16*)(void*)(instruction_pos+1) & 0x0000FFFF;
                        if( data & 0x00008000){
                            data |= 0xFFFF0000;;
                        }
                    }else{
                        data = *(i8*)(void*)(instruction_pos+1)  & 0x000000FF;
                        if( data & 0x00000080){
                            data |= 0xFFFFFF00;;
                        }
                    }

                    printf("to %s val %d",w ? "AX" : "AL", data);
                    break;
                default:break;
            }
            break;
        default:
            break;
    }

    return offset;
}


u32 decodeCMP(u8* instruction_pos){
    UNUSED instruction_pos;
    u32 offset = 2;
    u8 hi = (*instruction_pos & 0xF0)>>4;
    u8 lo = (*instruction_pos & 0x0F);
    char memNameBuf[255]={0};
    
    printf("CMP ");
    switch(lo>>2){
        case 0:THIS_SPOT(NULL);
        case 1:THIS_SPOT(NULL);
        case 2:
            u8 d = (lo & 2)>>1;
            u8 w =  lo & 1;
            u8 byte2 = *(instruction_pos + 1);
            u8 mod   = (byte2 & 192)>>6;
            u8 reg   = (byte2 & 56 )>>3;
            u8 rm    = (byte2 & 7);
            u32 displ= mod == 1 ? *(u8*)(instruction_pos+2):
                       mod == 2 ? *(u16*)(void*)(instruction_pos+2): 0 ;
            // Set the correct offset(the return) for the next instruction
            u8 modoffsets[] = {2,3,4,2};
            offset = modoffsets[mod];
            /* THIS_SPOT("offset %d", offset); */
            switch(mod){
                case 0:
                    snprintf(memNameBuf, 255,"[ %s ]",memNames0[rm]);
                    printf("to %s val in %s ",d?(w?regNamesW1[reg]:regNamesW0[reg]): memNameBuf,
                                            d?(memNameBuf):(w?regNamesW1[reg]:regNamesW0[reg])
                    );
                    break;
                case 1:
                    u8 sign = displ & 128;
                    if(displ>0) snprintf(memNameBuf, 255,"[ %s %s %d ]",memNames0[rm],sign > 0 ? "-": "+", displ);
                    else snprintf(memNameBuf, 255,"[ %s ]",memNames0[rm]);
                    printf("to %s val in %s ",d?(w?regNamesW1[reg]:regNamesW0[reg]): memNameBuf,
                                            d?(memNameBuf):(w?regNamesW1[reg]:regNamesW0[reg])
                    );
                    break;
                case 2:
                    THIS_SPOT(NULL);
                    break;
                case 3: 
                    snprintf(memNameBuf, 255,"[ %s ]",w?regNamesW1[rm]:regNamesW0[rm]);
                    printf("to %s val in %s ",d?(w?regNamesW1[reg]:regNamesW0[reg]): memNameBuf,
                                            d?(memNameBuf):(w?regNamesW1[reg]:regNamesW0[reg])
                    );
                    break;
                default: break;
            }
            break;
        case 3: // Immediate
            w = lo & 1;
            u8 wOffsets[2] = {2,3};
            offset = wOffsets[w];
            i32 data = 0;
            if(w){
                data = *(i16*)(void*)(instruction_pos+1) & 0x0000FFFF;
                if( data & 0x00008000){
                    data |= 0xFFFF0000;;
                }
            }else{
                data = *(i8*)(void*)(instruction_pos+1)  & 0x000000FF;
                if( data & 0x00000080){
                    data |= 0xFFFFFF00;;
                }
            }

            printf("to %s val %d",w ? "AX" : "AL", data);
            break;
        default:break;
    }

    return offset;
}



