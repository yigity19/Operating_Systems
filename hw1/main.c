//Rahman ve Rahim Olan Allah'ın adıyla

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(){
    int i, res, depth = 5;
    printf("grand parent pid: %d\n",getpid());
    for (i = 0; i < depth; i++){
        res = fork();
        //printf("middle parent pid: %d parent pid %d\n", getpid(), getppid());
        if (res == 0){
        depth = depth - 1;
            while(depth > 0){
                depth = depth - 1;
                res = fork();
                if(res != 0){
                    wait(NULL);
                    exit(0);
                }        
            printf("-----child pid: %d     parent pid: %d\n", getpid(), getppid());
            }
            pause();
        }

    }
    wait(NULL);
    exit(0);
}