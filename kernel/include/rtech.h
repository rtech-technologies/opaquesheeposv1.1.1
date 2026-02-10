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

/* Rtech Standard Library - System Services */

void print(const char *message);
const char *input(const char *prompt);
size_t fwrite(const char *path, const void *data, size_t size, size_t count);
size_t fappend(const char *path, const void *data, size_t size, size_t count);
size_t fread(const char *path, void *data, size_t size, size_t count);
int fdelete(const char *path);
void flist(void);
void run(const char *path);

int dmake(const char *path);
int dremove(const char *path);
int dmove(const char *from, const char *to);
int fFormat(const char *label);

void *memcpy_simple(void *dest, const void *src, size_t count);
void *memset_simple(void *dest, int value, size_t count);
int streq(const char *s1, const char *s2);
void exec_line(char *line);
size_t strlen(const char *s);
int str_startswith(const char *s, const char *prefix);
char *str_split_once(char *s, char delim);

const char *svc_get_os_name(void);
const char *svc_get_os_version(void);
uint64_t svc_get_uptime(void);
size_t svc_get_memory_usage(void);

/* Low-level I/O provided by the system */
void iolib_clear(uint32_t color);
void iolib_reset_cursor(void);
char iolib_getc(void);

#endif
