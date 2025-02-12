#include "memory_simulator.h"
#include "asm_sim_types.inl"
#include "debug_utils.h"
#include "small_utils.h"
#include <stdio.h>
#include <stdlib.h>


b8 access_check[] = { 1,1,1,/*AX*/ 1,1,1,/*BX*/
                      1,1,1,/*CX*/ 1,1,1,/*DX*/
                      0,0,1,/*SI*/ 0,0,1,/*DI*/
                      0,0,1,/*BP*/ 0,0,1,/*SP*/
                      0,0,1,/*IP*/ 1,1,1,/*FLAGS*/
                      0,0,1,/*CS*/ 0,0,1,/*DS*/
                      0,0,1,/*SS*/ 0,0,1,/*ES*/};

void createRegisters(asm_host* host){
    host->reg = calloc(1,sizeof(reg86));
}

void MovToRegister(asm_host* host, u32 reg_id, u8 hl, i16 val){
    if(!access_check[reg_id*3+hl]){
        printf("[ERR]: Unable to write to reg %d with HL of %d\n", reg_id, hl);
        return;
    }

    if(hl == REG_X){
        i16* regs = (i16*)(void*)&host->reg->x;
        // Turn to Little Endian
        i16 tmp_val = 0;
        tmp_val |= (val&0x00FF)<<8;
        tmp_val |= (val&0xFF00)>>8;
        regs[reg_id] = tmp_val;
    }else{
        i8* regs = (i8*)(void*)&host->reg->hl;
        u8 _hl = hl == REG_HIGH ? 0 : 1;
        regs[reg_id*2 + _hl] = (i8)val;
    }
}

void GetFromReg(asm_host* host, u32 reg_id, u8 hl, i16* val){
    UNUSED host, UNUSED reg_id, UNUSED hl, UNUSED val;
    if(!access_check[reg_id*3+hl]){
        printf("[ERR]: Unable to write to reg %d with HL of %d\n", reg_id,hl);
        return;
    }

    if(hl == REG_X){
        i16* regs = (i16*)(void*)&host->reg->x;
        *val = regs[reg_id];
        //Trun to Big Endian
        /* i16 tmp_val = 0; */
        /* tmp_val |= (*val&0x00FF)<<8; */
        /* tmp_val |= (*val&0xFF00)>>8; */
        /* *val = tmp_val; */
    }else{
        i8* regs = (i8*)(void*)&host->reg->hl;
        u8 _hl = hl == REG_HIGH ? 0 : 1;
        *val  = (i16)regs[reg_id*2+_hl];
    }


}


