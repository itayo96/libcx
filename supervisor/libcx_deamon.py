#!/usr/bin/python3
import logging
import sys
import client_handler
from server import daemon_uds_server
from logging import getLogger
from pathlib import Path
from utils.logging import setup_logging

log = getLogger(__name__)

daemon_folder = "~/.libcx/"
daemon_main_uds_filename = str(Path(daemon_folder + "daemon.uds").expanduser())


def main():
    setup_logging()
    create_folder(daemon_folder)
    server = daemon_uds_server.DaemonUDSServer(daemon_main_uds_filename,
                                               client_handler.create,
                                               client_handler.libc_call_callback)


def create_folder(path: str) -> None:
    """
    :param path: The path of the folder to create
    """
    Path(path).mkdir(parents=True, exist_ok=True)


if __name__ == '__main__':
    main()
