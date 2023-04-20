#include <pthread.h>

#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <unistd.h>


#define BUFFERSIZE 250

#define NUM_THREADS 1


// Global

int pointerByte = 0;

struct thread_data {
  int thread_id;
  FILE *file;
  //int posicion;
};

struct thread_data thread_data_array[NUM_THREADS];

void leer(FILE *file, char *buffer) {
  fseek(file, pointerByte, SEEK_SET); // Indicador de posicion en el archivo
  int num_bytes_read;
  int num_lines = 0;
  //num_bytes_read = fread(buffer, sizeof(char), BUFFERSIZE, file);
  fread(buffer, sizeof(char), BUFFERSIZE, file);
  for (int i = 0; i < BUFFERSIZE; i++) {
    if (buffer[i] == '\n') {
      num_lines++;
    }
  }

  pointerByte += BUFFERSIZE; //+= num_lines; // Update pointerByte with the number of bytes read
  
  //Corregir posicion de archivo a ultimo salto de linea
  int posiciones=0;
  int indice = BUFFERSIZE-1;
  
  while(buffer[indice]!='\n'){
         posiciones++;
         indice--;
      }   

   //Corregir pointerByte
   pointerByte-= (posiciones + 1);

   char newBuffer[BUFFERSIZE-posiciones];

   //corregir buffer (no dejar linea a medias)
   for(int j=0;j<(BUFFERSIZE-posiciones-1);j++){
      newBuffer[j] = buffer[j];
   }
   fwrite(newBuffer, sizeof(char), (BUFFERSIZE-posiciones), stdout);
}

void *parse(void *threadarg) {

  struct thread_data *my_data;

  char buffer[BUFFERSIZE];
  int id;
  FILE *file;
  int posicion;

  my_data = (struct thread_data *)threadarg;
  id = my_data->thread_id;
  file = my_data->file;
  //posicion = my_data->posicion;


  while (1){
  //posicion = pointerByte;
  //MUTEX BLOQUEA
  leer(file, buffer);
  //MUTEX LIBERA
  //PROCESAMIENTO
  }
  
  pthread_exit(NULL);
}

int main() {

  FILE *fp = fopen("WarAndPeace.txt", "r");
  pthread_t threads[NUM_THREADS];

  for (long i = 0; i < NUM_THREADS; i++) {

    //printf("In main: creating thread %ld\n", i);
    thread_data_array[i].thread_id = i;
    thread_data_array[i].file = fp;
    //thread_data_array[i].posicion = pointerByte;
    int rc = pthread_create(&threads[i], NULL, parse, (void *)&thread_data_array[i]);

    sleep(1);

    if (rc) {
      printf("ERROR; return code from pthread_create() is %d\n", rc);
      exit(-1);
    }
  }
  
  fclose(fp);
  return 0;
}
