//
// Created by Zeyad Osama on 2019-10-14.
//

#include <stdlib.h>
#include <stdio.h>
#include "../include/validations.h"

int allocation_check(const char *buffer) {
    // failure check.
    if (!buffer) {
        fprintf(stderr, "error: allocation error\n");
        exit(EXIT_FAILURE);
    }
    return 1;
}