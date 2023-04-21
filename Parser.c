#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <regex.h>

#define BUFFERSIZE 5000
#define NUM_THREADS 1
#define MAX_LINE_LENGTH 100
#define expresion "war"
#define texto "wap1200.txt"

// Global
int pointerByte = 0;
pthread_mutex_t mutex,mutex2;

struct thread_data {
  int thread_id;
  FILE *file;
};

struct thread_data thread_data_array[NUM_THREADS];


void procesar(char * buffer,int id){
    
    
    int test = 0;
    int status2;
    regex_t regex;
    int reti;
    
    char *line = strtok(buffer, "\n");
    while (line != NULL) {
    	
        //printf("%s\n", line);
        reti = regcomp(&regex, expresion, 0);
	if (reti) {
	    status2 = pthread_mutex_lock(&mutex2);
	    fprintf(stderr, "Could not compile regex\n");
	    status2 = pthread_mutex_unlock(&mutex2);
  	    //return 1;
	}
	   
	// Execute regular expression
	reti = regexec(&regex, line, 0, NULL, 0);
	if (!reti) {
	    status2 = pthread_mutex_lock(&mutex2);
	    printf("\n-----\n");
	    printf("Thread id: %d found match: %s: \n",id,line);
            //printf(line);
            printf("\n-----\n");
            status2 = pthread_mutex_unlock(&mutex2);
	    test++;
	} 
	else if (reti == REG_NOMATCH) {
		
		//printf("No match found\n");
	} 
	else {
	    regerror(reti, &regex, line, sizeof(line));
	    status2 = pthread_mutex_lock(&mutex2);
	    fprintf(stderr, "Regex match failed: %s\n", line);
	    status2 = pthread_mutex_unlock(&mutex2);
	    //return 1;
	}
	    
	// Free compiled regular expression
	regfree(&regex);
	//break;
        line = strtok(NULL, "\n");
        
    }
    //printf("\nThread: %d , Coincidencias: %d\n", id,test);
    
}

int line_size(char * linea){
	int size=0;
	for(int i = 0; i < MAX_LINE_LENGTH && linea[i] != '\0'; i++){
		size++;
	}
	return size;
}

void leer(FILE *file,int id) {
     int status;
     char line[MAX_LINE_LENGTH];
     
     char buffer[BUFFERSIZE];
     //limpiar el buffer
     for(int j=0;j<BUFFERSIZE-1;j++){
      buffer[j]=0;
    }
     status = pthread_mutex_lock(&mutex);
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
     
     status = pthread_mutex_unlock(&mutex);
     //printf("%s",buffer);
     
     //voy a llamar a procesar desde aca por ahora
     //despues decidimos si devolvemos buffer a parse
     //para modularlo bien... esto es solo prueba
     procesar(buffer,id);
     sleep(1);
     //return buffer;
}

void *parse(void *threadarg) {

  struct thread_data *my_data;

  
  int id;
  FILE *file;

  my_data = (struct thread_data *)threadarg;
  id = my_data->thread_id;
  file = my_data->file;

  //while (!EOF){}
  //MUTEX BLOQUEA
  //leer(file, buffer);
  //char buffer[BUFFERSIZE];
  //leer(file,buffer);
  int c = getc(file);
  while (c != EOF) {
      leer(file,id);
      c = getc(file);
   }
  //leer(file);
  //printf(buffer);
  //MUTEX LIBERA
  //PROCESAMIENTO
  
  
  pthread_exit(NULL);
}

int main() {
  printf("\nBuscando expresion regular: %s \n",expresion);
  int status,status2;
  //arriba esta el define de "texto" para cambiarlo + facil
  FILE *fp = fopen(texto, "r");
  pthread_t threads[NUM_THREADS];
  status = pthread_mutex_init(&mutex,NULL);
  status2 = pthread_mutex_init(&mutex,NULL);

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
    status2 = pthread_join(threads[i],NULL);
  }
  status = pthread_mutex_destroy(&mutex);
  status2 = pthread_mutex_destroy(&mutex2);
  
  fclose(fp);
  return 0;
}
