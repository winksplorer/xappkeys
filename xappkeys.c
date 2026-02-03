/* xappkeys - intercepting per-app macro daemon */
/* See LICENSE file for copyright and license details. */
#include <fcntl.h>
#include <linux/input.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "config.h"

#define CURRENT_WIN window_binds[i]

static int
inputinit(void)
{
    int input_fd = open(INPUT_DEVICE_PATH, O_RDONLY | O_NONBLOCK | O_CLOEXEC);
    if (input_fd < 0) {
        perror("open");
        return -1;
    }

    if (ioctl(input_fd, EVIOCGRAB, 1) < 0) {
        perror("ioctl");
        close(input_fd);
        return -1;
    }

    return input_fd;
}

static void
inputclose(int fd)
{
    if (ioctl(fd, EVIOCGRAB, 0) < 0)
        perror("ioctl");

    if (close(fd) < 0)
        perror("close");
}

static int
inputhandle(Macro binds[], int num_binds, int input_fd)
{
    struct input_event ev;
    ssize_t n = read(input_fd, &ev, sizeof(ev));
    if (n != (ssize_t)sizeof(ev) || ev.type != EV_KEY) return 0;
    
    for (int i = 0; i < num_binds; ++i) {
        if (binds[i].keycode == ev.code && ev.value == binds[i].state) {
            switch (fork()) {
            case 0:
                setsid();
                execvp(binds[i].argv[0], binds[i].argv);
                perror("execvp");
                exit(1);
            case -1:
                perror("fork");
            }
        }
    }
    

    return 0;
}

static Display*
x11init(void)
{
    Display *dpy = XOpenDisplay(NULL);
    if (!(dpy = XOpenDisplay(NULL))) {
        perror("XOpenDisplay");
        return NULL;
    }

    fcntl(ConnectionNumber(dpy), F_SETFD, FD_CLOEXEC);
    Window root = RootWindow(dpy, DefaultScreen(dpy));
    XSelectInput(dpy, root, PropertyChangeMask);
    XFlush(dpy);

    return dpy;
}

static void
x11close(Display *dpy)
{
    if (!dpy) return;
    XCloseDisplay(dpy);
}

static Window
activewin(Display *dpy) {
    Atom prop = XInternAtom(dpy, "_NET_ACTIVE_WINDOW", False), type;
    int form;
    unsigned long len;
    unsigned long remain;
    unsigned char *data;

    if (XGetWindowProperty(dpy,XDefaultRootWindow(dpy),prop,0,1024,False,
        XA_WINDOW,&type,&form,&len,&remain,&data)) {
        perror("get_active_window: XGetWindowProperty");
        return 0;
    }
     
    Window win = ((Window*) data)[0];
    XFree(data);
    return win;
}

static char*
winname(Display *dpy, Window win)
{
    XClassHint hint;
    char *n = malloc(64);

    if (XGetClassHint(dpy, win, &hint)) {
        n = strncpy(n, hint.res_class, 63);
        XFree(hint.res_name);
        XFree(hint.res_class);
    }

    return n;
}

static Window
x11handle(Display *dpy, Window focused)
{
    while (XPending(dpy)) {
        XEvent ev;
        XNextEvent(dpy, &ev);

        if (ev.type == PropertyNotify &&
            ev.xproperty.atom == XInternAtom(dpy, "_NET_ACTIVE_WINDOW", False))
        {
            Window w = activewin(dpy);
            if (w && w != focused) {
                return w;
            }
        }
    }

    return 0;
}

int 
main(void)
{
    Display *dpy;
    int input;
    Macro active[256];
    unsigned char num_active = window_binds[0].num_macros;
    Window focused = 0;
    struct pollfd fds[2];
    unsigned char i;

    if (!(dpy = x11init()))
        return 1;

    if ((input = inputinit()) < 0) {
        x11close(dpy);
        return 1;
    }

    for (i = 0; i < window_binds[0].num_macros; ++i)
        active[i] = window_binds[0].macros[i];

    fds[0].fd = ConnectionNumber(dpy); fds[0].events = POLLIN;
    fds[1].fd = input; fds[1].events = POLLIN;

    for (;;) {
        if (poll(fds, 2, -1) == -1) {
            perror("poll");
            x11close(dpy);
            inputclose(input);
            return 1;
        }

        if (fds[0].revents & POLLIN) {
            char *class;
            if (!(focused = x11handle(dpy, focused)))
                continue;

            class = winname(dpy, focused);
            num_active = window_binds[0].num_macros;

            for (i = 1; i < LEN(window_binds); ++i) {
                unsigned char j;
                if (!CURRENT_WIN.xclass || strcmp(CURRENT_WIN.xclass, class) != 0)
                    continue;

                for (j = 0; j < CURRENT_WIN.num_macros; ++j) {
                    active[j + window_binds[0].num_macros]
                        = CURRENT_WIN.macros[j];
                    num_active++;
                }

                break;
            }

            free(class);
        }

        if (fds[1].revents & POLLIN)
            inputhandle(active, num_active, input);
    }

    x11close(dpy);
    inputclose(input);
    return 0;
}
