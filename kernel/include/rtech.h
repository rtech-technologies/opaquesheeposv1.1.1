#ifndef RTECH_H
#define RTECH_H

/* Rtech Standard Library - Core Types */

#if !defined(_STDINT_H) && !defined(_EFI_BIND_H) && !defined(__UINT8_TYPE__)
typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;

typedef signed char        int8_t;
typedef signed short       int16_t;
typedef signed int         int32_t;
typedef signed long long   int64_t;
#else
// Use compiler built-ins if available and no header included
#ifndef uint8_t
#ifdef __UINT8_TYPE__
typedef __UINT8_TYPE__ uint8_t;
#endif
#endif
#ifndef uint16_t
#ifdef __UINT16_TYPE__
typedef __UINT16_TYPE__ uint16_t;
#endif
#endif
#ifndef uint32_t
#ifdef __UINT32_TYPE__
typedef __UINT32_TYPE__ uint32_t;
#endif
#endif
#ifndef uint64_t
#ifdef __UINT64_TYPE__
typedef __UINT64_TYPE__ uint64_t;
#endif
#endif
#endif

#ifndef size_t
#ifdef __SIZE_TYPE__
typedef __SIZE_TYPE__ size_t;
#else
typedef unsigned long long size_t;
#endif
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif

#endif
