#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include <sys/wait.h>

pid_t spawnChild(const char* program, char** argv)
{
    pid_t ch_pid = fork();
    if (ch_pid == -1) 
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (ch_pid == 0) 
    {
        execvp(program, argv);
        perror("execve");
        exit(EXIT_FAILURE);
    } else {
        return ch_pid;
    }
}

void execute( char **argv)
{
    pid_t child;
    int wstatus;
    child = spawnChild(argv[0], argv);

    if (waitpid(child, &wstatus, WUNTRACED | WCONTINUED) == -1) 
    {
        perror("waitpid");
        exit(EXIT_FAILURE);
    }
    printf("finished process with pid - %d\n", child);
}

void listdir(const char *name, int depth, char *argv[], int argc)
{
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(name)))
        return;

    while ((entry = readdir(dir)) != NULL) 
    {
        if (entry->d_type == DT_DIR) 
        {
            char path[PATH_MAX];
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
            printf("in [%s]\n", path);
            if (argc > 1)
            {
                chdir(path);
                execute(argv);
            }

            listdir(path, depth, argv, argc);
        }
    }
    closedir(dir);
}

int main(int argc, char *argv[]) {
    argc--;
    argv++;
    if (argc > 1)
    {
        execute(argv);
    }
    else
    {
       printf("no command given\n"); 
    }
    listdir(".", 0, argv, argc);
    exit(EXIT_SUCCESS);
}