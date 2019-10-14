cmake_minimum_required(VERSION 3.13)
project(Shell C)

set(CMAKE_C_STANDARD 11)

add_executable(Shell src/main.c src/string_operations.c include/string_operations.h src/shell_operations.c include/shell_operations.h src/commands.c include/commands.h include/constants.h src/validations.c include/validations.h src/logger.c include/logger.h)