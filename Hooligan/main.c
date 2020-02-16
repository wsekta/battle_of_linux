//
// Created by wojtek on 12.02.2020.
//
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>

int enemy_pgid;
int target_pid;
int was_opened_flag;
int prepared_flag;
int attacked_flag;

char totem_path[255];

struct timespec wait_time;

void print_error(const char *msg) {
    if (!errno)
        errno = -1;
    perror(msg);
    exit(-1);
}

void prepare();

void attack();

void chld_handler(int sig, siginfo_t *si, void *data);


int main(int argc, char *argv[]) {
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO | SA_NODEFER ;
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = chld_handler;
    if (sigaction(SIGBUS, &sa, NULL) == -1)
        print_error("SIGBUS sigaction error");
    if (sigaction(SIGALRM, &sa, NULL) == -1)
        print_error("SIGALRM sigaction error");
    srand(time(NULL) + getpid());
    if (argc != 2)
        print_error("too few arguments");
    char *p;
    enemy_pgid = strtol(argv[1], &p, 10);
    if (*p != 0)
        print_error("wrong argument");

    char *camp = getenv("CAMP");
    if (!camp)
        print_error("environment variable(s) not defined");
    strcat(totem_path, camp);
    strcat(totem_path, "Totem");

    wait_time.tv_sec = 0;
    wait_time.tv_nsec = 100l;

    was_opened_flag = 0;
    prepared_flag = 0;
    attacked_flag = 0;

    int totem_fd;
    while (1) {
        totem_fd = open(totem_path, O_WRONLY | O_NDELAY);
        if (totem_fd == -1 && errno != ENXIO)
            print_error("totem open error");
        else if (totem_fd != -1) {
            if (!prepared_flag)
                prepare();
            close(totem_fd);
        } else if (!attacked_flag && was_opened_flag)
            attack();
        nanosleep(&wait_time, NULL);
    }
}

void prepare() {
    target_pid = 0;
    prepared_flag = 1;
    was_opened_flag = 1;
    attacked_flag = 0;
    if (rand() % 2) {
        signal(SIGUSR1, SIG_DFL);
        signal(SIGUSR2, SIG_IGN);
    } else {
        signal(SIGUSR1, SIG_IGN);
        signal(SIGUSR2, SIG_DFL);
    }
    kill(-enemy_pgid, SIGALRM);
}

void attack() {
    if (target_pid) {
        if (rand() % 2)
            kill(target_pid, SIGUSR1);
        else
            kill(target_pid, SIGUSR2);
    } else {
        kill(getppid(), SIGRTMIN + 13);
    }
    attacked_flag = 1;
    prepared_flag = 0;
}

void chld_handler(int sig, siginfo_t *si, void *data) {
    if (sig == SIGALRM)
        kill(si->si_pid, SIGBUS);
    else if (sig == SIGBUS || target_pid == 0)
        target_pid = si->si_pid;
}