void SetFlagReg(asm_host* host, i16 val1, i16 val2, i32 result){
    /* THIS_SPOT(0); */
    UNUSED host, UNUSED val1, UNUSED val2, UNUSED result;
    

    u8  arth_op = (result & 0xF0000000) >> 28;
    u8  w       = (result & 0x0F000000) >> 24;
    // sterilize result
    i32 _result = 0;
    if (w) _result |= result  & 0x000FFFFF; 
    else _result   |= result  & 0x00000FFF;

    // Flip endians
    i16 _val1 = 0; UNUSED _val1;
    _val1 |= (val1&0xFF00)>>8;
    _val1 |= (val1&0x00FF)<<8;
    i16 _val2 = 0; UNUSED _val2;
    _val2 |= (val2&0xFF00)>>8;
    _val2 |= (val2&0x00FF)<<8;
    i16 _result_f = 0 | (_result&0xFF00)>>8;
    _result_f    |= (_result&0x00FF)<<8;

    /* THIS_SPOT("result %i",_result); */
    if (_result == 0){ //Turn on ZF and PF and off SF,CF,OF,AF
        i16 flags = REG_FLG_ZF | REG_FLG_PF;
        host->reg->x.flag &= 0x700; // Turn off all other flags except DF,IF,TF
        if(w){ //16bit
            // Check AF
            i16 r_check  = _result_f & 0x800;
            i16 v1_check = _val1   & 0x800;
            flags |= REG_FLG_AF;

            // Check CF
            i16 val1_check = (val1 & 0x8000);
            i16 result_check = (_result & 0x8000);
            switch (arth_op){
                case ARTH_ADD: 
                    if( val1_check && !result_check ) flags |= REG_FLG_CF;
                    break;
                case ARTH_SUB:
                    if( result_check && !val1_check ) flags |= REG_FLG_CF;
                    break;
                default:break;
            }

        }else{//8bit
            //Check AF
            i8 r_check  =  _result_f & 0x08;
            i8 v1_check =  _val1   & 0x08;
            flags |= REG_FLG_AF;

            //Check CF
            i16 val1_check = (val1 & 0x80);
            i16 result_check = (_result & 0x80);
            switch (arth_op){
                case ARTH_ADD: 
                    if( val1_check && !result_check ) flags |= REG_FLG_CF;
                    break;
                case ARTH_SUB:
                    if( result_check && !val1_check ) flags |= REG_FLG_CF;
                    break;
                default:break;
            }
        }

        host->reg->x.flag |= flags; // Turn on ZF and PF
        
    }else{ // Analyze flags
        host->reg->x.flag &= 0x700; // Turn off all other flags except DF,IF,TF
        
        // Turn off ZF
        host->reg->x.flag &= ~0x0040;

        // Check for SF and set appropriately
        if(w){
            if(_result & 0x8000) host->reg->x.flag |= REG_FLG_SF;
            else host->reg->x.flag &= ~REG_FLG_SF;
        }else{
            if(_result & 0x80) host->reg->x.flag |= REG_FLG_SF;
            else host->reg->x.flag &= ~REG_FLG_SF;
        }

        // Check for CF
        if(w){ // 16bit Values
            i16 val1_check = (val1 & 0x8000);
            i16 result_check = (_result & 0x8000);
            switch (arth_op){
                case ARTH_ADD: 
                    if( val1_check && !result_check ) host->reg->x.flag |= REG_FLG_CF;
                    else host->reg->x.flag &= ~REG_FLG_CF;
                    break;
                case ARTH_SUB:
                    if( result_check && !val1_check ) host->reg->x.flag |= REG_FLG_CF;
                    else host->reg->x.flag &= ~REG_FLG_CF;
                    break;
                default:break;
            }
        }else{ // 8bit Values
            i16 val1_check = (val1 & 0x80);
            i16 result_check = (_result & 0x80);
            switch (arth_op){
                case ARTH_ADD: 
                    if( val1_check && !result_check ) host->reg->x.flag |= REG_FLG_CF;
                    else host->reg->x.flag &= ~REG_FLG_CF;
                    break;
                case ARTH_SUB:
                    if( result_check && !val1_check ) host->reg->x.flag |= REG_FLG_CF;
                    else host->reg->x.flag &= ~REG_FLG_CF;
                    break;
                default:break;
            }
        }


        // Check for OF
        if(w){ //16bit
            i16 val1_check = val1 & 0x8000; // DST val
            i16 val2_check = val2 & 0x8000; // SRC val
            switch(arth_op){
                case ARTH_ADD:
                    if(val1_check == val2_check){
                        i16 result_check = _result & 0x8000;
                        if(val1_check != result_check) host->reg->x.flag |= REG_FLG_OF;
                        else host->reg->x.flag &= ~REG_FLG_OF;
                    }else host->reg->x.flag &= ~REG_FLG_OF;
                    break;
                case ARTH_SUB:
                    if(val1_check != val2_check){
                        i16 result_check = _result & 0x8000;
                        if(val1_check != result_check) host->reg->x.flag |= REG_FLG_OF;
                        else host->reg->x.flag &= ~REG_FLG_OF;
                    }else host->reg->x.flag &= ~REG_FLG_OF;
                    break;
                default:break;
            }
        }else{ //8bit
            u8 val1_check = val1 & 0x80; // DST val
            u8 val2_check = val2 & 0x80; // SRC val
            switch(arth_op){
                case ARTH_ADD: 
                    if(val1_check == val2_check){
                        u8 result_check = _result & 0x80;
                        if(val1_check != result_check) host->reg->x.flag |= REG_FLG_OF;
                        else host->reg->x.flag &= ~REG_FLG_OF;
                    }else host->reg->x.flag &= ~REG_FLG_OF;
                    break;
                case ARTH_SUB:
                    if(val1_check != val2_check){
                        u8 result_check = _result & 0x00000080;
                        if(val1_check != result_check) host->reg->x.flag |= REG_FLG_OF;
                        else host->reg->x.flag &= ~REG_FLG_OF;
                    }else host->reg->x.flag &= ~REG_FLG_OF;
                    break;
                default:break;
            }
        }

        // Check for AF
        i16 val1_check   = 0;
        i16 result_check = 0;
        switch(arth_op){
            case ARTH_ADD:
                val1_check = (val1 & 24)>>3;
                result_check = (_result & 24)>>3;
                b8 add_map[16]={
                //    0  1  2  3
                /*0*/ 0, 0, 0, 1,
                /*1*/ 0, 0, 1, 1,
                /*2*/ 0, 1, 0, 1,
                /*3*/ 1, 1, 0, 0
                };
                if(add_map[result_check*4 + val1_check]) host->reg->x.flag |= REG_FLG_AF;
                else host->reg->x.flag &= ~REG_FLG_AF;
                break;
            case ARTH_SUB: 
                val1_check = (val1 & 24)>>3;
                result_check = (_result & 24)>>3;
                b8 sub_map[16]={
                //    0  1  2  3
                /*0*/ 0, 0, 1, 1,
                /*1*/ 0, 0, 1, 1,
                /*2*/ 0, 0, 0, 0,
                /*3*/ 1, 0, 0, 0
                };
                if(sub_map[result_check*4 + val1_check]) host->reg->x.flag |= REG_FLG_AF;
                else host->reg->x.flag &= ~REG_FLG_AF;
                break;
            default:break;
        }



        // Check for PF
        //      This was taken from a reply in Computer Enhance's comments where this person
        //      George Laskowsky gave this implementation for the parity counting. I'll test it
        //      in isolation to figure out the concept but it looks it will take less instructions than
        //      the naive method of going through all the bits using bitshifting and checking each bit if
        //      if it is 1.
        //      The reply can be seen here: https://www.computerenhance.com/p/simulating-add-jmp-and-cmp/comment/15243013
        i8 parity_counter=0;
        if(w){//16bit
            i16 val=result-((result>>1)&0x5555);
            val = (val & 0x3333) + ((val>>2)&0x3333);
            parity_counter = ((val*0x1111)>>12) & 0x0F;
        }else{//8bit
            i8 val=result-((result>>1)&0x55);
            val = (val & 0x33) + ((val>>2)&0x33);
            parity_counter = ((val*0x11)>>4) & 0x0F;
        }
        /* THIS_SPOT("P=%d", parity_counter); */
        if(parity_counter % 2 == 0) host->reg->x.flag |= REG_FLG_PF;
        else host->reg->x.flag &= ~REG_FLG_PF;

    }


}


