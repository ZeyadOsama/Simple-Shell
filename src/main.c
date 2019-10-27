///
/// main.c
/// Shell
///
/// @author Zeyad Osama.
/// @date 2019-10-12.
/// @copyright © 2019 Zeyad Osama. All rights reserved.
///

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <time.h>

/**
 * defining identifying macros.
 */
#define BUFFER_SIZE 256
#define TOKEN_BUFFER_SIZE 64
#define TOKEN_DELIMITERS " \t\r\n\a"

/**
 * defining bool data type.
 */
typedef enum {
    false = 0, true = 1
} bool;


/**
 * constants.
 */
char *SHELL_TITLE = "Shell > ";
char *ERROR = "error";

/**
 * an integer which can be accessed as an atomic entity
 * even in the presence of asynchronous interrupts made by signals.
 */
volatile sig_atomic_t signal_status;

/**
 * current working directory path for logging usages.
 */
char cwd[PATH_MAX];

/**
 * @return number of pre-implemented commands.
 */
int get_commands_num();

/**
 * @brief reads a line from the terminal.
 * @return read line as string.
 */
char *read_string(void);

/**
 * @brief split a string into tokens.
 * @param string to be splitted.
 * @return tokens.
 */
char **tokenize_string(char *string);

/**
 * @brief checks if a given line has '&' at it's end.
 * @param tokens to be checked.
 * @return boolean upon the check.
 */
bool has_ampersand(char **tokens);

/**
 * @brief replaces "&" with NULL to be correctly excuted.
 */
void remove_ampersand(char **tokens, int index);

/**
 * @param tokens to know it's length.
 * @return int value representing length.
 */
int get_tokens_length(char **tokens);

/**
 * @param args list of args.
 * args[0] "cd".
 * args[1] directory to be changed to.
 * @return running status, 1 to continue executing.
 */
int command_cd(char **args);

/**
 * @param args list of args.
 * @return running status, 0 to terminate execution.
 */
int command_exit(char **args);

/**
 * @brief Launch a program and wait for it to terminate.
 * @param args Null terminated list of arguments (including program).
 * @return Always returns 1, to continue execution.
 */
int execute(char **args, bool background);

/**
 * @brief Execute shell built-in or launch program.
 * @param args Null terminated list of arguments.
 * @return 1 if the shell should continue running, 0 if it should terminate
 */
int detect_command(char **args, bool background);

/**
 * @brief constantly running the shell.
 */
void run(void);

/**
 * @brief handles signal from signal().
 * @param signal
 */
void signal_handler(int signal);

/**
 * @brief logs the process id and it's exit status in .txt file included in the same directory.
 * @param pid process id.
 * @param status exit status.
 * @return boolean upon file writing success.
 */
bool write_to_file(pid_t pid, sig_atomic_t signal_status, int status);

/**
 *
 * @param buffer to be checked if allocated.
 * @return boolean upon the check.
 */
bool allocation_check(const char *buffer);

/**
 * @brief commands string.
 */
char *commands_name[] = {
        "exit",
        "cd"
};

/**
 * @brief commands function.
 */
int (*commands_function[])(char **args) = {
        &command_exit,
        &command_cd
};

/**
 * @return program exit status.
 */
int main(int argc, char *argv[]) {
    // store program start working directory for any further need.
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror(ERROR);
        return EXIT_FAILURE;
    }
    // program start.
    run();
    // program terminated successfully.
    return EXIT_SUCCESS;
}

char *read_string(void) {
    // creating a buffer for storing terminal line.
    // setting buffer size to be allocated.
    size_t buffer_size = BUFFER_SIZE;
    // allocating memory for buffer.
    char *buffer = malloc(sizeof(char) * buffer_size);

    // positioning index for assigning chars to their positions in buffer
    int index = 0;
    // current being read char.
    int c;

    // failure check.
    // check if buffer was allocated successfully.
    allocation_check(buffer);

    // read line from terminal until "\n" is encountered.
    while (true) {
        c = getchar();

        // check if entered string was an enter.
        // if enter, re-run.
        if (c == '\n' && index == 0)
            run();

        // generating the buffer iteratively.
        if (c == EOF)
            exit(EXIT_SUCCESS);
        else if (c == '\n') {
            buffer[index] = '\0';
            return buffer;
        } else
            buffer[index] = (char) c;
        index++;

        // buffer size exceeded the pre-defined size.
        // reallocate.
        if (index >= buffer_size) {
            buffer_size += BUFFER_SIZE;
            buffer = realloc(buffer, buffer_size);

            // failure check.
            // check if buffer was reallocated successfully.
            allocation_check(buffer);
        }
    }
}

