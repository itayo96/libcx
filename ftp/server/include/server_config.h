#ifndef SERVER_CONFIG_H
#define SERVER_CONFIG_H

static constexpr int DEFAULT_PORT = 12321;

static constexpr int MAX_CONNECTIONS = 5;

static std::string ROOT_DIR;

static constexpr struct timeval TIMEOUT = {
    .tv_sec = 10,
    .tv_usec = 0,
};

#endif // SERVER_CONFIG_H