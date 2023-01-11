#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char **argv) {
    /* check for correct number of command line arguments */
    if (argc < 4) {
        printf("Usage: %s <register_pipe_name> <session_pipe_name> <box_name>\n", argv[0]);
        return -1;
    }

    const char* register_pipe_name = argv[1];
    const char* session_pipe_name = argv[2];
    const char* box_name = argv[3];

    /* open the register pipe for writing */
    int register_pipe_fd = open(register_pipe_name, O_WRONLY);
    /* check for error opening the register pipe */
    if (register_pipe_fd < 0) {
        printf("Error opening register pipe\n"); /* Tirar */
        return -1;
    }

    /* format message to send to mbroker */
    char message = "[ code = 9 (uint8_t) ] | [ message (char[1024]) ]";

    /* send the message to the mbroker via the register pipe */
    write(register_pipe_fd, message, strlen(message));
    close(register_pipe_fd);


    /* open the session pipe for writing 
    int session_pipe_fd = open(session_pipe_name, O_WRONLY);
    /* check for error opening the session pipe 
    if (session_pipe_fd < 0) {
        printf("Error opening session pipe\n");
        return -1;
    }

    /* publisher can now write messages to the session pipe 
    while(1){
        char msg[512];
        printf("Enter the message to publish:");
        scanf("%s", msg);
        write(session_pipe_fd, msg, strlen(msg));
    }
    close(session_pipe_fd);*/
    return 0;
}
