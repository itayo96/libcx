#include <stdint.h>

enum class ELibCall : uint32_t
{
    // Memory Allocation
    calloc,

    // File Operations
    fopen,
    fclose,
    fputc,
    fputs,
    fgetc,
    fgets,
    fread,
    fwrite,
    feof,
    fseek,
    ftell,
    fprintf,
    fscanf,
};
