// #ifndef _GNU_SOURCE
// #define _GNU_SOURCE
// #endif

// #include "libcx.h"

// #include <stdio.h>
// #include <stdlib.h>
// #include <thread>
// #include <unistd.h>
// #include <sys/types.h>
// #include <sys/socket.h>
// #include <sys/un.h>
// #include <stdlib.h>

// static void * (*real_calloc)(size_t, size_t);

// // extern "C" void *calloc(size_t nmemb, size_t size)
// // {
// //     real_calloc = (decltype(real_calloc))dlsym(RTLD_NEXT, "calloc");

// //     puts("calloc2\n");

// //     void *return_value = real_calloc(nmemb, size);

// //      size_t msg_length = message_builder::build_message(
// //          libcx.buffer, 
// //          ELibCall::calloc, 
// //          libcx.pid, 
// //          (uint64_t)nmemb, 
// //          (uint64_t)size, 
// //          (uint64_t)return_value);

// //     libcx.report(msg_length);

// //     return return_value;
// // }