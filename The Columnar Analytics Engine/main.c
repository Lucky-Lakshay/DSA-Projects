#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
    FILE *data = fopen("data.csv","rb");

    if(data==NULL){
        printf("error opening the file..");
        return -1;
    }

    
    
    fclose(data);

    return 0;
}