//
// Created by wojtek on 12.02.2020.
//
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>

int ring_flag;
int fo_fd;
int child_no;

char fi_path[255];
char fo_path[255];

struct timespec wait_time;

void print_error(const char *msg) {
    if (!errno)
        errno = -1;
    perror(msg);
    exit(-1);
}

void setup();

void make_child();

void try_to_register();

int main() {
    setup();
    while (1) {
        if (ring_flag) {
            //TODO
        } else {
            char c[1];
            if (read(fo_fd, c, 1))
                make_child();
            if(child_no >= 3)
                try_to_register();
        }
        nanosleep(&wait_time, NULL);
    }
}

void setup() {
    child_no = 0;
    ring_flag = 0;
    char *camp = getenv("CAMP");
    if (!camp)
        print_error("environment variable(s) not defined");
    wait_time.tv_nsec = 10;
    wait_time.tv_sec = 0;

    strcat(fi_path, camp);
    strcat(fi_path, "FI");
    strcat(fo_path, camp);
    strcat(fo_path, "FO");

    while (1) {
        fo_fd = open(fo_path, O_RDONLY | O_NDELAY | O_NONBLOCK);
        if (fo_fd == -1) {
            if (errno != ENOENT)
                print_error("open fifo error");
            else
                nanosleep(&wait_time, NULL);
        } else
            break;
    }
}

void make_child() {
    //TODO
    ++child_no;
}

void try_to_register()
{
    int fi_fd = open(fi_path, O_WRONLY | O_NDELAY | O_NONBLOCK);
    if(fi_fd == -1 && errno != ENXIO)
        print_error("fi open error");
    else if(fi_fd == -1 && errno == ENXIO)
        return;
    //TODO
    //printf("zapisane\n");
    close(fi_fd);
}