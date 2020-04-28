from logging import getLogger
from .research_logger import get_logger
import struct
from .message_structs import *

log = getLogger(__name__)

class ClientHandler:
    """
    Dictionary of reports for all supported lib calls
    """
    reports = {
        0 : calloc_report,
        1 : fopen_report,
        2 : fclose_report,
    }

    def __init__(self, init_pid):
        log.info(f"Handling a new client (initial pid is {init_pid})")
        get_logger("kaka").info("pipi")
        _init_pid = init_pid

    def libc_call_callback(self, pid: int, lib_call: int, data : bytes) -> None:
        """
        A callback function for the server to call to.
        """
        log.info(f"Called a libc method (pid={pid}, lib_call={lib_call}, data={data})")

        report = self.reports[lib_call].from_bytes(data)
        print(report)

