from .client_handler import ClientHandler


class ClientHandlerWrapper:
    """
    Represents an handler for an executable (consists of multiple pid's)
    """

    def __init__(self, *argc):
        self._handler = ClientHandler(*argc)

    def libc_call_callback(self, *argc) -> None:
        """
        A callback function for the server to call to.
        """
        self._handler.libc_call_callback(*argc)


def create(*argc) -> ClientHandlerWrapper:
    """
    A quick snippet to create a new client_handler object
    :return: The client_handler object
    """
    return ClientHandlerWrapper(*argc)


def libc_call_callback(context: ClientHandlerWrapper, *argc) -> None:
    """
    A quick snippet to create a new client_handler object
    :return: The client_handler object
    """
    context.libc_call_callback(*argc)
