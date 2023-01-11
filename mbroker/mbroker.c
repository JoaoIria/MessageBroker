#include "logging.h"
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

char msg[] = "mensagem de teste";
char buffer[1024];

int main(int argc, char **argv) {

    (void)argc;
    (void)argv;

    int max_sessions;
    char register_pipe_name[1024];
    int i;

    strcpy(register_pipe_name, argv[1]);
    max_sessions = atoi(argv[2]);

    printf("%s %d",register_pipe_name, max_sessions);

    mkfifo(register_pipe_name, max_sessions);
    int fd = open(register_pipe_name, O_RDONLY);
     
    if (fd == -1){
        return -1;
    }
    
    for(i = 0; i < max_sessions; i++){
        if (read(fd, &buffer[i], sizeof(int))== -1){
            return -1;
        }
        
    }
    close(fd);

    return 0;
}