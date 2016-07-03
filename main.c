#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bluetooth.h"

int main(int argc, char* argv[])
{
    struct bluetooth_connection connection;
    char buffer[100];

    if (argc == 1) {
        printf("Usage: %s [device]\n\n", argv[0]);
        return 1;
    }

    printf("Opening %s\n", argv[1]);

    if (bluetooth_init(&connection, argv[1]) > 0) {
        return 1;
    };

    bluetooth_send(&connection, "ATI\r");
    bluetooth_read(&connection, buffer, sizeof(buffer));
    printf("%s", buffer);
    bluetooth_close(&connection);

    return 0;
}
