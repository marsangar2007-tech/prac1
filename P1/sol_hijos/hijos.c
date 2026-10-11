#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

pid_t pid_sp, pid;

void despierta() {}



int crearMemoria(int elementos) {
    return shmget(IPC_PRIVATE, sizeof(int) * elementos, IPC_CREAT | 0666);
}

int *conectarMemoria(int vector) {
    return (int *) shmat(vector, 0, 0);
}

void desconectarMemoria(int *vector) {
    shmdt(vector);
}

void borrarMemoria(int vector) {
    shmctl(vector, IPC_RMID, NULL);
}



void imprimirSuperPadre(int y, int *vy) {
    int i;
    printf("Soy el super padre %d, mis hijos finales son: ", getpid());
    for (i = 0; i < y; i++) {
        printf("%d", vy[i]);
        if (i != y - 1) {
            printf(", ");
        }
    }
    printf("\n");
}



/* Cadena de x hijos anidados. Devuelve el valor de i con el que sale del bucle. */
int crearCadenaHijos(int x, int *vx) {
    int i;
    for (i = 1; i <= x; i++) {
        pid = fork();
        if (pid != 0) {
            
            wait(NULL);
            break;
        } 
        
    }
    return i;
}

void imprimirSubhijo (int x, int *vx){
    printf("Soy el subhijo %d, mis padres son: ", getpid());
    for (int j = 0; j < x; j++) {
        printf("%d", vx[j]);
        if (j != x - 1) {
            printf(", ");
        }
    }
    printf("\n");
}

/* Hijos finales: los crea el último proceso de la cadena. */
void crearHijosFinales(int y, int *vy, int x, int *vx) {
    int i;
    for (i = 1; i <= y; i++) {
        pid = fork();
        if (pid == 0) {
            vy[i - 1] = getpid();
            imprimirSubhijo(x, vx);
            signal(SIGALRM, despierta);
            alarm(10);
            pause();
            break;
        }
    }
    if (i == y + 1) {
        for (i = 1; i <= y; i++) {
            wait(NULL);
        }
        
    }
}



int main(int argc, char *argv[]) {
    int i, x, y, vectorX, vectorY;
    int *vx, *vy;

    if (argc != 3) {
        printf("Error, usage: ./hijos x y\n");
        return 0;
    }

    pid_sp = getpid();
    x = atoi(argv[1]);
    y = atoi(argv[2]);

    vectorX = crearMemoria(x);
    vx = conectarMemoria(vectorX);

    vectorY = crearMemoria(y);
    vy = conectarMemoria(vectorY);

    i = crearCadenaHijos(x, vx);

    if (i == 1) {
        imprimirSuperPadre(y, vy);
    } 
    else {
        if (i == x + 1) {
        crearHijosFinales(y, vy, x, vx);
        }
    }

    desconectarMemoria(vx);
    desconectarMemoria(vy);

    // solo el super padre la borra, porque es el último en terminar
    if (i == 1) {
        borrarMemoria(vectorX);
        borrarMemoria(vectorY);
    }

    return 0;
}  