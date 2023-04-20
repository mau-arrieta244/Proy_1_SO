#include <stdio.h>
#include <stdlib.h>
#include <regex.h>

 
#define MAX_LINE_LENGTH 1000
 
int main() {
    FILE    *textfile;
    char    line[MAX_LINE_LENGTH];
    int test = 0;
    regex_t regex;
    int reti;
     
    textfile = fopen("DonQuixote.txt", "r");
    if(textfile == NULL)
        return 1;
        
    /*
    while(i < lineasUsadas){
    	fgets(line, MAX_LINE_LENGTH, textfile)
    	i++;
    
    }
    */
        
        
    //fseek(textfile, 2, SEEK_SET); // Indicador de posicion en el archivo 
    while(fgets(line, MAX_LINE_LENGTH, textfile)){
    
    	//COMPARACION DEL LARGO DE LINE, SI EL LARGO DE LINE ES MAYOR AL BUFFER RESTANTE
    	//ENTONCES YA NO SE TRABAJA ESA LINEA Y NO SE AUMENTA EL CONTADOR DE LINEAS
    	
    	//SINO, ALMACENA LA LINEA Y SIGUE
    
        printf(line);
        /*reti = regcomp(&regex, "El ingenioso hidalgo", 0);
	    if (reti) {
		fprintf(stderr, "Could not compile regex\n");
		return 1;
	    }
	    
	    // Execute regular expression
	    reti = regexec(&regex, line, 0, NULL, 0);
	    if (!reti) {
		printf("Match found: Quijote\n");
		printf(line);
		test++;
	    } else if (reti == REG_NOMATCH) {
		//printf("No match found\n");
	    } else {
		regerror(reti, &regex, line, sizeof(line));
		fprintf(stderr, "Regex match failed: %s\n", line);
		return 1;
	    }
	    
	    // Free compiled regular expression
	    regfree(&regex);*/
	    break;
    }
     
    fclose(textfile);
    printf("Number of coincidencias: %d", test);
    return 0;
}
