#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

void print_help() {
    printf("How to use: oss [-h] [-n proc] [-s simul] [-t iter]\n");
    printf("  -h      Show help message\n");
    printf("  -n proc Total number of children to launch\n");
    printf("  -s simul Maximum number of children to run simultaneously\n");
    printf("  -t iter Number of iterations to pass to the user process\n");
}

int main(int argc, char *argv[]) {
    int totalChildren = 0;
    int maxSimul = 0;
    int iterations = 0;
    const char optstr[] = "hn:s:t:";
    char opt;
    pid_t pid;
    int childrenLaunched = 0;
    int childrenRunning = 0;
    
    while ((opt = getopt(argc, argv, optstr)) != -1) {
        switch (opt) {
            case 'h':
                print_help();
                return 0;
            case 'n':
                totalChildren = atoi(optarg);
                break;
            case 's':
                maxSimul = atoi(optarg);
                break;
            case 't':
                iterations = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Invalid option\n");
                print_help();
                return 1;
        }
    }
    
    if (totalChildren <= 0 || maxSimul <= 0 || iterations <= 0) {
        fprintf(stderr, "Parameters (-n, -s, -t) must be positive integers\n");
        print_help();
        return 1;
    }
    
    printf("OSS: Launching %d children, %d simultaneous, with %d iterations\n",
           totalChildren, maxSimul, iterations);
    
    while (childrenLaunched < totalChildren && childrenRunning < maxSimul) {
        pid = fork();
        if (pid == 0) {
            char strArg[20];
            char *args[] = {"./user", strArg, NULL};
            execvp(args[0], args);
        } else if (pid > 0) {
            childrenLaunched++;
            childrenRunning++;
            printf("OSS: Launched child %d (PID: %d), total running: %d\n",
                   childrenLaunched, pid, childrenRunning);
        } else {
            perror("fork failed");
            exit(1);
        }
    }
    
    while (childrenLaunched < totalChildren) {
        int status;
        pid_t childpid = wait(&status);
        
        if (childpid > 0) {
            childrenRunning--;
            printf("OSS: Child %d finished, status: %d\n", childpid, WEXITSTATUS(status));
            
            pid = fork();
            
            if (pid == 0) {
                char strArg[20];
                char *args[] = {"./user", strArg, NULL};
                execvp(args[0], args);
            } else if (pid > 0) {
                childrenLaunched++;
                childrenRunning++;
                printf("OSS: Launched child %d (PID: %d), total running: %d\n",
                       childrenLaunched, pid, childrenRunning);
            } else {
                perror("fork failed");
                exit(1);
            }
        }
    }
    
    printf("OSS: All children launched, waiting for remaining %d to finish...\n", childrenRunning);
    
    int status;
    pid_t childpid;
    while ((childpid = wait(&status)) > 0) {
        childrenRunning--;
        printf("OSS: Child %d finished, status: %d. Remaining: %d\n",
               childpid, WEXITSTATUS(status), childrenRunning);
    }
    printf("OSS: Summary - Launched %d children total\n", totalChildren);
    return 0;
}