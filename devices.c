#include <stdio.h>
#include <stdlib.h>
#include "devices.h"

int temperature_get()
{
    return (rand() % 30) + 10;
}

void fan_set(unsigned int state)
{
    printf("Setting fan to %d\n", state);
}

void heater_set(unsigned int state)
{
    printf("Setting heater to %d\n", state);
}
