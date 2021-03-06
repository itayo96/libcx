from logging import getLogger
from .message_structs import *
import pickle
from datetime import datetime

log = getLogger(__name__)


def dump_memory_reports(pid: int, lib_call: int, report):
    raise NotImplementedError


def dump_file_operations_reports(pid: int, lib_call: int, report):
    """
    Parse reports that are related to file calls - PICKLE them!
    """
    file_db = open("db/file_report_db", 'ab')
    log.debug(f"Writing to db/file_report_dp {report}")
    pickle.dump({
        "timestamp": datetime.now(),
        "pid": pid,
        "lib_call": lib_call,
        "data": bytes(report)
    }, file_db)  # tuple (timestamp, pid, libcall, report object as bytes)
    file_db.close()

    """
    Usage example for unpicking the db:

        file_db = open("db/file_report_db", 'rb')
        timestamp1, pid1, libcall1, data1 = pickle.load(file_db)
        timestamp2, pid2, libcall2, data2 = pickle.load(file_db)
        file_db.close()

        report1 = ClientHandler.reports[libcall1].from_bytes(data1)
        report2 = ClientHandler.reports[libcall2].from_bytes(data2)
        print(report1)
        print(report2)
    
    If you iterate over the pickle file, make sure to wrap with try-except and break on "EOFError" or "UnpicklingError",
    that will indicate the you reached the end of the file.

    Note: using pickle.load() will NOT delete the items you read from the db
    """


class ClientHandler:
    """
    Dictionary of reports for all supported lib calls
    """

    """
    Dictionary of report parsers
    """
    report_parsers = {
        'memory': {
            'reports': [0],
            'parser':  dump_memory_reports
        },

        'files': {
            'reports': [*range(1, 14)],
            'parser': dump_file_operations_reports
        }
    }

    def __init__(self, init_pid):
        log.info(f"Handling a new client (initial pid is {init_pid})")
        _init_pid = init_pid

    def libc_call_callback(self, pid: int, lib_call: int, data: bytes) -> None:
        """
        A callback function for the server to call to.
        """
        log.info(f"Called a libc method pid={pid}, lib_call={lib_call} data={data.hex()}")

        # create a report object for the lib call
        if lib_call not in reports:
            raise NotImplementedError()
        report = reports[lib_call].from_bytes(data)

        # parse the report according to it's type
        for parser in self.report_parsers.values():
            if lib_call in parser['reports']:
                parser['parser'](pid, lib_call, report)
