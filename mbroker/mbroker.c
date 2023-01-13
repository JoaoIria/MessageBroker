#include "logging.h"
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

/*typedef struct box{
    int id;
    char content;
};*/



char* register_pipe_name;
int max_sessions;
/*int num_threads = 0;
pthread_mutex_t mb_mutex = PTHREAD_MUTEX_INITIALIZER;

void* session_handler(void* session_pipe_name) {
    const char* pipe_name = (const char*) session_pipe_name;
    int fd = open(pipe_name, O_RDONLY);

    /* Initializes a user session */
    /* Criar switch */

    /*close(fd);
    free((void*) pipe_name);
    pthread_mutex_lock(&mb_mutex);
    num_threads--;
    pthread_mutex_unlock(&mb_mutex);
    return NULL;
}*/

int main(int argc, char **argv) {

    if (argc < 3) {
        return -1;
    }

    register_pipe_name = (char*) malloc(sizeof(char) * 256);
    /*char * register_pipe_name = argv[1];*/
    if(register_pipe_name == NULL) {
        printf("Error allocating memory\n");
        return -1;
    }

    strcpy(register_pipe_name,argv[1]);
    max_sessions = atoi(argv[2]);
    if(max_sessions <= 0){
       return -1;
    }
    pthread_t session_threads[max_sessions];

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

    while (1) {
        /*char sv_msg[3];*/
        char msg;
        ssize_t n = read(fd, &msg, sizeof(char));
        if (n <= 0) {
            continue;
        }
        printf("recebi %zu bytes", n);
        // sv_msg[n] = '\0';
        /* Check if there is space for a new thread */
        // pthread_mutex_lock(&mb_mutex);
        // if (num_threads >= max_sessions) {
           // pthread_mutex_unlock(&mb_mutex);
            /* wait for a thread to finish */
        //    continue;
        //}
        //num_threads++;
        //pthread_mutex_unlock(&mb_mutex);

        /*char* session_pipe_name_copy = strdup(sv_msg);
        if(pthread_create(&session_threads[num_threads-1], NULL, session_handler, session_pipe_name_copy[1]) != 0) {
            printf("Error creating thread\n");
            continue;
        }*/
    }

    close(fd);

    /*for (int i = 0; i < num_threads; i++)
        pthread_join(session_threads[i], NULL);*/

    unlink(register_pipe_name);
    free(register_pipe_name);
    return 0;
}