void printReg(asm_host* host, u32 reg_id, u8 HL){
    my_print_context regcon = {0};
    regcon.col2 = 0x424242FF;
    regcon.text2= 0xF4511EFF;
    i16 data;
    /* printf("Printing REG %d\n", reg_id); */
    switch(reg_id){
        case REG_A:
            regcon.col1 = 0xF1C40FFF;
            if(HL==REG_HIGH){
                print_bubble(regcon.text2, regcon.col2, 0, "AH", "s", 0);
                print_bubble(regcon.text1, regcon.col1, -1, &host->reg->hl.ah, "x2", 0);
            }else if (HL==REG_LOW){
                print_bubble(regcon.text2, regcon.col2, 0, "AL", "s", 0);
                print_bubble(regcon.text1, regcon.col1, -1, &host->reg->hl.al, "x2", 0);
            }else if (HL== REG_X){
                print_bubble(regcon.text2, regcon.col2, 0, "AX", "s", 0);
                print_bubble(regcon.text1, regcon.col1, -1, &host->reg->hl.al, "x2", 0);
                regcon.col1 = 0xBE9100FF;
                print_bubble(regcon.text1, regcon.col1, -1, &host->reg->hl.ah, "x2", 0);
            }
            break;

        case REG_B:
            regcon.col1 = 0xE67E22FF;
            if(HL==REG_HIGH){
                print_bubble(regcon.text2, regcon.col2, 0, "BH", "s", 0);
                print_bubble(regcon.text1, regcon.col1, -1, &host->reg->hl.bh, "x2", 0);
            }else if (HL==REG_LOW){
                print_bubble(regcon.text2, regcon.col2, 0, "BL", "s", 0);
                print_bubble(regcon.text1, regcon.col1, -1, &host->reg->hl.bl, "x2", 0);
            }else if (HL== REG_X){
                print_bubble(regcon.text2, regcon.col2, 0, "BX", "s", 0);
                print_bubble(regcon.text1, regcon.col1, -1, &host->reg->hl.bl, "x2", 0);
                regcon.col1 = 0xB34B00FF;
                print_bubble(regcon.text1, regcon.col1, -1, &host->reg->hl.bh, "x2", 0);
            }
            break;
        case REG_C:

            regcon.col1 = 0xE74C3CFF;
            if(HL==REG_HIGH){
                print_bubble(regcon.text2, regcon.col2, 0, "CH", "s", 0);
                print_bubble(regcon.text1, regcon.col1, -1, &host->reg->hl.ch, "x2", 0);
            }else if (HL==REG_LOW){
                print_bubble(regcon.text2, regcon.col2, 0, "CL", "s", 0);
                print_bubble(regcon.text1, regcon.col1, -1, &host->reg->hl.cl, "x2", 0);
            }else if (HL== REG_X){
                print_bubble(regcon.text2, regcon.col2, 0, "CX", "s", 0);
                print_bubble(regcon.text1, regcon.col1, -1, &host->reg->hl.cl, "x2", 0);
                regcon.col1 = 0x810000FF;
                regcon.text1 = 0xEEEEEEFF;
                print_bubble(regcon.text1, regcon.col1, -1, &host->reg->hl.ch, "x2", 0);
            }
            //
            break;

        case REG_D:
            regcon.col1 = 0xECF0F1FF;
            //868A8B
            if(HL==REG_HIGH){
                print_bubble(regcon.text2, regcon.col2, 0, "DH", "s", 0);
                print_bubble(regcon.text1, regcon.col1, -1, &host->reg->hl.dh, "x2", 0);
            }else if (HL==REG_LOW){
                print_bubble(regcon.text2, regcon.col2, 0, "DL", "s", 0);
                print_bubble(regcon.text1, regcon.col1, -1, &host->reg->hl.dl, "x2", 0);
            }else if (HL== REG_X){
                print_bubble(regcon.text2, regcon.col2, 0, "DX", "s", 0);
                print_bubble(regcon.text1, regcon.col1, -1, &host->reg->hl.dl, "x2", 0);
                regcon.col1 = 0x868A8BFF;
                regcon.text1 = 0xEEEEEEFF;
                print_bubble(regcon.text1, regcon.col1, -1, &host->reg->hl.dh, "x2", 0);
            }
            break;

        case REG_SI:
            if(HL != REG_X){
                printf("Err: reg SI needs HL be REG_X");
                return;
            }
            regcon.col1 = 0x2ECC71FF;
            print_bubble(regcon.text2, regcon.col2, 0, "SI", "s", 0);
            print_bubble(regcon.text1, regcon.col1, -1, &host->reg->x.si, "x4", 0);
            break;

        case REG_DI:
            if(HL != REG_X){
                printf("Err: reg DI needs HL be REG_X");
                return;
            }
            regcon.col1 = 0x3498DBFF;
            print_bubble(regcon.text2, regcon.col2, 0, "DI", "s", 0);
            print_bubble(regcon.text1, regcon.col1, -1, &host->reg->x.di, "x4", 0);
            break;

        case REG_BP:
            if(HL != REG_X){
                printf("Err: reg BP needs HL be REG_X");
                return;
            }
            regcon.col1 = 0x9B59B6FF;
            print_bubble(regcon.text2, regcon.col2, 0, "BP", "s", 0);
            print_bubble(regcon.text1, regcon.col1, -1, &host->reg->x.bp, "x4", 0);
            break;

        case REG_SP:
            if(HL != REG_X){
                printf("Err: reg SP needs HL be REG_X");
                return;
            }
            regcon.col1  = 0x34495EFF;
            regcon.text1 = 0xECF0F1FF;
            print_bubble(regcon.text2, regcon.col2, 0, "SP", "s", 0);
            print_bubble(regcon.text1, regcon.col1, -1, &host->reg->x.sp, "x4", 0);
            break;

        case REG_IP:
            if(HL != REG_X){
                printf("Err: reg IP needs HL be REG_X");
                return;
            }
            regcon.col1  = 0x34495EFF;
            regcon.text1 = 0xECF0F1FF;
            print_bubble(regcon.text2, regcon.col2, 0, "IP", "s", 0);
            print_bubble(regcon.text1, regcon.col1, -1, &host->reg->x.ip, "x4", 0);
            break;

        case REG_FLAG:
            regcon.col1  = 0x34495EFF;
            regcon.text1 = 0xECF0F1FF;
            data = host->reg->x.flag;
            print_bubble(regcon.text2, regcon.col2, 0, "FLAG", "s", 0);
            print_bubble(regcon.col2,  regcon.text2, -1, OBJ2BIN(data), "s", 0);
            break;

        case REG_CS:
            if(HL != REG_X){
                printf("Err: reg CS needs HL be REG_X");
                return;
            }
            regcon.col1  = 0x34495EFF;
            regcon.text1 = 0xECF0F1FF;
            print_bubble(regcon.text2, regcon.col2, 0, "CS", "s", 0);
            print_bubble(regcon.text1, regcon.col1, -1, &host->reg->x.cs, "x4", 0);
            break;
        case REG_DS:
            if(HL != REG_X){
                printf("Err: reg DS needs HL be REG_X");
                return;
            }
            regcon.col1  = 0x34495EFF;
            regcon.text1 = 0xECF0F1FF;
            print_bubble(regcon.text2, regcon.col2, 0, "DS", "s", 0);
            print_bubble(regcon.text1, regcon.col1, -1, &host->reg->x.ds, "x4", 0);
            break;
        case REG_SS:
            if(HL != REG_X){
                printf("Err: reg SS needs HL be REG_X");
                return;
            }
            regcon.col1  = 0x34495EFF;
            regcon.text1 = 0xECF0F1FF;
            print_bubble(regcon.text2, regcon.col2, 0, "SS", "s", 0);
            print_bubble(regcon.text1, regcon.col1, -1, &host->reg->x.ss, "x4", 0);
            break;
        case REG_ES:
            if(HL != REG_X){
                printf("Err: reg ES needs HL be REG_X");
                return;
            }
            regcon.col1  = 0x34495EFF;
            regcon.text1 = 0xECF0F1FF;
            print_bubble(regcon.text2, regcon.col2, 0, "ES", "s", 0);
            print_bubble(regcon.text1, regcon.col1, -1, &host->reg->x.es, "x4", 0);
            break;
        default:break;
    }

}

