from logging import getLogger
import client_handler.message_structs
from utils.logging import setup_logging
import research.files_dataset
import pickle

log = getLogger(__name__)


def main():
    setup_logging()
    dataset = generate_dataset()
    pass


def generate_dataset():
    dataset = research.files_dataset.dataset()
    file_db = open("db/file_report_db", 'rb')
    reports = []
    while True:
        try:
            x = pickle.load(file_db)
            dataset.add(load_report_object(x[2], x[3]), x[0])
        except EOFError:
            break
    file_db.close()
    return dataset


def load_report_object(id, data):
    return client_handler.message_structs.reports[id].from_bytes(data)


if __name__ == '__main__':
    main()
