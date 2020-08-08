import sys
import glob
import research.dataframe_generation
import  research.analyze
from logging import getLogger, ERROR
from utils.logging import setup_logging
import pickle
import datetime

log = getLogger(__name__)


def main():
    setup_logging()
    getLogger("research.files_dataset").setLevel(ERROR)
    getLogger("matplotlib.font_manager").setLevel(ERROR)

    if sys.argv[1] == "dataframe":
        generate_dataframe()
    elif sys.argv[1] == "analyze":
        analyze()
    else:
        log.error("Unknown parameter: [dataframe/analyze]")


def analyze():
    files = glob.glob("db/*.csv")
    _ = map(lambda f: research.dataframe_generation.load_dataframe(f), files)

    a = research.analyze.analyzer(next(_)).create_analyze_data()
    save_analyze_data(a, f"db/analyze_data.anlzdat")

    pass


def find_anomalies():
    files = glob.glob("db/*.csv")
    _ = map(lambda f: research.dataframe_generation.load_dataframe(f), files)

    load_analyze_data("db/analyze_data.anlzdat").find_anomalies(next(_))

    pass


def generate_dataframe():
    log.info("Starting dataframe generation process...")
    num = 0
    for d in research.dataframe_generation.generate_dataframe():
        path = f"db/dataframe{num}.csv"
        log.info(f"Saving into {path}")
        d.to_csv(path)
        num += 1
    log.info("Done")


def save_analyze_data(d, filename):
    f = open(filename, 'w')
    pickle.dump(d, f)
    f.close()


def load_analyze_data(filename):
    f = open(filename, 'r')
    d = pickle.load(f)
    f.close()

    return d

if __name__ == '__main__':
    main()
