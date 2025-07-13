#include <string.h>
#include <stdio.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

static Display* dpy;
static Atom NET_ACTIVE;
static Atom UTF8_STRING;

static void xak_x11_get_class(Display* dpy, Window win, char* buf, unsigned long n)
{
    XClassHint hint;
    buf[0] = 0;

    if (XGetClassHint(dpy, win, &hint)) {
        if (hint.res_class) {
            strncpy(buf, hint.res_class, n-1);
            buf[n-1] = 0;
        }

        if (hint.res_name)  XFree(hint.res_name);
        if (hint.res_class) XFree(hint.res_class);
    }
}

static Window xak_x11_get_active(Display *dpy, Window root)
{
    Atom actual;
    int format;
    unsigned long n;
    unsigned long extra;
    unsigned char* prop = NULL;
    Window win = 0;

    if (XGetWindowProperty(dpy, root, NET_ACTIVE,
                           0, 1, False, AnyPropertyType,
                           &actual, &format, &n, &extra,
                           &prop) == Success && prop) {
        if (format == 32 && n == 1)
            win = *(Window*)prop;
        XFree(prop);
    }
    return win;
}

int xak_x11_init(void) {
    dpy = XOpenDisplay(NULL);
    if (!dpy) {
        fprintf(stderr, "failed on XOpenDisplay\n");
        return 0;
    }

    int screen    = DefaultScreen(dpy);
    Window root   = RootWindow(dpy, screen);
    NET_ACTIVE    = XInternAtom(dpy, "_NET_ACTIVE_WINDOW", False);
    UTF8_STRING   = XInternAtom(dpy, "UTF8_STRING", False); // not strictly needed

    // listen for property changes on root
    XSelectInput(dpy, root, PropertyChangeMask);

    return ConnectionNumber(dpy);

    #if 0
    // print current active window once at start
    Window last = xak_x11_get_active(dpy, root);
    // if (last) print_class(dpy, last);

    for (;;) {
        XEvent ev;
        XNextEvent(dpy, &ev);
        if (ev.type == PropertyNotify &&
            ev.xproperty.atom == NET_ACTIVE) {
            Window w = xak_x11_get_active(dpy, root);
            if (w && w != last) {
                last = w;
                // print_class(dpy, w);
            }
        }
    }

    XCloseDisplay(dpy);
    #endif
}

void xak_x11_close(void) {
    if (dpy) {
        XCloseDisplay(dpy);
        dpy = NULL;
    }
}