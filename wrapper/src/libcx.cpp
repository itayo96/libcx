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

    main()
    {
        struct sockaddr_un addr;
        int size;
        auto path =  home_relative_path("/.libcx/daemon.uds");
        const char* sock_path = path.c_str();
        struct connection_msg msg;


        fd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (fd == -1)
        {
            printf("error creating control socket - %d\n", errno);
            goto handle_error;
        }

        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, sock_path, strlen(sock_path));

        printf("Connecting to %s\n", addr.sun_path);

        if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
        {
            printf("error connecting control socket - %d\n", errno);
            goto handle_error;
        }

        msg.opcode = 0x3001;
        msg.pid = static_cast<uint32_t>(getpid());

        size = write(fd, (uint8_t*)&msg, sizeof(msg));
        if (size != sizeof(msg))
        {
            printf("error sending connection message - %d\n", size);
            goto handle_error;
        }

        // This code actually runs before programs main and even before its static constructors
        printf("libcx main start pid = %d\n", static_cast<int>(getpid()));

    handle_error:
        printf("exit libcx main\n");
    }

    void report()
    {
        libc_call_report_msg msg;

        msg.opcode = 0x1308;
        msg.pid = static_cast<uint32_t>(getpid());
        msg.code = 1;

        int size = write(fd, (uint8_t*)&msg, sizeof(msg));
        if (size != sizeof(msg))
        {
            printf("error sending connection message - %d\n", size);
            return;
        }
    }

} _;

static void * (*real_calloc)(size_t, size_t);
int c = 0;

void *calloc(size_t a, size_t b)
{
    real_calloc = (decltype(real_calloc))dlsym(RTLD_NEXT, "calloc");
    c++;

    puts("calloc2\n");
    _.report();

    return real_calloc(a, b);
}