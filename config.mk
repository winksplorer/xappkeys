OUT = xappkeys
PREFIX = /usr/local

CC = cc
LD = $(CC)

USED_CFLAGS = $(CFLAGS) -std=c99 -Wall -Wextra -pedantic -Werror -D_POSIX_C_SOURCE=200809L
USED_LDFLAGS = $(LDFLAGS) -lX11