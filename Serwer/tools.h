//
// Created by wojtek on 10.02.2020.
//

#ifndef BATTLE_OF_LINUX_TOOL_H
#define BATTLE_OF_LINUX_TOOL_H

struct registration_info{
    int pid;
    int chld_pgid;
} reg[2];

char fi_path[255];
char fo_path[255];

double recruit_dly;
long recruit_max;
double ring_reg;

int ring_flag;
int arbiter_pid;
int fi_fd;

void print_error(const char * msg);

void setup();

void feeder();

void ring_registration();

#endif //BATTLE_OF_LINUX_TOOL_H
