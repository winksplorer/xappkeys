#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/input.h>

#include "xak_input.h"

static int input_fd = -1;

// inits input code
int xak_input_init(void) {
    input_fd = open(XAK_INPUT_DEVICE_PATH, O_RDONLY | O_NONBLOCK);
    if (input_fd == -1) {
        perror("open");
        return -1;
    }

    if (ioctl(input_fd, EVIOCGRAB, 1) == -1) {
        perror("ioctl");
        close(input_fd);
        return -1;
    }

    return input_fd;
}

// handles input events
int xak_input_handle(void) {
    struct input_event ev;
    ssize_t n = read(input_fd, &ev, sizeof(ev));
    if (n == (ssize_t)sizeof(ev)) {
        if (ev.type == EV_KEY) {
            printf("Key %d %s\n", ev.code,
                ev.value == 1 ? "pressed" :
                ev.value == 0 ? "released" :
                ev.value == 2 ? "repeated" : "unknown");
        }
    }

    return 0;
}

// uninits input code
void xak_input_close(void) {
    if (input_fd != -1) {
        if (ioctl(input_fd, EVIOCGRAB, 0) == -1) {
            perror("ioctl");
        }

        close(input_fd);
        input_fd = -1;
    }
}