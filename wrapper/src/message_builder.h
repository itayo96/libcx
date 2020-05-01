#ifndef MESSAGE_BUILDER_H
#define MESSAGE_BUILDER_H

#include "message_opcodes.h"
#include <cstdio>
#include <sys/types.h>
#include <string.h>

namespace message_builder
{
    struct buffer
    {
        uint8_t *ptr;
        uint64_t size;

        buffer(uint8_t * buff, size_t len) : ptr(buff), size(len) {}
    };

    template<class Param>
    static size_t parse(uint8_t *ptr, Param param)
    {
        memcpy(ptr, &param, sizeof(Param));
        return sizeof(Param);
    }

    template<>
    inline size_t parse<buffer>(uint8_t *ptr, buffer param)
    {
        memcpy(ptr, &param.size, sizeof(param.size));
        memcpy(ptr + sizeof(param.size), param.ptr, param.size);
        return sizeof(param.size) + param.size;
    }


    template<class Final>
    static size_t serialize(uint8_t *ptr, Final final)
    {
        return parse(ptr, final);
    }

    template<class Current, class ...Next>
    static size_t serialize(uint8_t *ptr, Current current, Next ...next)
    {
        size_t current_size = parse(ptr, current);
        size_t next_size = serialize<Next...>(ptr + current_size, next...);

        return current_size + next_size;
    }

    template<class ...Params>
    static size_t build_message(uint8_t *ptr, ELibCall lib_call, pid_t pid, Params ...params)
    {
        uint32_t params_length = serialize(ptr + sizeof(uint32_t), lib_call, pid, params...);
        uint32_t entire_message_length = params_length + sizeof(uint32_t);
        parse(ptr, entire_message_length);
        return entire_message_length;
    }
}

#endif // ifndef MESSAGE_BUILDER_H