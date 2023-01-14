#include "logging.h"
#include "messages.h"
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/*static void print_usage() {
    fprintf(stderr, "usage: \n"
                    "   manager <register_pipe_name> create <box_name>\n"
                    "   manager <register_pipe_name> remove <box_name>\n"
                    "   manager <register_pipe_name> list\n");
}*/

int main(int argc, char **argv) {

    /* check for correct number of command line arguments */
    if (argc < 5) {
        return -1;
    }

    const char* register_pipe_name = argv[1];
    const char* session_pipe_name = argv[2];
    const char* action = argv[3];
    const char* box_name = argv[4];
    __uint8_t i;
    int bytes_msg,flag = 1;

    /* print to check all variables */
    printf("mbroker.register_pipe = '%s' session_pipe_name = '%s' box_name = '%s' action = '%s' \n",register_pipe_name, session_pipe_name,box_name,action);

    /* open the register pipe for writing */
    int register_pipe_fd = open(register_pipe_name, O_WRONLY);
    /* check for error opening the register pipe */
    if (register_pipe_fd < 0) {
        printf("Error opening register pipe\n"); /* Tirar */
        return -1;
    }

    /* format message to send to mbroker */
    if (strcmp(action,"create") == 0){
        flag = 0;
        i = 3;
        bytes_msg = 1029;
    }
    if (strcmp(action,"remove") == 0){
        flag = 0;
        i = 5;
        bytes_msg = 1029;
    }
    if (strcmp(action,"list") == 0){
        flag = 0;
        i = 7;
        bytes_msg = 58;
    }
    if(flag == 1){
        printf("Action unknown:_'%s'\n",action);
        return -1;
    }
    
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

    /* open the session pipe for reading */ 
    int session_pipe_fd = open(session_pipe_name, O_RDONLY);
    /* check for error opening the session pipe */
    if (session_pipe_fd < 0) {
        printf("Error opening session pipe\n");
        return -1;
    }

    char msg[bytes_msg];
    while(1){
        memset(msg, 0, sizeof(msg));
        ssize_t flg = read(session_pipe_fd,msg,(size_t)bytes_msg);
        if(flg == -1){
            return -1;
        }
        printf("codigo: %d \n",msg[0]);
        printf("A Mensagem recebida foi: %s \n",msg);
        break;
    }
    close(session_pipe_fd);
    unlink(session_pipe_name);
    return 0;
}
