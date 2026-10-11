#include <string.h> 
 
main(int argc, char *argv[], char *env[]) { 
   int df, cont=0; 
   char buffer[128]; 
 
   df=creat("variables.txt", 0755); 
   if (df<0) { 
      perror("Error al crear archivo"); 
      exit(-1); 
   } 
   while (env[cont] != NULL) { 
      strcpy(buffer, env[cont]); 
      strcat(buffer, "\n"); 
      if (write(df, buffer, strlen(buffer)) != strlen(buffer)) { 
         perror("Error al escribir"); 
         break; /* NO SIGUE */ 
      } 
      cont++; 
   } 
   close(df); 
} 