#!/usr/bin/python3
from pathlib import Path
import daemon_uds_server
import logging
import sys

daemon_folder = "~/.libcx/"
daemon_main_uds_filename = str(Path(daemon_folder + "daemon.uds").expanduser())


def create_folder(path):
    """
    :param path: The path of the folder to create
    """
    Path(path).mkdir(parents=True, exist_ok=True)


def main():
    setup_logging()
    create_folder(daemon_folder)
    server = daemon_uds_server.DaemonUDSServer(daemon_main_uds_filename, lambda a : a)
    server.run()


def setup_logging():
    root = logging.getLogger()
    root.setLevel(logging.DEBUG)

    handler = logging.StreamHandler(sys.stdout)
    handler.setLevel(logging.DEBUG)
    formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
    handler.setFormatter(formatter)
    root.addHandler(handler)

if __name__ == '__main__':
    main()
