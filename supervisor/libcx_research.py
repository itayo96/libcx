import sys
import glob
import research.dataframe_generation
import  research.analyze
from logging import getLogger, ERROR
from utils.logging import setup_logging


log = getLogger(__name__)


def main():
    setup_logging()
    getLogger("research.files_dataset").setLevel(ERROR)
    getLogger("matplotlib.font_manager").setLevel(ERROR)

    if sys.argv[1] == "dataframe":
        generate_dataframe()
    elif sys.argv[2] == "analyze":
        analyze()
    else:
        log.error("Unknown parameter: [dataframe/analyze]")


def analyze():
    files = glob.glob("research/dataframe*.csv")
    for d in map(lambda f: research.dataframe_generation.load_dataframe(f), files):
        research.analyze.analyze(d)

    pass


def generate_dataframe():
    log.info("Starting dataframe generation process...")
    num = 0
    for d in generate_dataframe():
        path = f"db/dataframe{num}.csv"
        log.info(f"Saving into {path}")
        d.to_csv(path)
        num += 1
    log.info("Done")


if __name__ == '__main__':
    main()
