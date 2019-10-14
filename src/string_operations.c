//
// Created by Zeyad Osama on 2019-10-12.
//

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../include/string_operations.h"
#include "../include/validations.h"

char *read_line(void) {
    // buffer.
    size_t buffer_size = BUFFER_SIZE;
    char *buffer = malloc(sizeof(char) * buffer_size);

    int position = 0;
    int c;

    // failure check.
    allocation_check(buffer);

    while (1) {
        c = getchar();

        if (c == EOF)
            exit(EXIT_SUCCESS);
        else if (c == '\n') {
            buffer[position] = '\0';
            return buffer;
        } else
            buffer[position] = (char) c;

        position++;

        // buffer size exceeded.
        // reallocate.
        if (position >= buffer_size) {
            buffer_size += BUFFER_SIZE;
            buffer = realloc(buffer, buffer_size);

            // failure check.
            allocation_check(buffer);
        }
    }
}

char **split_line(char *string) {
    int buffer_size = TOKEN_BUFFER_SIZE, position = 0;
    char **tokens = malloc(buffer_size * sizeof(char *));
    char *token, **tokens_backup;

    // failure check.
    allocation_check((const char *) tokens);

    token = strtok(string, TOKEN_DELIMITERS);
    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= buffer_size) {
            buffer_size += TOKEN_BUFFER_SIZE;
            tokens_backup = tokens;
            tokens = realloc(tokens, buffer_size * sizeof(char *));

            // failure check.
            if (!tokens) {
                free(tokens_backup);
                fprintf(stderr, "error: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, TOKEN_DELIMITERS);
    }
    tokens[position] = NULL;
    return tokens;
}

int has_ampersand(char **string) {
    
}
