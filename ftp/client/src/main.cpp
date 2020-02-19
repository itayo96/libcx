#include <iostream>
#include "argh.h"
#include "client.h"

using namespace std;

void print_usage()
{
    cout << "usage: ftp-client get --src=source_file_on_server [--dest=destination_directory]\n";
    cout << "usage: ftp-client put --type=fragments|block --src=source_file [--dest=destination_directory_on_server]\n";
    cout << "usage: ftp-client attack --type=double-free|privilege-escalation\n";
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        cout << "Too few arguments, type ftp-client -h|--help|--usage for usage\n";
        return 1;
    }

    auto cmdl = argh::parser(argc, argv);

    if (cmdl[{"-h", "--help", "--usage"}])
    {
        print_usage();
        return 0;
    }

    Client::client_command config;
    string command(cmdl[1]);

    if (command.compare("put") == 0)
    {
        if (!cmdl("--type") || !cmdl("--src"))
        {
            cout << "Missing arguments (Put), type ftp-client --help for usage\n";
            return 1;
        }

        string type;
        cmdl("type") >> type;
        if (type.compare("fragments") == 0)
        {
            config.command = ECommandType::PutFragments;
        }
        else if (type.compare("block") == 0)
        {
            config.command = ECommandType::PutSingleBlock;
        }
        else
        {
            cout << "Put type not supported, type ftp-client --help for usage\n";
        }

        cmdl("src") >> config.src_file;
        cmdl("dest", "") >> config.dest_dir;
    }
    else if (command.compare("get") == 0)
    {
        if (!cmdl("--src"))
        {
            cout << "Missing arguments (Get), type ftp-client --help for usage\n";
            return 1;
        }

        config.command = ECommandType::Get;
        cmdl("src") >> config.src_file;
        cmdl("dest", "") >> config.dest_dir;
    }
    else if (command.compare("attack") == 0)
    {
        if (!cmdl("--type"))
        {
            cout << "Missing arguments (Attack), type ftp-client --help for usage\n";
            return 1;
        }

        string type;
        cmdl("type") >> type;
        if (type.compare("privilege-escalation") == 0)
        {
            config.command = ECommandType::AttackPrivilegeEscalation;
        }
        else if (type.compare("double-free") == 0)
        {
            config.command = ECommandType::AttackDoubleFree;
        }
        else
        {
            cout << "Attack type not supported, type ftp-client --help for usage\n";
        }
    }
    else
    {
        cout << "Command not supported, type ftp-client -h|--help|--usage for usage\n";
        return 1;
    }

    // create and setup client
    Client client;
    if (!client.setup())
    {
        cout << "Error setting up client\n";
        return 1;
    }

    // execute the command parsed from cli
    if (!client.execute(config))
    {
        cout << "Error running client command\n";
        return 1;
    }

    cout << "Client finished running successfully!\n";
    return 0;
}