#ifndef XAK_CONFIG_H
#define XAK_CONFIG_H

// ------ do not touch
#include <stddef.h>
#include <linux/input-event-codes.h>

#define ARR_SZ(arr) (sizeof(arr) / sizeof((arr)[0]))
#define KEY_RELEASED 0
#define KEY_PRESSED 1

typedef struct {
    int keycode;
    int state;
    char* const* argv;
} KeyBinding;

typedef struct {
    const char* xclass;
    const int num_bindings;
    const KeyBinding* bindings;
} WindowBind;
// ------

// what device to read from
#define XAK_INPUT_DEVICE_PATH "/dev/input/by-id/usb-2dc8_8BitDo_Retro_18_Numpad_24F218A7BC-if01-event-kbd"

// launched application details
#define XAK_TARGET_GID 1000
#define XAK_TARGET_UID 1000
#define XAK_TARGET_USER "wink"
#define XAK_TARGET_HOME "/home/wink"

// bindings that will be active across any window
static const KeyBinding global_bindings[] = {
    { KEY_ESC, KEY_PRESSED, (char*[]){ "/usr/bin/alacritty", "--hold", "-e", "/usr/bin/env", NULL } }
};

// example: bindings for alacritty
static const KeyBinding alacritty_bindings[] = {
    { KEY_KP1, KEY_PRESSED, (char*[]){ "/usr/bin/xdotool", "type", "hi", NULL } },
    { KEY_KP2, KEY_PRESSED, (char*[]){ "/usr/bin/xdotool", "type", "hello", NULL } }
};

// list of bindings for each window class
// first argument = "firefox" means these bindings will only be active when firefox is the currently focused window
// get the first argument by running xprop on your desired window and use the value of WM_CLASS(STRING)
static const WindowBind window_binds[] = {
    { NULL, ARR_SZ(global_bindings), global_bindings }, // make sure this is first
    { "Alacritty", ARR_SZ(alacritty_bindings), alacritty_bindings }
};

#endif