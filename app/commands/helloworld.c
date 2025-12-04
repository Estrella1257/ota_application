#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "shell.h"

int cmd_helloworld(int argc,char *argv[])
{
    shellPrint(shellGetCurrent(),"%s\n","hello world");
    shellWriteString(shellGetCurrent(),"hello world\n");
    shellPrint(shellGetCurrent(),"we have %d parameters\n",argc);

    for(int i = 0;i < argc;i++)
    {
        shellPrint(shellGetCurrent(),"argv[%d]:%s\n",i,argv[i]);
    }

    return 0;
}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN)|SHELL_CMD_DISABLE_RETURN,helloworld,cmd_helloworld,this is a helloworld command);
   
