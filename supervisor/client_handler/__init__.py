from .client_handler import client_handler


class client_handler_wrapper:
    """
    Represents an handler for an executable (consists of multiple pid's)
    """

    def __init__(self, init_pid):
        self._handler = client_handler(init_pid)

    def libc_call_callback(self, pid: int, code: int) -> None:
        """
        A callback function for the server to call to.
        """
        self._handler.libc_call_callback(pid, code)


async def create(pid) -> client_handler_wrapper:
    """
    A quick snippet to create a new client_handler object
    :return: The client_handler object
    """
    return client_handler_wrapper(pid)


async def libc_call_callback(context: client_handler_wrapper, pid: int, code: int) -> None:
    """
    A quick snippet to create a new client_handler object
    :return: The client_handler object
    """
    context.libc_call_callback(pid, code)
