#ifndef MESSAGES_H
#define MESSAGES_H

#include <sys/types.h>
#include <unistd.h>
#include "shared_config.h"

enum class EOpcodes
{
    ProtocolStartRequest,
    ProtocolStartResponse,
    ProtocolEnd,
    PutRequest,
    PutResponse,
    GetRequest,
    GetResponse,
    DataFragment,
    DataFragmentAck,
    Dump,
    Disconnect,
};

enum class EProtocolType
{
    Put,
    Get,
};

enum class EProtocolPermission
{
    Allowed,
    Denied,
};

enum class EProtocolStatus
{
    Success,
    GeneralError,
    FileNotFoundError,
};

enum class EPutType
{
    Fragments,
    SingleBlock,
};


struct Header
{
    EOpcodes opcode;
    size_t size;
    int client_id;

    Header(EOpcodes opcode, size_t size, pid_t client_id) : 
        opcode(opcode), size(size), client_id((int)client_id) {}
};


struct ProtocolStartRequest
{
    Header header;
    EProtocolType type;

    ProtocolStartRequest() : header(EOpcodes::ProtocolStartRequest, sizeof(ProtocolStartRequest), getpid()) {}
};

struct ProtocolStartResponse
{
    Header header;
    EProtocolPermission permission;

    ProtocolStartResponse() : header(EOpcodes::ProtocolStartResponse, sizeof(ProtocolStartResponse), getpid()) {}
};

struct ProtocolEnd
{
    Header header;
    EProtocolStatus status;

    ProtocolEnd() : header(EOpcodes::ProtocolEnd, sizeof(ProtocolEnd), getpid()) {}
};

struct PutRequest
{
    Header header;
    EPutType put_type;
    char file_path[MAX_PATH_LEN];
    size_t file_size;

    PutRequest() : header(EOpcodes::PutRequest, sizeof(PutRequest), getpid()) {}
};

struct PutResponse
{
    Header header;
    EProtocolStatus status;

    PutResponse() : header(EOpcodes::PutResponse, sizeof(PutResponse), getpid()) {}
};

struct GetRequest
{
    Header header;
    char file_path[MAX_PATH_LEN];

    GetRequest() : header(EOpcodes::GetRequest, sizeof(GetRequest), getpid()) {}
};

struct GetResponse
{
    Header header;
    EProtocolStatus status;
    size_t file_size;

    GetResponse() : header(EOpcodes::GetResponse, sizeof(GetResponse), getpid()) {}
};

struct DataFragment
{
    Header header;
    size_t payload_len;
    uint8_t payload[FRAGMENT_SIZE];

    DataFragment() : header(EOpcodes::DataFragment, sizeof(DataFragment), getpid()) {}
};

struct DataFragmentAck
{
    Header header;
    EProtocolStatus status;

    DataFragmentAck() : header(EOpcodes::DataFragmentAck, sizeof(DataFragmentAck), getpid()) {}
};

struct Disconnect
{
    Header header;

    Disconnect() : header(EOpcodes::Disconnect, sizeof(Disconnect), getpid()) {}
};

static constexpr size_t MAX_MSG_SIZE = sizeof(DataFragment);

#endif // MESSAGES_H