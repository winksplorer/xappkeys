#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/input.h>

#include "xak_input.h"
#include "xak_x11.h"

int input_fd = -1;

// inits input code
int xak_input_init(void) {
    input_fd = open(XAK_INPUT_DEVICE_PATH, O_RDONLY | O_NONBLOCK);
    if (input_fd == -1) {
        perror("open");
        return -1;
    }

    // "grab" the device so only we get it's data
    if (ioctl(input_fd, EVIOCGRAB, 1) == -1) {
        perror("ioctl");
        close(input_fd);
        input_fd = -1;
        return -1;
    }

    return input_fd;
}

// handles input events
int xak_input_handle(KeyBinding binds[], int num_binds) {
    struct input_event ev;
    ssize_t n = read(input_fd, &ev, sizeof(ev));
    if (n != (ssize_t)sizeof(ev) || ev.type != EV_KEY) return 0;
    
    for (int i = 0; i < num_binds; ++i) {
        if (binds[i].keycode == ev.code && ev.value == binds[i].state) {
            // first fork
            pid_t child = fork();
            if (child == -1) perror("fork");
            else if (child) return 0;

            // second fork
            pid_t child2 = fork();
            if (child2 == -1) {
                perror("fork");
                _exit(1);
            }
            else if (child2) _exit(0);

            // detatch terminal
            if (setsid() == -1) {
                perror("setsid");
                _exit(1);
            }

            #if XAK_EXPECT_ROOT
            // drop privileges
            if (setgid(XAK_TARGET_GID) || setuid(XAK_TARGET_UID)) {
                perror("setgid/setuid");
                _exit(1);
            }

            // final setup
            setenv("USER", XAK_TARGET_USER, 1);
            setenv("LOGNAME", XAK_TARGET_USER, 1);
            setenv("HOME", XAK_TARGET_HOME, 1);
            #endif
            
            xak_x11_child_close();
            xak_input_child_close();

            // run the file (with path). any return means it failed.
            execvp(binds[i].argv[0], binds[i].argv);
            perror("execvp");
            _exit(1);
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

// uninits input code (for child use only)
void xak_input_child_close(void) {
    if (input_fd != -1) close(input_fd);
}