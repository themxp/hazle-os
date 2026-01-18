#ifndef SYSCONF_H
#define SYSCONF_H

#include "types.h"

#define SYSCONF_PATH "/.systemconf"

typedef struct {
    char bin_path[64];
    char prompt[32];
    uint8_t default_fg;
    uint8_t default_bg;
    uint32_t history_size;
    char version[16];
    char name[32];
} sysconf_t;

void sysconf_init(void);
bool sysconf_load(void);
sysconf_t *sysconf_get(void);
const char *sysconf_get_bin_path(void);
const char *sysconf_get_prompt(void);

#endif
