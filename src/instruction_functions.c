#include "instruction_functions.h"
#include "memory_simulator.h"
#include "small_utils.h"
#include <stdio.h>

#include "debug_utils.h"
#ifndef _TABLE
    #include "helper_tables.h"
#endif

// Based on Intel 8086 manual. chapter: "Machine Instruction Encoding and Decoding"
// |  1st Byte  |x| 2nd Byte   |x| 3rd Byte |x| 4th Byte |x| 5th Byte |x| 6-th Byte |x|
// ||OPCODE|D|W||x| MOD|REG|R/M|x|   Displ  |x|   Displ  |x|   Data   |x|   Data    |x|
// |------------|x|------------|x|----------|x|----------|x|----------|x|-----------|x|
// ||000000|0|0||x| 00 |000|000|x|          |x|          |x|          |x|           |x|
// |------------|x|------------|x|----------|x|----------|x|----------|x|-----------|x|
// __ 1st Byte ________________________________________________________
// OpCode -> describes the operation
// D      -> describes if REG is src=0 or dst=1
// S      -> sometimes D can be the S flag, it means that `Data` is sign filled to fit in the register
//           if S is 1 it means Data is 8bit and is `sign filled` to match the 18bit register. If S is 0,
//           Data is already 16bit.
// W      -> describes if the operans are 1 byte or 1 word(2 bytes)
// __ Secibd Byte _____________________________________________________
// MOD    -> defines if RM is a register or memory address with offset either 8 or 16bits
// REG    -> Address that is a register. Some commands REG is an opCode extension
// R/M    -> Address that is either register or memory depending on the value of MOD
//

/*
 * Gets Instruction ID based on enum
*/
u32 getInstructionID(u8* reader_pos){
    u8 opCode = *(u8*)reader_pos;
    u8 opCodeHi = ( opCode & 0xF0 ) >>4;
    u8 opCodeLo =   opCode & 0x0F;
    u32 instrID =cmdIDTable[opCodeHi*16+opCodeLo];
    return instrID;
}


void decodeBinary(asm_host* host, u8* binary_buffer, u64 buf_len){
    my_print_context maincon = {.offset=0, .col1=0x757575FF, .text1=0x424242FF, .col2=0, .text2=0};
    u32 i=0;
    u32 instruction_number = 1;
    char instruction_numberBuffer[15]={0};
    u8* reader = binary_buffer;
    
    while(i<(u32)buf_len){
        u8 dbg_byte = *reader;
        u32 instructionID = getInstructionID(reader);
        sprintf_s(instruction_numberBuffer,15,"%03d",instruction_number);
        print_bubble(maincon.text1, maincon.col1,0,instruction_numberBuffer,"s",0);
        /* printf("%03d  ",instruction_number ); */
        /* printf("%03d -(%02X %s)- (%03d) ",instruction_number, dbg_byte, OBJ2BIN(dbg_byte),instructionID); */
        
        u32 offset = 0;
        if(instruction_number > 20 )break;
        switch(instructionID){
            case op_MOV:
                offset = decodeMOV(host, reader);
                /* THIS_SPOT("Offset is %d", offset); */
                break;
            case op_ADD:
                offset = decodeADD(host, reader);
                break;
            case op_SUB:
                offset = decodeSUB(host, reader);
                break;
            case op_CMP:
                offset = decodeCMP(host, reader);
                break;
            case op_JMP:
                offset = decodeJMP(reader);
                break;
            case op_LES:
                /* printf("- %s -\n",OBJ2BIN(*(u8*)reader)); */
                offset = decodeMOV(host, reader);
                break;
            default:
                printf("TODO: %s", instructionNames[instructionID]);
                offset++;
                break;
        }

        reader += offset;
        i += offset;
        instruction_number ++;
        putchar('\n');
    }
    putchar('\n');
}