void printRegFull(asm_host* host){
    my_print_context regcon = {0};
    regcon.col2 = 0x424242FF;
    regcon.text2= 0xF4511EFF;
    //AX
    regcon.col1 = 0xF1C40FFF;
    print_bubble(regcon.text2, regcon.col2, 0, "AX", "s", 0);
    print_bubble(regcon.text1, regcon.col1, -1, &host->reg->hl.ah, "x2", 0);
    regcon.col1 = 0xBE9100FF;
    print_bubble(regcon.text1, regcon.col1, -1, &host->reg->hl.al, "x2", 1);
    //BX
    regcon.col1 = 0xE67E22FF;
    print_bubble(regcon.text2, regcon.col2, 0, "BX", "s", 0);
    print_bubble(regcon.text1, regcon.col1, -1, &host->reg->hl.bh, "x2", 0);
    regcon.col1 = 0xB34B00FF;
    print_bubble(regcon.text1, regcon.col1, -1, &host->reg->hl.bl, "x2", 1);
    //CX
    regcon.col1 = 0xE74C3CFF;
    print_bubble(regcon.text2, regcon.col2, 0, "CX", "s", 0);
    print_bubble(regcon.text1, regcon.col1, -1, &host->reg->hl.ch, "x2", 0);
    regcon.col1 = 0x810000FF;
    regcon.text1 = 0xEEEEEEFF;
    print_bubble(regcon.text1, regcon.col1, -1, &host->reg->hl.cl, "x2", 1);
    //DX
    regcon.col1 = 0xECF0F1FF;
    regcon.text1 = 0x454545FF;
    print_bubble(regcon.text2, regcon.col2, 0, "DX", "s", 0);
    print_bubble(regcon.text1, regcon.col1, -1, &host->reg->hl.dh, "x2", 0);
    regcon.col1 = 0x868A8BFF;
    regcon.text1 = 0xEEEEEEFF;
    print_bubble(regcon.text1, regcon.col1, -1, &host->reg->hl.dl, "x2", 1);
    //SI
    regcon.col1 = 0x2ECC71FF;
    i16 data = 0;
    data |= ((host->reg->x.si & 0x00FF)<<8);
    data |= ((host->reg->x.si & 0xFF00)>>8);
    print_bubble(regcon.text2, regcon.col2, 0, "SI", "s", 0);
    print_bubble(regcon.text1, regcon.col1, -1, &data, "x4", 1);
    //DI
    regcon.col1 = 0x3498DBFF;
    data = 0;
    data |= ((host->reg->x.di & 0x00FF)<<8);
    data |= ((host->reg->x.di & 0xFF00)>>8);
    print_bubble(regcon.text2, regcon.col2, 0, "DI", "s", 0);
    print_bubble(regcon.text1, regcon.col1, -1, &data, "x4", 1);
    //BP
    regcon.col1 = 0x9B59B6FF;
    data = 0;
    data |= ((host->reg->x.bp & 0x00FF)<<8);
    data |= ((host->reg->x.bp & 0xFF00)>>8);
    print_bubble(regcon.text2, regcon.col2, 0, "BP", "s", 0);
    print_bubble(regcon.text1, regcon.col1, -1, &data, "x4", 1);
    //SP
    regcon.col1  = 0x34495EFF;
    regcon.text1 = 0xECF0F1FF;
    data = 0;
    data |= ((host->reg->x.sp & 0x00FF)<<8);
    data |= ((host->reg->x.sp & 0xFF00)>>8);
    print_bubble(regcon.text2, regcon.col2, 0, "SP", "s", 0);
    print_bubble(regcon.text1, regcon.col1, -1, &data, "x4", 1);

    regcon.offset = 15;
    //CS
    printf("\033[4A\033[%dC",regcon.offset);
    regcon.col1  = 0x34495EFF;
    regcon.text1 = 0xECF0F1FF;
    data = 0;
    data |= ((host->reg->x.cs & 0x00FF)<<8);
    data |= ((host->reg->x.cs & 0xFF00)>>8);
    print_bubble(regcon.text2, regcon.col2, 0, "CS", "s", 0);
    print_bubble(regcon.text1, regcon.col1, -1, &data, "x4", 0);
    //DS
    printf("\033[1B\033[12D");
    regcon.col1  = 0x34495EFF;
    regcon.text1 = 0xECF0F1FF;
    data = 0;
    data |= ((host->reg->x.ds & 0x00FF)<<8);
    data |= ((host->reg->x.ds & 0xFF00)>>8);
    print_bubble(regcon.text2, regcon.col2, 0, "DS", "s", 0);
    print_bubble(regcon.text1, regcon.col1, -1, &data, "x4", 0);
    //SS
    printf("\033[1B\033[12D");
    regcon.col1  = 0x34495EFF;
    regcon.text1 = 0xECF0F1FF;
    data = 0;
    data |= ((host->reg->x.ss & 0x00FF)<<8);
    data |= ((host->reg->x.ss & 0xFF00)>>8);
    print_bubble(regcon.text2, regcon.col2, 0, "SS", "s", 0);
    print_bubble(regcon.text1, regcon.col1, -1, &data, "x4", 0);
    //ES
    printf("\033[1B\033[12D");
    regcon.col1  = 0x34495EFF;
    regcon.text1 = 0xECF0F1FF;
    data = 0;
    data |= ((host->reg->x.es & 0x00FF)<<8);
    data |= ((host->reg->x.es & 0xFF00)>>8);
    print_bubble(regcon.text2, regcon.col2, 0, "ES", "s", 0);
    print_bubble(regcon.text1, regcon.col1, -1, &data, "x4", 0);
    //FLAG
    printf("\033[4A\033[12C");
    data = host->reg->x.flag;
    print_bubble(regcon.text2, regcon.col2, 0, " ••••ODIT SZ•A•P•C ", "s", 0);
    printf("\033[1B\033[28D");
    print_bubble(regcon.text2, regcon.col2, 0, "FLAG", "s", 0);
    print_bubble(regcon.col2,  regcon.text2, -1, OBJ2BIN(data), "s", 0);
    printf("\033[3B");
    putchar('\n');
}
