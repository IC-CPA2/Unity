#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* getString(char* string){
  char * token = strtok(string, ";");
  return token;
}

int getDist(char* string){
  int dist = 0;
  char* token = strtok(string, ";");
  char arr[3][50];
  int i = 0;
  
  while(token != NULL){
    strcpy(arr[i], token);
		token = strtok(NULL, ";");
    i++;
	}
  dist = atoi(arr[1]);
  return dist;
}

int getPos(char* string){
  int pos = 0;
  char* token = strtok(string, ";");
  char arr[3][50];
  int i = 0;
  
  while(token != NULL){
    strcpy(arr[i], token);
		token = strtok(NULL, ";");
    i++;
	}
  pos = atoi(arr[2]);
  return pos;
}


int main(void) {
  char str[] = "colour;8;5";
  printf ("%d", getPos(str));
  return 0;
}
