#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "devices.h"
#include "controller.h"

void run_controller(struct controller_state *controller)
{
    while (true) {
        if (controller->automatic == ON) {
            if (temperature_get() > controller->setpoint) {
                heater_set(OFF);
                controller->heater = OFF;
                fan_set(ON);
                controller->fan = ON;
            }
            else if (temperature_get() < controller->setpoint) {
                fan_set(OFF);
                controller->fan = OFF;
                heater_set(ON);
                controller->heater = ON;
            }
            else if (temperature_get() == controller->setpoint) {
                fan_set(OFF);
                controller->fan = OFF;
                heater_set(OFF);
                controller->heater = OFF;
            }
        }

        sleep(5);
    }
}

void set_controller_state(struct controller_state *controller, control control, int setting)
{
    switch (control) {
        case FAN:
            fan_set(setting);
            controller->fan = setting;
            break;
        case HEATER:
            heater_set(setting);
            controller->heater = setting;
            break;
        case SETPOINT:
            controller->setpoint = setting;
            break;
        case AUTO:
            controller->automatic = setting;
            break;
    }
}

char *get_controller_state(struct controller_state *controller, control control)
{
    char *response = malloc(20);

    switch (control) {
        case TEMP:
            snprintf(response, sizeof(response), "TEMP[%d]", temperature_get());
            break;
        case FAN:
            snprintf(response, sizeof(response), "FAN[%s]", controller->fan == ON ? "ON" : "OFF");
            break;
        case HEATER:
            snprintf(response, sizeof(response), "HEATER[%s]", controller->heater == ON ? "ON" : "OFF");
            break;
        case SETPOINT:
            snprintf(response, sizeof(response), "SETPOINT[%d]", controller->setpoint);
            break;
        case AUTO:
            snprintf(response, sizeof(response), "AUTO[%s]", controller->automatic == ON ? "ON" : "OFF");
            break;
    }

    return response;
}
