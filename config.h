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


// what device to read from
#define XAK_INPUT_DEVICE_PATH "/dev/input/by-id/usb-2dc8_8BitDo_Retro_18_Numpad_24F218A7BC-if01-event-kbd"

// custom macros, not required
#define WINCYCLE(name) (char*[]){ "wincycle", name, NULL }

// bindings that will be active across any window
static const KeyBinding global_bindings[] = {
    // dot = turn monitors off
    { KEY_KPDOT, KEY_RELEASED, (char*[]){ "xset", "dpms", "force", "off", NULL } },

    // 0 = alacritty
    { KEY_KP0, KEY_PRESSED, WINCYCLE("alacritty") },

    // 1 = apple music via weston
    { KEY_KP1, KEY_PRESSED, WINCYCLE("weston") },

    // 2 = codium
    { KEY_KP2, KEY_PRESSED, WINCYCLE("codium") },

    // 3 = vesktop
    { KEY_KP3, KEY_PRESSED, WINCYCLE("vesktop") },

    // 4 = steam
    { KEY_KP4, KEY_PRESSED, WINCYCLE("steam") },

    // 5 = thunar
    { KEY_KP5, KEY_PRESSED, WINCYCLE("thunar") },

    // 6 = qutebrowser. fuck you qutebrowser
    { KEY_KP6, KEY_PRESSED, WINCYCLE("thorium-browser") },

    // 7 = chromium
    { KEY_KP7, KEY_PRESSED, WINCYCLE("chromium") },

    // 8 = jellyfin. chromium WHY
    { KEY_KP8, KEY_PRESSED, (char*[]){"sh", "-c", "wincycle jellyfinmediaplayer", NULL} },

    // 9 = bitwarden
    { KEY_KP9, KEY_PRESSED, WINCYCLE("bitwarden") }
};


// bindings for vscodium
static const KeyBinding vscodium_bindings[] = {
    // enter = search by file
    { KEY_KPENTER, KEY_RELEASED, (char*[]){ "xdotool", "key", "Control_L+p", NULL } },

    // plus = search by function
    { KEY_KPPLUS, KEY_RELEASED, (char*[]){ "xdotool", "key", "Control_L+p", "key", "at", NULL } },

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

// list of bindings for each window class
// first argument = "firefox" means these bindings will only be active when firefox is the currently focused window
// get the first argument by running xprop on your desired window and use the value of WM_CLASS(STRING)
static const WindowBind window_binds[] = {
    { NULL, ARR_SZ(global_bindings), global_bindings }, // make sure this is first
    { "VSCodium", ARR_SZ(vscodium_bindings), vscodium_bindings },
    { "Alacritty", ARR_SZ(alacritty_bindings), alacritty_bindings }
};

#endif
