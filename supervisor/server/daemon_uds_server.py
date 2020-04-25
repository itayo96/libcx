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

    def __init__(self, address, register_client_callback, handle_libc_call_callback):
        self._register_client_callback = register_client_callback
        self._handle_libc_call_callback = handle_libc_call_callback

        self._loop = asyncio.get_event_loop()

        server = self._loop.run_until_complete(self._create_server(address))
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
            await self._handle_client(reader, writer)

        return asyncio.start_unix_server(foo, path=address, loop=self._loop)

    async def _handle_client(self, reader: asyncio.StreamReader, writer: asyncio.StreamWriter) -> None:
        pid = None
        context = None

        try:
            libc_call_code, pid, payload = await self._get_libc_call_report(reader)
            log.info(f"Received a message containing a libc call {libc_call_code} from pid {pid}")

            context = self._register_client_callback(pid)

            while True:
                self._handle_libc_call_callback(context, pid, libc_call_code, payload)
                libc_call_code, pid, payload = await self._get_libc_call_report(reader)
                log.info(f"Received a message containing a libc call {libc_call_code} from pid {pid}")

        except self.InvalidMessage:
            log.warning("Received invalid message, someone is cheating!", exc_info=True)
        except ConnectionAbortedError:
            log.info("Connection aborted from client")

    async def _get_libc_call_report(self, reader: asyncio.StreamReader) -> tuple:
        """
        :param reader: The StreamReader to read from
        :return: a tuple containing (libc_call_code, pid, payload)
        """
        size_field_size = 4
        remaining_header_size = 8
        entire_header_size = size_field_size + remaining_header_size

        size_stream = await reader.read(size_field_size)

        if len(size_stream) < size_field_size:
            raise ConnectionAbortedError(size_field_size, size_stream)

        entire_message_size = struct.unpack("I", size_stream)[0]

        if entire_message_size < entire_header_size:
            raise ConnectionAbortedError

        data = await reader.read(entire_message_size - size_field_size)

        if len(data) != entire_message_size - size_field_size:
            raise self.InvalidMessage()

        return struct.unpack("II", data[:remaining_header_size]) + (data[remaining_header_size:],)

    class InvalidMessage(Exception):
        """
        Base class for all message related errors
        """

        def __init__(self):
            self.message_opcode = 0

        def __init__(self, message_opcode: int):
            self.message_opcode = message_opcode
