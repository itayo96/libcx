import logging
import os
import socket
import binascii
import struct
from proc_status import get_process_info

class DaemonUDSServer:
    """
    An UDS based server for the agents to communicate with. This class handles the communication and not the actual
    logic.
    """

    def __init__(self, address, libc_usage_callback):
        logging.debug("Creating daemon server {}".format(address))
        self._create_server(address)

    def run(self):
        """
        Starts running the server. This method is blocking
        :return: None
        """
        if not self.is_open():
            raise self.ServerNotRunning()

        self._control_socket.listen(1)

        logging.debug("Received a new client")

        connection, client_address = self._control_socket.accept()

        data = connection.recv(8)
        opcode, pid = struct.unpack("II", data)

        logging.debug("opcode = {}, pid = {}, process name = {}".format(opcode, pid, get_process_info(pid).name))

        logging.debug(data)

    def is_open(self):
        """
        :return: Weather the server is open or not
        """
        return self._control_socket is not None

    def _create_server(self, address):
        """
        Creates the server
        :param address: The UDS control filename
        """
        try:
            os.unlink(address)
        except OSError:
            if os.path.exists(address):
                raise FileNotFoundError("Can't open UDS file")

        self._control_socket = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        self._control_socket.bind(address)

    class ServerNotRunning(Exception):
        """
        Exceptions the occurs when operating on a closed server
        """

    _control_socket = None
