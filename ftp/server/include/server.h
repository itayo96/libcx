#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <cstring>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include "session.h"

using namespace std;

class Server
{
public:
    struct server_config
    {
        string root_dir;
    };

    Server() : _is_initialized(false) {};
    ~Server() { close(_socket); };

    /**
     * initialize the server 
     */
    bool setup(server_config & config);

    /**
     * run the server
     */
    void run();

private:
    static void * handle_connection(void * connection);

private:
    bool _is_initialized;
    int _socket;
};

#endif // SERVER_H