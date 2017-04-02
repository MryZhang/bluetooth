#ifndef CONTROLLER_H
#define CONTROLLER_H

#define ON 1
#define OFF 0

struct controller_state {
    int setpoint;
    int fan;
    int heater;
    int automatic;
};

typedef enum { FAN, HEATER, SETPOINT, AUTO, TEMP } control;

void run_controller(struct controller_state *controller);
void set_controller_state(struct controller_state *controller, control control, int setting);
char *get_controller_state(struct controller_state *controller, control control);

#endif
