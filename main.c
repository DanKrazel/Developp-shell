/* Compile with: g++ -Wall –Werror -o shell shell.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "shell.h"
#define READ  0
#define WRITE 1
#define m 10000
#ifdef __unix__
/*std::ios_base::eof()*/
#define IS_POSIX 1
    #define _BSD_SOURCE
#else
    #define IS_POSIX 0
#endif



int main()
{
    printf("--------------------------------SIMPLE SHELL--------------------------------\n");
    printf("-----------------------Type 'exit' or send EOF to exit----------------------\n");
    while (1) {
        /* Print the command prompt */
        //path_B();
        printDir();
        fflush(NULL);
        /* Read a command line */

        if (!fgets(line, 1024, stdin))
            return 0;


        int input = 0;
        int first = 1;

        char* cmd = line;
        char* next = strchr(cmd, '|'); /* Find first '|' */
        char *s=next;

        while (next != NULL) {
            /* 'next' points to '|' */
            *next = '\0';
            input = run(cmd, input, first, 0);
            cmd = next + 1;
            printf("%s",cmd);
            next = strchr(cmd, '|'); /* Find next '|' */
            first = 0;
        }
        int count=0;
        if ((line[0]=='c')&&(line[1]=='a')&&(line[2]=='t') && (line[4]=='>')){
            NameFile();
            NCat();
            count=0;
        }
        if((line[0]=='c')&&(line[1]=='a')&&(line[2]=='t') && line[3]!=' '){
            NameFile();
            Cat();
            count=0;

        }
        if ((line[0]=='w')&&(line[1]=='c')&&(line[2]==' ')){
            NameFile();
            NWC();
            count=0;
        }
        if ((line[0]=='c')&&(line[1]=='p')&&(line[2]==' ')){
            NameFile();
            cp();
            count=0;
        }
        
        if ((line[0]=='c')&&(line[1]=='d')&&(line[2]==' ')){
            cd();
        }
        input = run(cmd, input, first, 1);
        cleanup(n);
        n = 0;
    }
    return 0;
}
