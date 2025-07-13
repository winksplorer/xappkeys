# xappkeys
App-specific macro keybinding tool for X11 with direct device interception

> [!NOTE]
> This is mainly for my own use, so it might completely break itself or your system

## Running without root

`config.h` lets you specify XAK_EXPECT_ROOT, however it is HIGHLY recommended **not** to run as root.

But devices in `/dev/input/by-id` require root. Here's some quick instructions on how to change that:

### Create a group

```sh
doas groupadd -f input-readers
doas usermod -aG input-readers $USER
```

### Create a udev rule

```sh
# /etc/udev/rules.d/99-input.rules
KERNEL=="event*", SUBSYSTEM=="input", GROUP="input-readers", MODE="0640"
```

### Apply udev and group changes

> If this doesn't work, just reboot.

```sh
doas udevadm control --reload
doas udevadm trigger
newgrp input-readers
```