#include "logging.h"
#include "messages.h"
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

/*typedef struct box{
    int id;
    char content;
};*/

char box_name[32];
char session_pipe_name[256];
char* register_pipe_name;
int max_sessions;
char sv_order_msg[289];
char msg[1024];


void get_order(int fd){
    memset(sv_order_msg,0,sizeof(sv_order_msg));
    memset(session_pipe_name,0,sizeof(session_pipe_name));
    memset(box_name,0,sizeof(box_name));
    memset(msg,0,sizeof(msg));

    char *token;
    ssize_t n = read(fd, sv_order_msg, sizeof(sv_order_msg)-1);
    if(n <= 0){
        return;
    }

    printf("recebi %zu bytes\n", n);
    printf("A mensagem enviada para o mbroker foi: %s\n", sv_order_msg);
    printf("res: %i\n", (int)sv_order_msg[0]);

    token = strtok(sv_order_msg," ");
    token = strtok(NULL, " "); // skipping the first token
    strcpy(session_pipe_name, token);
    token = strtok(NULL, " ");
    strcpy(box_name, token);
}

int main(int argc, char **argv) {
    
    if (argc < 3) {
        return -1;
    }
    register_pipe_name = argv[1];
    max_sessions = atoi(argv[2]);
    if(max_sessions <= 0){
       return -1;
    }

    printf("mbroker.register_pipe = '%s' max_sessions = '%d' \n",register_pipe_name, max_sessions);

    if (access(register_pipe_name, F_OK) == 0){
        unlink(register_pipe_name); /* Quando dou unlink ao pipe ?*/
    }

    if (mkfifo(register_pipe_name, 0666) < 0) {
        printf("Error creating register pipe\n"); 
        return -1;
    }

    int fd = open(register_pipe_name, O_RDONLY); /* O_BLOCK ?? */
    if (fd < 0) {
        printf("Error opening register pipe\n"); 
        return -1;
    }

    get_order(fd);

    while (1) {
        switch ((int)sv_order_msg[0]){
        case 1:
            int session_pipe_fd = open(session_pipe_name, O_RDONLY);
            if (session_pipe_fd < 0) {
                printf("Error opening session pipe\n");
            return -1;
            }
            ssize_t flg = read(session_pipe_fd,msg,1024);
            if(flg == -1){
                return -1;
            }
            printf("A Mensagem do user foi: %s \n", msg);
            break;
        case 3:
            int session_pipe_fd = open(session_pipe_name, O_WRONLY);
            if (session_pipe_fd < 0) {
                printf("Error opening session pipe\n");
                return -1;
            }
            /* POR MUDARRRRR */
            ssize_t flg = write(session_pipe_fd,msg,1024);
            if(flg == -1){
                return -1;
            }
            printf("A Mensagem enviada foi: %s \n", msg);
            break;
        default:
            break; /*get_order(fd);*/
        }
    }
    close(fd);
    unlink(register_pipe_name);
    return 0;
}


