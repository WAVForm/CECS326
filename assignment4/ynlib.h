#ifndef YNLIB_H
#define YNLIB_H

#include <stdlib.h>

int str_to_int(char* arg){
    int place = 0;
    int i = 0;
    while(arg[i] != '\0'){
        if(arg[i] < '0' || arg[i] > '9'){
            return -1; //only digits allowed (no negatives)
        }
        place = (place == 0)?1:(place*=10);
        i++;
    }
    int amt = 0;
    i = 0;
    while(arg[i] != '\0'){
        amt += (arg[i] - '0') * place;
        place /= 10;
        i++;
    }
    return amt;
}

char* int_to_str(int arg){
    int temp = arg;
    int size = 0;
    while(temp > 0){
        temp /= 10;
        size++;
    }
    char* str = (char*)malloc((size+1)*sizeof(char));
    str[size] = '\0';
    for(int i = size-1; i >= 0; i--){
        str[i] = (char)((arg % 10) + '0');
        arg /= 10;
    }
    return str;
}

char* concat_strs(char* a, char* b){
    int size = 0;
    int i = 0;
    while(a[i] != '\0'){
        size++;
        i++;
    }
    i = 0;
    while(b[i] != '\0'){
        size++;
        i++;
    }
    char* str = (char*)malloc((size+1)*sizeof(char));
    str[size] = '\0';
    i = 0;
    int str_i = 0;
    while(a[i] != '\0'){
        str[str_i] = a[i];
        i++;
        str_i++;
    }
    i = 0;
    while(b[i] != '\0'){
        str[str_i] = b[i];
        i++;
        str_i++;
    }
    return str;
}

int str_eq(char* a, char* b){
    int i = 0;
    while(a[i] != '\0' && b[i] != '\0' && a[i] == b[i]){
        i++;
    }
    if(a[i] == '\0' && b[i] == '\0'){
        return 1;
    }
    else{
        return 0;
    }
}

#endif