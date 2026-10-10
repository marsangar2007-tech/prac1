#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

void manejador(int s){}




pid_t crearColumnas(int y){
    int i;
    pid_t pid_y = -1;

    for(i=0;i<y;i++){
        pid_y=fork();
        if(pid_y==0){
            break;
        }
    }
    return pid_y;
}


int crearFilas(int x, pid_t pid_y){
    int j;
    pid_t pid_x=-1;

    for(j=0;j<x-1;j++){
        if(pid_y==0){
            pid_x=fork();
            if(pid_x!=0){
                break;
            }
        }
    }
    return j;
}



void comportamientoHoja(void){
    signal(SIGUSR1, manejador);   // la hoja sí reacciona a la señal
    pause();
    printf("Hoja %d muere\n", getpid());
    exit(0);
}

void comportamientoIntermedio(void){
    wait(NULL);                   // SIG_IGN no interrumpe el wait
    printf("Intermedio %d muere\n", getpid());
    exit(0);
}

void mostrarArbol(void){
    char cadPadre[20];
    pid_t pid_cmd;

    sprintf(cadPadre, "%d", getpid());
    pid_cmd = fork();
    if(pid_cmd == 0){
        execlp("pstree", "pstree", "-c", cadPadre, NULL);
        exit(1);                  // solo si execlp falla
    }
    wait(NULL);                   // espera a que acabe el pstree
}

void comportamientoPadre(int y){
    int i;

    signal(SIGALRM, manejador);
    alarm(1);
    pause();                      // da tiempo a crear el árbol 

    mostrarArbol();

    kill(0, SIGUSR1);             // señal a todo el grupo: solo las hojas 
                                   //(último proceso de la rama)la tratan

    for(i=0;i<y;i++){
        wait(NULL);
    }
    printf("Padre %d muere el último\n", getpid());
}



int main(int argc, char* argv[]){
    int x,y,j;
    pid_t pid_y;

    if(argc!=3){
        printf("Se necesita el número de filas y columnas\n");
        exit(1);
    }

    x=atoi(argv[1]);
    y=atoi(argv[2]);

    signal(SIGUSR1, SIG_IGN);     // Para que solo hagan SIGUSR1 las hojas.

    pid_y=crearColumnas(y);
    j=crearFilas(x, pid_y);

    if(pid_y==0){
        if(j==x-1){
            comportamientoHoja();
        }
        else{
            comportamientoIntermedio();
        }
    }
    else{
        comportamientoPadre(y);
    }
    return 0;
}