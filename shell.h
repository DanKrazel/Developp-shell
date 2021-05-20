/* Compile with: g++ -Wall –Werror -o shell shell.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include<readline/readline.h>
#include<readline/history.h>
#define READ  0
#define WRITE 1
#define m 10000
#ifdef __unix__
//std::ios_base::eof()
#define IS_POSIX 1
    #define _BSD_SOURCE
#else
    #define IS_POSIX 0
#endif


/* The array below will hold the arguments: args[0] is the command. */
static char* args[512];
pid_t pid;
int command_pipe[2];
static char line[1024];
static char text[1024];
static char copy[1024];
static int n = 0; /* number of calls to 'command' */
static int run(char* cmd, int input, int first, int last);
static void split(char* cmd);

static void NWC(){

    FILE *fp;
    fp = fopen(text,"r");
    char c;
    int count = 0;
    // declare variables
    int linc, word, ch;

    // initialize count variables with zero
    linc = word = ch = 0;

    // read multiline string

    // check every character
    for (c = getc(fp); c != EOF; c = getc(fp)){
        // if it is new line then
        // one line and one word completed
        if(c=='\n')
        {
            linc++;
            word++;
        }

            // else it is a character
        else
        {
            // if character is space or tab
            // then one word is also completed
            if(c==' '||c=='\t')
            {
                word++;
                ch++;
            }

                // it was not '\n', sapace or tab
                // it is a normal character
            else {
                ch++;
            }
        }
    }

    // display count values
    for (int j=0;j<sizeof line;j++){
        if(line[j]=='\0')
            break;
        if(line[j]=='-' &&(line[j+1]=='l')){
            printf("Line counts = %d\n", linc);
        }
        else if(line[j]=='-' &&(line[j+1]=='w')){
            printf("Word counts = %d\n", word);
        }
        else if (line[j]=='-' &&(line[j+1]=='c')){
            printf("\nCharacter counts = %d\n", ch);
        }
    }

    fclose (fp);
}

static void cp(){
    FILE *fptr1, *fptr2;
    char filename[100],c;
    // Open one file for reading
    fptr1 = fopen(text, "r");
    if (fptr1 == NULL)
    {
        printf("Cannot open file %s \n", text);
        exit(0);
    }

    // Open another file for writing
    fptr2 = fopen(copy, "w");
    if (fptr2 == NULL)
    {
        printf("Cannot open file %s \n", copy);
        exit(0);
    }

    // Read contents from file
    c = fgetc(fptr1);
    while (c != EOF)
    {
        fputc(c, fptr2);
        c = fgetc(fptr1);
    }

    printf("\nContents copied to %s \n", copy);

    fclose(fptr1);
    fclose(fptr2);

}

static void Cat(){
    char c[1000];
    FILE *fptr;
    if ((fptr = fopen(text, "r")) == NULL) {
        printf("Error! opening file\n");
        // Program exits if file pointer returns NULL.
        exit(1);
    }
 
    // reads text until newline is encountered
    fscanf(fptr, "%[^\n]", c);
    printf("Data from the file:\n%s", c);
    fclose(fptr);
}

static void NCat(){
    char str[100];
    FILE *fp;
    fp = fopen (text,"w");
    for(int i = 0; i < m+1;i++)
    {
       fgets(str, sizeof str, stdin);
       fseek(fp, 0, SEEK_CUR);   /* Add this line */
       fputs(str, fp);
       //printf("%s",str);
       if((str[0]=='^')&&(str[1]=='C')&&(str[2]=='\n'))
           break;
    }
    fclose (fp);
}

static void NameFile(){
    memset(text, '\0', sizeof(text));
    int count=0;
    for (int j=0;j<sizeof line;j++){
        if(line[j]=='\0')
            break;
        if(line[j]=='>'){
            for (int k=j+2;k<sizeof line;k++){
                if(line[k]=='\n')
                    break;
                text[count]= line[k];
                count++;
            }
        }
        if(line[j]=='t' && line[j+2]=='\n'){
            for (int k=j+2;k<sizeof line;k++){
                if(line[k]=='\n')
                    break;
                text[count]= line[k];
                count++;
            }
        }
        if(line[j]=='-' &&(line[j+1]=='l'||line[j+1]=='w'||line[j+1]=='c' )){
            for (int k=j+3;k<sizeof line;k++){
                if(line[k]=='\n')
                    break;
                text[count]= line[k];
                count++;
            }
        }
        if(line[j]=='c' && line[j+1]=='p') {
            int countff= 0;
            for (int k = j + 3; k < sizeof line; k++) {
            //    for (k != 'EOF'; c = getc(fp)){
                if (line[k-1] == ' ' && count!=0 && countff!=0)
                    break;
                if (line[k] != ' ' && ((line[k+1] >= 'a' && line[k+1] <= 'z') || (line[k+1]>= 'A' && line[k+1]<= 'Z')||line[k+1]=='.'||line[k+1]==' ' )){
                    if (line[k] == ' ' && count!=0)
                        break;
                    text[count] = line[k];
                    count++;

                }
                else {
                    for (int g = k + 1; g < sizeof line; g++) {
                        if (line[g] == '\n')
                            break;
                        copy[countff] = line[g];
                        countff++;
                    }
                }
            }
        }
    }
}

static int command(int input, int first, int last)
{
    int pipettes[2];

    /* Invoke pipe */
    pipe( pipettes );
    pid = fork();

    if (pid == 0) {
        if (first == 1 && last == 0 && input == 0) {
            // First command
            dup2( pipettes[WRITE], STDOUT_FILENO );
        } else if (first == 0 && last == 0 && input != 0) {
            // Middle command
            dup2(input, STDIN_FILENO);
            dup2(pipettes[WRITE], STDOUT_FILENO);
        } else {
            // Last command
            dup2( input, STDIN_FILENO );
        }

        if (execvp( args[0], args) == -1)
            _exit(EXIT_FAILURE); // If child fails
    }

    if (input != 0)
        close(input);

    // Nothing more needs to be written
    close(pipettes[WRITE]);

    // If it's the last command, nothing more needs to be read
    if (last == 1)
        close(pipettes[READ]);

    return pipettes[READ];
}

static void cleanup(int n)
{
    int i;
    for (i = 0; i < n; ++i)
        wait(NULL);
}


static int run(char* cmd, int input, int first, int last)
{
    split(cmd);
    if (args[0] != NULL) {
        if (strcmp(args[0], "exit") == 0)
            exit(0);
        n += 1;
        return command(input, first, last);
    }
    return 0;
}

static char* skipwhite(char* s)
{
    while (isspace(*s)) ++s;
    return s;
}

static void split(char* cmd)
{
    cmd = skipwhite(cmd);
    char* next = strchr(cmd, ' ');
    int i = 0;

    while(next != NULL) {
        next[0] = '\0';
        args[i] = cmd;
        ++i;
        cmd = skipwhite(next + 1);
        next = strchr(cmd, ' ');
    }

    if (cmd[0] != '\0') {
        args[i] = cmd;
        next = strchr(cmd, '\n');
        next[0] = '\0';
        ++i;
    }

    args[i] = NULL;
}

static void cd()
{

  chdir("/home/dan/Bureau/HW2_shell/dir");
}

void printDir()
{
    char cwd[1024];
    char* username = getenv("USER");
    getcwd(cwd, sizeof(cwd));
    printf("\n%s:%s>>", username,cwd);
}





