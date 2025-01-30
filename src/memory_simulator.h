#pragma once
#include "asm_sim_types.inl"

typedef enum REG_NAMES{
    REG_A,
    REG_C,
    REG_D,
    REG_B,
    REG_SP,
    REG_BP,
    REG_SI,
    REG_DI,
    REG_IP,
    REG_FLAG,
    REG_CS,
    REG_DS,
    REG_SS,
    REG_ES,
}REG_NAMES;

typedef enum SUB_REG{
    REG_LOW,REG_HIGH, REG_X
}SUB_REG;


void createRegisters(asm_host* host);

void MovToRegister(asm_host* host, u32 reg_id, u8 hl, i16 val);
void GetFromReg(asm_host* host, u32 reg_id, u8 hl, i16* val);

void printReg(asm_host* host, u32 reg_id, u8 HL);
void printRegFull(asm_host* host);
