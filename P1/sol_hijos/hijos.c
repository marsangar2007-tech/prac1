#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

pid_t pid_sp, pid;

int crearMemoria(int elementos){
    return shmget(IPC_PRIVATE, sizeof(int) * cantidad, IPC_CREAT | 0666);
}

int *conectarMemoria(int vector){
   return  (int *) shmat(shmid, 0, 0);
}


int main(int argc, char* argv){
        int i, x, y, vectorX,vectorY;
        int *vx, *vy;

        if(argc != 3){
            printf("Error, usage: ./hijos x y\n");
        }

        else{
            pid_sp=getpid();
            x=atoi(argv[1]);
            y=atoi(argv[2]);

            vectorX=crearMemoria(x);
            vx=conectarMemoria(vx);

            vectorY=crearMemoria(y);
            vy=conectarMemoria(vy);
        }


}