u32 decodeMOV(asm_host* host, u8* instruction_pos){
    my_print_context movcon = {0};
    movcon.col1  = 0xF44336FF;
    movcon.text1 = 0x212121FF;
    u32 offset = 2;
    u8 hi = (*instruction_pos & 0xF0)>>4;
    u8 lo = *instruction_pos  & 0x0F;

    char memNameBuf[255]    = {0};
    char instPrintBuf[512]  = {0};
    u32  instPrintBufOffset =  0;

    instPrintBufOffset += snprintf(instPrintBuf+instPrintBufOffset, 512 - instPrintBufOffset,
                                    "MOV ");
    i16 data = 0;
    i32 reg_id      = -1;
    i8  sub_reg_id  = -1;
    // proceed based on high bit type
    switch(hi){
        case 8: // Basic MOV REG to MEM/R
            /* THIS_SPOT(0); */
            u8 type = (lo & 12) >> 2;               // get lower 4 bits
            u8 d    = (lo&2) >> 1;                  // get direction/ to or from REG
            u8 w    = (lo&1);                       // get size of REGs
            u8 byte2 = *(instruction_pos+1);        // quick reference for the second Byte in the instruction( all cases where 8 is the high 4 bits, we have at least 1 byte after )
            u8 mod  = ( byte2 & 192 ) >> 6;         // get the MOD description
            u8 reg  = (byte2 & 56) >> 3;            // get the REG address ID
            u8 rm   = (byte2 & 7);                  // get the REG or MEM addres ID
            // Set the correct offset(the return) for the next instruction
            u8 offsets[] = {2,3,4,2};
            offset = offsets[mod];

            // ━━━━━━━━━━━━━━━━━━━━ Proceed based on type/lo bits ━━━━━━━━━━━━━━━━━━━━
            switch(type){
                case 2: // REG/MEM from/to REG
                    switch(mod){
                        case 3: // R/M is REG
                            /* THIS_SPOT(0); */
                            reg_id     = d ? reg : (w ? rm   : subRegId[rm]);
                            sub_reg_id = d ? (w ? REG_X: subRegAccess[reg]) :  ( w ? REG_X : subRegAccess[rm]) ; 

                            u32 get_id     = d ? (w ? rm   : subRegId[rm]) : (w ? reg : subRegId[reg]);
                            u8  get_sub_id = d ?  (w ? REG_X: subRegAccess[rm]) :  ( w ? REG_X : subRegAccess[reg]);
                            GetFromReg(host,get_id , get_sub_id, &data); 
                            /* THIS_SPOT("%d %d %d %d",reg_id,sub_reg_id,get_id,get_sub_id); */
                            instPrintBufOffset += snprintf(instPrintBuf+instPrintBufOffset, 512 - instPrintBufOffset,
                                                            "to %s from %s ",d?(w?regNamesW1[reg]:regNamesW0[reg]):(w?regNamesW1[rm]:regNamesW0[rm]),
                                                            d?(w?regNamesW1[rm]:regNamesW0[rm]):(w?regNamesW1[reg]:regNamesW0[reg])
                            );
                            break;
                        case 0: // R/M is Mem with no offset
                            if(rm != 6){ // All non direct address values
                                snprintf(memNameBuf, 255,"[ %s ]",memNames0[rm]);
                                instPrintBufOffset += snprintf(instPrintBuf+instPrintBufOffset, 512 - instPrintBufOffset,
                                                    "to %s from %s ",d?(w?regNamesW1[reg]:regNamesW0[reg]): memNameBuf,
                                                    d?(memNameBuf):(w?regNamesW1[reg]:regNamesW0[reg]));
                            }
                            else{ // R/M is direct address where [displ]
                                // Direct Access uses the only 2 displacement bytes to describe the address
                                i32 displ = *(u16*)(void*)(instruction_pos+2);
                                snprintf(memNameBuf, 255,"[ %d ]",displ);
                                instPrintBufOffset += snprintf(instPrintBuf+instPrintBufOffset, 512 - instPrintBufOffset,
                                                    "to %s from %s ", w?regNamesW1[reg]:regNamesW0[reg], memNameBuf );
                                offset = 4;
                            }
                            break;

                        case 1: // R/M is Mem with 8bit offset
                            i8 offset8 = *(i8*)(instruction_pos + 2 );
                            if(offset8 != 0) // Just for printing
                                snprintf(memNameBuf, 255,"[ %s %s %d ]",memNames0[rm], offset8 > 0 ?"+":"-", ABS(offset8));
                            else
                                snprintf(memNameBuf, 255,"[ %s ]",memNames0[rm]);

                            instPrintBufOffset += snprintf(instPrintBuf+instPrintBufOffset, 512 - instPrintBufOffset,
                                                    "to %s from %s ",d?(w?regNamesW1[reg]:regNamesW0[reg]): memNameBuf,
                                                    d?(memNameBuf):(w?regNamesW1[reg]:regNamesW0[reg]) );
                            break;
                        case 2: // R/M is Mem with16 bit offset
                            i16 offset16 = *(i16*)(instruction_pos + 2 );
                            if(offset16!=0) // Just for printing
                                snprintf(memNameBuf, 255,"[ %s %s %d ]",memNames0[rm],offset16 > 0 ?"+":"-", ABS(offset16));
                            else
                                snprintf(memNameBuf, 255,"[ %s ]",memNames0[rm]);

                            instPrintBufOffset += snprintf(instPrintBuf+instPrintBufOffset, 512 - instPrintBufOffset,
                                                    "to %s from %s ",d?(w?regNamesW1[reg]:regNamesW0[reg]): memNameBuf,
                                                    d?(memNameBuf):(w?regNamesW1[reg]:regNamesW0[reg]) );
                            break;
                        default:
                            break;
                    }
                    break;

                case 3:// Segment REG from/to REG
                    /* THIS_SPOT(0); */
                    u32 seg_ids[]={REG_ES,REG_CS,REG_SS,REG_DS};
                    if(d){ // Immediate to Segment 
                        /* THIS_SPOT(0); */
                        reg_id      = seg_ids[reg];
                        sub_reg_id  = REG_X;
                        GetFromReg(host, rm, REG_X, &data);
                    }else{ // From REG to Segment
                        reg_id      = rm;
                        sub_reg_id  = REG_X;
                        
                        u32 get_reg_id      = seg_ids[reg];
                        u8  get_sub_reg_id  = REG_X;
                        GetFromReg(host, get_reg_id, get_sub_reg_id, &data);
                    }

                    /* printf(" [%d %d - (%4X)] ",rm,REG_X, data); */
                    snprintf(memNameBuf, 255, "[ %s ]",segRegNames[reg]);
                    instPrintBufOffset += snprintf(instPrintBuf+instPrintBufOffset, 512 - instPrintBufOffset,
                                          "to %s from %s", d ?  memNameBuf: regNamesW1[rm] , d ? regNamesW1[rm] : memNameBuf);

                    break;
                default:
                    break;
            }
            break;

        case 10: // MEM TO ACCUMULATOR(AX)
            d = ( lo & 0b0010 ) >> 1;
            w = lo & 0b0001;
            i32 displ_10 = w ? *(i16*)(void*)(instruction_pos+1):
                        *(i8*)(void*)(instruction_pos+1);
            snprintf(memNameBuf, 255, "[ %d ]", displ_10);
            instPrintBufOffset += snprintf(instPrintBuf+instPrintBufOffset, 512 - instPrintBufOffset,
                                    "to %s from %s ", d ? memNameBuf : "AX",
                                     d ? "AX": memNameBuf);
            offset = w ? 3 : 2;
            break;

        case 11: // MOV IMMEDIATE to REG, data could be 8bit or 16bit bytes
            /* THIS_SPOT(0); */
            instPrintBufOffset += snprintf(instPrintBuf+instPrintBufOffset, 512 - instPrintBufOffset,
                                  "IMM ");
            w = (lo&8)>>3;
            reg = (lo&7);
            
            instPrintBufOffset += snprintf(instPrintBuf+instPrintBufOffset, 512 - instPrintBufOffset,
                            "to %s ", w ? regNamesW1[reg] : regNamesW0[reg]);
            reg_id = w ? reg : subRegId[reg];
            sub_reg_id = w ? REG_X : subRegAccess[reg];
            
            if(w){
                i16 tmp =*(i16*)(instruction_pos+1);
                data = 0;   //Fix for order of 16bit values because they are flipped on my computer (Low|High) instead of (High|Low) if we map the same memory to 8bit[2]
                data |= (tmp &0x00FF) << 8;
                data |= (tmp &0xFF00) >> 8;

                instPrintBufOffset += snprintf(instPrintBuf+instPrintBufOffset, 512 - instPrintBufOffset,
                                      "the 16 bit value %d ", tmp);
            }else{
                data = (i16)*(i8*)(instruction_pos+1);
                instPrintBufOffset += snprintf(instPrintBuf+instPrintBufOffset, 512 - instPrintBufOffset,
                                      "the 08 bit value %d ", data);
            }
            offset = w ? 3 : 2;
            break;

        case 12: // IMMEDIATE to REG/MEM 
            THIS_SPOT(0);
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

            data = mod_d_check == 0  ? (i16)*( u8*)(void*)(instruction_pos + 2) : // 0 Displacement       | Data is 1 byte 
                   mod_d_check == 1  ? (i16)*( u8*)(void*)(instruction_pos + 3) : // 1 Byte displacement  | Data is 1 Byte
                   mod_d_check == 2  ? (i16)*( u8*)(void*)(instruction_pos + 4) : // 2 Bytes displacement | Data is 1 Byte
                   mod_d_check == 3  ? (i16)*( u8*)(void*)(instruction_pos + 2) : // 0 Displacement(REG)  | Data is 1 Byte
                   mod_d_check == 4  ? (i16)*(u16*)(void*)(instruction_pos + 2) : // 0 Displacment        | Data is 2 Bytes
                   mod_d_check == 5  ? (i16)*(u16*)(void*)(instruction_pos + 3) : // 1 Byte displacement  | Data is 2 Bytes
                   mod_d_check == 6  ? (i16)*(u16*)(void*)(instruction_pos + 4) : // 2 Bytes displacement | Data is 2 Bytes
                   mod_d_check == 7  ? (i16)*(u16*)(void*)(instruction_pos + 2) : // 0 Displacement(REG)  | Data is 2 Bytes
                       0xFFFF;
            offset = mod_d_check == 0 ? 3 : 
                     mod_d_check == 1 ? 4 :
                     mod_d_check == 2 ? 5 :
                     mod_d_check == 3 ? 3 :
                     mod_d_check == 4 ? 4 :
                     mod_d_check == 5 ? 5 :
                     mod_d_check == 6 ? 6 :
                     mod_d_check == 7 ? 4 : 2;

            if(displ != 0)
                snprintf(memNameBuf, 255,"[ %s %s %d ]",memNames0[rm], displ > 0 ?"+":"-", ABS(displ));
            else
                snprintf(memNameBuf, 255,"[ %s ]",memNames0[rm]);

            instPrintBufOffset += snprintf(instPrintBuf+instPrintBufOffset, 512 - instPrintBufOffset,
                                "to %s the %s value %d",memNameBuf, w? "16bit":"8bit", data);
            break;

        default:
            break;
    }
    
    MovToRegister(host, reg_id,sub_reg_id,data);
    print_bubble(movcon.text1, movcon.col1, -1, instPrintBuf, "s", 0);
    printReg(host, reg_id, REG_X);
    return offset;
}



