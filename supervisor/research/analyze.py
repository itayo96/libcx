from builtins import bytes

import pandas as pd
import numpy as np
import scipy as sp
import scipy.stats
import matplotlib.pyplot as plt
import os
import math

pd.options.display.width = 0


class analyze_data:
    large_groups_stats = None
    files_not_in_large_group_stats = None
    groups = []
    cols_to_calc_stats = ['writes_amount', 'writes_count_mean', 'writes_count_normal_stddev', 'writes_size_mean',
                          'writes_size_normal_stddev', 'fputss_mean', 'fputss_normal_stddev']

    @staticmethod
    def analyze(df):
        x = analyze_data()
        x.load(df)

        return x

    def load(self, df):
        different_files_in_dir_threshold = 10
        df = df[df['name'].notnull()]

        cols_to_group_by = ['dir', 'open_mode', 'ext']

        df.loc[:, "dir"] = df.copy()["name"].apply(os.path.dirname)
        df.loc[:, "ext"] = df.copy()["name"].apply(lambda x: os.path.splitext(x)[1])

        group_counts = dirs_counts = df.groupby(cols_to_group_by)['dir'].count().reset_index(name="count")
        large_groups = dirs_counts[dirs_counts["count"] > different_files_in_dir_threshold]
        large_groups_files = pd.merge(df, large_groups)
        large_groups_stats = large_groups_files.groupby(cols_to_group_by)[self.cols_to_calc_stats].agg(['mean', 'std'])

        files_not_in_large_group = df[~df.name.isin(large_groups_files.name)]
        files_not_in_large_group_stats = files_not_in_large_group.groupby(["name", "open_mode"])[self.cols_to_calc_stats].agg(['mean', 'std'])

        self.large_groups_stats = large_groups_stats
        self.files_not_in_large_group_stats = files_not_in_large_group_stats
        self.large_groups_files = large_groups_files

    def detect(self, df):
        df = df[df['name'].notnull()]
        df.loc[:, "score"] = 0
        df.loc[:, "dir"] = df.copy()["name"].apply(os.path.dirname)
        df.loc[:, "ext"] = df.copy()["name"].apply(lambda x: os.path.splitext(x)[1])

        files_in_groups = pd.merge(df, self.large_groups_stats, on=["dir", "open_mode", "ext"], how="inner")
        known_files_not_in_large_group_stats = pd.merge(df, self.files_not_in_large_group_stats, on=["name", "open_mode"], how="inner")
        unkown_files = pd.merge

        unknown_files = pd.merge(df, known_files_not_in_large_group_stats[["name", "open_mode"]], on=["name", "open_mode"], how="outer", indicator=True)
        unknown_files = unknown_files[unknown_files["_merge"] == "left_only"]
        unknown_files.columns = unknown_files.columns.get_level_values(0)
        unknown_files = unknown_files.drop("_merge", axis=1)

        unknown_files = pd.merge(unknown_files, files_in_groups[["name", "open_mode"]], on=["name", "open_mode"], how="outer", indicator=True)
        unknown_files = unknown_files[unknown_files["_merge"] == "left_only"]
        unknown_files.columns = unknown_files.columns.get_level_values(0)
        unknown_files = unknown_files.drop("_merge", axis=1)

        known_files = files_in_groups.append(known_files_not_in_large_group_stats)

        known_files["score"] = known_files.apply(lambda r: self.get_score(r), axis=1)

        pass

    def get_score(self, r):
        penalty_std_zero = 2
        penalty_new_type_of_usage = 2
        penalty_strange_property_max = 1

        score = 0

        for c in self.cols_to_calc_stats:
            mean, std = r[(c, "mean")], r[(c, "std")]

            if not math.isnan(std) and not math.isnan(mean):

                # If std==0 there are no difference in all the samples, and it must be the mean
                if std == 0:
                    if r[c] != mean:
                        score += penalty_std_zero
                else:
                    cdf = self.get_norm(r[(c, "mean")], r[(c, "std")]).cdf(r[c])
                    score += math.pow(np.abs(0.5 - cdf) * 2, 3) * penalty_strange_property_max
            else:
                if not math.isnan(r[c]):
                    score += penalty_new_type_of_usage

        return score

    norms = {}
    def get_norm(self, mean, stddev):
        if not (mean, stddev) in self.norms:
            self.norms[(mean, stddev)] = sp.stats.norm(mean, stddev)
        return self.norms[(mean, stddev)]

class analyzer:
    dirs_with_alot_of_files = None
    dirs_with_small_amount_of_files = None
    files_that_are_in_dirs_with_small_amount_of_files = None
    files_that_are_in_dirs_with_allot_of_files = None

    def __init__(self, d: pd.DataFrame):
        self.df = d

    def create_analyze_data(self):
        return analyze_data.analyze(self.df)
