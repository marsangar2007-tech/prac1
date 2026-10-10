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

/* ---------- Memoria compartida ---------- */

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

/* ---------- Impresión ---------- */

void imprimirAncestros(int i, int *vx) {
    printf("%d", pid_sp);
    for (int j = 0; j < i - 1; j++) {
        printf(", %d", vx[j]);
    }
    printf("\n");
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

/* ---------- Creación de procesos ---------- */

/* Cadena de x hijos anidados. Devuelve el valor de i con el que sale del bucle. */
int crearCadenaHijos(int x, int *vx) {
    int i;
    for (i = 1; i <= x; i++) {
        pid = fork();
        if (pid != 0) {
            
            wait(NULL);
            break;
        } else {
            vx[i - 1] = getpid();
            imprimirAncestros(i, vx);
        }
    }
    return i;
}

/* Hijos finales: los crea el último proceso de la cadena. */
void crearHijosFinales(int y, int *vy) {
    int i;
    for (i = 1; i <= y; i++) {
        pid = fork();
        if (pid == 0) {
            vy[i - 1] = getpid();
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

/* ---------- main ---------- */

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
        crearHijosFinales(y, vy);
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