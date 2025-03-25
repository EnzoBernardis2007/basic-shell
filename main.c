#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h> 
#include <unistd.h>
#include <string.h>

#define LSH_BUFFER_SIZE 1024

#define LSH_TOK_BUFFER_SIZE 64
#define LSH_TOK_DELIMITER " \t\r\n\a"

// forward declarations of funcs
int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);
int lsh_create_file(char **agrs);

// list of builtin commands
char *builtin_str[] = {
    "cd",
    "help",
    "exit",
    "create-file"
};

// list of corresponding funcs
int (*builtin_func[]) (char **) = {
    &lsh_cd,
    &lsh_help,
    &lsh_exit,
    &lsh_create_file
};

int lsh_num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}

// implementations
int lsh_cd(char **args) {
    if(args[1] == NULL) {
        // check if has argument to cd
        fprintf(stderr, "lsh: expected argument to \"cd\"\n");
    } else {
        // ifc chdir return anything besides 0, some error happened
        if(chdir(args[1]) != 0) {
            perror("lsh");
        }
    }

    // 1 == success
    return 1;
}

int lsh_help(char **args) {
    printf("Enzo Bernardis LSH\n");
    printf("type program names and arguments, and hit enter\n");
    printf("The following are built in:\n");

    for(int i = 0; i < lsh_num_builtins(); i++) {
        printf("* %s\n", builtin_str[i]);
    }

    printf("use the man command for information on other programs.\n");
    return 1;
}

int lsh_exit(char **args) {
    return 0;
}

int lsh_create_file(char **args) {
    const char *filename = args[1];

    if(args[1] == NULL) {
        fprintf(stderr, "lsh: cannot create file without a argument for name\n");
    } else {
        FILE *file = fopen(filename, "w");

        if(file == NULL) {
            perror("error trying to create file");
        }

        fclose(file);
        printf("file created with success: %s\n", filename);
    }
}

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
        if(c == EOF || c == '\n') { 
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

int lsh_launch(char **args) {
    pid_t pid, wpid;
    int status;

    pid = fork();
    if(pid == 0) {
        // child process
        if(execvp(args[0], args) == -1) {
            perror("lsh");
        }

        exit(EXIT_FAILURE);
    } else if(pid < 0) {
        // error forking
        perror("lsh");
    } else {
        // parent process
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while(!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

int lsh_execute(char **args) {
    // empty command
    if(args == NULL) {
        return 1;
    }

    // iterates through the array until it finds the command
    for(int i = 0; i < lsh_num_builtins(); i++) {
        if(strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }

    // if it is not a builtin command, go to launch
    return lsh_launch(args);
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
    lsh_loop();

    return EXIT_SUCCESS;
}