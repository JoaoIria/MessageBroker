#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_THREADS 100 /* 100 for example, but can change */

const char* register_pipe_name;
int max_sessions;
pthread_t session_threads[MAX_THREADS];
int num_threads = 0;
pthread_mutex_t mb_mutex = PTHREAD_MUTEX_INITIALIZER;

void* session_handler(void* session_pipe_name) {
    const char* pipe_name = (const char*) session_pipe_name;
    int fd = open(pipe_name, O_RDONLY);

    /* Initializes a user session */

    close(fd);
    free((void*) pipe_name);
    pthread_mutex_lock(&mb_mutex);
    num_threads--;
    pthread_mutex_unlock(&mb_mutex);
    return NULL;
}

int main(int argc, char **argv) {
    if (argc < 3) {
        printf("Usage: %s <register_pipe_name> <max_sessions>\n", argv[0]); /*tirar*/
        return -1;
    }

    strcpy(register_pipe_name,argv[1]);
    max_sessions = atoi(argv[2]);

    if (mkfifo(register_pipe_name, 0666) < 0) {
        printf("Error creating register pipe\n"); /*tirar*/
        return -1;
    }

    int fd = open(register_pipe_name, O_RDONLY | O_NONBLOCK);
    if (fd < 0) {
        printf("Error opening register pipe\n"); /*tirar*/
        return -1;
    }

    while (1) {
        char session_pipe_name[256];
        int n = read(fd, session_pipe_name, sizeof(session_pipe_name));
        if (n < 0) {
            continue;
        }
        session_pipe_name[n] = '\0';
        /* allows for functions like printf and strlen 
        to know where the string ends and stops reading from it */ 

        /* Check if there is space for a new thread */
        pthread_mutex_lock(&mb_mutex);
        if (num_threads >= max_sessions) {
            pthread_mutex_unlock(&mb_mutex);
            /* wait for a thread to finish */
            continue;
        }
        num_threads++;
        pthread_mutex_unlock(&mb_mutex);

        char* session_pipe_name_copy = strdup(session_pipe_name);
        pthread_create(&session_threads[num_threads-1], NULL, session_handler, session_pipe_name_copy);
    }
    close(fd);
    for (int i = 0; i < num_threads; i++)
        pthread_join(session_threads[i], NULL);
    unlink(register_pipe_name);
    return 0;
}
