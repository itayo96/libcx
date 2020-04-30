from enum import Enum
import client_handler.message_structs
from time import time

class record:

    def __init__(self, open_time: float):
        self.open_time = open_time

    open_time: time
    close_time: time

    # The duration the file was open in seconds
    open_duration: float = 0

    # The open mode of the file, can be r/w/rw/a+ etc...
    open_mode: str = ""

    # The amount of write operations
    writes_count: int = 0
    reads_count: int = 0


class dataset:
    """
    This class holds the dataset for the files. you can add raw data to process it and to create
    standardized data records for each file 
    """

    def add(self, report, timestamp):
        if isinstance(report, client_handler.message_structs.fopen_report):
            self._handle_fopen(fd=report.return_val, time=timestamp)

        elif isinstance(report, client_handler.message_structs.fclose_report):
            self._handle_fclose(report.stream, timestamp)

        elif isinstance(report, client_handler.message_structs.fwrite_report):
            self._handle_fwrite(report.stream, timestamp)

        else:
            raise NotImplementedError()

        pass

    def _handle_fopen(self, fd, time):
        self._create_new_file_record(fd=fd, time=time)

    def _handle_fclose(self, fd, time):
        self._close_file_record(fd=fd, time=time)

    def _handle_fwrite(self, fd, time):
        r = self._find_open_file_record(fd)
        r.writes_count += 1

    def _find_open_file_record(self, fd) -> record:
        x = next(x for x in self.open_records if x["fd"] == fd)
        return x["record"]

    def _create_new_file_record(self, fd, time):
        self.open_records.append({
            "fd": fd,
            "record": record(open_time=time)
        })

    def _close_file_record(self, fd, time):
        x = next(x for x in self.open_records if x["fd"] == fd)
        self.open_records.remove(x)
        self.closed_records.append(x)
        x["record"].close_time = time

    closed_records = []
    open_records = []
