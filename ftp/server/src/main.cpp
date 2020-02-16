#include <iostream>
#include "argh.h"
#include "messages.h"
#include "server.h"

using namespace std;

void print_usage()
{
    cout << "usage: ftp-server --root=server_root_directory \n";
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cout << "Too few arguments, type ftp-server -h|--help|--usage for usage\n";
        return 1;
    }

    auto cmdl = argh::parser(argc, argv);

    if (cmdl[{"-h", "--help", "--usage"}])
    {
        print_usage();
        return 0;
    }

    Server::server_config config;

    if (cmdl({"-r", "--root"}))
    {
        cmdl({"-r", "--root"}, -1) >> config.root_dir;
    }
    else
    {
        cout << "You must provide a root directory\n";
        return 1;
    }
    
    cout << config.root_dir;

    Server server;

    if (!server.setup(config))
    {
        cout << "Failed to setup server\n";
        return 1;
    }

    server.run();

    return 0;
}