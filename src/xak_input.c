#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/input.h>

#include "xak_input.h"

static int input_fd = -1;

int xak_input_init(void) {
    // todo: make this NOT hardcoded
    input_fd = open("/dev/input/by-id/usb-2dc8_8BitDo_Retro_18_Numpad_24F218A7BC-if01-event-kbd", O_RDONLY | O_NONBLOCK);
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

void xak_input_close(void) {
    if (input_fd != -1) {
        if (ioctl(input_fd, EVIOCGRAB, 0) == -1) {
            perror("ioctl");
        }

        close(input_fd);
        input_fd = -1;
    }
}