#define _GNU_SOURCE

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <thread>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>

#include "message_builder.h"

static constexpr size_t SUPERVISOR_BUFFER_SIZE = 2000;
static uint8_t buff[SUPERVISOR_BUFFER_SIZE];

struct connection_msg
{
    uint32_t opcode;
    uint32_t pid;
};

struct libc_call_report_msg
{
    uint32_t opcode;
    uint32_t pid;
    uint32_t code;
};

std::string home_relative_path(const std::string& path)
{
    return std::string(getenv("HOME")) + path;
}

static struct main
{    
    int fd;
    pid_t pid;

    main()
    {
        pid = getpid();

        fd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (fd == -1)
        {
            printf("error creating control socket - %d\n", errno);
            return;
        }

        struct sockaddr_un addr;
        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;

        auto path = home_relative_path("/.libcx/daemon.uds");
        const char* sock_path = path.c_str();
        
        strncpy(addr.sun_path, sock_path, strlen(sock_path));

        printf("Connecting to %s\n", addr.sun_path);

        if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
        {
            printf("error connecting control socket - %d\n", errno);
            return;
        }

        // This code actually runs before programs main and even before its static constructors
        printf("libcx main start pid = %d\n", static_cast<int>(pid));
    }

    void report(uint8_t *ptr, size_t size)
    {
        for (int i = 0; i < size; i++)
        {
            printf("0x%02X, ", ptr[i]);
        }
        printf("\n");

        int actual_size = write(fd, ptr, size);
        if (actual_size != size)
        {
            printf("error sending connection message - %d\n", actual_size);
            return;
        }
    }
} _;

static void * (*real_calloc)(size_t, size_t);
int c = 0;

void *calloc(size_t nmemb, size_t size)
{
    real_calloc = (decltype(real_calloc))dlsym(RTLD_NEXT, "calloc");
    c++;

    puts("calloc2\n");

    void *return_value = real_calloc(nmemb, size);

    size_t msg_length = message_builder::build_message(
        buff, ELibCall::calloc, _.pid, nmemb, size, return_value);

    _.report(buff, msg_length);

    return return_value;
}