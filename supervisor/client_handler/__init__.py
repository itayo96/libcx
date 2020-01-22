from .client_handler import ClientHandler


class client_handler_wrapper:
    """
    Represents an handler for an executable (consists of multiple pid's)
    """

    def __init__(self, init_pid):
        self._handler = ClientHandler(init_pid)

    def libc_call_callback(self, pid: int, code: int) -> None:
        """
        A callback function for the server to call to.
        """
        self._handler.libc_call_callback(pid, code)


def create(pid) -> client_handler_wrapper:
    """
    A quick snippet to create a new client_handler object
    :return: The client_handler object
    """
    return client_handler_wrapper(pid)


def libc_call_callback(context: client_handler_wrapper, pid: int, code: int) -> None:
    """
    A quick snippet to create a new client_handler object
    :return: The client_handler object
    """
    context.libc_call_callback(pid, code)
