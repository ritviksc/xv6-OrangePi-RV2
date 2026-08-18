#ifndef TYPES_H
#define TYPES_H

typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char uchar;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long uint64;

typedef uint64 pde_t;

// SBI typedefs
typedef int sbi_eid_t; // signed 32-bit value
typedef int sbi_fid_t; // signed 32-bit value
typedef long sbi_ret_t; // XLEN width
typedef unsigned long sbi_arg_t; // XLEN width

#endif
