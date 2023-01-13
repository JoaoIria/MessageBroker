#include "logging.h"
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

    i = 1;
    char sv_order_msg[289];
    memset(sv_order_msg, 0, sizeof(sv_order_msg));

    // copy the value of i to the first byte of sv_order_msg
    memcpy(sv_order_msg, &i, sizeof(__uint8_t)); 
   
    // add a space after i
    memcpy(sv_order_msg + sizeof(__uint8_t), " ", sizeof(char)); 

    // add session_pipe_name
    memcpy(sv_order_msg + sizeof(__uint8_t) + sizeof(char), session_pipe_name, strlen(session_pipe_name)); 
    
    // add a space after session_pipe_name
    memcpy(sv_order_msg + sizeof(__uint8_t) + sizeof(char) + strlen(session_pipe_name), " ", sizeof(char)); 

    // add box_name
    memcpy(sv_order_msg + sizeof(__uint8_t) + sizeof(char) * 2 + strlen(session_pipe_name), box_name, sizeof(char) * strlen(box_name)); 

    // add null terminator
    sv_order_msg[sizeof(__uint8_t) + sizeof(char) * 2 + strlen(session_pipe_name) + strlen(box_name)] = '\0'; // add null terminator
    /*printf("A mensagem enviada para o mbroker foi: %s\n", sv_order_msg);
    printf("A mensagem enviada para o mbroker foi: %d\n", sv_order_msg[0]);*/



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
    i = 9;

    while(1){
        printf("Enter the message to publish:");
        if(scanf("%s", msg)==1){
        }
        else{
            return -1;
        }
        ssize_t flg2 = write(session_pipe_fd, msg, sizeof(msg));
        if(flg2 == -1){
            return -1;
        }
    }
    close(session_pipe_fd);
    unlink(session_pipe_name);
    return 0;
}
