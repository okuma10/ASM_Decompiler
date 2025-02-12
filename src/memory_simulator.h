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

typedef enum SUB_REG: u8{
    REG_LOW,REG_HIGH, REG_X
}SUB_REG;

typedef enum REG_FLAGS : u16 {
    REG_FLG_CF = 1,
    REG_FLG_PF = 4,
    REG_FLG_AF = 16,
    REG_FLG_ZF = 64,
    REG_FLG_SF = 128,
    REG_FLG_TF = 256,
    REG_FLG_IF = 512,
    REG_FLG_DF = 1024,
    REG_FLG_OF = 2048
}REG_FLAGS;

typedef enum Arithmetic: u8{ // I'll encode the arithmetic to the top 4 bits of the 32bit result passed for the set flags
    ARTH_ADD = 1,
    ARTH_SUB,
    ARTH_MLT
}Arithmetic;


void createRegisters(asm_host* host);

void MovToRegister(asm_host* host, u32 reg_id, u8 hl, i16 val);
void GetFromReg(asm_host* host, u32 reg_id, u8 hl, i16* val);
void SetFlagReg(asm_host* host, i16 val1, i16 val2, i32 result);

void printReg(asm_host* host, u32 reg_id, u8 HL);
void printRegFull(asm_host* host);
