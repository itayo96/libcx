from logging import getLogger
from .research_logger import get_logger

log = getLogger(__name__)


class ClientHandler:
    """
    Represents an handler for an executable (consists of multiple pid's)
    """

    def __init__(self, init_pid):
        log.info(f"Hand`ling a new client (initial pid is {init_pid})")
        get_logger("kaka").info("pipi")

    def libc_call_callback(self, pid: int, code: int) -> None:
        """
        A callback function for the server to call to.
        """
        log.info(f"Called a libc method (pid={pid}, code={code})")
