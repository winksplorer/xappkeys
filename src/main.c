#include <stdio.h>
#include <stdlib.h>

#include "xak_x11.h"

int main(void)
{
    printf("xak_x11_init: ");
    fflush(stdout);

    if (xak_x11_init()) {
        exit(1);
    }

    printf("ok\n");
}
 