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
        regs[reg_id] = val;
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
    }else{
        i8* regs = (i8*)(void*)&host->reg->hl;
        u8 _hl = hl == REG_HIGH ? 0 : 1;
        *val  = (i16)regs[reg_id*2+_hl];
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
                print_bubble(regcon.text1, regcon.col1, -1, &host->reg->hl.ah, "x2", 0);
                regcon.col1 = 0xBE9100FF;
                print_bubble(regcon.text1, regcon.col1, -1, &host->reg->hl.al, "x2", 0);
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
                print_bubble(regcon.text1, regcon.col1, -1, &host->reg->hl.bh, "x2", 0);
                regcon.col1 = 0xB34B00FF;
                print_bubble(regcon.text1, regcon.col1, -1, &host->reg->hl.bl, "x2", 0);
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
                print_bubble(regcon.text1, regcon.col1, -1, &host->reg->hl.ch, "x2", 0);
                regcon.col1 = 0x810000FF;
                regcon.text1 = 0xEEEEEEFF;
                print_bubble(regcon.text1, regcon.col1, -1, &host->reg->hl.cl, "x2", 0);
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
                print_bubble(regcon.text1, regcon.col1, -1, &host->reg->hl.dh, "x2", 0);
                regcon.col1 = 0x868A8BFF;
                regcon.text1 = 0xEEEEEEFF;
                print_bubble(regcon.text1, regcon.col1, -1, &host->reg->hl.dl, "x2", 0);
            }
            break;

        case REG_SI:
            if(HL != REG_X){
                printf("Err: reg SI needs HL be REG_X");
                return;
            }
            regcon.col1 = 0x2ECC71FF;
            data = 0;
            data |= ((host->reg->x.si & 0x00FF)<<8);
            data |= ((host->reg->x.si & 0xFF00)>>8);
            print_bubble(regcon.text2, regcon.col2, 0, "SI", "s", 0);
            print_bubble(regcon.text1, regcon.col1, -1, &data, "x4", 0);
            break;

        case REG_DI:
            if(HL != REG_X){
                printf("Err: reg DI needs HL be REG_X");
                return;
            }
            regcon.col1 = 0x3498DBFF;
            data = 0;
            data |= ((host->reg->x.di & 0x00FF)<<8);
            data |= ((host->reg->x.di & 0xFF00)>>8);
            print_bubble(regcon.text2, regcon.col2, 0, "DI", "s", 0);
            print_bubble(regcon.text1, regcon.col1, -1, &data, "x4", 0);
            break;

        case REG_BP:
            if(HL != REG_X){
                printf("Err: reg BP needs HL be REG_X");
                return;
            }
            regcon.col1 = 0x9B59B6FF;
            data = 0;
            data |= ((host->reg->x.bp & 0x00FF)<<8);
            data |= ((host->reg->x.bp & 0xFF00)>>8);
            print_bubble(regcon.text2, regcon.col2, 0, "BP", "s", 0);
            print_bubble(regcon.text1, regcon.col1, -1, &data, "x4", 0);
            break;

        case REG_SP:
            if(HL != REG_X){
                printf("Err: reg SP needs HL be REG_X");
                return;
            }
            regcon.col1  = 0x34495EFF;
            regcon.text1 = 0xECF0F1FF;
            data = 0;
            data |= ((host->reg->x.sp & 0x00FF)<<8);
            data |= ((host->reg->x.sp & 0xFF00)>>8);
            print_bubble(regcon.text2, regcon.col2, 0, "SP", "s", 0);
            print_bubble(regcon.text1, regcon.col1, -1, &data, "x4", 0);
            break;

        case REG_IP:
            if(HL != REG_X){
                printf("Err: reg IP needs HL be REG_X");
                return;
            }
            regcon.col1  = 0x34495EFF;
            regcon.text1 = 0xECF0F1FF;
            data = 0;
            data |= ((host->reg->x.ip & 0x00FF)<<8);
            data |= ((host->reg->x.ip & 0xFF00)>>8);
            print_bubble(regcon.text2, regcon.col2, 0, "IP", "s", 0);
            print_bubble(regcon.text1, regcon.col1, -1, &data, "x4", 0);
            break;

        case REG_FLAG:
            regcon.col1  = 0x34495EFF;
            regcon.text1 = 0xECF0F1FF;
            print_bubble(regcon.text2, regcon.col2, 0, "FLAG", "s", 0);
            print_bubble(regcon.text1, regcon.col1, -1, &host->reg->x.flag, "x4", 0);
            break;

        case REG_CS:
            if(HL != REG_X){
                printf("Err: reg CS needs HL be REG_X");
                return;
            }
            regcon.col1  = 0x34495EFF;
            regcon.text1 = 0xECF0F1FF;
            data = 0;
            data |= ((host->reg->x.cs & 0x00FF)<<8);
            data |= ((host->reg->x.cs & 0xFF00)>>8);
            print_bubble(regcon.text2, regcon.col2, 0, "CS", "s", 0);
            print_bubble(regcon.text1, regcon.col1, -1, &data, "x4", 0);
            break;
        case REG_DS:
            if(HL != REG_X){
                printf("Err: reg DS needs HL be REG_X");
                return;
            }
            regcon.col1  = 0x34495EFF;
            regcon.text1 = 0xECF0F1FF;
            data = 0;
            data |= ((host->reg->x.ds & 0x00FF)<<8);
            data |= ((host->reg->x.ds & 0xFF00)>>8);
            print_bubble(regcon.text2, regcon.col2, 0, "DS", "s", 0);
            print_bubble(regcon.text1, regcon.col1, -1, &data, "x4", 0);
            break;
        case REG_SS:
            if(HL != REG_X){
                printf("Err: reg SS needs HL be REG_X");
                return;
            }
            regcon.col1  = 0x34495EFF;
            regcon.text1 = 0xECF0F1FF;
            data = 0;
            data |= ((host->reg->x.ss & 0x00FF)<<8);
            data |= ((host->reg->x.ss & 0xFF00)>>8);
            print_bubble(regcon.text2, regcon.col2, 0, "SS", "s", 0);
            print_bubble(regcon.text1, regcon.col1, -1, &data, "x4", 0);
            break;
        case REG_ES:
            if(HL != REG_X){
                printf("Err: reg ES needs HL be REG_X");
                return;
            }
            regcon.col1  = 0x34495EFF;
            regcon.text1 = 0xECF0F1FF;
            data = 0;
            data |= ((host->reg->x.es & 0x00FF)<<8);
            data |= ((host->reg->x.es & 0xFF00)>>8);
            print_bubble(regcon.text2, regcon.col2, 0, "ES", "s", 0);
            print_bubble(regcon.text1, regcon.col1, -1, &data, "x4", 0);
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
    
    putchar('\n');
}
