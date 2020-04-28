from hydration import *

class calloc_report(Struct):
    nmemb = UInt64
    size = UInt64
    return_val = UInt64

class fopen_report(Struct):
    return_val = UInt64
    filename_len = UInt64()
    filename = Vector(length=filename_len)
    mode_len = UInt64()
    mode = Vector(length=mode_len)

class fclose_report(Struct):
    stream = UInt64
    return_val = UInt64