from logging import getLogger
from .research_logger import get_logger
import struct

log = getLogger(__name__)


class ClientHandler:
    """
    Represents an handler for an executable (consists of multiple pid's)
    """

    def __init__(self, init_pid):
        log.info(f"Hand`ling a new client (initial pid is {init_pid})")
        get_logger("kaka").info("pipi")
        self._init_pid = init_pid

        self._lib_calls_table = {
            0: (handle_calloc, 12),
            1: (handle_malloc, 8)
        }

        self.heap_range = range(0)
        self.allocated_ranges = list()

    @staticmethod
    def maximize_ranges(*ranges):
        return range(min(r.start for r in ranges), max(r.stop for r in ranges))

    def handle_memory_allocation(self, start_addr : int, allocation_size : int) -> None:
        current_range = range(start_addr, start_addr + allocation_size)
        self.heap_range = self.maximize_ranges(self.heap_range, current_range)
        self.allocated_ranges.append(current_range)

    def handle_memory_freeing(self, free_addr : int) -> None:
        if free_addr not in (r.start for r in self.allocated_ranges):
            log.error(f"This is what we were trying to catch")

        # Remvoe from allocated ranges
        self.allocated_ranges = [r for r in self.allocated_ranges if r.start != free_addr]

    def handle_malloc(self, data : bytes) -> None:
        size, start_addr = struct.unpack("qQ", data)
        self.handle_memory_allocation(start_addr, size)

    def handle_calloc(self, data : bytes) -> None:
        nmemb, size, start_addr = struct.unpack("qqQ", data)
        self.handle_memory_allocation(start_addr, nmemb * size)
        

    def libc_call_callback(self, pid: int, lib_call: int, data : bytes) -> None:
        """
        A callback function for the server to call to.
        """
        log.info(f"Called a libc method (pid={pid}, lib_call={lib_call}, data={data})")

        callback, expected_length = self._lib_calls_table[lib_call]
        if len(data) != expected_length:
            log.error(f"Expected {expected_length} bytes but got {len(data)}")
            return

        self.callback(data)       

