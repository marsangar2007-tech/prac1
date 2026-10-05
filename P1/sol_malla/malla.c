#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void manejador(int s){}

int main(int argc, char* argv[]){
    int x,y,i,j;
    pid_t pid_x,pid_y,pid;

    if(argc!=3){
        printf("Se necesita el número de filas y columnas\n");
        exit(1);
    }

    x=atoi(argv[1]);
    y=atoi(argv[2]);

    for(i=0;i<y;i++){
        pid_y=fork();
        if(pid_y==0){
            break;
        }
    }
    
    pid_x=-1;
    for(j=0;j<x-1;j++){
        if(pid_y==0){  // si es el primer proceso de la columna (y) y queremos que actúe
            pid_x=fork();          // el padre en cada llamada al fork() de pid_x (las filas)
            if(pid_x!=0){
                break;
            }
        }
    }

    if(pid_y==0){
        if(j==x-1){
            
            /* soy el proceso pxj: me quedo esperando una señal en vez de dormir */
            signal(SIGUSR1, manejador);
            printf("Proceso con pid %d esperando señal...\n", getpid());
            pause();
            exit(0); //los hijos que ya han completado todo el bucle finalizan
        }
        else{
            wait(NULL);     // los padres de algún hijo (procesos intermedios) esperan a que finalice el hijo.
            exit(0);        // cuándo el hijo acaba terminan también.
        }
       
    }
    else{
        for(i=0;i<y;i++){  //para los hijos iniciales
            wait(NULL);
        }
    }
    

    


    
    

}