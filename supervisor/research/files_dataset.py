import scipy as sp
import scipy.stats
import numpy as np
from datetime import datetime
from logging import getLogger
from typing import Any, List
from research.utils import vector_to_string, generate_stats
import client_handler.message_structs
from time import time

log = getLogger(__name__)


class file_record:

    def __init__(self, open_time: float, name: str, open_mode: str):
        self.name = name
        self.open_time = open_time
        self.open_duration = 0
        self.open_mode = open_mode
        self.writes = []
        self.fputss = []

    open_time: time
    close_time: time

    # The duration the file was open in seconds
    open_duration: float

    # The open mode of the file, can be r/w/rw/a+ etc...
    open_mode: str

    def report_write(self, size, count, timestamp, return_value):
        self.writes.append({
            "size": size,
            "count": count,
        })
        pass

    def report_fputs(self, str_len, timestamp, return_value):
        self.fputss.append(str_len)
        pass

    def calculate_write_size_stats(self):
        if len(self.writes) == 0:
            return None, None, None, None

        data = list(map(lambda x: x["size"], self.writes))
        return generate_stats(data)

    def calculate_write_count_stats(self):
        if len(self.writes) == 0:
            return None, None, None, None

        data = list(map(lambda x: x["count"], self.writes))
        return generate_stats(data)

    def calculate_fputs_stats(self):
        if len(self.fputss) == 0:
            return None, None, None, None

        return generate_stats(self.fputss)


class dataset:
    """
    This class holds the dataset for the files. you can add raw data to process it and to create
    standardized data records for each file 
    """

    def __init__(self):
        self.start_time = None
        self.last_action_time = None
        self.closed_records = []
        self.open_records = []

    def add(self, report, timestamp):

        if self.start_time is None:
            self.start_time = timestamp
        self.last_action_time = timestamp

        if isinstance(report, client_handler.message_structs.fopen_report):
            self._handle_fopen(vector_to_string(report.filename), vector_to_string(report.mode), timestamp,
                               int(report.return_val))

        elif isinstance(report, client_handler.message_structs.fclose_report):
            self._handle_fclose(int(report.stream), timestamp)

        elif isinstance(report, client_handler.message_structs.fwrite_report):
            self._handle_fwrite(int(report.stream), int(report.size), int(report.nmemb), timestamp,
                                int(report.return_val))

        elif isinstance(report, client_handler.message_structs.fputs_report):
            self._handle_fputs(int(report.stream), int(report.str_len), timestamp, int(report.return_val))

        elif isinstance(report, client_handler.message_structs.fgets_report):
            self._handle_fgets(int(report.stream), timestamp, int(report.return_val))

        else:
            log.warning(f"Missing handler for message type {type(report)}")

        pass

    def get_run_duration(self):
        return self.last_action_time - self.start_time

    def get_number_of_files_opened(self):
        return len(self.open_records) + len(self.closed_records)

    def get_data(self) -> List[file_record]:
        return self.open_records + self.closed_records

    def _get_open_file_wrapped_record(self, fd: int) -> Any:
        try:
            return next(x for x in self.open_records if x["fd"] == fd)
        except StopIteration:
            log.warning("Received a reference to an unknown file, creating it")
            return self._create_new_file_record(fd=fd, timestamp=None, name=None, mode=None)

    def _get_open_file_record(self, fd: int) -> file_record:
        return self._get_open_file_wrapped_record(fd)["record"]

    def _create_new_file_record(self, fd: int, timestamp, name, mode):
        x = {
            "fd": fd,
            "record": file_record(open_time=timestamp, name=name, open_mode=mode)
        }

        self.open_records.append(x)
        log.debug(f"A new open file record was created")

        return x

    def _close_file_record(self, fd: int, timestamp: datetime):
        x = self._get_open_file_wrapped_record(fd)
        self.open_records.remove(x)
        self.closed_records.append(x)
        x["record"].close_time = timestamp

    # ---------- Handlers ---------------------

    def _handle_fopen(self, filename: str, mode: str, timestamp: datetime, return_value: int):
        self._create_new_file_record(fd=return_value, timestamp=timestamp, name=filename, mode=mode)
        log.info(f"Opened a new file (fd={return_value}, filename=\"{filename}\", mode=\"{mode}\"")

    def _handle_fclose(self, fd: int, timestamp: datetime):
        self._close_file_record(fd=fd, timestamp=timestamp)
        log.info(f"Closing the file {fd}")

    def _handle_fwrite(self, fd, size: int, count: int, timestamp: datetime, return_value: int):
        r = self._get_open_file_record(fd)
        log.info(f"Write operations for {size}x{count} (total: {size * count} bytes)")
        r.report_write(size, count, timestamp, return_value)

    def _handle_fputs(self, fd: int, str_len: int, timestamp: datetime, return_value: int):
        r = self._get_open_file_record(fd)
        log.info(f"puts operations for {str_len} chars")
        r.report_fputs(str_len, timestamp, return_value)

    def _handle_fgets(self, fd: int, timestamp: datetime, return_val: int):
        pass
