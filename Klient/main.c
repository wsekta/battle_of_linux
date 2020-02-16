//
// Created by wojtek on 12.02.2020.
//
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <wait.h>

struct registration_info {
    int pid;
    int chld_pgid;
} reg_info;

int ring_flag;
int successful_registration_flag;
int win_flag;
int fo_fd;
int child_no;
int chld_pgid;
int server_pid;
int children_pids[255];

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

void child_main();

void try_to_register();

void do_at_win();

int main() {
    setup();
    while (1) {
        if (!ring_flag) {
            char c[1];
            if (!successful_registration_flag && child_no < 255)
                if (read(fo_fd, c, 1) == 1) {
                    make_child();
                }
            if (child_no >= 3)
                try_to_register();
        } else {
            if (win_flag >= child_no)
                do_at_win();
        }
        successful_registration_flag = 0;
        siginfo_t si;
        if (child_no) {
            do {
                waitid(P_ALL, 0, &si, WNOHANG | WEXITED);
                if (si.si_pid) {
                    win_flag = 0;
                    if (!(--child_no)) {
                        union sigval sv;
                        sv.sival_int = 0;
                        if(ring_flag) {
                          //printf("przegranko %d\n",getpid());
                            sigqueue(server_pid, SIGRTMIN + 13, sv);
                            ring_flag = 0;
                            chld_pgid = 0;
                        }
                    }
                }
            } while (si.si_pid);
        }
        nanosleep(&wait_time, NULL);
    }
}

void parent_handler(int sig, siginfo_t *si, void *data) {
    if (chld_pgid != getpgid(si->si_pid)) {
        win_flag = 0;
      //printf("%d idzie na wojne z %d mając %d potomkow\n", getpid(), si->si_value.sival_int,child_no);
        for (int i = 0; i < child_no; ++i)
            sigqueue(children_pids[i], SIGRTMIN + 13, si->si_value);
        ring_flag = 1;
        server_pid = si->si_pid;
    }
    if (chld_pgid == getpgid(si->si_pid)){
        win_flag++;
    }
}

void setup() {
    reg_info.pid = getpid();
    chld_pgid = 0;
    child_no = 0;
    ring_flag = 0;
    successful_registration_flag = 0;
    char *camp = getenv("CAMP");
    if (!camp)
        print_error("environment variable(s) not defined");
    wait_time.tv_nsec = 10;
    wait_time.tv_sec = 0;

    strcat(fi_path, camp);
    strcat(fi_path, "FI");
    strcat(fo_path, camp);
    strcat(fo_path, "FO");

    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = parent_handler;
    if (sigaction(SIGRTMIN + 13, &sa, NULL) == -1)
        print_error("SIGRTMIN+13 sigaction error");

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
    int child_pid = fork();
    switch (child_pid) {
        case -1:
            print_error("fork_error");
            break;
        case 0:
            child_main();
            break;
        default:
            setpgid(child_pid, chld_pgid);
            chld_pgid = getpgid(child_pid);
            break;
    }
    children_pids[child_no++] = child_pid;
}

void try_to_register() {
    int fi_fd = open(fi_path, O_WRONLY);
    if (fi_fd == -1 && errno != ENXIO && errno !=EINTR)
        print_error("fi open error");
    else if (fi_fd == -1 && errno == ENXIO)
        return;
    reg_info.chld_pgid = chld_pgid;
    write(fi_fd, &reg_info, sizeof(reg_info));
    successful_registration_flag = 1;
    close(fi_fd);
}

void chld_handler(int sig, siginfo_t *si, void *data) {
    char str[10];
    sprintf(str, "%d", si->si_value.sival_int);
    execl("Hooligan", "Hooligan", str, NULL);
}

void child_main() {
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = chld_handler;
    if (sigaction(SIGRTMIN + 13, &sa, NULL) == -1)
        print_error("SIGRTMIN+13 sigaction error");
    while (1)
        pause();
}

void do_at_win()
{
  //printf("wygrywa: %d\n", getpid());
    ring_flag = 0;
    int child_no_cpy = child_no;
    kill(-chld_pgid, SIGTERM);
    chld_pgid = 0;
    siginfo_t si;
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 1000000;
    nanosleep(&ts, NULL);
    do {
        waitid(P_ALL, 0, &si, WNOHANG | WEXITED);
        if (si.si_pid) {
            --child_no;
        }
    } while (si.si_pid);
    for (int i = 0; i < child_no_cpy; ++i)
        make_child();
    union sigval sv;
    sv.sival_int = 1;
    sigqueue(server_pid, SIGRTMIN + 13, sv);
}