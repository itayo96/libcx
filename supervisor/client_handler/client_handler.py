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
            0: (self.handle_calloc, 16),
            1: (self.handle_malloc, 12),
            2: (self.handle_free, 8),
            3: (self.handle_memset, 21)
        }

        self.heap_range = range(0)
        self.allocated_ranges = list()

    @staticmethod
    def maximize_ranges(*ranges : range) -> range:
        return range(min(r.start for r in ranges), max(r.stop for r in ranges))

    def in_allocated_heap(addr : int) -> bool:
        return any(addr in r for r in self.allocated_ranges)

    def handle_memory_allocation(self, start_addr : int, allocation_size : int) -> None:
        current_range = range(start_addr, start_addr + allocation_size)
        self.heap_range = self.maximize_ranges(self.heap_range, current_range)
        self.allocated_ranges.append(current_range)

    def handle_memory_freeing(self, free_addr : int) -> None:
        if free_addr not in (r.start for r in self.allocated_ranges):
            log.error(f"Tried freeing memory which is not allocated {free_addr}, {self.allocated_ranges}")

        # Remvoe from allocated ranges
        self.allocated_ranges = [r for r in self.allocated_ranges if r.start != free_addr]

    def check_memory_access(self, access_range : range):
        ends = [access_range.start, access_range.end]

        if any(end in self.heap_range for end in ends):
            if not all(self.in_allocated_heap(end) for end in ends):
                log.error(f"Accessing non allocated heap address {access_range}, {ends}")

    def handle_malloc(self, data : bytes) -> None:
        size, start_addr = struct.unpack("qQ", data)
        size, start_addr = struct.unpack("IQ", data)
        self.handle_memory_allocation(start_addr, size)

    def handle_calloc(self, data : bytes) -> None:
        nmemb, size, start_addr = struct.unpack("qqQ", data)
        nmemb, size, start_addr = struct.unpack("IIQ", data)
        self.handle_memory_allocation(start_addr, nmemb * size)

    def handle_free(self, data : bytes) -> None:
        addr = struct.unpack('Q', data)
        self.handle_memory_freeing(addr)

    def handle_memset(self, data : bytes) -> None:
        source, value, length, return_addr = struct.unpack("QbIQ", data)
        self.check_memory_access(range(source, source + length))

    def libc_call_callback(self, pid: int, lib_call: int, data : bytes) -> None:
        """
        A callback function for the server to call to.
        """
        log.info(f"Called a libc method (pid={pid}, lib_call={lib_call}, data={data})")

        callback, expected_length = self._lib_calls_table[lib_call]
        if len(data) != expected_length:
            log.error(f"Expected {expected_length} bytes but got {len(data)}")
            return

        callback(data)       

