import pandas as pd
import numpy as np
import scipy as sp


def analyze(d: pd.DataFrame):
    # Ideas:
    # 1) Group file by folders, identify "unknown files", e.g. files we never seen before opened. Then, compare the
    #    files (same suffix, same prefix etc..)
    #
    # 2) Group by similar characteristics (e.g. mode and write stats). Then compare the folders. create a way to
    #    quantify how much they are different and then cluster it. if there are any small clusters, they might be
    #    suspicious.
    #
    # 3) Files that are accessed small amount of times.
    #
    # 4)
    #
    pass
