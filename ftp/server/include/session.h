#ifndef SESSION_H
#define SESSION_H

#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;

class Session
{
public:
    Session(const sockaddr_in & client_addr, const int & client_fd) : 
        _client_addr(client_addr), _client_socket(client_fd) {}

    ~Session() {}

    /**
     * starts a session with a client (get/put file)
     */
    bool start();

private:
    sockaddr_in _client_addr;
    int _client_socket;
};

#endif // SESSION_H