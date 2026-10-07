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

int main(int argc, char *argv[]){
	if(argc != 3){	
		printf("Error. Uso: %s fichero tam_trozo\n", argv[0]);
	}
	else{
		partir(argv[1], atoi(argv[2])); // nombre del fichero y tamaño.
	}
	return 0;
}



void partir(char nombre[], int tamanyo){
	int i;	
	int canal[2]; // = {lectura, escritura}
	char ficheroTrozo[50];
	int entrada, salida; 
	struct stat propiedades; 	// propiedades del fichero
	int 	totalTrozos, 		// trozos que se van a generar 
		bytesLeidos; 		// bytes leidos de la tuberia
 	char *buffer;		

	buffer = (char *) malloc(sizeof(char) * tamanyo);	// es el vector de caracteres (bytes) que utiliza el padre para leer del fichero original.
	entrada = open(nombre, O_RDONLY); // PADRE: ABRE EL FICHERO EN MODO EN LECTURA.
	if(entrada < 0){
		printf("Error. No existe el fichero\n");
	}
	else{
		stat(nombre, &propiedades);	// le paso a stat la direccion de la variable propfichero para que me la rellene con la info del fichero.
		totalTrozos = propiedades.st_size/tamanyo;	// el numero de trozos a leer = total del fichero / total del trozo
		if(propiedades.st_size % tamanyo != 0){
			totalTrozos++; 	// si el tamaño del fichero no es multiplo del tamaño del trozo, hace falta un trozo para el resto... :)
		}
		for(i = 0; i < totalTrozos; i++){
			pipe(canal); 		// crea la tuberia distinta para cada hijo.
			if(fork() != 0){
				bytesLeidos = read(entrada, buffer, tamanyo); 	// el padre lee del fichero
				write(canal[1], buffer, bytesLeidos);			// y escribe en el tubo.
			}
			else{ 
				ejecutarHijo(canal, nombre, i, buffer, tamanyo);
				break;
			}
		}
		if(i == totalTrozos){
			free(buffer);
			// bucle en el que el padre espera a los hijos.
			esperarHijos(totalTrozos);
		}
	}
}


void prepararNombreTrozo(char *ficheroTrozo, char *nombre, int i){
	// creo el nombre del fichero nombre.h00, nombre
	if(i < 10){ // si es menor que 10, le pongo un 0 delante.
		sprintf(ficheroTrozo, "%s.h0%d", nombre, i);
	}
	else{
		sprintf(ficheroTrozo, "%s.h%d", nombre, i);
	}
}


void ejecutarHijo(int canal[], char *nombre, int i, char *buffer, int tamanyo){
	int salida;
	int bytesLeidos;
	char ficheroTrozo[50];

	prepararNombreTrozo(ficheroTrozo, nombre, i);
	salida = creat(ficheroTrozo, 0666);	
	// aqui habria que hacer un bucle hasta que el hijo consiguiera leer
	// del tubo todo lo que tiene.
	//////////////////// bucle mientras que no lea todos los que queria leer.
	bytesLeidos = read(canal[0], buffer, tamanyo);		// lee del tubo			
	write(salida, buffer, bytesLeidos);			// escribe en el fichero
	/////////////////////
	close(salida);
	free(buffer);
}


void esperarHijos(int totalTrozos){
	for(int i = 0; i < totalTrozos; i++){
		wait(NULL);
	}
}


