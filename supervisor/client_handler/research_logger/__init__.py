import logging
import os
from datetime import datetime



class DynamicModuleFileHandler(object):

    def __init__(self, folder, logger, handler_factory, **kw):
        kw['filename'] = os.path.join(folder, logger.name)
        self._handler = handler_factory(**kw)

    def __getattr__(self, n):
        if hasattr(self._handler, n):
            return getattr(self._handler, n)
        raise AttributeError(n)


def get_logger(process_name: str):
    now = datetime.now()
    current_time = now.strftime("%Y_%m_%d-%H_%M_%S")

    research_logger = logging.getLogger("research_logs." + process_name + "_" + current_time + ".log")
    research_logger.setLevel(logging.DEBUG)

    research_handler = DynamicModuleFileHandler(os.curdir, research_logger, logging.FileHandler)
    formatter = logging.Formatter('%(asctime)s - %(name)s - %(message)s')
    research_handler.setFormatter(formatter)
    research_logger.addHandler(research_handler)

    return research_logger
