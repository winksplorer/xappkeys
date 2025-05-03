#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xutil.h>

void print_window_info(Display *display, Window window) {
    XWindowAttributes attrs;
    char *window_name = NULL;
    XClassHint class_hint;
    
    if (XGetWindowAttributes(display, window, &attrs)) {
        printf("Window ID: 0x%lx\n", window);
        printf("Window attributes:\n");
        printf("  - x: %d, y: %d\n", attrs.x, attrs.y);
        printf("  - width: %d, height: %d\n", attrs.width, attrs.height);
        printf("  - mapped: %s\n", attrs.map_state == IsViewable ? "yes" : "no");
        
        if (XFetchName(display, window, &window_name)) {
            printf("  - name: %s\n", window_name);
            XFree(window_name);
        }

        // Get window class
        if (XGetClassHint(display, window, &class_hint)) {
            printf("  - class: %s\n", class_hint.res_class);
            printf("  - instance: %s\n", class_hint.res_name);
            XFree(class_hint.res_name);
            XFree(class_hint.res_class);
        }
    } else printf("Failed to get attributes for window 0x%lx\n", window);
}

Window window_by_class(Display *display, Window current, const char *needle) {
    Window root, parent, *children;
    unsigned children_count;
    XClassHint class_hint;
    XWindowAttributes attrs;

    if (!XGetWindowAttributes(display, current, &attrs)) return 0;
    if (attrs.map_state != IsViewable) return 0;

    // check the window class
    if (XGetClassHint(display, current, &class_hint)) {
        int found = 0;
        if (strcmp(class_hint.res_class, needle) == 0 || 
            strcmp(class_hint.res_name, needle) == 0) {
            found = 1;
        }
        XFree(class_hint.res_name);
        XFree(class_hint.res_class);
        if (found) return current;
    }

    Window retval = 0;

    /* Check all subwindows recursively. */
    if(0 != XQueryTree(display, current, &root, &parent, &children, &children_count)) {
        unsigned i;
        for(i = 0; i < children_count; ++i) {
            Window win = window_by_class(display, children[i], needle);
            if(win != 0) {
                retval = win;
                break;
            }
        }
        XFree(children);
    }

    return retval;
}

int main(void) {
    // open X display
    Display* display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "Unable to open X display\n");
        return 1;
    }

    Window target_window = window_by_class(display, XDefaultRootWindow(display), "Alacritty");

    printf("\n=== Found Window ===\n");
    print_window_info(display, target_window);
    printf("===========================\n\n");
}