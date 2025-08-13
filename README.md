# xappkeys
App-specific macro keybinding tool for X11 with direct device interception

> [!NOTE]
> This is mainly for my own use, so it might completely break itself or your system

## Running without root

`config.h` lets you specify `XAK_EXPECT_ROOT`, however it is HIGHLY recommended **not** to run as root.

Add yourself to the `input` group (if it exists) and reboot:

```sh
doas usermod -aG input $USER && doas reboot
```
