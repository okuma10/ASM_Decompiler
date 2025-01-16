#pragma once

#pragma region Byte to GB,MB,KB conversion constants
#ifndef GB2B 
    #define GB2B(x) ((long long)x * 1073741824)
#endif
#ifndef MB2B
    #define MB2B(x) ((long long)x * 1048576)
#endif
#ifndef KB2B
    #define KB2B(x) ((long long)x * 1024)
#endif
#ifndef B2GB 
    #define B2GB(x) ((double)x / 1073741824)
#endif
#ifndef B2MB
    #define B2MB(x) ((double)x / 1048576)
#endif
#ifndef B2KB
    #define B2KB(x) ((double)x / 1024)
#endif
#pragma endregion



#pragma region Variable Type shorthands
// Unsigned Integers
typedef unsigned char           u8;
typedef unsigned short          u16;
typedef unsigned int            u32;
typedef unsigned long long      u64;
// Signed Integers
typedef char                    i8;
typedef short                   i16;
typedef int                     i32;
typedef long long               i64;
// float
typedef float                   f32;
typedef double                  f64;

// Booleans
typedef int                     b32;
typedef _Bool                   b8;
#pragma endregion

#define UNUSED (void)

#define ARRAY_LEN(x) (sizeof(x)/sizeof((x)[0]))
