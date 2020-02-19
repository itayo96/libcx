#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <string>
#include <cstring>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <fstream>
#include "shared_config.h"
#include "messages.h"

using namespace std;

enum class ECommandType
{
    Get,
    PutFragments,
    PutSingleBlock,
    AttackDoubleFree,
    AttackPrivilegeEscalation,
};

enum class EGetStates
{
    WaitForData,
    SendDataAck,
    SendDisconnect,
    Finish,
};

enum class EPutStates
{
    SendData,
    WaitForDataAck,
    SendDisconnect,
    Finish,
};

class Client
{
public:
    struct client_command
    {
        ECommandType command;
        string src_file;
        string dest_dir;
    };

    Client() {}
    ~Client() { close(_socket); }

    /**
     * Initialize the client
     */
    bool setup();

    /**
     * Execute a command - get/put(s)/attack(s)
     */
    bool execute(const client_command & command);

private:
    bool get(const string & src_file, const string & dest_dir);
    bool put(const string & src_file, const string & dest_dir, EPutType type);
    bool attack_privilege_escalation();
    bool attack_double_free();

private:
    bool _is_initialized;
    int _socket;
};

#endif // CLIENT_H