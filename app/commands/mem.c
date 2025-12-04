#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "shell.h"
#include "main.h"

int cmd_mem(int argc, char *argv[])
{
    int size = atoi(argv[1]);
    uint8_t *ptr = malloc(size);
 
    shellPrint(shellGetCurrent(), "Allocated %d bytes at address %p\n", size, ptr);

    free(ptr);

    return 0;
}