u32 decodeADD(asm_host* host, u8* instruction_pos){
    my_print_context addcon={.offset=0,.col1=0x64DD17FF, .text1=0x424242FF, .col2=0, .text2=0};
    u32 offset = 2;
    u8 hi = (*instruction_pos & 0xF0)>>4;
    u8 lo = (*instruction_pos & 0x0F);
    char memNameBuf[255]    ={0};
    char instPrintBuf[512]  ={0};
    u32  instPrintBufOffset = 0;
    i16  data       =  0;
    i16  dst_data   =  0;
    i16  src_data   =  0;
    i32  dst_id     = -1;
    i32  src_id     = -1;
    u8   dst_access = -1;
    u8   src_access = -1;
    i16  result     =  0;
    i16  arth_op    =  0;
    UNUSED dst_id,UNUSED arth_op, UNUSED dst_access;

    switch(hi){
        case 0:
            instPrintBufOffset = +snprintf(instPrintBuf+instPrintBufOffset, 512-instPrintBufOffset,
                                          "ADD ");
            switch(lo>>2){
                case 0:
                    /* THIS_SPOT(0); */
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
                            THIS_SPOT(0);
                            snprintf(memNameBuf, 255,"[ %s ]",memNames0[rm]);
                            instPrintBufOffset += snprintf(instPrintBuf+instPrintBufOffset, 512-instPrintBufOffset,
                                                    "to %s val in %s ",d?(w?regNamesW1[reg]:regNamesW0[reg]): memNameBuf,
                                                    d?(memNameBuf):(w?regNamesW1[reg]:regNamesW0[reg])
                            );
                            break;
                        case 1:
                            THIS_SPOT(0);
                            u8 sign = displ & 128;
                            if(displ>0) snprintf(memNameBuf, 255,"[ %s %s %d ]",memNames0[rm],sign > 0 ? "-": "+", displ);
                            else snprintf(memNameBuf, 255,"[ %s ]",memNames0[rm]);
                            instPrintBufOffset += snprintf(instPrintBuf+instPrintBufOffset, 512-instPrintBufOffset,
                                                    "to %s val in %s ",d?(w?regNamesW1[reg]:regNamesW0[reg]): memNameBuf,
                                                    d?(memNameBuf):(w?regNamesW1[reg]:regNamesW0[reg])
                            );
                            break;
                        case 2:
                            THIS_SPOT(NULL);
                            break;
                        case 3: // RM is REG
                            /* THIS_SPOT(0); */
                            
                            if(d){ //DST is REG
                                dst_id      = w ? reg   : subRegId[reg];
                                dst_access  = w ? REG_X : subRegAccess[reg];  
                                src_id      = w ? rm    : subRegId[rm];
                                src_access  = w ? REG_X : subRegAccess[rm];
                            }else{ // SRC is REG
                                dst_id      = w ? rm    : subRegId[rm];
                                dst_access  = w ? REG_X : subRegAccess[rm];  
                                src_id      = w ? reg   : subRegId[reg];
                                src_access  = w ? REG_X : subRegAccess[reg];
                            }


                            GetFromReg(host, src_id, src_access, &src_data);
                            GetFromReg(host, dst_id, dst_access, &dst_data);
                            result = dst_data + src_data;
                                                                //             arithmetic       bit
                                                                //             operation        size
                            SetFlagReg(host, dst_data, src_data, (i32)result | (ARTH_ADD)<<28 | (w)<<24);

                            snprintf(memNameBuf, 255,"[ %s ]",w?regNamesW1[rm]:regNamesW0[rm]);
                            instPrintBufOffset += snprintf(instPrintBuf+instPrintBufOffset, 512-instPrintBufOffset,
                                                    "to %s val in %s ",d?(w?regNamesW1[reg]:regNamesW0[reg]): memNameBuf,
                                                    d?(memNameBuf):(w?regNamesW1[reg]:regNamesW0[reg])
                            );
                            break;
                        default: break;
                    }
                    break;
                case 1:
                    THIS_SPOT(0);
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

                    instPrintBufOffset += snprintf(instPrintBuf+instPrintBufOffset, 512-instPrintBufOffset,
                                                    "to %s val %d",w ? "AX" : "AL", data);
                    break;
                default:
                    break;
            }
            break;
        case 1:
            THIS_SPOT("ADD with carry RegMem/ACCUMULATOR");
            break;
        case 8: // Special grouping of ADD,OR,ADC,SBB,AND,SUB,XOR,CMP \\ Immediate?
            addcon.col1 = 0xBDBDBDFF;
            addcon.text1 = 0x424242FF;// Just coloring

            // Getting info from instruction
            u8 s = (lo & 2)>>1;
            u8 w =  lo & 1;
            u8 byte2 = *(instruction_pos + 1);
            u8 mod   = (byte2 & 192)>>6;
            u8 ext   = (byte2 & 56 )>>3;
            u8 rm    = (byte2 & 7);
            
            // Get displacement value
            i32 displ = mod == 1 ? *(i8*)(void*)(instruction_pos+2):
                        mod == 2 ? *(i16*)(void*)(instruction_pos+2): 
                        (mod == 0 && rm == 6) ? *(i16*)(void*)(instruction_pos+2) : 0;
            u8 size_check = w&&s ? 1 : 0; // when w&&s is 1 size is 8bit else it's 16bit
            // When we have Instructions with S bit, then 0 means the data is 16bits, when it's 1 it means data is 8bit but it's sign bit is extended so it fits 16bits.
            //  This works only when W is 1, because it means that the register is 16 bit. So the value must be properly fitted. This is only for the CPU not us, but it
            //  affects the offset and the size we must retrive from the instruction_pos
            //                                                                          ↓- I'm doing this because of the i32 type that data is. If i16 or i8 is negative then the negative bit will flood the remaning 4 bytes and will change the value;
            i32 data =  mod == 1 ? ( (size_check==0) ? *(i16*)(void*)(instruction_pos+3) : *(i8*)(void*)(instruction_pos+3) ) :
                        mod == 2 ? ( (size_check==0) ? *(i16*)(void*)(instruction_pos+4) : *(i8*)(void*)(instruction_pos+4) ) :
                                     (size_check==0) ? *(i16*)(void*)(instruction_pos+2) : *(i8*)(void*)(instruction_pos+2) ;
            

            // Set offset
            u8 offsetTable[]={3,4,5,3,
                              4,5,6,4};
            offset = offsetTable[w*4+mod];
            offset -= w ? (s ? 1 : 0) : 0;// Adjusting the offset based on the Signed(S) bit
            
            u8    extTypes[8] = {  0,        1,    2,    3,    4,     5,      6,      7}; //based on 8086
            char* extNames[]  = {"ADD",    "OR", "ADC","SBB","AND", "SUB",   "XOR", "CMP"};
            u8    extOps[]    = {ARTH_ADD, -1,    -1,   -1,    -1,  ARTH_SUB, -1,   ARTH_SUB};
            
            arth_op = extOps[ext];

            if(mod == 3){ // RM is REG
                /* THIS_SPOT("RM is REG"); */
                snprintf(memNameBuf,255, " %s ", w ? regNamesW1[rm]:regNamesW0[rm]);
                dst_id      = w ? rm : subRegId[rm];
                dst_access  = w ? REG_X : subRegAccess[rm];

                GetFromReg(host, dst_id,dst_access, &dst_data);
                src_data = data; // I think i32 is too much for what we need
                switch(extOps[ext]){
                    case ARTH_ADD:
                        result = dst_data + src_data;
                        break;
                    case ARTH_SUB:
                        result = dst_data - src_data;
                        break;
                    default:break;
                }
                                                    // add to result arithmetic op and size(w)
                SetFlagReg(host, dst_data, src_data, ( (i32)result&0x0000FFFF ) | extOps[ext]<<28 | w<<24);
                /* THIS_SPOT(" %d + %d = %d",dst_data,src_data,result); */

            }else if(mod == 0){ // RM is MEM
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

            instPrintBufOffset += snprintf(instPrintBuf+instPrintBufOffset, 512-instPrintBufOffset,
                                          "%s to %s , %d ",extNames[ext], memNameBuf, data);

            break;
        default:
            break;
    }
    
    i16 write_result=0;
    write_result |= (result&0xFF00)>>8;
    write_result |= (result&0x00FF)<<8;
    MovToRegister(host, dst_id, dst_access, write_result);
    /* SetFlagReg(host, dst_data, src_data, result); */
    
    print_bubble(addcon.text1,addcon.col1,-1, instPrintBuf,"s", 0);
    printReg(host, dst_id, dst_access);
    printReg(host, REG_FLAG, REG_X);
    return offset;
}



