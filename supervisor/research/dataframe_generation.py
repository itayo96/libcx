from logging import getLogger
import numpy as np
from pandas import DataFrame, read_csv
from sklearn.cluster import DBSCAN
import client_handler.message_structs
import research.files_dataset
import pickle
import math

log = getLogger(__name__)


def load_dataframe(path):
    return read_csv(path)


def generate_dataframe():
    log.info("Generating dataset...")
    process_datasets = generate_dataset()

    log.info("Merging similar processes...")
    datasets = merge_similar_processes(process_datasets.values())
    log.info(f"Merged into {len(datasets)} processes")

    log.info(f"Creating dataframe...")
    for records in datasets:
        record_list = []

        for file in records:
            writes_count_stats = file.calculate_write_count_stats()
            writes_size_stats = file.calculate_write_size_stats()
            fputss_stats = file.calculate_fputs_stats()

            record_list.append(
                {
                    "name": file.name,
                    "open_mode": file.open_mode,
                    "writes_amount": len(file.writes),
                    "writes_count_mean": writes_count_stats[0],
                    "writes_count_normal_stddev": writes_count_stats[1],
                    #"writes_count_skewness": writes_count_stats[2],
                    #"writes_count_kurtosis": writes_count_stats[3],
                    "writes_size_mean": writes_size_stats[0],
                    "writes_size_normal_stddev": writes_size_stats[1],
                    #"writes_size_skewness": writes_size_stats[2],
                    #"writes_size_kurtosis": writes_size_stats[3],
                    "fputss_mean": fputss_stats[0],
                    "fputss_normal_stddev": fputss_stats[1],
                    #"fputss_skewness": fputss_stats[2],
                    #"fputss_kurtosis": fputss_stats[3],
                })

        yield DataFrame(record_list)


def extract_process_dataset_features(process_dataset: research.files_dataset.dataset):
    scales = (1.5, 0.4)
    return math.log(process_dataset.get_number_of_files_opened()) * scales[0], math.log(
        process_dataset.get_run_duration().microseconds / 1000) * scales[1]


def merge_similar_processes(processes_datasets):
    d = np.array(list(map(extract_process_dataset_features, processes_datasets)))
    d_scaled = d  # preprocessing.scale(d)

    # plt.scatter(list(map(lambda a: a[0], d_scaled)), list(map(lambda a: a[1], d_scaled)))
    # plt.xlabel("log(number of files opened)")
    # plt.ylabel("log(run duration in milli)")
    # plt.show()

    dbscan = DBSCAN()
    dbscan.fit(d)

    data = {}

    for d, label in zip(processes_datasets, dbscan.labels_):
        if label not in data.keys():
            data[label] = []

        data[label].extend(list(map(lambda a: a["record"], d.closed_records)))
        data[label].extend(list(map(lambda a: a["record"], d.open_records)))

    return list(data.values())


def generate_dataset():
    datasets = {}
    file_db = open("db/file_report_db", 'rb')
    reports = []
    while True:
        try:
            reports.append(pickle.load(file_db))
        except EOFError:
            break

    reports.sort(key=lambda x: x["timestamp"])

    for a in reports:
        if a["pid"] not in datasets.keys():
            datasets[a["pid"]] = research.files_dataset.dataset()

        dataset = datasets[a["pid"]]
        dataset.add(load_report_object(a["lib_call"], a["data"]), a["timestamp"])

    file_db.close()

    log.info(f"Received datasets from {len(datasets)} processes")

    return datasets


def load_report_object(id, data):
    return client_handler.message_structs.reports[id].from_bytes(data)
