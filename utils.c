#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"


int getInt(const char* prompt){
    printf("%s",prompt);
    int input = 0;
    scanf("%d",&input);
    return input;
}
char* getString(const char* prompt){
    printf("%s",prompt);
    char *input = NULL;
    char c;
    int size = 0;
    scanf(" %c", &c);
    input = (char*)malloc(sizeof(char));// mem alloc for first letter input
    input[size] = c;
    size++;
    
    while((c = getchar()) != '\n' && c != EOF){ 
        input =(char*) realloc(input,(size + 1) * sizeof(char)); // mem alloc for each letter
        input[size] = c;
        size++;
    }
    input = (char*)realloc(input,(size +1)* sizeof(char));// realloc for the end of string char
    input[size] = '\0';
    return input;
}


int giveDesX(int x, int dir){
    int vert = 0;
    int hori = 0; 
        switch(dir) {
        case 0: 
            vert = -1;
            break;
        case 1: 
            vert = 1;
            break;
        case 2:
            hori = -1;
            break;
        case 3:
            hori = 1;
            break;
        default: break;;
    }
    return x+ hori;
}
int giveDesY(int y, int dir){
    int vert = 0;
    int hori = 0; 
        switch(dir) {
        case 0: 
            vert = -1;
            break;
        case 1: 
            vert = 1;
            break;
        case 2:
            hori = -1;
            break;
        case 3:
            hori = 1;
            break;
        default: break;
    }
    return y+ vert;
}