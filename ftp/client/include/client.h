#ifndef CLIENT_H
#define CLIENT_H

using namespace std;

enum class ECommandType
{
    Get,
    PutFragments,
    PutSingleBlock,
    AttackDoubleFree,
    AttackGetPassword,
};

class Client
{
public:
    struct client_config
    {
        ECommandType command;
        string src_file;
        string dest_dir;
    };
};

#endif // CLIENT_H