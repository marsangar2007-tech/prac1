#include <signal.h> 
void alarma() { 
    printf("acabo de recibir un SIGALRM\n"); 
    } 
main() { 
    signal(SIGALRM,alarma);  
    printf("Acabo de programar la captura de un SIGALRM\n"); 
    alarm(3); 
    printf("Ahora he programado la alarma en 3 seg.\n"); 
    pause(); 
    printf("Ahora continúo con la ejecución normal\n"); 
} 