u32 decodeSUB(asm_host* host, u8* instruction_pos){
    UNUSED instruction_pos;
    my_print_context subcon = {0};
    subcon.col1 = 0xAEEA00FF;
    subcon.text1 = 0x424242FF;
    u32 offset = 2;
    u8 hi = (*instruction_pos & 0xF0)>>4;
    u8 lo = (*instruction_pos & 0x0F);
    char memNameBuf[255]={0};
    char instPrintBuf[512] = {0};
    u32  instPrintBufOffset=0;
    i16 data1=0;
    i16 data2=0;
    u32 reg_id = -1;
    u8  reg_access = -1;
    instPrintBufOffset += snprintf(instPrintBuf+instPrintBufOffset, 512-instPrintBufOffset,
                                    "SUB ");

    switch(hi){
        case 1:
            THIS_SPOT(NULL);
            break;
        case 2:
            /* THIS_SPOT(0); */
            switch(lo>>2){
                case 0:THIS_SPOT(NULL);break;
                case 1:THIS_SPOT(NULL);break;
                case 2: // SUB to REG/RM fom RM/REG - takes the value at DST and substracts SRC and writes the result to DST
                    /* THIS_SPOT(0); */
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
                        case 0:// RM is memory address with no offset
                            snprintf(memNameBuf, 255,"[ %s ]",memNames0[rm]);
                            instPrintBufOffset += snprintf(instPrintBuf+instPrintBufOffset, 512-instPrintBufOffset,
                                                    "to %s val in %s ",d?(w?regNamesW1[reg]:regNamesW0[reg]): memNameBuf,
                                                    d?(memNameBuf):(w?regNamesW1[reg]:regNamesW0[reg]) );
                            break;
                        case 1: // RM is memory address with 8bit offset
                            u8 sign = displ & 128;
                            if(displ>0) snprintf(memNameBuf, 255,"[ %s %s %d ]",memNames0[rm],sign > 0 ? "-": "+", displ);
                            else snprintf(memNameBuf, 255,"[ %s ]",memNames0[rm]);

                            instPrintBufOffset += snprintf(instPrintBuf+instPrintBufOffset, 512-instPrintBufOffset,
                                                    "to %s val in %s ",d?(w?regNamesW1[reg]:regNamesW0[reg]): memNameBuf,
                                                    d?(memNameBuf):(w?regNamesW1[reg]:regNamesW0[reg]));
                            break;
                        case 2: // RM is memory address with 16 bit offset
                            THIS_SPOT(NULL);
                            break;
                        case 3: // RM is REG 
                            /* THIS_SPOT(0); */
                            reg_id = w ? reg: subRegId[reg] ;
                            reg_access = w ? REG_X : subRegAccess[reg];
                            u32 rm_id  = w ? rm: subRegId[rm];
                            u8  rm_access = w ? REG_X : subRegAccess[rm];
                            i16 tmp_data = 0;

                            // ────────────────────────────── Get and SUB Data ───────────────────────────
                            if(d){ //DST is REG
                                GetFromReg( host, reg_id, reg_access, &data1);
                                GetFromReg( host, rm_id, rm_access, &data2);

                                tmp_data = data1 - data2;
                                SetFlagReg(host,data1,data2, (i32)tmp_data | (ARTH_SUB << 28) | ((w) << 24));
                                /* THIS_SPOT("%d - %d = %d",data1,data2,tmp_data); */
                                /* THIS_SPOT("%s",OBJ2BIN(tmp_data)); */
                                // Result is Big endian, but MovToRegister accepts little endian
                                data1 = 0;
                                data1 |= (tmp_data & 0x00FF)<<8;
                                data1 |= (tmp_data & 0xFF00)>>8;

                                MovToRegister(host, reg_id,reg_access, data1);
                                
                            }else{ // SRC is REG
                                GetFromReg( host, rm_id, rm_access, &data1);
                                GetFromReg( host, reg_id, reg_access, &data2);

                                tmp_data = data1 - data2;
                                i32 result = (i32)tmp_data & 0x000FFFFF;                    // W
                                SetFlagReg(host,data1,data2, result | (ARTH_SUB)<<28 | ((w) << 24));
                                /* THIS_SPOT("%d - %d = %d",data1,data2,tmp_data); */
                                /* THIS_SPOT("%s",OBJ2BIN(tmp_data)); */
                                // Result is Big endian, but MovToRegister accepts little endian
                                data1 = 0;
                                data1 |= (tmp_data & 0x00FF)<<8;
                                data1 |= (tmp_data & 0xFF00)>>8;

                                MovToRegister(host, rm_id,rm_access, data1);
                                reg_id      = rm_id; // Hack fix for print
                                reg_access  = rm_access; // Hack fix for print
                            }

                            /* THIS_SPOT("%s",OBJ2BIN(data1)); */
                            /* if(data1 == 0){ */
                            /*     THIS_SPOT(0); */
                            /*     host->reg->x.flag |=  REG_FLG_ZF; // Turn on Zero flag */
                            /*     host->reg->x.flag &= ~REG_FLG_SF; // Trun off Sign flag */
                            /* }else{ */
                            /*     host->reg->x.flag &= ~REG_FLG_ZF; // Turn off Zero flag */
                            /*     if(data1&0x0080){ // Turn on or off Sign flag if Sign flag is enabled in data1 */
                            /*         host->reg->x.flag |= REG_FLG_SF; */
                            /*     }else{ */
                            /*         host->reg->x.flag &= ~REG_FLG_SF; */
                            /*     } */
                            /* } */

                            snprintf(memNameBuf, 255,"[ %s ]",w?regNamesW1[rm]:regNamesW0[rm]);
                            instPrintBufOffset += snprintf(instPrintBuf+instPrintBufOffset, 512-instPrintBufOffset,
                                                    "to %s val in %s ",d?(w?regNamesW1[reg]:regNamesW0[reg]): memNameBuf,
                                                    d?(memNameBuf):(w?regNamesW1[reg]:regNamesW0[reg]) );
                            break;
                        default: break;
                    }
                    break;
                case 3:
                    THIS_SPOT(0);
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

                    instPrintBufOffset += snprintf(instPrintBuf+instPrintBufOffset, 512-instPrintBufOffset,
                                                   "to %s val %d",w ? "AX" : "AL", data);
                    break;
                default:break;
            }
            break;
        default:
            break;
    }
    
    print_bubble(subcon.text1, subcon.col1,-1, instPrintBuf, "s", 0);
    printReg(host, reg_id,reg_access);
    printReg(host, REG_FLAG,REG_X);
    return offset;
}


