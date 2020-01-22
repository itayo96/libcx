#ifndef MESSAGE_BUILDER_H
#define MESSAGE_BUILDER_H

#include "message_opcodes.h"

#include <sys/types.h>
#include <string.h>

namespace message_builder
{
    struct buffer
    {
        uint8_t *ptr;
        size_t size;
    };

    template<class Param>
    size_t parse(uint8_t *ptr, Param param)
    {
        memcpy(ptr, &param, sizeof(Param));
        return sizeof(Param);
    }

    template<>
    size_t parse<buffer>(uint8_t *ptr, buffer param)
    {
        memcpy(ptr, &param.size, sizeof(param.size));
        memcpy(ptr + sizeof(param.size), &param.ptr, param.size);
        return sizeof(param.size) + param.size;
    }


    template<class Final>
    size_t serialize(uint8_t *ptr, Final final)
    {
        return parse(ptr, final);
    }

    template<class Current, class ...Next>
    size_t serialize(uint8_t *ptr, Current current, Next ...next)
    {
        size_t current_size = parse(ptr, current);
        size_t next_size = serialize<Next...>(ptr + current_size, next...);

        return current_size + next_size;
    }

    template<class ...Params>
    size_t build_message(uint8_t *ptr, ELibCall lib_call, pid_t pid, Params ...params)
    {
        return serialize(ptr, lib_call, pid, params...);
    }
}



#endif // ifndef MESSAGE_BUILDER_H