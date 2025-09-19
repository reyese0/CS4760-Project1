#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int iterations = atoi(argv[1]);
    
    for (int i = 1; i <= iterations; i++) {
        pid_t pid = getpid();
        pid_t ppid = getppid();
        
        printf("USER PID:%d PPID:%d Iteration:%d before sleeping\n", pid, ppid, i);
        sleep(1);
        printf("USER PID:%d PPID:%d Iteration:%d after sleeping\n", pid, ppid, i);
    }
    return 0;
}