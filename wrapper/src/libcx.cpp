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

static struct main
{
    main()
    {
        struct sockaddr_un addr;
        int fd, size;
        char * sock_path = "~/.libcx/daemon.uds";
        struct connection_msg msg;

        fd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (fd == -1)
        {
            printf("error creating control socket - %d\n", errno);
            goto handle_error;
        }

        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, sock_path, sizeof(sock_path));

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
} _;

static void * (*real_calloc)(size_t, size_t);
int c = 0;

void *calloc(size_t a, size_t b)
{
    real_calloc = (decltype(real_calloc))dlsym(RTLD_NEXT, "calloc");
    c++;

    puts("calloc2\n");

    return real_calloc(a, b);
}