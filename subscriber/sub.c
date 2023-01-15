#include "logging.h"
#include "messages.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>


/* FUNCTION REMOVED FROM LAB_SIGNALS - https://github.com/tecnico-so/lab_signals/blob/main/src/intquit.c */

static void sig_handler(int sig) {
  static int count = 0;

  // UNSAFE: This handler uses non-async-signal-safe functions (printf(),
  // exit();)
  if (sig == SIGINT) {
    // In some systems, after the handler call the signal gets reverted
    // to SIG_DFL (the default action associated with the signal).
    // So we set the signal handler back to our function after each trap.
    //
    if (signal(SIGINT, sig_handler) == SIG_ERR) {
      exit(EXIT_FAILURE);
    }
    count++;
    fprintf(stderr, "Caught SIGINT (%d)\n", count);
    return; // Resume execution at point of interruption
  }

  // Must be SIGQUIT - print a message and terminate the process
  fprintf(stderr, "Caught SIGQUIT - that's all folks!\n");
  exit(EXIT_SUCCESS);
}


int main(int argc, char **argv) {

    /* check for correct number of command line arguments */
    if (argc < 4) {
        return -1;
    }

    const char* register_pipe_name = argv[1];
    const char* session_pipe_name = argv[2];
    const char* box_name = argv[3];
    __uint8_t i;

    /* print to check all variables */
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

    /* Close the register pipe */
    close(register_pipe_fd);

    /* Unlink the session pipe */
    unlink(session_pipe_name);

    /* Create a named pipe for the session */
    if (mkfifo(session_pipe_name, 0666) < 0) {
        printf("Error creating session pipe\n"); 
        return -1;
    }

    /* open the session pipe for writing */ 
    int session_pipe_fd = open(session_pipe_name, O_RDONLY);
    /* check for error opening the session pipe */
    if (session_pipe_fd < 0) {
        printf("Error opening session pipe\n");
        return -1;
    }

    if (signal(SIGINT, sig_handler) == SIG_ERR) {
        exit(EXIT_FAILURE);
    }
    if (signal(SIGQUIT, sig_handler) == SIG_ERR) {
        exit(EXIT_FAILURE);
    }

    char msg[1024];
    while(1){
        ssize_t flg2 = read(session_pipe_fd, msg, 1024);
        if(flg2 <= 0){
            perror("Error reading from session pipe");
            break;
            }
        else if (flg2 > 0) {
            printf("Mensagem presente na caixa: '%s' \n", msg);
            break;
        }
    }
    close(session_pipe_fd);
    unlink(session_pipe_name);
    return 0;
}