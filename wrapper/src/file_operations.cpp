#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "libcx.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>

/**
 * Function wrappers declerations
 */
static FILE * (*_fopen)(const char *filename, const char *mode);
static int (*_fclose)(FILE *stream);
static int (*_fputc)(int character, FILE *stream);
static int (*_fputs)(const char *str, FILE *stream);
static int (*_fgetc)(FILE *stream);
static char * (*_fgets)(char *str, int n, FILE *stream);
static size_t (*_fread)(void *ptr, size_t size, size_t nmemb, FILE *stream);
static size_t (*_fwrite)(const void *ptr, size_t size, size_t nmemb, FILE *stream);
static int (*_feof)(FILE *stream);
static int (*_fseek)(FILE *stream, long int offset, int whence);
static long int (*_ftell)(FILE *stream);
static int (*_fprintf)(FILE *stream, const char *format, ...);
static int (*_fscanf)(FILE *stream, const char *format, ...);



extern "C" FILE *fopen(const char *filename, const char *mode)
{
    _fopen = (decltype(_fopen))dlsym(RTLD_NEXT, "fopen");

    puts("wrapped fopen\n");

    FILE * return_value = _fopen(filename, mode);

    size_t msg_length = message_builder::build_message(
        libcx.buffer, 
        ELibCall::fopen, 
        libcx.pid,
        (uint64_t)return_value,
        message_builder::buffer((uint8_t*)filename, strlen(filename)), 
        message_builder::buffer((uint8_t*)mode, strlen(mode)));

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
        (uint64_t)stream,
        (uint64_t)return_value);

    libcx.report(msg_length);

    return return_value;
}

extern "C" int fputc(int character, FILE *stream)
{
    _fputc = (decltype(_fputc))dlsym(RTLD_NEXT, "fputc");

    puts("wrapped fputc\n");

    int return_value = _fputc(character, stream);

    size_t msg_length = message_builder::build_message(
        libcx.buffer, 
        ELibCall::fputc, 
        libcx.pid,
        (uint64_t)stream,
        (uint32_t)character,
        (uint32_t)return_value);

    libcx.report(msg_length);

    return return_value;
}

extern "C" int fputs(const char *str, FILE *stream)
{
    _fputs = (decltype(_fputs))dlsym(RTLD_NEXT, "fputs");

    puts("wrapped fputs\n");

    int return_value = _fputs(str, stream);

    size_t msg_length = message_builder::build_message(
        libcx.buffer, 
        ELibCall::fputs, 
        libcx.pid,
        (uint64_t)stream,
        (uint32_t)strlen(str), 
        (uint32_t)return_value);

    libcx.report(msg_length);

    return return_value;
}

extern "C" int fgetc(FILE *stream)
{
    _fgetc = (decltype(_fgetc))dlsym(RTLD_NEXT, "fgetc");

    puts("wrapped fgetc\n");

    int return_value = _fgetc(stream);

    size_t msg_length = message_builder::build_message(
        libcx.buffer, 
        ELibCall::fgetc, 
        libcx.pid,
        (uint64_t)stream,
        (uint32_t)return_value);

    libcx.report(msg_length);

    return return_value;
}

extern "C" char * fgets(char *str, int n, FILE *stream)
{
    _fgets = (decltype(_fgets))dlsym(RTLD_NEXT, "fgets");

    puts("wrapped fgets\n");

    char * return_value = _fgets(str, n, stream);

    size_t msg_length = message_builder::build_message(
        libcx.buffer, 
        ELibCall::fgets, 
        libcx.pid,
        (uint64_t)stream,
        (uint32_t)strlen(str),
        (uint32_t)n,
        (uint64_t)return_value ? strlen(return_value) : 0);

    libcx.report(msg_length);

    return return_value;
}

extern "C" size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    _fread = (decltype(_fread))dlsym(RTLD_NEXT, "fread");

    puts("wrapped fread\n");

    size_t return_value = _fread(ptr, size, nmemb, stream);

    size_t msg_length = message_builder::build_message(
        libcx.buffer, 
        ELibCall::fread, 
        libcx.pid,
        (uint64_t)stream,
        (uint64_t)ptr,
        (uint32_t)size,
        (uint32_t)nmemb,
        (uint32_t)return_value);

    libcx.report(msg_length);

    return return_value;
}

extern "C" size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    _fwrite = (decltype(_fwrite))dlsym(RTLD_NEXT, "fwrite");

    puts("wrapped fwrite\n");

    size_t return_value = _fwrite(ptr, size, nmemb, stream);

    size_t msg_length = message_builder::build_message(
        libcx.buffer, 
        ELibCall::fwrite, 
        libcx.pid,
        (uint64_t)stream,
        (uint64_t)ptr,
        (uint32_t)size,
        (uint32_t)nmemb,
        (uint32_t)return_value);

    libcx.report(msg_length);

    return return_value;
}

extern "C" int feof(FILE *stream)
{
    _feof = (decltype(_feof))dlsym(RTLD_NEXT, "feof");

    puts("wrapped feof\n");

    int return_value = _feof(stream);

    size_t msg_length = message_builder::build_message(
        libcx.buffer, 
        ELibCall::feof, 
        libcx.pid,
        (uint64_t)stream,
        (uint64_t)return_value);

    libcx.report(msg_length);

    return return_value;
}

extern "C" int fseek(FILE *stream, long int offset, int whence)
{
    _fseek = (decltype(_fseek))dlsym(RTLD_NEXT, "fseek");

    puts("wrapped fseek\n");

    int return_value = _fseek(stream, offset, whence);

    size_t msg_length = message_builder::build_message(
        libcx.buffer, 
        ELibCall::fseek, 
        libcx.pid,
        (uint64_t)stream,
        (uint64_t)offset,
        (uint32_t)whence,
        (uint64_t)return_value);

    libcx.report(msg_length);

    return return_value;
}

extern "C" long int ftell(FILE *stream)
{
    _ftell = (decltype(_ftell))dlsym(RTLD_NEXT, "ftell");

    puts("wrapped ftell\n");

    long int return_value = _ftell(stream);

    size_t msg_length = message_builder::build_message(
        libcx.buffer, 
        ELibCall::ftell, 
        libcx.pid,
        (uint64_t)stream,
        (uint64_t)return_value);

    libcx.report(msg_length);

    return return_value;
}

extern "C" int fprintf(FILE *stream, const char *format, ...)
{
    _fprintf = (decltype(_fprintf))dlsym(RTLD_NEXT, "fprintf");

    puts("wrapped fprintf\n");

    va_list args;
    va_start(args, format);
    int return_value = _fprintf(stream, format, args);
    va_end(args);

    size_t msg_length = message_builder::build_message(
        libcx.buffer, 
        ELibCall::fprintf, 
        libcx.pid,
        (uint64_t)stream,
        message_builder::buffer((uint8_t*)format, strlen(format)),
        (uint64_t)return_value);

    libcx.report(msg_length);

    return return_value;
}

extern "C" int fscanf(FILE *stream, const char *format, ...)
{
    _fscanf = (decltype(_fscanf))dlsym(RTLD_NEXT, "fscanf");

    puts("wrapped fscanf\n");

    va_list args;
    va_start(args, format);
    int return_value = _fscanf(stream, format, args);
    va_end(args);

    size_t msg_length = message_builder::build_message(
        libcx.buffer, 
        ELibCall::fscanf, 
        libcx.pid,
        (uint64_t)stream,
        message_builder::buffer((uint8_t*)format, strlen(format)),
        (uint64_t)return_value);

    libcx.report(msg_length);

    return return_value;
}
