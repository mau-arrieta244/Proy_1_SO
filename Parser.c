#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//Global
int pointerByte = 0;

void leer(int puntero){
   FILE *fp = fopen("DonQuixote.txt", "r");
   fseek(fp, puntero, SEEK_SET); // Indicador de posicion en el archivo, donde "iniciar" a leer
   char buffer[250]={0};
   int num_bytes_read;
   int num_lines = 0;
    
   num_bytes_read = fread(buffer, sizeof(char), 250, fp);
   for (int i = 0; i < num_bytes_read; i++) {
      if (buffer[i] == '\n') {
         num_lines++;
      }
   }
   
   if (num_bytes_read > 0) {
      fwrite(buffer, sizeof(char), num_bytes_read, stdout);
   }
   fclose(fp);
   pointerByte += num_bytes_read += num_lines; // Update pointerByte with the number of bytes read
}

int main() {
    leer(pointerByte);
    leer(pointerByte); 
    leer(pointerByte);

    return 0;
}
