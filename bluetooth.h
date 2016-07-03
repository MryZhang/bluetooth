#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <termios.h>

struct bluetooth_connection {
    int fd;
    struct termios config;
};

int bluetooth_init(struct bluetooth_connection *con, const char *device);
void bluetooth_server_start(struct bluetooth_connection *con);
void bluetooth_server_stop(struct bluetooth_connection *con);
int bluetooth_send(struct bluetooth_connection *con, const char *str);
int bluetooth_read(struct bluetooth_connection *con, char *buffer, size_t size);
void bluetooth_pin_get(struct bluetooth_connection *con);
void bluetooth_pin_set(struct bluetooth_connection *con, const char *pin);
void bluetooth_close(struct bluetooth_connection *con);

#endif
