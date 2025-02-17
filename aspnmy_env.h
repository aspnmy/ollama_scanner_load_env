#ifndef ASPNMY_ENV_H
#define ASPNMY_ENV_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_LINE_LENGTH 4096
#define MAX_ENV_VARS 1000
#define MAX_KEY_LENGTH 256
#define MAX_VALUE_LENGTH 4096

typedef struct {
    char key[MAX_KEY_LENGTH];
    char value[MAX_VALUE_LENGTH];
} EnvVar;

typedef struct {
    EnvVar vars[MAX_ENV_VARS];
    int count;
} EnvVars;

// 核心函数声明
int load_env_file(const char* path, EnvVars* vars);
int save_to_bashrc(const EnvVars* vars);
void print_vars(const EnvVars* vars, int color);
int reload_env(void);
void uninstall_env(void);

#endif
