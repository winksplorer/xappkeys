#ifndef XAK_X11_H
#define XAK_X11_H

#include <X11/Xlib.h>
#include <stdbool.h>

void xak_x11_get_focused_class(char* buf, unsigned long n);
int xak_x11_init(void);
int xak_x11_handle(void);
void xak_x11_close(void);

#endif