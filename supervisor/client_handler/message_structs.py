from hydration import *

class calloc_report(Struct):
    nmemb = UInt64()
    size = UInt64()
    return_val = UInt64()

class fopen_report(Struct):
    return_val = UInt64() # the file descriptor for the new file
    filename_len = UInt64()
    filename = Vector(length=filename_len)
    mode_len = UInt64() 
    mode = Vector(length=mode_len) # the options the file was opened with (r/w/r+w etc..)

class fclose_report(Struct):
    stream = UInt64() # the file descriptor
    return_val = UInt64()

class fputc_report(Struct):
    stream = UInt64() # the file descriptor
    character = UInt32()
    return_val = UInt32() # on success - same as character

class fputs_report(Struct):
    stream = UInt64() # the file descriptor
    str_len = UInt32()
    return_val = UInt32() # on success - same as str_len

class fgetc_report(Struct):
    stream = UInt64() # the file descriptor
    return_val = UInt32() # the character read from the file

class fgets_report(Struct):
    stream = UInt64() # the file descriptor
    str_len = UInt32()
    n = UInt32()
    return_val = UInt64()

class fread_report(Struct):
    stream = UInt64() # the file descriptor
    ptr = UInt64() # pointer to buffer to read into
    size = UInt32()
    nmemb = UInt32()
    return_val = UInt32()

class fwrite_report(Struct):
    stream = UInt64() # the file descriptor
    ptr = UInt64() # pointer to buffer to write from
    size = UInt32()
    nmemb = UInt32()
    return_val = UInt32()

class feof_report(Struct):
    stream = UInt64() # the file descriptor
    return_val = UInt64()

class fseek_report(Struct):
    stream = UInt64() # the file descriptor
    offset = UInt64()
    whence = UInt32()
    return_val = UInt64()

class ftell_report(Struct):
    stream = UInt64() # the file descriptor
    return_val = UInt64()

class fprintf_report(Struct):
    stream = UInt64() # the file descriptor
    format_string_len = UInt64()
    format_string = Vector(length=format_string_len)
    return_val = UInt64()

class fscanf_report(Struct):
    stream = UInt64() # the file descriptor
    format_string_len = UInt64()
    format_string = Vector(length=format_string_len)
    return_val = UInt64()
