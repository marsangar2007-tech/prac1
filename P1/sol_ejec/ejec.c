#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <signal.h>

pid_t arb,a,b,x,y,z;



void pstree(){
    char cadArb[100];
    sprintf(cadArb, "%d", arb);
    pid_t comandoPstree;
    comandoPstree=fork();
    if(comandoPstree == 0){
        // el proceso se convierte en el comando.
        execlp("pstree", "pstree", "-c", cadArb, NULL);
    }
    else{
        wait(NULL); // para que el hijo acabe el comando
        kill(arb, SIGUSR1); // para que inicie la destruccion del arbo.
        pause();
    }
}

void vacio(){
    //manejador de señales
}

void destroy_ejec(){
    kill(a, SIGUSR2);
}

void destroy_A(){
    kill(b,SIGUSR2);
}



void run_processX(){
    x=fork();
    if(x==0){
        printf("Soy el proceso X mi pid es %d. Mi padre es %d. Mi abuelo es %d. Mi bisabuelo %d\n", getpid(), b, a, arb);
        signal(SIGUSR2,vacio);
        pause();
        printf("Soy X y muero\n");
        exit(0);
    }
}

void run_processY(){
    y=fork();
    if(y==0){
        printf("Soy el proceso Y mi pid es %d. Mi padre es %d. Mi abuelo es %d. Mi bisabuelo %d\n", getpid(), b, a, arb);
        signal(SIGUSR2,vacio);
        pause();
        printf("Soy Y y muero\n");
        exit(0);
    }
}

void run_processZ(int tiempo){
    z=fork();
    if(z==0){
        printf("Soy el proceso Z: mi pid es %d. Mi padre es %d. Mi abuelo es %d. Mi bisabuelo %d\n", getpid(), b, a, arb);
        signal(SIGALRM,vacio);
        alarm(tiempo);
        pause();
        kill(a,SIGUSR1); // llamo a a para que ejecute el pstree y empieze con la destrucción controlada del árbol
        signal(SIGUSR2,vacio);
        pause();
        printf("Soy Z y muero\n");
        exit(0);
    }
}




void procesoB(int tiempo){
    b=getpid();
    printf("Soy el proceso B: mi pid es %d. Mi padre es %d.Mi abuelo es %d\n",b,a,arb);

    //Ahora hago x, y z de una forma en la que gracias al pause al pause() y al exit(0)
    // se pueden hacer correctamente las ramificaciones qie pide el eninciado.

    run_processX();
    run_processY();
    run_processZ(tiempo);

    signal(SIGUSR2,vacio);
    pause();            // aqui llega desde destroy A. Importante.
    kill(x,SIGUSR2);
    wait(NULL);
    kill(y,SIGUSR2);
    wait(NULL);
    kill(z,SIGUSR2);
    wait(NULL);
    printf("Soy B y muero\n");
    exit(0);
}

// Proceso A: crea B y propaga la destrucción.
void procesoA(int tiempo){
    a=getpid();  //el pid de A
    printf("Soy el proceso A: mi pid es %d. Mi padre es %d\n",a,arb);
    b=fork();

    if(b!=0){
        signal(SIGUSR1,pstree);
        signal(SIGUSR2,destroy_A);
        pause();
        wait(NULL);
        printf("Soy A y muero\n");
        exit(0);
    }
    else{
        procesoB(tiempo);
    }
}

// Proceso ejec (el padre): espera la señal y empieza la destrucción.
void procesoEjec(){
    signal(SIGUSR1,destroy_ejec);
    pause();  //espera a que reciba el kill para ejecutar la signal

    wait(NULL); //espera a que muera un hijo
    printf("Soy ejec y muero.");
    exit(0);
}



// Devuelve 1 si los argumentos son correctos y 0 si no.
int comprobarArgs(int argc){
    if(argc!=2){
        printf("Pon dos argumentos con ./ejec y el tiempo");
        return 0;
    }
    return 1;
}



int main(int argc,char* argv[]){
    int tiempo; //el tiempo con el que se llama a A desde Z

    if(comprobarArgs(argc)){
        arb=getpid();
        printf("Soy el proceso ejec: mi pid es %d\n",arb);
        tiempo=atoi(argv[1]);

        a=fork();
        if(a!=0){
            procesoEjec();
        }
        else{  //si es el hijo:
            procesoA(tiempo);
        }
    }

    return 0;
}