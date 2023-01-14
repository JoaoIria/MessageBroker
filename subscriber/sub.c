#include "logging.h"
#include "messages.h"
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
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
    __uint8_t i;

    printf("mbroker.register_pipe = '%s' session_pipe_name = '%s' box_name = '%s' \n",register_pipe_name, session_pipe_name,box_name);

    /* open the register pipe for writing */
    int register_pipe_fd = open(register_pipe_name, O_WRONLY);
    /* check for error opening the register pipe */
    if (register_pipe_fd < 0) {
        printf("Error opening register pipe\n"); /* Tirar */
        return -1;
    }

    /* format message to send to mbroker */
    i = 2;
    char sv_order_msg[289];
    memset(sv_order_msg,0,289);
    strcpy(sv_order_msg,create_sv_order_msg(i,session_pipe_name,box_name));

    /* send the message to the mbroker via the register pipe */
    ssize_t flg1 = write(register_pipe_fd, sv_order_msg, sizeof(sv_order_msg));
    if(flg1 == -1){
        return -1;
    }
    close(register_pipe_fd);

    unlink(session_pipe_name);

    if (mkfifo(session_pipe_name, 0666) < 0) {
        printf("Error creating session pipe\n"); 
        return -1;
    }

    /* open the session pipe for writing */ 
    int session_pipe_fd = open(session_pipe_name, O_WRONLY);
    /* check for error opening the session pipe */
    if (session_pipe_fd < 0) {
        printf("Error opening session pipe\n");
        return -1;
    }
    /* publisher can now write messages to the session pipe */

    char msg[1024];
    char *token;
    while(1){
        ssize_t flg2 = read(session_pipe_fd, msg, 1024);
        if(flg2 == -1){
            return -1;
        }
        token = strtok(msg," ");
        token = strtok(NULL, " "); // skipping the first token
        strcpy(msg, token);
        printf("Mensagem requisitada: '%s' \n", token);
    }
    close(session_pipe_fd);
    unlink(session_pipe_name);
    return 0;
}