//
// Created by wojtek on 11.02.2020.
//
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>

int active_flag;
int totem_fd;

char totem_path[255];

double totem_op;
double totem_cl;

void print_error(const char *msg) {
    if (!errno)
        errno = -1;
    perror(msg);
    exit(-1);
}

void handler() {
    active_flag = !active_flag;
}

void setup();

struct timespec ftots(double time);

int main() {
    setup();
    while (1) {
        if (active_flag) {
            totem_fd = open(totem_path, O_RDONLY | O_NDELAY);
            if (totem_fd == -1)
                print_error("open fifo error");
            struct timespec time = ftots(totem_op);
          //printf("totem otwarty\n");
            nanosleep(&time, NULL);

            if (!active_flag)
                continue;

            if (close(totem_fd))
                print_error("close fifo error");
            time = ftots(totem_cl);
          //printf("totem zamkniety\n");
            nanosleep(&time, NULL);
        } else {
            if (fcntl(totem_fd, F_GETFD) != -1 || errno != EBADF)
                close(totem_fd);
          //printf("wakacje\n");
            pause();
        }
    }
}

void setup() {
    char *camp = getenv("CAMP");
    char *totem_op_str = getenv("TOTEM_OP");
    char *totem_cl_str = getenv("TOTEM_CL");
    if (!camp || !totem_op_str || !totem_cl_str)
        print_error("environment variable(s) not defined");

    char *tmp;
    totem_op = strtod(totem_op_str, &tmp);
    if (*tmp)
        print_error("wrong environment variable RECRUIT_DLY");
    totem_cl = strtod(totem_cl_str, &tmp);
    if (*tmp)
        print_error("wrong environment variable RECRUIT_DLY");

    strcat(totem_path, camp);
    strcat(totem_path, "Totem");

    struct stat stat1;
    if (stat(totem_path, &stat1))
        if (mkfifo(totem_path, S_IRUSR | S_IWUSR))
            print_error("error while creating fifo");

    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGRTMIN + 13, &sa, NULL) == -1)
        print_error("sigaction error");

    active_flag = 0;
    totem_fd = 0;
}

struct timespec ftots(double time) {
    time /= 100;
    struct timespec res = {(int) time, (time - (int) time) * 1000000000l};
    return res;
}