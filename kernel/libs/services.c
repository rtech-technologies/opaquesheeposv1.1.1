#include "services.h"
#include "sys.h"

static service_t* registry[8]; // Max 8 services

void service_register(service_t* svc) {
    if (svc->type < 8) {
        registry[svc->type] = svc;
        if (svc->init) svc->init();
    }
}

service_t* service_get(int type) {
    if (type < 8) {
        return registry[type];
    }
    return NULL;
}

// THE APL WRAPPERS (sys.h implementations)
void print(const char* message) {
    service_t* s = service_get(SERVICE_IO);
    if (s && s->write) s->write(message);
}

const char* input(const char* prompt) {
    if (prompt) print(prompt);
    service_t* s = service_get(SERVICE_INPUT);
    return (s && s->read_string) ? s->read_string() : "";
}

size_t fwrite(const char* path, const void* data, size_t size, size_t count) {
    service_t* s = service_get(SERVICE_FILESYS);
    return (s && s->write_file) ? s->write_file(path, data, size, count) : 0;
}

size_t fread(const char* path, void* data, size_t size, size_t count) {
    service_t* s = service_get(SERVICE_FILESYS);
    return (s && s->read_file) ? s->read_file(path, data, size, count) : 0;
}
