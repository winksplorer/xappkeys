#ifndef XAK_CONFIG_H
#define XAK_CONFIG_H

// note: this is my actual config

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

// HIGHLY recommended to not have root
#define XAK_EXPECT_ROOT 0

// what device to read from
#define XAK_INPUT_DEVICE_PATH "/dev/input/by-id/usb-2dc8_8BitDo_Retro_18_Numpad_24F218A7BC-if01-event-kbd"

// launched application details (not required if XAK_EXPECT_ROOT is 0)
#define XAK_TARGET_GID 1000
#define XAK_TARGET_UID 1000
#define XAK_TARGET_USER "wink"
#define XAK_TARGET_HOME "/home/wink"

// bindings that will be active across any window
static const KeyBinding global_bindings[] = {
    // dot = screensaver
    { KEY_KPDOT, KEY_RELEASED, (char*[]){ "xscreensaver-command", "-activate", NULL } },

    // 0 = alacritty
    { KEY_KP0, KEY_PRESSED, (char*[]){ "/home/wink/Documents/wincycle.sh", "alacritty", NULL } },

    // 1 = apple music via weston
    { KEY_KP1, KEY_PRESSED, (char*[]){ "/home/wink/Documents/wincycle.sh", "weston", NULL } },

    // 2 = codium
    { KEY_KP2, KEY_PRESSED, (char*[]){ "/home/wink/Documents/wincycle.sh", "codium", NULL } },

    // 3 = vesktop
    { KEY_KP3, KEY_PRESSED, (char*[]){ "/home/wink/Documents/wincycle.sh", "vesktop", NULL } },

    // 4 = steam
    { KEY_KP4, KEY_PRESSED, (char*[]){ "/home/wink/Documents/wincycle.sh", "steam", NULL } },

    // 5 = thunar
    { KEY_KP5, KEY_PRESSED, (char*[]){ "/home/wink/Documents/wincycle.sh", "thunar", NULL } },

    // 6 = qutebrowser
    { KEY_KP6, KEY_PRESSED, (char*[]){ "/home/wink/Documents/wincycle.sh", "qutebrowser", NULL } },

    // 7 = chromium
    { KEY_KP7, KEY_PRESSED, (char*[]){ "/home/wink/Documents/wincycle.sh", "chromium", NULL } },

    // 8 = freetube
    { KEY_KP8, KEY_PRESSED, (char*[]){ "/home/wink/Documents/wincycle.sh", "freetube", NULL } },

    // 9 = vesktop + type ":sob:" + alt-tab
    { KEY_KP9, KEY_PRESSED, (char*[]){ "/bin/sh", "-c", "/home/wink/Documents/wincycle.sh vesktop && xdotool type :sob: && xdotool sleep 0.1 key Return sleep 0.3 keydown Alt key Tab sleep 0.1 keyup Alt", NULL } },
};


// bindings for vscodium
static const KeyBinding vscodium_bindings[] = {
    // enter = search by function
    { KEY_KPENTER, KEY_RELEASED, (char*[]){ "xdotool", "key", "Control_L+p", "key", "at", NULL } },

    // plus = search by file
    { KEY_KPPLUS, KEY_RELEASED, (char*[]){ "xdotool", "key", "Control_L+p", NULL } },

    // minus = go to line
    { KEY_KPMINUS, KEY_RELEASED, (char*[]){ "xdotool", "key", "Control_L+p", "key", "colon", NULL } },
};

// bindings for alacritty
static const KeyBinding alacritty_bindings[] = {
    // enter = make clean && make
    { KEY_KPENTER, KEY_RELEASED, (char*[]){ "/bin/sh", "-c", "xdotool type \"make clean && make\"; xdotool key Return", NULL } },

    // plus = doas apt install
    { KEY_KPPLUS, KEY_RELEASED, (char*[]){ "xdotool", "type", "doas apt install ", NULL } },

    // minus = clear
    { KEY_KPMINUS, KEY_RELEASED, (char*[]){ "/bin/sh", "-c", "xdotool type clear; xdotool key Return", NULL } },
};

// bindings for qutebrowser
static const KeyBinding qutebrowser_bindings[] = {
    // enter = open -t
    { KEY_KPENTER, KEY_RELEASED, (char*[]){ "xdotool", "key", "O", NULL } },

    // plus = forward
    { KEY_KPPLUS, KEY_RELEASED, (char*[]){ "xdotool", "key", "L", NULL } },

    // minus = back
    { KEY_KPMINUS, KEY_RELEASED, (char*[]){ "xdotool", "key", "H", NULL } },
};

// list of bindings for each window class
// first argument = "firefox" means these bindings will only be active when firefox is the currently focused window
// get the first argument by running xprop on your desired window and use the value of WM_CLASS(STRING)
static const WindowBind window_binds[] = {
    { NULL, ARR_SZ(global_bindings), global_bindings }, // make sure this is first
    { "VSCodium", ARR_SZ(vscodium_bindings), vscodium_bindings },
    { "Alacritty", ARR_SZ(alacritty_bindings), alacritty_bindings },
    { "qutebrowser", ARR_SZ(qutebrowser_bindings), qutebrowser_bindings }
};

#endif