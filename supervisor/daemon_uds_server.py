from logging import getLogger
import os
import asyncio
import struct

log = getLogger(__name__)


class DaemonUDSServer:
    """
    An UDS based server for the agents to communicate with. This class handles the communication and not the actual
    logic.
    """

    def __init__(self, address, register_client_callback):
        self._register_client_callback = register_client_callback

        self._loop = asyncio.get_event_loop()
        server_coro = self._create_server(address)
        server = self._loop.run_until_complete(server_coro)

        self._loop.run_forever()

        server.close()
        self._loop.run_until_complete(server.wait_closed())
        self._loop.close()

    def _create_server(self, address: str) -> asyncio.coroutine:
        try:
            os.unlink(address)
        except OSError:
            if os.path.exists(address):
                raise FileNotFoundError("Can't open UDS file")

        # self._control_socket = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        # self._control_socket.bind(address)
        async def foo(reader, writer):
            await self._handle_new_client(reader, writer)

        return asyncio.start_unix_server(foo, path=address, loop=self._loop)

    async def _handle_new_client(self, reader: asyncio.StreamReader, writer: asyncio.StreamWriter) -> None:
        pid = None

        try:
            pid = await self._read_connection_message(reader)
        except self.InvalidClientConnectionMessage:
            log.warning("Received invalid message, someone is cheating!", exc_info=True)

        log.info(f"Received a new valid connection message from pid {pid}")

        await self._register_client_callback(pid)

    async def _read_connection_message(self, reader: asyncio.StreamReader) -> int:
        connection_message_size = 8
        connection_message_opcode = 0x3001

        opcode, pid = struct.unpack("II", await reader.read(connection_message_size))

        if opcode != connection_message_opcode:
            raise self.InvalidClientConnectionMessage

        return pid

    class InvalidClientConnectionMessage(Exception):
        """
        Invalid client connection messaged received
        """
