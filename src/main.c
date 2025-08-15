#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <linux/input.h>

#include "xak_x11.h"
#include "xak_input.h"
#include "xak_signal.h"

// entry point
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

    // setup vars
    KeyBinding active_binds[256];
    int num_global_binds = window_binds[0].num_bindings, // the total amount of binds that work rn
        num_active_binds = num_global_binds; // how many active binds are NOT window-specific

    // copy global binds
    for (int i = 0; i < num_global_binds; ++i) active_binds[i] = window_binds[0].bindings[i];

    // setup pollfds
    struct pollfd fds[2];
    fds[0].fd = x;
    fds[0].events = POLLIN;

    fds[1].fd = input;
    fds[1].events = POLLIN;

    // main loop
    for (;;) {
        // poll info from either x11 or our input device
        if (poll(fds, 2, -1) == -1) {
            perror("poll");
            xak_x11_close();
            xak_input_close();
            return 1;
        }

        // x11 events
        if (fds[0].revents & POLLIN) {
            if (!xak_x11_handle()) continue;

            // what window are we in?
            char class[128];
            xak_x11_get_focused_class(class, sizeof(class));

            // forget all previous window-specific binds
            num_active_binds = num_global_binds;

            // go through window binds, and check if the class matches ours
            for (unsigned long i = 1; i < ARR_SZ(window_binds); ++i) {
                const WindowBind* bind = &window_binds[i];

                if (bind->xclass && strcmp(bind->xclass, class) == 0) {
                    // copy bindings
                    for (int j = 0; j < bind->num_bindings; ++j) {
                        active_binds[j + num_global_binds] = bind->bindings[j];
                        num_active_binds++;
                    }

                    break;
                }
            }
        }

        // input events
        if (fds[1].revents & POLLIN)
            xak_input_handle(active_binds, num_active_binds);
    }

    // exit
    xak_x11_close();
    xak_input_close();
    return 0;
}