#include "stdio.h"
#include "string.h"

void execute_command(const char* cmd)
{
    if (strcmp(cmd, "help") == 0)
    {
        printf("help clear about version echo\n");
    }
    else if (strcmp(cmd, "clear") == 0)
    {
        printf("\033[2J");
    }
    else if (strcmp(cmd, "about") == 0)
    {
        printf("SibiOS - Custom OS\n");
    }
    else if (strcmp(cmd, "version") == 0)
    {
        printf("v1.0\n");
    }
    else if (strncmp(cmd, "echo ", 5) == 0)
    {
        printf("%s\n", cmd + 5);
    }
    else
    {
        printf("Unknown command\n");
    }
}
