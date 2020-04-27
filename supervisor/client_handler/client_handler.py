from logging import getLogger
from .research_logger import get_logger
import struct

log = getLogger(__name__)

def decode_string(data):
    log.debug(len(data))
    string_length = struct.unpack("Q", data)[0]
    return data[8:(8 + string_length)].decode("unicode_escape"), string_length
    
def parse_fopen(data):
    return_value = struct.unpack("Q", data[:8])[0]
    filename, filename_length = decode_string(data[8:])
    mode, mode_length = decode_string(data[8+filename_length:])

    return {"filename": filename, "mode": mode, "return": return_value}


class ClientHandler:
    """
    Represents an handler for an executable (consists of multiple pid's)
    """
    formatters = {
        0: { # calloc
            "format": "qqQ",
            "vars": ["count", "size", "return"]
        },
        1: { # fopen
            "function": parse_fopen
        },
        2: { #fclose
            "format": "qQ",
            "vars": ["fd", "return"]
        },
    }

    def __init__(self, init_pid):
        log.info(f"Handling a new client (initial pid is {init_pid})")
        get_logger("kaka").info("pipi")
        _init_pid = init_pid

    def libc_call_callback(self, pid: int, lib_call: int, data : bytes) -> None:
        """
        A callback function for the server to call to.
        """
        #log.info(f"Called a libc method (pid={pid}, lib_call={lib_call}, data={data})")

        obj = None

        if "function" in self.formatters[lib_call].keys():
            obj = self.formatters[lib_call]["function"](data)
        else:
            # log.debug("formatting {} {}".format(data, self.formatters[lib_call]["format"]))
            values = struct.unpack(self.formatters[lib_call]["format"], data)

            obj = dict()
            for var, val in zip(self.formatters[lib_call]["vars"], values):
                obj[var] = val


        # Make sure len(data) fits the expected size of the specific lib_call
        log.info("The object is {}".format(obj))

