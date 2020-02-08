#include "session.h"

bool Session::start()
{
    // wait for protocol start message
    ProtocolStartRequest start_request;
    ssize_t len = read(_client_socket, reinterpret_cast<void *>(&start_request), sizeof(start_request));

    if (len != sizeof(start_request))
    {
        cout << "[Session::start] Error reading protocol start request - " << len << "\n";
        return false;
    }

    if (start_request.header.opcode != EOpcodes::ProtocolStartRequest || 
        start_request.header.size != sizeof(ProtocolStartRequest))
    {
        cout << "[Session::start] Invalid message received\n";
        return false;
    }

    // save client id for this session
    _client_id = start_request.header.client_id;
    cout << "[Session::start] Starting protocol for client " << _client_id << "\n";

    // send a response to the client
    if (!send_protocol_start_response())
    {
        return false;
    }

    // execute the requested command
    if (start_request.type == EProtocolType::Put)
    {
        return put();
    }
    else if (start_request.type == EProtocolType::Get)
    {
        return get();
    }
    else
    {
        cout << "[Session::start] Invalid protocol type\n";
        return false;
    } 
}

bool Session::get()
{
    return true;
}

bool Session::put()
{
    return true;
}

bool Session::put_fragments()
{
    return true;
}

bool Session::put_single_block()
{
    return true;
}

bool Session::send_protocol_start_response()
{
    // create response
    ProtocolStartResponse response;
    response.permission = EProtocolPermission::Allowed; // can add blacklist check if you want

    // send response
    ssize_t len = write(_client_socket, reinterpret_cast<char *>(&response), sizeof(response));

    if (len != sizeof(response))
    {
        cout << "[Session::send_protocol_start_response] Error sending response - " << len << "\n";
        return false;
    }

    return true;
}
