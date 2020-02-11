#include "session.h"

Session::Session(const sockaddr_in & client_addr, const int & client_fd) :
    _client_addr(client_addr), _client_socket(client_fd)
{
    // set deafult timeout on client socket
    int ret = setsockopt(_client_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&TIMEOUT, sizeof(TIMEOUT));
    if (ret != 0)
    {
        cout << "[Session] Error setting timeout\n";
    }
}

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
    ifstream file;
    EGetStates state = EGetStates::WaitForGetRequest;
    ssize_t len;
    bool finished_sending = false;

    while (state != EGetStates::Finish)
    {
        switch (state)
        {
            case EGetStates::WaitForGetRequest:
            {
                GetRequest request;

                len = read(_client_socket, reinterpret_cast<void *>(&request), sizeof(request));
                if (len < 0)
                {
                    cout << "[Session::get] Error reading get request - " << len << "\n";
                    return false;
                }
                
                if (len != sizeof(GetRequest) || request.header.opcode != EOpcodes::GetRequest
                    || request.header.size != sizeof(GetRequest))
                {
                    cout << "[Session::get] Invalid get request\n";
                    return false;
                }

                cout << "[Session::get] Got get request\n";
                std::string file_path(request.file_path);
                std::string full_path = ROOT_DIR + file_path;

                // open requested file for reading
                file.open(full_path, ios_base::binary);
                if (!file.good())
                {
                    cout << "[Session::get] File not found\n";
                    state = EGetStates::FileNotFound;
                    break;
                }

                state = EGetStates::SendData;
                break;
            }

            case EGetStates::SendData:
            {
                DataFragment fragment;

                if (!file.is_open())
                {
                    cout << "[Session::get] File not open for reading\n";
                    return false;
                }

                file.read((char*)fragment.payload, FRAGMENT_SIZE);
                fragment.payload_len = file ? FRAGMENT_SIZE : (size_t)file.gcount();

                len = send(_client_socket, &fragment, sizeof(DataFragment), 0);
                if (len < 0)
                {
                    cout << "[Session::get] Error sending data fragment - " << len << "\n";
                    file.close();
                    return false;
                }

                cout << "[Session::get] Sent data fragment\n";
                finished_sending = file ? false : true;
                state = EGetStates::WaitForDataAck;
                break;
            }

            case EGetStates::WaitForDataAck:
            {
                DataFragmentAck ack;

                len = read(_client_socket, reinterpret_cast<void *>(&ack), sizeof(ack));
                if (len < 0)
                {
                    cout << "[Session::get] Error reading data fragment ack - " << len << "\n";
                    file.close();
                    return false;
                }
                
                if (len != sizeof(DataFragmentAck) || ack.header.opcode != EOpcodes::DataFragmentAck
                    || ack.header.size != sizeof(DataFragmentAck))
                {
                    cout << "[Session::get] Invalid data fragment ack\n";
                    file.close();
                    return false;
                }

                if (ack.status == EProtocolStatus::Success)
                {
                    cout << "[Session::get] Got data fragment ack\n";
                    state = finished_sending ? EGetStates::WaitForDisconnect : EGetStates::SendData;
                }
                else
                {
                    cout << "[Session::get] Fragment ack bad status - " << (int)ack.status << "\n";
                    state = EGetStates::Finish;
                }
                
                break;
            }

            case EGetStates::FileNotFound:
            {
                GetResponse response;
                response.status = EProtocolStatus::FileNotFoundError;
                response.file_size = 0;

                len = send(_client_socket, &response, sizeof(GetResponse), 0);
                if (len < 0)
                {
                    cout << "[Session::get] Error sending file not found - " << len << "\n";
                    return false;
                }

                state = EGetStates::Finish;
                break;
            }

            case EGetStates::WaitForDisconnect:
            {
                Disconnect disconnect;

                len = read(_client_socket, reinterpret_cast<void *>(&disconnect), sizeof(disconnect));
                if (len < 0)
                {
                    cout << "[Session::get] Error reading disconnect message - " << len << "\n";
                    file.close();
                    return false;
                }
                
                if (len != sizeof(Disconnect) || disconnect.header.opcode != EOpcodes::Disconnect
                    || disconnect.header.size != sizeof(Disconnect))
                {
                    cout << "[Session::get] Invalid disconnect message\n";
                    file.close();
                    return false;
                }

                cout << "[Session::get] Got disconnect message\n";
                state = EGetStates::Finish;
                break;
            }
        
            default:
            {
                cout << "[Session::get] Invalid state reached\n";
                return false;
            }
        }
    }

    file.close();
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
    ssize_t len = write(_client_socket, reinterpret_cast<char*>(&response), sizeof(response));

    if (len != sizeof(response))
    {
        cout << "[Session::send_protocol_start_response] Error sending response - " << len << "\n";
        return false;
    }

    return true;
}