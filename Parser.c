#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <regex.h>

#define BUFFERSIZE 5000
#define NUM_THREADS 2
#define MAX_LINE_LENGTH 100
#define expresion "war"
#define texto "wap1200.txt"
#define MAX_OUTPUT_SIZE 1000000

// Global
int pointerByte = 0;
pthread_mutex_t mutex,mutex2;
char output[MAX_OUTPUT_SIZE];

struct thread_data {
  int thread_id;
  FILE *file;
};

struct thread_data thread_data_array[NUM_THREADS];


void procesar(char * buffer){
    /*
    -Recibe un Buffer tamanno BUFFERSIZE con lineas
    -Lee linea por linea y analiza regex en c/u
    -Si encuentra match, agrega linea a output buffer (gigante)
    */
    int test = 0;
    
    int status2;
    regex_t regex;
    int reti;
    
    char *line = strtok(buffer, "\n");
    //procesa el buffer linea por linea
    while (line != NULL) {
    	
        reti = regcomp(&regex, expresion, 0);
	if (reti) {
	    //fprintf(stderr, "Could not compile regex\n");
	}
	  
	// Execute regular expression
	reti = regexec(&regex, line, 0, NULL, 0);
	if (!reti) {
	    pthread_mutex_lock(&mutex);
	    strcat(output,line);
	    strcat(output,"\n");
	    sleep(0.2);
	    pthread_mutex_unlock(&mutex);
	    //printf("\n-----\n");
	    //printf("Thread id: %d found match: %s: \n",id,line);
            //printf(line);
            //printf("\n-----\n");
	    //test++;
	} 
	else if (reti == REG_NOMATCH) {
		
		//printf("No match found\n");
	} 
	else {
	    regerror(reti, &regex, line, sizeof(line));
	    fprintf(stderr, "Regex match failed: %s\n", line);
	}
	    
	// Free compiled regular expression
	regfree(&regex);
	//siguiente linea
        line = strtok(NULL, "\n");
        
    }
    
}

int line_size(char * linea){
	/*
	tamanno en caracteres de una linea
	*/
	int size=0;
	for(int i = 0; i < MAX_LINE_LENGTH && linea[i] != '\0'; i++){
		size++;
	}
	return size;
}

char * leer(FILE *file,char * buffer) {
     /*
     -Recibe un puntero a FILE (podriamos cambiarlo para que lo abra en todas las llamadas)...
     -...y de esta manera no se comparta entre los hilos , tal vez ayuda
     -Recibe un buffer tamanno BUFFERSIZE
     -Bloquea con mutex
     -Avanza en archivo
     -Lee en archivo y va almacenando en buffer linea por linea
     -Retorna buffer a funcion principal del hilo (Parse)
     */
     
     char line[MAX_LINE_LENGTH];
     
     //char buffer[BUFFERSIZE];
     //limpiar el buffer
     for(int j=0;j<BUFFERSIZE-1;j++){
      buffer[j]=0;
    }
     
     //Empezar en inicio de archivo
     fseek(file,0, SEEK_SET);
     //avanzar en el archivo
     int posicion = 0;
     while(posicion < pointerByte){
    	fgets(line, MAX_LINE_LENGTH, file);
    	posicion++;
     }
     
     //se resetea con cada llamada a leer...
     //indicador de que tan lleno esta el buffer
     int ocupado = 0;
     
     while(fgets(line, MAX_LINE_LENGTH, file)){
     
     //COMPARACION DEL LARGO DE LINE, SI EL LARGO DE LINE ES MAYOR AL BUFFER RESTANTE
     //ENTONCES YA NO SE TRABAJA ESA LINEA Y NO SE AUMENTA EL CONTADOR DE LINEAS
     //SINO, ALMACENA LA LINEA Y SIGUE
     	     
     	     int size = line_size(line);
	     if( (BUFFERSIZE - ocupado) < size){
	     	break;	
	     }
	     
	     else{
	     	strcat(buffer,line);
	     	ocupado+=size;
	     	pointerByte++;
	     }
	     //printf(line);
        
     }
     sleep(0.3);
     return buffer;
}

void *parse(void *threadarg) {
    struct thread_data *my_data;
    int id;
    int status;
    FILE *file;
    my_data = (struct thread_data *)threadarg;
    id = my_data->thread_id;
    file = my_data->file;
    
    char buffer[BUFFERSIZE];
    
    int c = getc(file);
    while (c != EOF) {
    	//limpiar el buffer
        for(int j=0;j<BUFFERSIZE-1;j++){
            buffer[j]=0;
        }
        status = pthread_mutex_lock(&mutex);
        leer(file,buffer);
        status = pthread_mutex_unlock(&mutex);
        procesar(buffer);
        c = getc(file);
    }
    pthread_exit(NULL);
}

int main() {
  printf("\nBuscando expresion regular: %s \n",expresion);
  int status,status2;
  //arriba esta el define de "texto" para cambiarlo + facil
  FILE *fp = fopen(texto, "r");
  pthread_t threads[NUM_THREADS];
  status = pthread_mutex_init(&mutex,NULL);

  for (long i = 0; i < NUM_THREADS; i++) {

    thread_data_array[i].thread_id = i;
    thread_data_array[i].file = fp;
    int rc = pthread_create(&threads[i], NULL, parse, (void *)&thread_data_array[i]);
    //sleep(1);

    if (rc) {
      printf("ERROR; return code from pthread_create() is %d\n", rc);
      exit(-1);
    }
  }
  
  for (int i=0; i<NUM_THREADS; i++){
    status = pthread_join(threads[i],NULL);
  }
  
  
  status = pthread_mutex_destroy(&mutex);
  fclose(fp);
  //cuando ya los hilos terminaron y agregaron al output su texto procesado...
  printf("%s \n",output);
  return 0;
}
