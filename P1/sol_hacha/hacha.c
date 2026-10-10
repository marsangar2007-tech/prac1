#include <stdio.h> 
#include <sys/wait.h>
#include <sys/stat.h> 
#include <unistd.h> 
#include <stdlib.h> 
#include <string.h>
#include <fcntl.h>


void partir(char *nombre, int tamanyo);
void prepararNombreTrozo(char *ficheroTrozo, char *nombre, int i);
void ejecutarHijo(int canal[], char *nombre, int i, char *buffer, int tamanyo);
void esperarHijos(int totalTrozos);
int calcularTotalTrozos(char nombre[], int tamanyo);
int crearTrozos(int entrada, char nombre[], int totalTrozos, char *buffer, int tamanyo);
void escribirTrozoEnTuberia(int entrada, int canal[], char *buffer, int tamanyo);



int main(int argc, char *argv[]){
	if(argc != 3){	
		printf("Error. Uso: %s fichero tam_trozo\n", argv[0]);
	}
	else{
		partir(argv[1], atoi(argv[2])); 
	}
	return 0;
}



void partir(char nombre[], int tamanyo){
    int i;
    int entrada;
    int totalTrozos;    
    char *buffer;

    buffer = (char *) malloc(sizeof(char) * tamanyo);   
    entrada = open(nombre, O_RDONLY); 
    if(entrada < 0){
        printf("Error. No existe el fichero\n");
    }
    else{
        totalTrozos = calcularTotalTrozos(nombre, tamanyo);
        i = crearTrozos(entrada, nombre, totalTrozos, buffer, tamanyo);
        if(i == totalTrozos){
            free(buffer);
            // bucle en el que el padre espera a los hijos.
            esperarHijos(totalTrozos);
        }
    }
}

int calcularTotalTrozos(char nombre[], int tamanyo){
    struct stat propiedades;    // propiedades del fichero
    int totalTrozos;

    stat(nombre, &propiedades); 
    totalTrozos = propiedades.st_size/tamanyo;  
    if(propiedades.st_size % tamanyo != 0){
        totalTrozos++;  
    }
    return totalTrozos;
}

// Devuelve i: totalTrozos en el padre, y menor que totalTrozos en un hijo.
int crearTrozos(int entrada, char nombre[], int totalTrozos, char *buffer, int tamanyo){
    int i;
    int canal[2]; // = {lectura, escritura}

    for(i = 0; i < totalTrozos; i++){
        pipe(canal);        
        if(fork() != 0){
            escribirTrozoEnTuberia(entrada, canal, buffer, tamanyo);
        }
        else{
            ejecutarHijo(canal, nombre, i, buffer, tamanyo);
            break;
        }
    }
    return i;
}

void escribirTrozoEnTuberia(int entrada, int canal[], char *buffer, int tamanyo){
    int bytesLeidos;    

    bytesLeidos = read(entrada, buffer, tamanyo);  
    write(canal[1], buffer, bytesLeidos);           
}

void prepararNombreTrozo(char *ficheroTrozo, char *nombre, int i){
	
	if(i < 10){ 
		sprintf(ficheroTrozo, "%s.h0%d", nombre, i);
	}
	else{
		sprintf(ficheroTrozo, "%s.h%d", nombre, i);
	}
}


void ejecutarHijo(int canal[], char *nombre, int i, char *buffer, int tamanyo){
    int salida;
    int bytesLeidos;
    int totalLeidos = 0;
    char ficheroTrozo[50];

    prepararNombreTrozo(ficheroTrozo, nombre, i);
    salida = creat(ficheroTrozo, 0666);

    
    do {
        bytesLeidos = read(canal[0], buffer, tamanyo);  
        if (bytesLeidos > 0) {
            write(salida, buffer, bytesLeidos);         
            totalLeidos += bytesLeidos;
        }
    } while (bytesLeidos > 0 && totalLeidos < tamanyo);

    close(salida);
    free(buffer);
}


void esperarHijos(int totalTrozos){
	for(int i = 0; i < totalTrozos; i++){
		wait(NULL);
	}
}


