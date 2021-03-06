#include "session.h"
#include "utils.h"

Session::Session(const sockaddr_in & client_addr, const int & client_fd, const string & root_dir) :
    _client_addr(client_addr), _client_socket(client_fd), _root_dir(root_dir)
{
    // set deafult timeout on client socket
    int ret = setsockopt(_client_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&TIMEOUT, sizeof(TIMEOUT));
    if (ret != 0)
    {
        cout << "[Session] Error setting timeout, errno - " << errno << endl;
    }
}

bool Session::start()
{
    // wait for protocol start message
    ProtocolStartRequest start_request;
    ssize_t len = read(_client_socket, reinterpret_cast<void *>(&start_request), sizeof(start_request));

    if (len != sizeof(start_request))
    {
        cout << "[Session::start] Error reading protocol start request - " << len << endl;
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
    cout << "[Session::start] Starting protocol for client " << _client_id << endl;

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
    FILE * file;
    EGetStates state = EGetStates::WaitForGetRequest;
    ssize_t len;
    bool finished_sending = false;
    size_t file_size;
    size_t sent_data = 0;
    std::string full_path;

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
                    cout << "[Session::get] Error reading get request - " << len << endl;
                    return false;
                }
                
                if (len != sizeof(GetRequest) || request.header.opcode != EOpcodes::GetRequest
                    || request.header.size != sizeof(GetRequest))
                {
                    cout << "[Session::get] Invalid get request\n";
                    return false;
                }

                cout << "[Session::get] Got get request\n";

                // open requested file for reading
                full_path = get_full_path(request.file_path);
                file = fopen(full_path.c_str(), "rb");
                if (file == nullptr)
                {
                    cout << "[Session::get] File not found\n";
                    state = EGetStates::FileNotFound;
                    break;
                }

                file_size = filesize(full_path);

                state = EGetStates::SendGetResponse;
                break;
            }

            case (EGetStates::SendGetResponse):
            {
                GetResponse response;
                response.status = EProtocolStatus::Success;
                response.file_size = file_size;
                printf("TOTAL SIZE: %d\n", response.file_size);

                len = send(_client_socket, &response, sizeof(GetResponse), 0);
                if (len < 0)
                {
                    cout << "[Session::get] Error sending get response - " << len << endl;
                    return false;
                }

                cout << "[Session::get] Sent get response\n";
                state = EGetStates::SendData;
                break;
            }

            case EGetStates::SendData:
            {
                DataFragment fragment;

                size_t res = fread(fragment.payload, sizeof(uint8_t), FRAGMENT_SIZE, file);
                fragment.payload_len = FRAGMENT_SIZE;
                sent_data += res;
                finished_sending = false;

                // check if there was an error or EOF was reached
                if (sent_data == file_size)
                {
                    printf("PENIS - %d\n", res);
                    finished_sending = true;
                    fragment.payload_len = res;
                }
                else if (res != FRAGMENT_SIZE)
                {
                    cout << "[Session::get] Error reading from file\n";
                    fclose(file);
                    return false;
                }

                len = send(_client_socket, &fragment, sizeof(DataFragment), 0);
                if (len < 0)
                {
                    cout << "[Session::get] Error sending data fragment - " << len << endl;
                    fclose(file);
                    return false;
                }

                cout << "[Session::get] Sent data fragment\n";
                state = EGetStates::WaitForDataAck;
                break;
            }

            case EGetStates::WaitForDataAck:
            {
                DataFragmentAck ack;
            static int i = 0;
                len = read(_client_socket, reinterpret_cast<void *>(&ack), sizeof(ack));
                if (len < 0)
                {
                    cout << "[Session::get] Error reading data fragment ack - " << len << endl;
                    fclose(file);
                    return false;
                }
                
                if (len != sizeof(DataFragmentAck) || ack.header.opcode != EOpcodes::DataFragmentAck
                    || ack.header.size != sizeof(DataFragmentAck))
                {
                    cout << "[Session::get] Invalid data fragment ack\n";
                    printf("len: %d (sizeof: %d), op: %d, hdr.size: %d\n", len, sizeof(DataFragmentAck), ack.header.opcode, ack.header.size);
                    fclose(file);
                    return false;
                }

                if (ack.status == EProtocolStatus::Success)
                {
                    cout << "[Session::get] Got data fragment ack " << i << "\n";
                    i++;
                    printf("finish %d\n", finished_sending?1:0);
                    state = finished_sending ? EGetStates::WaitForDisconnect : EGetStates::SendData;
                }
                else
                {
                    cout << "[Session::get] Fragment ack bad status - " << (int)ack.status << endl;
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
                    cout << "[Session::get] Error sending file not found - " << len << endl;
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
                    cout << "[Session::get] Error reading disconnect message - " << len << endl;
                    fclose(file);
                    return false;
                }
                
                if (len != sizeof(Disconnect) || disconnect.header.opcode != EOpcodes::Disconnect
                    || disconnect.header.size != sizeof(Disconnect))
                {
                    cout << "[Session::get] Invalid disconnect message\n";
                    fclose(file);
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

    fclose(file);
    return true;
}

bool Session::put()
{
    PutRequest request;

    // read put request message
    ssize_t len = read(_client_socket, reinterpret_cast<void *>(&request), sizeof(request));
    if (len < 0)
    {
        cout << "[Session::put] Error reading put request - " << len << endl;
        return false;
    }
    
    if (len != sizeof(PutRequest) || request.header.opcode != EOpcodes::PutRequest
        || request.header.size != sizeof(PutRequest))
    {
        cout << "[Session::put] Invalid put request\n";
        return false;
    }

    cout << "[Session::put] Got put request\n";
    std::string file_path(request.file_path);
    std::string full_path = _root_dir + file_path;

    // check the put type and forward to correct function
    switch (request.put_type)
    {
        case (EPutType::Fragments):
            return put_fragments(request);

        case (EPutType::SingleBlock):
            return put_single_block(request);

        default:
            cout << "[Session::put] Invalid put type\n";
            return false;
    }
}

bool Session::put_fragments(PutRequest & request)
{
    ssize_t len;
    std::string full_path = get_full_path(request.file_path);

    // open file for writing (if exists we override the existing data in the file)
    FILE * file = fopen(full_path.c_str(), "wb");
    if (file == nullptr)
    {
        cout << "[Session::put_fragments] Error opening file\n";
        return false;
    }

    // send put response to start receiving data
    PutResponse response;
    response.status = EProtocolStatus::Success;
    len = send(_client_socket, &response, sizeof(PutResponse), 0);
    if (len < 0)
    {
        cout << "[Session::put_fragments] Error sending put response - " << len << endl;
        return false;
    }

    EPutStates state = EPutStates::WaitForData;
    size_t received_data = 0;

    while (state != EPutStates::Finish)
    {
        switch (state)
        {
            case (EPutStates::WaitForData):
            {
                // read data fragment
                DataFragment fragment;
                len = read(_client_socket, reinterpret_cast<void *>(&fragment), sizeof(fragment));

                if (len < 0)
                {
                    cout << "[Session::put_fragments] Error reading data fragment - " << len << endl;
                    fclose(file);
                    return false;
                }

                if (fragment.header.opcode != EOpcodes::DataFragment || len != sizeof(fragment) ||
                    fragment.header.size != sizeof(fragment) || fragment.payload_len > FRAGMENT_SIZE)
                {
                    cout << "[Session::put_fragments] Invalid data fragment\n";
                    fclose(file);
                    return false;
                }

                if (fragment.payload_len + received_data > request.file_size)
                {
                    cout << "[Session::put_fragments] Too much data received\n";
                    fclose(file);
                    return false;
                }

                cout << "[Session::put_fragments] Got data fragment\n";

                // write data to file
                size_t res = fwrite(fragment.payload, sizeof(uint8_t), fragment.payload_len, file);
                if (res != fragment.payload_len)
                {
                    cout << "[Session::put_fragments] Error writing to file\n";
                    fclose(file);
                    return false;
                }

                received_data += fragment.payload_len;

                state = EPutStates::SendDataAck;
                break;
            }

            case (EPutStates::SendDataAck):
            {
                DataFragmentAck ack;
                ack.status = EProtocolStatus::Success;

                len = send(_client_socket, &ack, sizeof(ack), 0);
                if (len < 0)
                {
                    cout << "[Session::put_fragments] Error sending data fragment ack - " << len << endl;
                    fclose(file);
                    return false;
                }

                cout << "[Session::put_fragments] Sent data fragment ack\n";

                if (received_data == request.file_size)
                {
                    state = EPutStates::WaitForDisconnect;
                }
                else
                {
                    state = EPutStates::WaitForData;
                }

                break;
            }

            case (EPutStates::WaitForDisconnect):
            {
                // read disconnect message
                Disconnect disconnect;
                len = read(_client_socket, reinterpret_cast<void *>(&disconnect), sizeof(disconnect));

                if (len < 0)
                {
                    cout << "[Session::put_fragments] Error reading disconnect message - " << len << endl;
                    fclose(file);
                    return false;
                }

                if (disconnect.header.opcode != EOpcodes::Disconnect || len != sizeof(disconnect) ||
                    disconnect.header.size != sizeof(disconnect))
                {
                    cout << "[Session::put_fragments] Invalid disconnect message\n";
                    fclose(file);
                    return false;
                }

                cout << "[Session::put_fragments] Got disconnect\n";

                state = EPutStates::Finish;
                break;
            }

            default:
            {
                cout << "[Session::put_fragments] Invalid state reached\n";
                return false;
            }
        }
    }

    fclose(file);
    return true;
}

bool Session::put_single_block(PutRequest & request)
{
    ssize_t len;
    std::string full_path = get_full_path(request.file_path);

    if (request.file_size > MAX_FILE_SIZE)
    {
        cout << "[Session::put_single_block] File too big, try sending by fragments\n";
        return false;
    }

    char * data = (char *)calloc(request.file_size, sizeof(char));
    if (data == nullptr)
    {
        cout << "[Session::put_single_block] Error allocating memory\n";
        return false;
    }

    // open file for writing (if exists we override the existing data in the file)
    FILE * file = fopen(full_path.c_str(), "wb");
    if (file == nullptr)
    {
        cout << "[Session::put_single_block] Error opening file\n";
        return false;
    }

    // send put response to start receiving data
    PutResponse response;
    response.status = EProtocolStatus::Success;
    len = send(_client_socket, &response, sizeof(PutResponse), 0);
    if (len < 0)
    {
        cout << "[Session::put_single_block] Error sending put response - " << len << endl;
        return false;
    }

    size_t received_data = 0;
    EPutStates state = EPutStates::WaitForData;

    while (state != EPutStates::Finish)
    {
        switch (state)
        {
            case (EPutStates::WaitForData):
            {
                // read data fragment
                DataFragment fragment;
                len = read(_client_socket, reinterpret_cast<void *>(&fragment), sizeof(fragment));

                if (len < 0)
                {
                    cout << "[Session::put_single_block] Error reading data fragment - " << len << endl;
                    free(data);
                    fclose(file);
                    return false;
                }

                if (fragment.header.opcode != EOpcodes::DataFragment || len != sizeof(fragment) ||
                    fragment.header.size != sizeof(fragment) || fragment.payload_len > FRAGMENT_SIZE)
                {
                    cout << "[Session::put_single_block] Invalid data fragment\n";
                    free(data);
                    fclose(file);
                    return false;
                }

                if (fragment.payload_len + received_data > request.file_size)
                {
                    cout << "[Session::put_single_block] Too much data received\n";
                    free(data);
                    fclose(file);
                    return false;
                }

                cout << "[Session::put_single_block] Got data fragment\n";

                // copy payload to data buffer
                memcpy(data + received_data, reinterpret_cast<char *>(fragment.payload), fragment.payload_len);
                received_data += fragment.payload_len;

                state = EPutStates::SendDataAck;
                break;
            }

            case (EPutStates::SendDataAck):
            {
                DataFragmentAck ack;
                ack.status = EProtocolStatus::Success;

                len = send(_client_socket, &ack, sizeof(ack), 0);
                if (len < 0)
                {
                    cout << "[Session::put_single_block] Error sending data fragment ack - " << len << endl;
                    free(data);
                    fclose(file);
                    return false;
                }

                cout << "[Session::put_single_block] Sent data fragment ack\n";

                if (received_data == request.file_size)
                {
                    state = EPutStates::Dump;
                }
                else
                {
                    state = EPutStates::WaitForData;
                }

                break;
            }

            case (EPutStates::Dump):
            {
                // write the data we accumilated to the file
                size_t res = fwrite(data, sizeof(uint8_t), received_data, file);
                if (res != received_data)
                {
                    cout << "[Session::put_single_block] Error writing to file\n";
                    fclose(file);
                    return false;
                }

                // dalete the data buffer
                free(data);

                state = EPutStates::WaitForDisconnect;
                break;
            }

            case (EPutStates::WaitForDisconnect):
            {
                // read disconnect message
                Disconnect disconnect;
                len = read(_client_socket, reinterpret_cast<void *>(&disconnect), sizeof(disconnect));

                if (len < 0)
                {
                    cout << "[Session::put_single_block] Error reading disconnect message - " << len << endl;
                    free(data);
                    fclose(file);
                    return false;
                }

                if (disconnect.header.opcode != EOpcodes::Disconnect || len != sizeof(disconnect) ||
                    disconnect.header.size != sizeof(disconnect))
                {
                    cout << "[Session::put_single_block] Invalid disconnect message\n";
                    free(data);
                    fclose(file);
                    return false;
                }

                cout << "[Session::put_single_block] Got disconnect\n";

                state = EPutStates::Finish;
                break;
            }

            default:
            {
                cout << "[Session::put_single_block] Invalid state reached\n";
                free(data);
                return false;
            }
        }
    }
    
    fclose(file);
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
        cout << "[Session::send_protocol_start_response] Error sending response - " << len << endl;
        return false;
    }

    return true;
}