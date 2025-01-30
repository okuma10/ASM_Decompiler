#pragma once
#include "defines.inl"
#include "asm_sim_types.inl"

// Based on Intel 8086 manual. chapter: "Machine Instruction Encoding and Decoding"
// |  1st Byte  |x| 2nd Byte   |x| 3rd Byte |x| 4th Byte |x| 5th Byte |x| 6-th Byte |x|
// ||OPCODE|D|W||x| MOD|REG|R/M|x|   Displ  |x|   Displ  |x|   Data   |x|   Data    |x|
// |------------|x|------------|x|----------|x|----------|x|----------|x|-----------|x|
// ||000000|0|0||x| 00 |000|000|x|          |x|          |x|          |x|           |x|
// |------------|x|------------|x|----------|x|----------|x|----------|x|-----------|x|
// __ 1st Byte ________________________________________________________
// OpCode -> describes the operation
// D      -> describes if REG is src=0 or dst=1
//  S     -> sometimes D can be the S flag, it means that `Data` is sign filled to fit in the register
//           if S is 1 it means Data is 8bit and is `sign filled` to match the 18bit register. If S is 0,
//           Data is already 16bit.
// W      -> describes if the operans are 1 byte or 1 word(2 bytes)
// __ Secibd Byte _____________________________________________________
// MOD    -> defines if RM is a register or memory address with offset either 8 or 16bits
// REG    -> Address that is a register. Some commands REG is an opCode extension
// R/M    -> Address that is either register or memory depending on the value of MOD


void decodeBinary(asm_host* host, u8* binary_buffer, u64 buf_len);

u32 decodeMOV(asm_host* host, u8* instruction_pos);
u32 decodeADD(u8* instruction_pos);
u32 decodeSUB(u8* instruction_pos);
u32 decodeCMP(u8* instruction_pos);
u32 decodeJMP(u8* instruction_pos);
