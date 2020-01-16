#!/usr/bin/python3
from pathlib import Path
import daemon_uds_server

daemon_folder = "~/.libcx/"
daemon_main_uds_filename = daemon_folder + "daemon.uds"


def create_folder():
    """
    Creates the daemon folder if it doesnt exists
    """
    Path(daemon_folder).mkdir(parents=True, exist_ok=True)


def main():
    create_folder()
    server = daemon_uds_server.DaemonUDSServer(daemon_main_uds_filename, lambda a : a)
    server.run()


if __name__ == '__main__':
    main()
