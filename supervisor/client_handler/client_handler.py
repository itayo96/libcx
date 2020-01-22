from logging import getLogger
from .research_logger import get_logger
import struct

log = getLogger(__name__)


class ClientHandler:
    """
    Represents an handler for an executable (consists of multiple pid's)
    """

    def __init__(self, init_pid):
        log.info(f"Hand`ling a new client (initial pid is {init_pid})")
        get_logger("kaka").info("pipi")
        _init_pid = init_pid

    def libc_call_callback(self, pid: int, lib_call: int, data : bytes) -> None:
        """
        A callback function for the server to call to.
        """
        log.info(f"Called a libc method (pid={pid}, lib_call={lib_call}, data={data})")

        # Make sure len(data) fits the expected size of the specific lib_call
        log.info("Calloc params are {}".format(struct.unpack("qqQ", data)))

