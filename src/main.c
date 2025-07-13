#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <linux/input.h>

#include "xak_x11.h"
#include "xak_input.h"

static void xak_signal_handle(int sig) {
    printf("caught %s\n", strsignal(sig));
    xak_x11_close();
    xak_input_close();
    _exit(0);
}

static int xak_signal_setup(void) {
    struct sigaction sa;
    sa.sa_handler = xak_signal_handle;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    int signals[] = { SIGINT, SIGTERM, SIGHUP, SIGQUIT };
    for (size_t i = 0; i < sizeof(signals)/sizeof(signals[0]); ++i) {
        if (sigaction(signals[i], &sa, NULL) == -1) {
            perror("sigaction");
            return -1;
        }
    }

    return 0;
}

int main(void)
{
    // setup signals
    printf("xak_signal_setup: ");
    fflush(stdout);

    if (xak_signal_setup()) _exit(1);

    printf("ok\n");

    // setup x11
    printf("xak_x11_init: ");
    fflush(stdout);

    int x = xak_x11_init();
    if (x == -1) _exit(1);

    printf("ok - x11 connection fd=%d\n", x);

    // setup input
    printf("xak_input_init: ");
    fflush(stdout);

    int input = xak_input_init();
    if (input == -1) _exit(1);

    printf("ok - input device fd=%d\n", input);

    // setup pollfds
    struct pollfd fds[2];
    fds[0].fd = x;
    fds[0].events = POLLIN;

    fds[1].fd = input;
    fds[1].events = POLLIN;

    // poll
    for(;;) {
        if (poll(fds, 2, -1) == -1) {
            perror("poll");
            xak_x11_close();
            xak_input_close();
            return 1;
        }

        if (fds[0].revents & POLLIN) {
            xak_x11_handle();
            printf("x11 event\n");
        }

        if (fds[1].revents & POLLIN) {
            struct input_event ev;
            read(input, &ev, sizeof(ev));
            printf("input event\n");
        }
    }

    xak_x11_close();
    xak_input_close();
    return 0;
}