# xappkeys
App-specific macro keybinding tool for X11 with direct device interception

> [!NOTE]
> This is mainly for my own use, so it might completely break itself or your system

## Setup

Add yourself to the `input` group (if it exists) and reboot:

```sh
doas usermod -aG input $USER && doas reboot
```

Build and install xappkeys.

```sh
doas apt install clang libx11-dev git && \
    git clone https://github.com/winksplorer/xappkeys && \
    doas make all install
```

Run xappkeys.

```sh
/usr/local/bin/xappkeys
```