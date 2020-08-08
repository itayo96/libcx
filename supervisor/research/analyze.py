from builtins import bytes

import pandas as pd
import numpy as np
import scipy as sp
import matplotlib.pyplot as plt
import os
import math

pd.options.display.width = 0


class analyze_data:
    def __init__(self):
        large_groups_stats = None
        files_not_in_large_group_stats = None

    @staticmethod
    def analyze(df):
        different_files_in_dir_threshold = 10

        x = analyze_data()
        df = df[df['name'].notnull()]

        cols_to_group_by = ['dir', 'open_mode', 'ext']
        cols_to_calc_stats = ['writes_amount', 'writes_count_mean', 'writes_count_normal_stddev',
                              'writes_count_skewness', 'writes_count_kurtosis', 'writes_size_mean',
                              'writes_size_normal_stddev', 'writes_size_skewness', 'writes_size_kurtosis',
                              'fputss_mean', 'fputss_normal_stddev', 'fputss_skewness', 'fputss_kurtosis']

        df.loc[:, "dir"] = df.copy()["name"].apply(os.path.dirname)
        df.loc[:, "ext"] = df.copy()["name"].apply(lambda x: os.path.splitext(x)[1])
        df.loc[:, "ext"] = df.copy()["name"].apply(lambda x: os.path.splitext(x)[1])

        group_counts = dirs_counts = df.groupby(cols_to_group_by)['dir'].count().reset_index(name="count")
        large_groups = dirs_counts[dirs_counts["count"] > different_files_in_dir_threshold]
        large_groups_files = pd.merge(df, large_groups)
        large_groups_stats = large_groups_files.groupby(cols_to_group_by)[cols_to_calc_stats].agg(['mean', 'std'])

        files_not_in_large_group = df[~df.name.isin(large_groups_files.name)]
        files_not_in_large_group_stats = files_not_in_large_group.groupby(["name", "open_mode"])[
            cols_to_calc_stats].agg(['mean', 'std'])

        x.large_groups_stats = large_groups_stats
        x.files_not_in_large_group_stats = files_not_in_large_group_stats

        return x

class analyzer:
    dirs_with_alot_of_files = None
    dirs_with_small_amount_of_files = None
    files_that_are_in_dirs_with_small_amount_of_files = None
    files_that_are_in_dirs_with_allot_of_files = None

    def __init__(self, d: pd.DataFrame):
        self.df = d

    def create_analyze_data(self):
        return analyze_data.analyze(self.df)

