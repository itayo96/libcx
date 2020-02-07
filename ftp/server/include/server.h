#include <string>

using namespace std;

class Server
{
public:
    struct server_config
    {
        string root_dir;
    };

    Server();
    ~Server();

    bool setup();
    void run();

private:

};