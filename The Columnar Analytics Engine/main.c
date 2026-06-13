#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *my_strndup(const char *s, size_t n) {
    char *p = malloc(n + 1);
    if (p) {
        strncpy(p, s, n);
        p[n] = '\0';
    }
    return p;
}

typedef enum{
    TYPE_NUMBER,
    TYPE_STRING
} datatype;

struct column
{
    char *name;
    datatype type;
    void *data;
    size_t capacity;
    size_t size;
};


int main(){
    const char *delims = ",\n\r";
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
    
    struct column *col = (struct column *)malloc(sizeof(struct column));
    if(col==NULL){
        printf("memory allocation failed");
        return -1;
    }

    size_t cur_len=0;
    size_t col_len=0;
    size_t row_len=0;

    while(fgets(line+cur_len, line_length-cur_len, data)!=NULL){

        cur_len+=strlen(line+cur_len);
        if(line[cur_len-1]!='\n'){
            line_length*=2;
            char *temp = (char*)realloc(line, line_length);

            if (temp == NULL){
                fclose(data);
                free(line);
                printf("Memory Reallocation Failed\n");
                {
                    return -1;}
                }

            line = temp;
        }

        else {
            int i;
            const char *current = line;
            if(row_len==0){
                while (*current != '\0'){
                    size_t token_len = strcspn(current, delims);
                    if (token_len > 0) {
                        char *token = my_strndup(current, token_len);
                        if (token != NULL) {
                            struct column *temp = (struct column *)realloc(col, (col_len+2)*sizeof(struct column));
                            if (temp == NULL) {
                                printf("Reallocation failed!\n");
                                free(col);
                                return -1;
                            }
                            col = temp;
                            col[col_len].name = token;
                            col_len++;
                        }
                    }
                    current += token_len;
        
                    if (*current != '\0') {
                        current++; 
                    }
                }
            }
            cur_len=0;
            row_len++;
        }
    }
    if(cur_len>0){
        puts(line);
    }
    
    free(line);
    fclose(data);

    return 0;
}