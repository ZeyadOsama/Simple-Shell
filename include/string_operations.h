//
// Created by Zeyad Osama on 2019-10-12.
//

#ifndef SHELL_STRING_OPERATIONS_H
#define SHELL_STRING_OPERATIONS_H

/**
 * constants.
 */
#define BUFFER_SIZE 256
#define TOKEN_BUFFER_SIZE 64
#define TOKEN_DELIMITERS " \t\r\n\a"

/**
 * @return number of pre-implemented commands.
 */
int get_num_commands();

/**
 * @brief read string passed by stdin.
 * @return Read string.
 */
char *read_line(void);


/**
 * @brief split a string into tokens.
 * @param string to be splitted.
 * @return tokens.
 */
char **split_line(char *string);

int has_ampersand(char **string);

#endif //SHELL_STRING_OPERATIONS_H