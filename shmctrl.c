#include <stdio.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <stdlib.h>

int main() {

    char c;
    int shmid;
    char *shm;

    if ((shmid = shmget(4872, 1, IPC_CREAT | 0666)) < 0) {
        perror("shmget()");
        exit(EXIT_FAILURE);
    }

    if ((shm = (char*)shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat()");
        exit(EXIT_FAILURE);
    }
    else {
        *shm = 'n';
    }

    while(*shm != 'q') {
        c = getchar();
        if (c == 's') *shm = 's';
        if (c == 'n') *shm = 'n';
        //putchar(*shm);
    }

    return 0;
}