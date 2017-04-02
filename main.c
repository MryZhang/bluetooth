#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include "bluetooth.h"
#include "devices.h"
#include "controller.h"

static void handle_response(struct bluetooth_connection *con, struct controller_state *controller, char *buffer);
static int index_of(char *s, char c);
void *bluetooth_server_process(void *device);
void *controller_process(void *controller);

typedef enum { SET, REQ } message_type;

struct message {
    const char *message;
    control control;
    message_type type;
    int setting;
};

struct message messages[] = {
    { "FAN", FAN },
    { "HEATER", HEATER },
    { "AUTO", AUTO },
    { "TEMP", TEMP },
    { "SETPOINT", SETPOINT },
};

static const char *responses[] = {
    "\r\nOK\r\n",
    "\r\nCONNECT",
    "\r\nNO CARRIER\r\n"
};

enum { OK, CONNECT, NOCARRIER };

int main(int argc, char *argv[])
{
    if (argc == 1) {
        printf("Usage: %s [device]\n\n", argv[0]);
        return 1;
    }

    struct controller_state controller = { 21, OFF, OFF, ON };

    void *bluetooth_server_args[] = { argv[1], &controller };
    pthread_t bluetooth_server_thread;
    pthread_create(&bluetooth_server_thread, NULL, bluetooth_server_process, bluetooth_server_args);

    pthread_t controller_thread;
    pthread_create(&controller_thread, NULL, controller_process, (void *)&controller);

    pthread_exit(NULL);
}

void *bluetooth_server_process(void *args)
{
    // This could probably be nicer...
    char *device = *((void **)args);
    struct controller_state *controller = *((void **)args + 1);

    struct bluetooth_connection connection;
    char buffer[100];

    if (bluetooth_init(&connection, device) > 0) {
        pthread_exit(NULL);
    }

    printf("Starting Bluetooth server\n");
    bluetooth_server_start(&connection);
    bluetooth_read(&connection, buffer, sizeof(buffer));

    if (strncmp(buffer, responses[OK], strlen(responses[OK])) != 0) {
        printf("There was a problem starting the Bluetooth server\n");
        return 0;
    }

    while (true) {
        if (bluetooth_read(&connection, buffer, sizeof(buffer) > 0)) {
            handle_response(&connection, controller, buffer);
        }
        sleep(1);
    }

    bluetooth_close(&connection);
    pthread_exit(NULL);
}

void *controller_process(void *controller)
{
    run_controller(controller);
    pthread_exit(NULL);
}

static void decode_message(struct message *message, char *buffer)
{
    for (int i = 0; i < sizeof(messages) / sizeof(messages[0]); i++) {
        if (strncmp(buffer, messages[i].message, strlen(messages[i].message)) == 0) {
            message->control = messages[i].control;
            break;
        }
    }

    int beginning = index_of(buffer, '[');
    int end = index_of(buffer, ']');

    if (end - beginning == 1) {
        message->type = REQ;
    }
    else {
        message->type = SET;
        *(buffer + end) = '\0';

        if (message->control == SETPOINT) {
            message->setting = atoi(buffer + beginning + 1);
        }
        else {
            if (strncmp(buffer + beginning + 1, "ON", strlen("ON")) == 0) {
                message->setting = ON;
            }
            else {
                message->setting = OFF;
            }
        }
    }
}

static void handle_response(struct bluetooth_connection *con, struct controller_state *controller, char *buffer)
{
    // TODO: Handle cases for disconnects and whatnot
    struct message message;
    decode_message(&message, buffer);

    if (message.type == SET) {
        set_controller_state(controller, message.control, message.setting);
        bluetooth_send(con, buffer);
    }
    else {
        char *response = get_controller_state(controller, message.control);
        bluetooth_send(con, response);
        free(response);
    }
    return;
}

static int index_of(char *s, char c)
{
    int i;
    char *d;

    for (i = 0, d = s; *d != '\0'; i++, d++) {
        if (*d == c) {
            return i;
        }
    }
    return -1;
}
