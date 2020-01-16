#!/usr/bin/python3
import daemon_uds_server
import logging
import sys
from logging import getLogger
from pathlib import Path

log = getLogger(__name__)

daemon_folder = "~/.libcx/"
daemon_main_uds_filename = str(Path(daemon_folder + "daemon.uds").expanduser())


def create_folder(path):
    """
    :param path: The path of the folder to create
    """
    Path(path).mkdir(parents=True, exist_ok=True)


async def register_new_client(pid):
    log.info("Registering a new client")


def main():
    setup_logging()
    create_folder(daemon_folder)
    server = daemon_uds_server.DaemonUDSServer(daemon_main_uds_filename, register_new_client)


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
