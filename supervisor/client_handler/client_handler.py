from logging import getLogger

log = getLogger(__name__)


class ClientHandler:
    """
    Represents an handler for an executable (consists of multiple pid's)
    """

    def __init__(self, init_pid):
        log.info(f"Hand`ling a new client (initial pid is {init_pid})")

    def libc_call_callback(self, pid: int, code: int) -> None:
        """
        A callback function for the server to call to.
        """
        log.info(f"Called a libc method (pid={pid}, code={code})")
