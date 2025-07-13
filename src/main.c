#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#include "xak_x11.h"

static void xak_signal_handle(int sig) {
    printf("caught %s\n", strsignal(sig));
    xak_x11_close();
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
            perror("xak_signal_setup: sigaction");
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
    if (!x) _exit(1);

    printf("ok - x11 connection fd=%d\n", x);

    xak_x11_close();
    return 0;
}