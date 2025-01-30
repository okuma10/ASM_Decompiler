#pragma once
#include "defines.inl"

typedef struct xREG{
    i16 ax;
    i16 cx;
    i16 dx;
    i16 bx;
    i16 sp;
    i16 bp;
    i16 si;
    i16 di;
    i16 ip;
    i16 flag;
    i16 cs;
    i16 ds;
    i16 ss;
    i16 es;
}xREG;
typedef struct hlREG{
    i8 ah; i8 al;
    i8 ch; i8 cl;
    i8 dh; i8 dl;
    i8 bh; i8 bl;
}hlREG;

typedef union reg86{
    xREG x;
    hlREG hl;
}reg86;

typedef struct asm_host{
    reg86* reg; //registers are 128(8x16) bytes in size.
    void*  mem;
    u64    mem_size;
}asm_host;
