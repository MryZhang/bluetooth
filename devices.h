#ifndef DEVICES_H
#define DEVICES_H

int temperature_get();
void fan_set(unsigned int state);
void heater_set(unsigned int state);

#endif
