#ifndef SESSION_H
#define SESSION_H

#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cstring>
#include "messages.h"
#include "server_config.h"

using namespace std;

enum class EGetStates
{
    WaitForGetRequest,
    SendGetResponse,
    SendData,
    WaitForDataAck,
    WaitForDisconnect,
    FileNotFound,
    Finish,
};

enum class EPutStates
{
    WaitForData,
    SendDataAck,
    Dump,
    WaitForDisconnect,
    Finish,
};

class Session
{
public:
    Session(const sockaddr_in & client_addr, const int & client_fd);

    ~Session() {}

    /**
     * starts a session with a client (get/put file)
     */
    bool start();

private:
    /**
     * concate relative path from the server to the full path in the os
     */
    inline std::string get_full_path(char * relative_path)
    {
        std::string file_path(relative_path);
        std::string full_path = ROOT_DIR + file_path;
        return full_path;
    }

    /**
     * get file from server
     */
    bool get();

    /**
     * put file server
     */
    bool put();

    /**
     * put file server (fragment by fragment)
     */
    bool put_fragments(PutRequest & request);

    /**
     * put file server (single block)
     */
    bool put_single_block(PutRequest & request);

    /**
     * send a protocol start response message to the client
     */
    bool send_protocol_start_response();

private:
    sockaddr_in _client_addr;
    int _client_socket;
    int _client_id;
};

#endif // SESSION_H