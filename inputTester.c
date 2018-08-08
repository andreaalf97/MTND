#include<stdio.h>
#include<stdlib.h>

int main(int argc, char *argv[]){
  char *str = NULL;
  FILE *fp = NULL;

  char *temp = NULL;
  size_t len, nread;


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

  nread = getline(&temp, &len, fp);
  printf("%s\n", temp);


  fclose(fp);
  return 0;
}
