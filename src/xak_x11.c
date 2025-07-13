#include <string.h>
#include <stdio.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <stdbool.h>

#include "xak_x11.h"

static Display* dpy;
static Atom NET_ACTIVE;
static Atom UTF8_STRING;
static Window focused;
static Window root;

// gets the active window
static Window xak_x11_get_active(void)
{
    Atom actual;
    int format;
    unsigned long n;
    unsigned long extra;
    unsigned char* prop = NULL;
    Window win = 0;

    if (XGetWindowProperty(dpy, root, NET_ACTIVE, 0, 1, False, AnyPropertyType, &actual, &format, &n, &extra, &prop) == Success && prop) {
        if (format == 32 && n == 1)
            win = *(Window*)prop;
        XFree(prop);
    }
    return win;
}

// gets the wm_class of focused window
void xak_x11_get_focused_class(char* buf, unsigned long n)
{
    XClassHint hint;
    buf[0] = 0;

    if (XGetClassHint(dpy, focused, &hint)) {
        if (hint.res_class) {
            strncpy(buf, hint.res_class, n-1);
            buf[n-1] = 0;
        }

        if (hint.res_name)  XFree(hint.res_name);
        if (hint.res_class) XFree(hint.res_class);
    }
}

// inits x11 code
int xak_x11_init(void) {
    dpy = XOpenDisplay(NULL);
    if (!dpy) {
        fprintf(stderr, "failed on XOpenDisplay\n");
        return -1;
    }

    // vars
    int screen    = DefaultScreen(dpy);
    NET_ACTIVE    = XInternAtom(dpy, "_NET_ACTIVE_WINDOW", False);
    UTF8_STRING   = XInternAtom(dpy, "UTF8_STRING", False);
    root = RootWindow(dpy, screen);

    // listen for property changes on root
    XSelectInput(dpy, root, PropertyChangeMask);
    XFlush(dpy);

    // get the currently active window
    focused = xak_x11_get_active();

    return ConnectionNumber(dpy);
}

// handles x11 focus change events
int xak_x11_handle(void) {
    int changed = 0;

    while (XPending(dpy)) {
        XEvent ev;
        XNextEvent(dpy, &ev);

        if (ev.type == PropertyNotify && ev.xproperty.atom == NET_ACTIVE) {
            Window w = xak_x11_get_active();
            if (w && w != focused) {
                focused = w;
                changed = 1;
            }
        }
    }
    
    return changed;
}

// uninits x11 code
void xak_x11_close(void) {
    if (dpy) {
        XCloseDisplay(dpy);
        dpy = NULL;
    }
}