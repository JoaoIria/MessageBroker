/*#include "logging.h"
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char **argv) {

    if (argc < 4) {
        printf("Usage: %s <register_pipe_name> <session_pipe_name> <box_name>\n", argv[0]);
        return -1;
    }

    const char* register_pipe_name = argv[1];
    const char* session_pipe_name = argv[2];
    const char* box_name = argv[3];
    __uint8_t i;
    */