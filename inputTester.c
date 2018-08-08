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

  int statoMax;
  char carattereMax;
  int maxDepth;

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

  statoMax = 0;
  carattereMax = '_';
  maxDepth = 0;

  nread = getline(&temp, &len, fp); //contiene la prima transizione
  while(strcmp(temp, "acc\n") != 0){
    sscanf(temp, "%d%*c%c", &stato, &carattere);
    matrice[stato][(int)carattere] = matrice[stato][(int)carattere] + 1;
    if(matrice[stato][(int)carattere] > maxDepth){
      maxDepth = matrice[stato][(int)carattere];
      statoMax = stato;
      carattereMax = carattere;
    }
    nread = getline(&temp, &len, fp);
  }

  printf("Maxdepth: %d, statoMax: %d, carattereMax: %c\n", maxDepth, statoMax, carattereMax);

  fclose(fp);
  return 0;
}
