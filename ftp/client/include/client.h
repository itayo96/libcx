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
        string src_file; // file we want to put on server
        string dest_file; // file we want to get from server
        string source_dir; // location we want to save the file we got from server
        string dest_dir; // location we want to save the file we put on the server
    };
};

#endif // CLIENT_H