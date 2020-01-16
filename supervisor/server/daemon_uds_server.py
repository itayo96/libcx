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
            pid = (await self._get_connection(reader))[0]
            log.info(f"Received a new valid connection message from pid {pid}")

            context = await self._register_client_callback(pid)

            while True:
                pid, libc_call_code = await self._get_libc_call_report(reader)
                log.info(f"Received a message containing a libc call {libc_call_code} from pid {pid}")

                self._loop.create_task(self._handle_libc_call_callback(context, pid, libc_call_code))

        except self.InvalidMessage:
            log.warning("Received invalid message, someone is cheating!", exc_info=True)
        except ConnectionAbortedError:
            log.info("Connection aborted from client")

    async def _get_connection(self, reader: asyncio.StreamReader) -> tuple:
        """
        :param reader: The StreamReader to read from
        :return: a tuple containing (pid)
        """
        return await self._get_message(reader=reader, size=8, opcode=0x3001, message_format="I")

    async def _get_libc_call_report(self, reader: asyncio.StreamReader) -> tuple:
        """
        :param reader: The StreamReader to read from
        :return: a tuple containing (pid, libc_call_code)
        """
        return await self._get_message(reader=reader, size=12, opcode=0x1308, message_format="II")

    async def _get_message(self, reader: asyncio.StreamReader, size: int, opcode: int, message_format: str) -> tuple:
        data = await reader.read(size)

        if len(data) == 0:
            raise ConnectionAbortedError

        if len(data) != size:
            raise self.InvalidMessage()

        msg_opcode = struct.unpack("I", data[:4])[0]
        if msg_opcode != opcode:
            raise self.InvalidMessage(msg_opcode)

        return struct.unpack(message_format, data[4:])

    class InvalidMessage(Exception):
        """
        Base class for all message related errors
        """

        def __init__(self):
            self.message_opcode = 0

        def __init__(self, message_opcode: int):
            self.message_opcode = message_opcode
