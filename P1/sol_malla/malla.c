#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

void manejador(int s){}

int main(int argc, char* argv[]){
    int x,y,i,j;
    pid_t pid_x,pid_y,pid_cmd;
    char cadPadre[20];

    if(argc!=3){
        printf("Se necesita el número de filas y columnas\n");
        exit(1);
    }

    x=atoi(argv[1]);
    y=atoi(argv[2]);

    signal(SIGUSR1, SIG_IGN);   // todos ignoran SIGUSR1 (se hereda); solo las hojas la capturarán

    for(i=0;i<y;i++){
        pid_y=fork();
        if(pid_y==0){
            break;
        }
    }
    pid_x=-1;
    for(j=0;j<x-1;j++){
        if(pid_y==0){
            pid_x=fork();
            if(pid_x!=0){
                break;
            }
        }
    }

    if(pid_y==0){
        if(j==x-1){
            signal(SIGUSR1, manejador);   // la hoja sí reacciona a la señal
            pause();
            printf("Hoja %d muere\n", getpid());
            exit(0);
        }
        else{
            wait(NULL);                   // SIG_IGN no interrumpe el wait
            printf("Intermedio %d muere\n", getpid());
            exit(0);
        }
    }
    else{
        signal(SIGALRM, alarm);
        alarm(1);
        pause();                   // da tiempo a crear el árbol y a que las hojas instalen su manejador

        sprintf(cadPadre, "%d", getpid());
        pid_cmd = fork();
        if(pid_cmd == 0){
            execlp("pstree", "pstree", "-c", cadPadre, NULL);
            exit(1);                      // solo si execlp falla
        }
        wait(NULL);                       // espera a que acabe el pstree

        kill(0, SIGUSR1);                 // señal a todo el grupo: solo las hojas la tratan

        for(i=0;i<y;i++){
            wait(NULL);
        }
        printf("Padre %d muere el último\n", getpid());
    }
    return 0;
}