u32 decodeCMP(asm_host* host, u8* instruction_pos){
    UNUSED instruction_pos;
    my_print_context cmpcon = {.offset=0, .col1=0x2979FFFF, .text1=0xFFF8E1FF, .col2=0, .text2=0};

    u32 offset = 2;
    u8 hi = (*instruction_pos & 0xF0)>>4;
    u8 lo = (*instruction_pos & 0x0F);
    char memNameBuf[255]={0};
    char instPrintBuf[512];
    u32  instPrintBufOffset=0;
    u32 reg_id = -1;
    u8  reg_access = -1;


    instPrintBufOffset += snprintf(instPrintBuf+instPrintBufOffset,512-instPrintBufOffset,
                                   "CMP ");
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
                    instPrintBufOffset += snprintf(instPrintBuf+instPrintBufOffset,512-instPrintBufOffset,
                                                "to %s val in %s ",d?(w?regNamesW1[reg]:regNamesW0[reg]): memNameBuf,
                                                d?(memNameBuf):(w?regNamesW1[reg]:regNamesW0[reg]) );
                    break;
                case 1:
                    u8 sign = displ & 128;
                    if(displ>0) snprintf(memNameBuf, 255,"[ %s %s %d ]",memNames0[rm],sign > 0 ? "-": "+", displ);
                    else snprintf(memNameBuf, 255,"[ %s ]",memNames0[rm]);
                    instPrintBufOffset += snprintf(instPrintBuf+instPrintBufOffset,512-instPrintBufOffset,
                                   "to %s val in %s ",d?(w?regNamesW1[reg]:regNamesW0[reg]): memNameBuf,
                                   d?(memNameBuf):(w?regNamesW1[reg]:regNamesW0[reg])
                    );
                    break;
                case 2:
                    THIS_SPOT(NULL);
                    break;
                case 3: // RM is Rregistry 
                    i16 data1 = 0;
                    i16 data2 = 0;
                    i16 tmp_data = 0;
                    reg_id = w ? reg: subRegId[reg] ;
                    reg_access = w ? REG_X : subRegAccess[reg];
                    u32 rm_id  = w ? rm: subRegId[rm];
                    u8  rm_access = w ? REG_X : subRegAccess[rm];
                    
                    // ────────────────────────────── Get and SUB Data ───────────────────────────
                    if(d){ //DST is REG
                        GetFromReg( host, reg_id, reg_access, &data1);
                        GetFromReg( host, rm_id,  rm_access, &data2);

                        tmp_data = data1 - data2;
                        SetFlagReg(host, data1, data2, ((i32)tmp_data&0x000FFFFF) | (ARTH_SUB << 28) | (w << 24));
                        
                    }else{ // SRC is REG
                        GetFromReg( host, rm_id, rm_access, &data1);
                        GetFromReg( host, reg_id, reg_access, &data2);

                        tmp_data = data1 - data2;
                        SetFlagReg(host,data1,data2, ((i32)tmp_data&0x000FFFFF) | (ARTH_SUB << 28) | (w << 24));
                    }

                    snprintf(memNameBuf, 255,"[ %s ]",w?regNamesW1[rm]:regNamesW0[rm]);
                    instPrintBufOffset += snprintf(instPrintBuf+instPrintBufOffset,512-instPrintBufOffset,
                                            "to %s val in %s ",d?(w?regNamesW1[reg]:regNamesW0[reg]): memNameBuf,
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

            instPrintBufOffset += snprintf(instPrintBuf+instPrintBufOffset,512-instPrintBufOffset,
                                   "to %s val %d",w ? "AX" : "AL", data);
            break;
        default:break;
    }
    
    print_bubble(cmpcon.text1, cmpcon.col1,-1, instPrintBuf,"s",0);
    printReg(host, REG_FLAG,REG_X);
    return offset;
}



