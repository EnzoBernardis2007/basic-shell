#include <stdio.h>
#include <stdlib.h>

#define LSH_BUFFER_SIZE 1024

#define LSH_TOK_BUFFER_SIZE 64
#define LSH_TOK_DELIMITER " \t\r\n\a"

char *lsh_read_line() {
    // defining and allocating
    int buffer_size = LSH_BUFFER_SIZE;
    int position = 0;
    char *buffer = malloc(sizeof(char) * buffer_size);
    int c;

    // prevent allocating error
    if(!buffer) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    while(1) {
        c = getchar(); // read a character

        // if hit end of file, replace it and return
        if(c == EOF || c == "\n") { 
            buffer[position] = '\0';
            return buffer;
        } else {
            buffer[position] = c;
        }

        position++;

        // if exceeded the buffer, reallocate
        if(position > buffer_size) {
            buffer_size += LSH_BUFFER_SIZE;
            buffer = realloc(buffer, buffer_size);

            // prevent allocating error
            if(!buffer) {
                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}

char **lsh_split_line(char* line) {
    int buffer_size = LSH_TOK_BUFFER_SIZE, position = 0;
    char **tokens = malloc(buffer_size * sizeof(char*));
    char *token;

    // prevent allocating error
    if(!tokens) {
        fprintf(stderr, "lsh: allocating error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, LSH_TOK_DELIMITER);
    while(token != NULL) {
        tokens[position] = token;
        position++;

        // if exceeded the buffer, reallocate
        if(position > buffer_size) {
            buffer_size += LSH_TOK_BUFFER_SIZE;
            tokens = realloc(tokens, buffer_size * sizeof(char*));

            // prevent allocating error
            if(!tokens) {
                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, LSH_TOK_DELIMITER);
    }

    tokens[position] = NULL;
    return tokens;
}

void lsh_loop() {
    char *line;
    char **args;
    int status;

    do {
        printf("> ");
        line = lsh_read_line();
        args = lsh_split_line(line);
        status = lsh_execute(args);

        free(line);
        free(args);
    } while(status);
}

int main(int argc, char **argv) {
    return EXIT_SUCCESS;
}