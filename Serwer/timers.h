//
// Created by wojtek on 10.02.2020.
//

#ifndef BATTLE_OF_LINUX_TIMERS_H
#define BATTLE_OF_LINUX_TIMERS_H

#include <time.h>

int feeder_flag;
int reg_flag;

struct timespec ftots(double time);
void create_timer(double time,int sig, void(*handler)());
void feed_handler();
void registration_handler();

#endif //BATTLE_OF_LINUX_TIMERS_H