u32 decodeJMP(u8* instruction_pos){
    u32 offset = 2;
    u8 hi = (*instruction_pos & 0xF0)>>4;
    u8 lo = *instruction_pos  & 0x0F;
    my_print_context jmpcon = {.offset=0,.col1=0x1E88E5FF,.text1=0x000000FF,.col2=0x424242FF,.text2=0xE0E0E0FF};
    
    char instStringBuf[512]={0};
    u32  instStrBufOffset=0;UNUSED instStrBufOffset;
    char memNameBuf[255]={0};
    /* printf("JMP "); */
    /* instStrBufOffset += snprintf(instStringBuf,512,"JMP "); */
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
            /* printf("%s to (%d)",loopNames[lo],  offset); */
            instStrBufOffset += snprintf(instStringBuf+instStrBufOffset,512-instStrBufOffset, 
                                         "%s to (%d)",loopNames[lo], offset);
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
            /* printf("%s to (%d)",jumpNames[lo],  offset); */
            instStrBufOffset += snprintf(instStringBuf+instStrBufOffset,512-instStrBufOffset, 
                                         "%s to (%d)",jumpNames[lo], offset);
            /* THIS_SPOT("Conditional"); */
            break;
        default:break;
    }
    print_bubble(jmpcon.text1, jmpcon.col1, -1, instStringBuf, "s", 0);
    return offset;
}


