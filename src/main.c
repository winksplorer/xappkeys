#include <stdio.h>
#include <unistd.h>
#include <poll.h>
#include <linux/input.h>

#include "xak_x11.h"
#include "xak_input.h"
#include "xak_signal.h"

int main(void)
{
    // setup signals
    printf("xak_signal_init: ");
    fflush(stdout);

    if (xak_signal_init()) _exit(1);

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
    if (input == -1) {
        xak_x11_close();
        _exit(1);
    }

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
            ssize_t n = read(input, &ev, sizeof(ev));
            if (n == (ssize_t)sizeof(ev)) {
                if (ev.type == EV_KEY) {
                    printf("Key %d %s\n", ev.code,
                        ev.value == 1 ? "pressed" :
                        ev.value == 0 ? "released" :
                        ev.value == 2 ? "repeated" : "unknown");
                }
            }
        }
    }

    xak_x11_close();
    xak_input_close();
    return 0;
}