#ifndef MESSAGES_H
#define MESSAGES_H

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
    Error,
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

    Header(EOpcodes opcode) : opcode(opcode) {}
};


struct ProtocolStartRequest
{
    Header header;
    EProtocolType type;

    ProtocolStartRequest() : header(EOpcodes::ProtocolStartRequest) {}
};

struct ProtocolStartResponse
{
    Header header;
    EProtocolPermission permission;

    ProtocolStartResponse() : header(EOpcodes::ProtocolStartResponse) {}
};

struct ProtocolEnd
{
    Header header;
    EProtocolStatus status;

    ProtocolEnd() : header(EOpcodes::ProtocolEnd) {}
};

struct PutRequest
{
    Header header;
    EPutType put_type;
    char file_path[MAX_PATH_LEN];
    size_t file_size;

    PutRequest() : header(EOpcodes::PutRequest) {}
};

struct PutResponse
{
    Header header;
    EProtocolStatus status;

    PutResponse() : header(EOpcodes::PutResponse) {}
};

struct GetRequest
{
    Header header;
    char file_path[MAX_PATH_LEN];

    GetRequest() : header(EOpcodes::GetRequest) {}
};

struct GetResponse
{
    Header header;
    EProtocolStatus status;
    size_t file_size;

    GetResponse() : header(EOpcodes::GetResponse) {}
};

struct DataFragment
{
    Header header;
    size_t payload_len;
    uint8_t payload[FRAGMENT_SIZE];

    DataFragment() : header(EOpcodes::DataFragment) {}
};

struct DataFragmentAck
{
    Header header;
    EProtocolStatus status;

    DataFragmentAck() : header(EOpcodes::DataFragmentAck) {}
};

struct Disconnect
{
    Header header;

    Disconnect() : header(EOpcodes::Disconnect) {}
};

#endif // MESSAGES_H