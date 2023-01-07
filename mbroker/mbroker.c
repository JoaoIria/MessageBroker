#include "logging.h"
#include <unistd.h>

char msg[] = "mensagem de teste";
char buffer[1024];

int main(int argc, char **argv) {

    (void)argc;
    (void)argv;

    int p[2];
    if(pipe(p) == -1){
        return -1;
    }

    int id = fork();
    if(id == -1){
        return -1;
    }
    /* CHILD PROCESS */
    if(id == 0){
        close(p[0]);
        if (write(p[1],msg,sizeof(msg)) == -1){
            return -1;
        }
        close(p[1]);
    /* PARENT PROCESS */
    }else{
        close(p[1]);
        if (read(p[0],buffer,sizeof(msg)) == -1){
            return -1;
        }
        close(p[0]);
        printf("Got from child process -> %s\n", buffer);
    }

    return 0;
}