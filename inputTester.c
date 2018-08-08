#include<stdio.h>
#include<stdlib.h>

#define NSTATI 1000

int main(int argc, char *argv[]){
  int i, j;
  char *str = NULL;
  FILE *fp = NULL;

  char *temp = NULL;
  size_t len, nread;

  int stato;
  char carattere;

  int matrice[NSTATI][256];

  if(argc != 2){
    fprintf(stderr, "Errore chiamata programma\n");
    return -1;
  }

  str = argv[1];
  fp = fopen(str, "r");

  if(!fp){
    fprintf(stderr, "Errore apertura file\n");
    return -1;
  }

  for(i = 0; i < NSTATI; i++)
    for(j = 0; j < 256; j++)
      matrice[i][j] = 0;

  nread = getline(&temp, &len, fp); //contiene 'tr\n\0'
  nread = getline(&temp, &len, fp); //contiene la prima transizione
  while(strcmp(temp, "acc\n") != 0){
    sscanf(temp, "%d%*c%c", &stato, &carattere);
    matrice[stato][(int)carattere] = matrice[stato][(int)carattere] + 1;
    nread = getline(&temp, &len, fp);
  }

  for(i = 0; i < NSTATI; i++)
    for(j = 0; j < 256; j++)
      if(matrice[i][j] > 0)
        printf("Dallo stato %d, leggendo %c, ci sono %d transizioni\n", i, (char)j, matrice[i][j]);

  fclose(fp);
  return 0;
}
