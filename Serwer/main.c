//
// Created by wojtek on 10.02.2020.
//
#include "tools.h"
#include "timers.h"

#include <unistd.h>

int main(int argc, char *argv[]) {
    setup();
    while (1) {
        pause();
        if (feeder_flag)
            feeder();
        if (reg_flag && !ring_flag)
            ring_registration();
    }
}