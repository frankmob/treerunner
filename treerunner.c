
#include <errno.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include <sys/wait.h>

pid_t spawn(const char* program, char** argv, char path[PATH_MAX])
{
    pid_t ch_pid = fork();
    if (ch_pid == -1) {
        printf("fork: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (ch_pid == 0) { // child
        if (chdir(path) < 0) {
            printf("chdir: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        execvp(program, argv);
        printf("execvp: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    } else { // parent
        return ch_pid;
    }
}

void execute( char **argv, char path[])
{
    pid_t child;
    int wstatus;
    
    child = spawn(argv[0], argv, path);

    if (waitpid(child, &wstatus, WUNTRACED | WCONTINUED) == -1) {
        perror("waitpid");
        exit(EXIT_FAILURE);
    }
}

void listdir(const char *name, int depth, char *argv[], int argc)
{
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(name)))
        return;

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            char path[PATH_MAX];
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
            if (argc > 0) {
                execute(argv, path);
            } else {
               printf("%s\n", path);
            }
            listdir(path, depth, argv, argc);
        }
    }
    closedir(dir);
}

int main(int argc, char *argv[]) {
    argc--;
    argv++;
    
    if (argc > 0) {
        execute(argv, "./");
    } else {
       printf("no command given\n"); 
    }

    listdir(".", 0, argv, argc);
    exit(EXIT_SUCCESS);
}
