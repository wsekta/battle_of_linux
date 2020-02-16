//
// Created by wojtek on 10.02.2020.
//
#include "tools.h"
#include "timers.h"

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

void load_environment_variables();

void create_arbiter();

void do_at_end();

void print_error(const char *msg) {
    if (!errno)
        errno = -1;
    perror(msg);
    exit(-1);
}

void handler(int sig, siginfo_t *si, void *data) {
    if(si->si_pid == reg[0].pid)
        is_fighting[0]=0;
    else
        is_fighting[1]=0;
}

void setup() {
    srand(time(NULL));
    load_environment_variables();

    //create fifo
    struct stat stat1;
    if (stat(fi_path, &stat1))
        if (mkfifo(fi_path, S_IRUSR | S_IWUSR))
            print_error("error while creating fifo");
    if (stat(fo_path, &stat1))
        if (mkfifo(fo_path, S_IRUSR | S_IWUSR))
            print_error("error while creating fifo");

    create_timer(recruit_dly, SIGRTMIN + 0, feed_handler);
    create_timer(ring_reg, SIGRTMIN + 1, registration_handler);

    create_arbiter();

    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO | SA_NODEFER;
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = handler;
    if (sigaction(SIGRTMIN + 13, &sa, NULL) == -1)
        print_error("SIGRTMIN+13 sigaction error");

    fi_fd = -1;
    feeder_flag = 0;
    reg_flag = 0;
    ring_flag = 0;

    atexit(do_at_end);
}

void load_environment_variables() {
    char *camp = getenv("CAMP");
    char *recruit_dly_str = getenv("RECRUIT_DLY");
    char *recruit_max_str = getenv("RECRUIT_MAX");
    char *ring_reg_str = getenv("RING_REG");
    if (!camp || !recruit_dly_str || !recruit_max_str || !ring_reg_str)
        print_error("environment variable(s) not defined");
    char *tmp;
    recruit_dly = strtod(recruit_dly_str, &tmp);
    if (*tmp)
        print_error("wrong environment variable RECRUIT_DLY");
    recruit_max = strtol(recruit_max_str, &tmp, 10);
    if (*tmp)
        print_error("wrong environment variable RECRUIT_MAX");
    ring_reg = strtod(ring_reg_str, &tmp);
    if (*tmp)
        print_error("wrong environment variable RING_REG");

    strcat(fi_path, camp);
    strcat(fi_path, "FI");
    strcat(fo_path, camp);
    strcat(fo_path, "FO");
}

void feeder() {
    int fifo_fd = open(fo_path, O_WRONLY | O_NDELAY);
    if (fifo_fd == -1) {
        if (errno != ENXIO && errno != EINTR)
            print_error("open FO fifo");
    } else {
        int i = rand() % (recruit_max + 1);
       //printf("karmimy %d hooliganow\n",i);
        while (i--)
            write(fifo_fd, "y", 1);
    }
    close(fifo_fd);
    feeder_flag = 0;
}

void ring_registration() {
    if (fi_fd == -1) {
        fi_fd = open(fi_path, O_RDONLY | O_NDELAY | O_NONBLOCK);
        if (fi_fd == -1)
            print_error("fi open error");
    } else {
        if (!ring_flag) {
            if ((read(fi_fd, reg, 2 * sizeof(struct registration_info)) == 2 * sizeof(struct registration_info)) && reg[0].pid != reg[1].pid
                    && reg[0].chld_pgid != 0 && reg[0].chld_pgid != 1 && reg[1].chld_pgid != 0 && reg[1].chld_pgid != 1) {
                ring_flag = 1;
                is_fighting[0]=1;
                is_fighting[1]=1;
                union sigval sv;
              //printf("wojna między %d(dzieci %d) a %d(dzieci %d)\n",reg[0].pid,reg[0].chld_pgid,reg[1].pid,reg[1].chld_pgid);
                sv.sival_int = reg[1].chld_pgid;
                sigqueue(reg[0].pid, SIGRTMIN + 13, sv);
                sv.sival_int = reg[0].chld_pgid;
                sigqueue(reg[1].pid, SIGRTMIN + 13, sv);
                struct timespec ts;
                ts.tv_sec = 0;
                ts.tv_nsec = 10000000l;
                nanosleep(&ts,NULL);
                kill(arbiter_pid, SIGRTMIN + 13);
            }
        }
        char buff[255];
        while (read(fi_fd, buff, 255) == 255);
        close(fi_fd);
        fi_fd = -1;
    }
    reg_flag = 0;
}

void create_arbiter() {
    arbiter_pid = fork();
    switch (arbiter_pid) {
        case -1:
            print_error("fork error");
            break;
        case 0:
            if (execl("Arbiter", "Arbiter", NULL))
                print_error("execl error");
        default:
            break;
    }
}

void do_at_end() {
    unlink(fi_path);
    unlink(fo_path);
}
void end_of_war(){
    kill(arbiter_pid,SIGRTMIN+13);
  //printf("koniec wojny\n");
    ring_flag = 0;
    fi_fd = open(fi_path, O_RDONLY | O_NDELAY | O_NONBLOCK);
    char buff[255];
    while (read(fi_fd, buff, 255) == 255);
    close(fi_fd);
}