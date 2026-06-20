#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- MEMORY UTILITIES ---
char *my_strndup(const char *s, size_t n) {
    char *p = malloc(n + 1);
    if (p) {
        strncpy(p, s, n);
        p[n] = '\0';
    }
    return p;
}

// --- ARCHITECTURE ---
typedef enum {
    TYPE_NUMBER,
    TYPE_STRING
} DataType;

struct Column {
    char *name;       // Dynamically allocated string
    DataType type;    // TYPE_NUMBER or TYPE_STRING
    void *data;       // The wildcard array (either double* or char**)
    size_t capacity;  // How many items it can currently hold
    size_t size;      // How many items are actually in it
};

int main() {
    // FIX: Added \n and \r to delimiters to cleanly strip hidden newlines (Trap 3)
    const char *delims = ",\n\r"; 
    FILE *data = fopen("data.csv", "r");

    if (data == NULL) {
        printf("Error opening the file.\n");
        return -1;
    }

    size_t line_length = 4;
    char *line = (char*)malloc(line_length);
    if (line == NULL) return -1;

    // The Master Table
    struct Column *col = NULL; 
    
    size_t cur_len = 0;
    size_t col_len = 0; // Total number of columns (survives the loop!)
    size_t row_len = 0; // Total rows processed

    // --- CHECKPOINT 1: THE INGESTION ENGINE ---
    while (fgets(line + cur_len, line_length - cur_len, data) != NULL) {
        cur_len += strlen(line + cur_len);
        
        // If we haven't hit the end of the line, expand the buffer
        if (line[cur_len - 1] != '\n' && !feof(data)) {
            line_length *= 2;
            char *temp = (char*)realloc(line, line_length);
            if (temp == NULL) return -1;
            line = temp;
            continue; 
        }

        // We have a full, complete line in memory.
        const char *current = line;
        int col_idx = 0; // FIX: Tracks which column we are currently parsing in this row

        // Tokenize the line
        while (*current != '\0') {
            size_t token_len = strcspn(current, delims);
            
            if (token_len > 0) {
                char *token = my_strndup(current, token_len);

                // --- CHECKPOINT 2A: THE HEADER PASS ---
                if (row_len == 0) {
                    struct Column *temp = (struct Column *)realloc(col, (col_len + 1) * sizeof(struct Column));
                    if (temp == NULL) return -1;
                    col = temp;
                    
                    col[col_len].name = token; // Transfer ownership of the string to the struct
                    col[col_len].data = NULL;
                    col[col_len].capacity = 0;
                    col[col_len].size = 0;
                    col_len++;
                } 
                
                // --- CHECKPOINT 2B: THE LOOKAHEAD PASS (First Data Row) ---
                else if (row_len == 1) {
                    char *endptr;
                    double val = strtod(token, &endptr);

                    col[col_idx].capacity = 2; // Start with a small capacity
                    col[col_idx].size = 1;     // We are adding the 1st item right now

                    // If strtod parsed numbers perfectly, it's a NUMBER
                    if (token != endptr && *endptr == '\0') {
                        col[col_idx].type = TYPE_NUMBER;
                        col[col_idx].data = malloc(col[col_idx].capacity * sizeof(double));
                        
                        // CAST VOID* to DOUBLE* to store the value
                        ((double*)col[col_idx].data)[0] = val;
                        free(token); // We don't need the string anymore
                    } 
                    // Otherwise, it's a STRING
                    else {
                        col[col_idx].type = TYPE_STRING;
                        col[col_idx].data = malloc(col[col_idx].capacity * sizeof(char*));
                        
                        // CAST VOID* to CHAR** to store the string pointer
                        ((char**)col[col_idx].data)[0] = token; // Keep the token
                    }
                    col_idx++;
                }
            }
            
            current += token_len;
            if (*current != '\0') current++; // Skip the delimiter
        }

        cur_len = 0;
        row_len++;
    }

    // --- DEBUG OUTPUT ---
    // Let's prove the memory is structured correctly!
    printf("Successfully parsed %zu columns.\n\n", col_len);
    for(size_t i = 0; i < col_len; i++) {
        printf("Column [%zu]: %s\n", i, col[i].name);
        if (col[i].type == TYPE_NUMBER) {
            printf("  Type: NUMBER\n");
            printf("  Data[0]: %f\n", ((double*)col[i].data)[0]);
        } else {
            printf("  Type: STRING\n");
            printf("  Data[0]: %s\n", ((char**)col[i].data)[0]);
        }
        printf("\n");
    }

    // Clean up
    free(line);
    fclose(data);
    return 0;
}