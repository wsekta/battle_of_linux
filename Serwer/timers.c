//
// Created by wojtek on 10.02.2020.
//

#include "tools.h"
#include "timers.h"

#include <signal.h>

struct timespec ftots(double time) {
    struct timespec res = {(int) time, (time - (int) time) * 1000000000l};
    return res;
}

void create_timer(double time, int sig, void(*handler)()) {
    timer_t timerid;
    struct sigevent sev;
    struct itimerspec its;
    struct sigaction sa;

    sa.sa_flags = SA_SIGINFO;
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(sig, &sa, NULL) == -1)
        print_error("sigaction error");

    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = sig;
    sev.sigev_value.sival_ptr = &timerid;
    if (timer_create(CLOCK_REALTIME, &sev, &timerid) == -1)
        print_error("timer_create error");

    its.it_value = ftots(time);
    its.it_interval = its.it_value;
    if (timer_settime(timerid, 0, &its, NULL) == -1)
        print_error("timer_settime error");
}

void feed_handler() {
    feeder_flag = 1;
}

void registration_handler() {
    reg_flag = 1;
}