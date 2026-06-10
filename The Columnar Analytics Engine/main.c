#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
    FILE *data = fopen("data.csv","r");

    if(data==NULL){
        printf("error opening the file..");
        return -1;
    }

    size_t line_length = 4;
    char *line=(char*)malloc(line_length);

    if(line==NULL){
        fclose(data);
        printf("memory allocation failed");
        return -1;
    }

    size_t cur_len=0;

    while(fgets(line+cur_len, line_length-cur_len, data)!=NULL){
        cur_len=strlen(line);
        if(*(line+strlen(line)-1)!='\n'){
            line_length*=2;
            char *temp = (char*)realloc(line, line_length);

            if (temp == NULL){
                fclose(data);
                free(line);
                printf("Memory Reallocation Failed\n");
                return -1;}

            line = temp;
        }

        else {
            puts(line);
            cur_len=0;
        }
    }
    if(cur_len>0)puts(line);
    
    free(line);
    fclose(data);

    return 0;
}