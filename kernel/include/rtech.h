#ifndef RTECH_H
#define RTECH_H

/* Rtech Standard Library - Core Types */

#if !defined(_STDINT_H) && !defined(_EFI_BIND_H)
typedef unsigned long long size_t;
typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;

typedef signed char        int8_t;
typedef signed short       int16_t;
typedef signed int         int32_t;
typedef signed long long   int64_t;
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif

#endif
