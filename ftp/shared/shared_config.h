#ifndef SHARED_CONFIG_H
#define SHARED_CONFIG_H

static constexpr int DEFAULT_PORT = 12321;

static constexpr int FRAGMENT_SIZE = 1024;

static constexpr int MAX_PATH_LEN = 128;

static constexpr int MAX_FILE_SIZE = 1024 * 1024 * 10; // 10 MB

static constexpr struct timeval TIMEOUT = {
    .tv_sec = 10,
    .tv_usec = 0,
};

#endif // SHARED_CONFIG_H