from hydration import Vector
import numpy as np
import scipy as sp
import scipy.stats


def vector_to_string(vector: Vector) -> str:
    return ''.join(chr(i) for i in vector.value)


def generate_stats(data):
    mean = np.mean(data)
    normal_stddev = np.std(data) / mean
    skewness = sp.stats.skew(data)
    kurtosis = sp.stats.kurtosis(data)

    return mean, normal_stddev, skewness, kurtosis
