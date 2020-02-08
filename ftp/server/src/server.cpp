#include "server.h"
#include "server_config.h"

struct Connection
{
    int client_fd;
    sockaddr_in client_addr;
};

bool Server::setup(server_config & config)
{
    int err;

    // create server socket
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket < 0)
    {
        cout << "[Server::setup] Failed to create socket, errno" << _socket << "\n";
        return false;
    }

    // create server address
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = DEFAULT_PORT;

    // bind server socket
    err = bind(_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (err < 0)
    {
        cout << "[Server::setup] Failed to bind socket, errno" << err << "\n";
        return false;
    }

    ROOT_DIR = config.root_dir;
    _is_initialized = true;

    cout << "[Server::setup] Setup done, ready to run\n";

    return true;
}

void Server::run()
{
    // start listening for connections
    listen(_socket, MAX_CONNECTIONS);

    cout << "[Server::run] Running ...\n";

    while (1)
    {
        Connection conn;
        int len = sizeof(conn.client_addr);

        // wait for a new client to connect
        conn.client_fd = accept(_socket, (struct sockaddr *)&conn.client_addr, (socklen_t *)&len);
        
        if (conn.client_fd < 0)
        {
            cout << "[Server::run] Failed to accept connection, errno " << conn.client_fd << "\n";
            continue;
        }

        cout << "[Server::run] Recieved new connection\n";

        // create a thread to handle this session
        pthread_t thread;
        pthread_create(&thread, nullptr, Server::handle_connection, reinterpret_cast<void *>(&conn));
    }
}

void * Server::handle_connection(void * connection)
{
    cout << "[Server::handle_connection] Handling new connection, thread " << pthread_self() << "\n";

    // create and start a new session for this connection
    Connection * conn = reinterpret_cast<Connection*>(connection);
    Session session(conn->client_addr, conn->client_fd);
    
    if (session.start())
    {
        cout << "[Server::handle_connection] Successfully finished session, thread " << pthread_self() << "\n";
    }
    else
    {
        cout << "[Server::handle_connection] Error handling session, thread " << pthread_self() << "\n";
    }
}