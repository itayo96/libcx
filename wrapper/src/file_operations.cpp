#ifndef _GNU_SOURCE
#define _GNU_SOURCE

#include "libcx.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>

/**
 * Function wrappers declerations
 */
static FILE * (*_fopen)(const char *filename, const char *mode);
static int (*_fclose)(FILE *stream);
static int (*_fputc)(int character, FILE *stream);


/**
 * Function wrappers implementations
 */
extern "C" FILE *fopen(const char *filename, const char *mode)
{
    _fopen = (decltype(_fopen))dlsym(RTLD_NEXT, "fopen");

    puts("wrapped fopen\n");

    FILE * return_value = _fopen(filename, mode);

    size_t msg_length = message_builder::build_message(
        libcx.buffer, 
        ELibCall::fopen, 
        libcx.pid, 
        message_builder::buffer((uint8_t*)filename, strlen(filename)), 
        message_builder::buffer((uint8_t*)mode, strlen(mode)),
        return_value);

    libcx.report(msg_length);

    return return_value;
}

extern "C" int fclose(FILE *stream)
{
    _fclose = (decltype(_fclose))dlsym(RTLD_NEXT, "fclose");

    puts("wrapped fclose\n");

    int return_value = _fclose(stream);

    size_t msg_length = message_builder::build_message(
        libcx.buffer, 
        ELibCall::fclose, 
        libcx.pid, 
        stream,
        return_value);

    libcx.report(msg_length);

    return return_value;
}

extern "C" int fputc(int character, FILE *stream)
{
    _fputc = (decltype(_fputc))dlsym(RTLD_NEXT, "fputc");

    puts("wrapped fclose\n");

    int return_value = _fputc(character, stream);

    size_t msg_length = message_builder::build_message(
        libcx.buffer, 
        ELibCall::fclose, 
        libcx.pid,
        character,
        stream,
        return_value);

    libcx.report(msg_length);

    return return_value;
}

#endif