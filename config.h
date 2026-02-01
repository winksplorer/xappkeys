/* xappkeys - intercepting per-app macro daemon */
/* See LICENSE file for copyright and license details. */

/* Note: This is my actual config */

#define WINCYCLE(name) (char*[]){ "wincycle", name, NULL }
#define WINCYCLE_PROGRAM(name, program) (char*[]){ "wincycle", name, program, NULL }

/* ------ not configuration */
#include <stddef.h>
#include <linux/input-event-codes.h>

#define LEN(a) (sizeof(a) / sizeof(a)[0])
enum {
    KEYUP = 0,
    KEYDOWN = 1
};

typedef struct {
    int keycode;
    int state;
    char *const *argv;
} Macro;

typedef struct {
    const char *xclass;
    const unsigned char num_macros;
    const Macro *macros;
} WindowBind;
/* ------ */

/* your macro pad goes here */
#define INPUT_DEVICE_PATH \
    "/dev/input/by-id/usb-2dc8_8BitDo_Retro_18_Numpad_24F218A7BC-if01-event-kbd"

/* macros that will be active across any window */
static const Macro global_macros[] = {
    { KEY_KPDOT, KEYUP, (char*[]){ "xset", "dpms", "force", "off", NULL } },
    { KEY_KP0, KEYDOWN, WINCYCLE_PROGRAM("st-256color", "st") },
    { KEY_KP1, KEYDOWN, WINCYCLE_PROGRAM("weston compositor", "waydroid-session") },
    { KEY_KP2, KEYDOWN, WINCYCLE_PROGRAM("vscodium", "codium") },
    { KEY_KP3, KEYDOWN, WINCYCLE("vesktop") },
    { KEY_KP4, KEYDOWN, WINCYCLE("steam") },
    { KEY_KP5, KEYDOWN, WINCYCLE("thunar") },
    { KEY_KP6, KEYDOWN, WINCYCLE("librewolf") },
    { KEY_KP7, KEYDOWN, WINCYCLE("chromium") },
    { KEY_KP8, KEYDOWN, WINCYCLE("freetube") },
    { KEY_KP9, KEYDOWN, WINCYCLE("bitwarden") }
};

static const Macro vscodium_macros[] = {
    /* enter = search by file */
    { KEY_KPENTER, KEYUP, (char*[]){ "xdotool", "key", "Control_L+p", NULL } },

    /* plus = search by function */
    { KEY_KPPLUS, KEYUP, (char*[]){ "xdotool", "key", "Control_L+p", "key", "at", NULL } },

    /* minus = go to line */
    { KEY_KPMINUS, KEYUP, (char*[]){ "xdotool", "key", "Control_L+p", "key", "colon", NULL } },

    /* backspace = if err != nil { return err } */
    { KEY_BACKSPACE, KEYUP, (char*[]){ "/bin/sh", "-c", "xdotool type \"if err != nil { return err }\"; xdotool key Return", NULL} },
};

static const Macro st_macros[] = {
    /* enter = make clean && make */
    { KEY_KPENTER, KEYUP, (char*[]){ "/bin/sh", "-c", "xdotool type \"make clean && make\"; xdotool key Return", NULL } },

    /* plus = doas apt install */
    { KEY_KPPLUS, KEYUP, (char*[]){ "xdotool", "type", "doas apt install ", NULL } },

    /* minus = clear */
    { KEY_KPMINUS, KEYUP, (char*[]){ "xdotool", "key", "Control_L+l", NULL } },
};

/* main macro list. xclass is the WM_CLASS of an X window. */
static const WindowBind window_binds[] = {
    { NULL, LEN(global_macros), global_macros }, /* globals always are first */
    { "VSCodium", LEN(vscodium_macros), vscodium_macros },
    { "st-256color", LEN(st_macros), st_macros }
};