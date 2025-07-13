#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#include "xak_signal.h"
#include "xak_input.h"
#include "xak_x11.h"

// handles signal events
static void xak_signal_handle(int sig) {
    printf("caught %s\n", strsignal(sig));
    xak_x11_close();
    xak_input_close();
    _exit(0);
}

// sets up signal handler
int xak_signal_init(void) {
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