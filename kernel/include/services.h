#ifndef SERVICES_H
#define SERVICES_H

#include <stdint.h>
#include <stddef.h>

#define SERVICE_IO       0
#define SERVICE_INPUT    1
#define SERVICE_FILESYS  2

typedef struct {
    int type;
    const char* name;
    void (*init)(void);
    void (*write)(const char* message);
    const char* (*read_string)(void);
    size_t (*write_file)(const char* path, const void* data, size_t size, size_t count);
    size_t (*read_file)(const char* path, void* data, size_t size, size_t count);
} service_t;

void service_register(service_t* svc);
service_t* service_get(int type);

#endif