char **tokenize_string(char *string) {
    // creating a buffer for storing tokens.
    // setting buffer size to be allocated.
    int buffer_size = TOKEN_BUFFER_SIZE;

    // positioning index for assigning tokens to their positions in buffer
    int index = 0;

    // allocating memory for tokens.
    char **tokens = malloc(buffer_size * sizeof(char *));
    // pointers for each token string.
    char *token, **tokens_backup;

    // failure check.
    // check if buffer was allocated successfully.
    allocation_check((const char *) tokens);

    token = strtok(string, TOKEN_DELIMITERS);
    while (token != NULL) {
        // assigning tokens.
        tokens[index++] = token;

        // buffer size exceeded the pre-defined size.
        // reallocate.
        if (index >= buffer_size) {
            buffer_size += TOKEN_BUFFER_SIZE;
            tokens_backup = tokens;
            tokens = realloc(tokens, buffer_size * sizeof(char *));

            // failure check.
            // check if buffer was allocated successfully.
            if (!tokens) {
                free(tokens_backup);
                fprintf(stderr, "%s: allocation error\n", ERROR);
                exit(EXIT_FAILURE);
            }
        }
        // tokenize.
        token = strtok(NULL, TOKEN_DELIMITERS);
    }
    // terminating tokens array by NULL value.
    tokens[index] = NULL;
    return tokens;
}

void remove_ampersand(char **tokens, int index) {
    tokens[index - 1] = NULL;
}

bool has_ampersand(char **tokens) {
    return (strcmp(tokens[get_tokens_length(tokens) - 1], "&") == 0);
}

int get_tokens_length(char **tokens) {
    char **ptr = tokens;
    int i = 0;
    // iterate on tokens array to get it's length
    for (char *c = *ptr; c; c = *++ptr, i++);
    return i;
}

int get_commands_num() {
    return sizeof(commands_name) / sizeof(char *);
}

int command_cd(char **args) {
    if (args[1] == NULL)
        // directory not specified.
        fprintf(stderr, "%s: expected argument to \"cd\"\n", ERROR);
    else {
        if (args[2] != NULL)
            // extra non-supported argument.
            fprintf(stderr, "%s: too many arguments for \"cd\".\n", ERROR);
        else if (chdir(args[1]) != 0)
            // directory does not exists.
            perror(ERROR);
    }
    return 1;
}

int command_exit(char **args) {
    exit(EXIT_SUCCESS);
}

int execute(char **args, bool background) {
    // initializing signal handler
    signal(SIGCHLD, signal_handler);

    pid_t pid = fork();
    int status = 0;

    // child process.
    if (pid == 0) {
        if (execvp(args[0], args) == -1)
            // call to execvp was unsuccessful.
            perror(ERROR);
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        // wait in case background.
        // otherwise continue.
        if (!background)
            do {
                waitpid(pid, &status, WUNTRACED);
            } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        write_to_file(pid, signal_status, status);
    } else {
        // forking error.
        perror(ERROR);
    }
    return 1;
}

int detect_command(char **args, bool background) {
    // null entry.
    // neglect.
    if (args[0] == NULL)
        return 1;
    // loop on predefined functions.
    // if one of them is detected, current function calls it .
    // otherwise, preform normal executing.
    for (int i = 0; i < get_commands_num(); i++)
        if (strcmp(args[0], commands_name[i]) == 0)
            return (*commands_function[i])(args);
    return execute(args, background);
}

void run(void) {
    // terminal read line.
    char *line;
    // argument list.
    char **args;
    // boolean to check running status of shell
    int running;
    // boolean to check if the next execution is a background or foreground process.
    bool background;
    do {
        // shell title.
        printf("%s", SHELL_TITLE);

        line = read_string();
        args = tokenize_string(line);
        background = has_ampersand(args);
        if (background)
            remove_ampersand(args, get_tokens_length(args));
        running = detect_command(args, background);

        // deallocate.
        free(line);
        free(args);
    } while (running);
}

void signal_handler(int signal) {
    signal_status = signal;
}

bool write_to_file(pid_t pid, sig_atomic_t signal_status, int status) {
    FILE *fptr;
    char path[PATH_MAX];

    // defining path.
    strcpy(path, cwd);
    strcat(path, "/log.txt");

    // current time.
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    fptr = fopen(path, "a");

    // failure check
    if (fptr == NULL) {
        perror(ERROR);
        return false;
    }
    // write current log in file.
    fprintf(fptr, "%04d-%02d-%02d %02d:%02d:%02d "
                  "shell:: "
                  "process %d terminated "
                  "with signal status %d "
                  "and exit status %d.\n",
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,
            pid, signal_status, status);
    fclose(fptr);
    return true;
}

bool allocation_check(const char *buffer) {
    // failure check.
    if (!buffer) {
        fprintf(stderr, "%s: allocation error\n", ERROR);
        // segment fault occurred.
        exit(EXIT_FAILURE);
    }
    return true;
}
