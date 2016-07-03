#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include "bluetooth.h"

const char *bluetooth_commands[] =
{
    "AT+BTSRV=1\r",
    "AT+BTPIN",
    "+++",
};

enum { INIT, PIN, ESCAPE };

int bluetooth_init(struct bluetooth_connection *con, const char *device)
{
    con->fd = open(device, O_RDWR | O_NOCTTY);

    if (con->fd == -1) {
        perror("Opening device");
        return 1;
    }

    if (tcgetattr(con->fd, &con->config) < 0) {
        perror("Getting configuration");
        return 1;
    }

    // TODO: Do this properly, for now set all flags to zero
    con->config.c_iflag = 0;
    con->config.c_oflag = 0;
    con->config.c_lflag = 0;
    con->config.c_cflag = 0;

    // Set flags we need
    con->config.c_cflag = B115200 | CS8 | CREAD;

    // No minimum characters, time out after 0.1 seconds
    con->config.c_cc[VMIN] = 0;
    con->config.c_cc[VTIME] = 1;

    // Apply configuration
    if (tcsetattr(con->fd, TCSANOW, &con->config) < 0) {
        perror("Setting configuration");
        return 1;
    }

    return 0;
}

int bluetooth_send(struct bluetooth_connection *con, const char *str)
{
    return write(con->fd, str, strlen(str));
}

int bluetooth_read(struct bluetooth_connection *con, char *buffer, size_t size)
{
    int total, count = 0;

    // Keep reading until a timeout
    while ((count = read(con->fd, buffer + total, size)) > 0) {
        total += count;
        size -= count; // Update remaining characters in buffer
    }

    buffer[total] = '\0';
    return total;
}

void bluetooth_pin_get(struct bluetooth_connection *con)
{
    char buffer[20];
    snprintf(buffer, sizeof(buffer), "%s\r", bluetooth_commands[PIN]);
    bluetooth_send(con, buffer);
}

void bluetooth_pin_set(struct bluetooth_connection *con, const char *pin)
{
    char buffer[20];
    snprintf(buffer, sizeof(buffer), "%s=\"%s\"\r", bluetooth_commands[PIN], pin);
    bluetooth_send(con, buffer);
}

void bluetooth_server_start(struct bluetooth_connection *con)
{
    bluetooth_send(con, bluetooth_commands[INIT]);
}

void bluetooth_server_stop(struct bluetooth_connection *con)
{
    bluetooth_send(con, bluetooth_commands[ESCAPE]);
}

void bluetooth_close(struct bluetooth_connection *con)
{
    close(con->fd);
}
