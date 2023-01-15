#include "logging.h"
#include "messages.h"
#include "operations.h"
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>

typedef struct box{
    char id[32];
    uint64_t size;
    uint64_t pub_num;
    uint64_t sub_num;
}Box;

struct box_array{
    struct box * boxes;
    int size;
};

struct thread_data {
    pthread_t thread_id;
    char session_pipe_name[256];
    int thread_status;
};


char box_name[32];
char session_pipe_name[256];
char* register_pipe_name;
int max_sessions;
char sv_order_msg[289];
char msg[1024];
char aswser_msg46[1029];
char aswser_msg8[58];
__uint8_t i;
int32_t flag = 0;


void create_box_array(struct box_array *arr, size_t  x) {
    if(x == 0) {
        arr->boxes = NULL;
        arr->size = 0;
    } else {
        arr->boxes = malloc(x * sizeof(struct box));
        arr->size = (int)x;
    }
}

void increment_box_array(struct box_array *arr) {
    if(arr->boxes == NULL) {
        arr->boxes = malloc(sizeof(struct box));
    } else {
        arr->boxes = realloc(arr->boxes, (size_t)(arr->size + 1) * sizeof(struct box));
    }
    arr->size++;
}

void decrement_box_array(struct box_array *arr) {
    if(arr->size > 0) {
        arr->size--;
        if(arr->size == 0) {
            free(arr->boxes);
            arr->boxes = NULL;
        } else {
            arr->boxes = realloc(arr->boxes, (size_t)arr->size * sizeof(struct box));
        }
    }
}

void free_box_array(struct box_array *arr) {
    free(arr->boxes);
    arr->boxes = NULL;
    arr->size = 0;
}

int create_box(struct box_array *arr, char* name) {
    int p;
    for (p = 0; p < arr->size; p++) {
        if (strcmp(arr->boxes[p].id, name) == 0) {
            printf("Box with name %s already exists\n", name);
            return -1;
        }
    }
    increment_box_array(arr);
    strcpy(arr->boxes[arr->size - 1].id, name);
    printf("New box with name %s created\n", name);
    return 0;
}

int delete_box(struct box_array *arr, char* name) {
    int j,l;
    for (l = 0; l < arr->size; l++) {
        if (strcmp(arr->boxes[l].id, name) == 0) {
            for (j = l; j < arr->size - 1; j++) {
                arr->boxes[j] = arr->boxes[j + 1];
            }
            decrement_box_array(arr);
            printf("Box with name %s deleted\n", name);
            return 0;
        }
    }
    printf("Box with name %s not found\n", name);
    return -1;
}

void sort_boxes(struct box_array *arr) {
    int u, t;
    for (u = 0; u < arr->size - 1; u++) {
        for (t = 0; t < arr->size - u - 1; t++) {
            if (strcmp(arr->boxes[t].id, arr->boxes[t + 1].id) > 0) {
                struct box temp = arr->boxes[t];
                arr->boxes[t] = arr->boxes[t + 1];
                arr->boxes[t + 1] = temp;
            }
        }
    }
}

void* thread_handler(void* thread_data) {
    struct thread_data* data = (struct thread_data*) thread_data;
    // Lógica do programa, como a criação de caixas de mensagem, a remoção de caixas de mensagem e a listagem de caixas existentes
    data->thread_status = 0;
    return NULL;
    }

int get_order(int fd){
    memset(sv_order_msg,0,sizeof(sv_order_msg));
    memset(session_pipe_name,0,sizeof(session_pipe_name));
    memset(box_name,0,sizeof(box_name));
    memset(msg,0,sizeof(msg));

    char *token;
    ssize_t n = read(fd, sv_order_msg, sizeof(sv_order_msg)-1);
    if(n <= 0){
        return (int)n;
    }

    printf("recebi %zu bytes\n", n);
    printf("A mensagem enviada para o mbroker foi: %s\n", sv_order_msg);
    printf("res: %i\n", (int)sv_order_msg[0]);

    if(sv_order_msg[0] != 7 && (int)n > 1){
    token = strtok(sv_order_msg," ");
    token = strtok(NULL, " "); // skipping the first token
    strcpy(session_pipe_name, token);
    token = strtok(NULL, " ");
    strcpy(box_name, token);
    }
    if(sv_order_msg[0] == 7 && (int)n > 1){
    token = strtok(sv_order_msg," ");
    token = strtok(NULL, " "); // skipping the first token
    strcpy(session_pipe_name, token);
    }
    return (int)n;
}



int main(int argc, char **argv) {

    tfs_init(NULL);
    struct thread_data thread_list[max_sessions];
    pthread_mutex_t thread_list_mutex = PTHREAD_MUTEX_INITIALIZER;
    
    if (argc < 3) {
        return -1;
    }
    register_pipe_name = argv[1];
    max_sessions = atoi(argv[2]);
    if(max_sessions <= 0){
       return -1;
    }

    for (i = 0; i < max_sessions; i++) {
        thread_list[i].thread_status = -1;
    }

    struct box_array boxes_list;
    create_box_array(&boxes_list, 0);

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

    // Lógica principal do programa para processar mensagens do cliente e lidar com operações de caixa de mensagem
    while (1) {

        int bytes_read = get_order(fd);
        if (bytes_read == -1) {
            perror("Error reading from register pipe");
        } else {

            // Lógica para ler a mensagem do cliente através do pipe de registro

            // Lógica para processar a mensagem e realizar as operações de caixa de mensagem
            if (bytes_read > 0) {
                // Lógica para ler o nome do pipe de sessão do cliente e armazená-lo na estrutura de dados da thread
                pthread_mutex_lock(&thread_list_mutex);
                for (i = 0; i < max_sessions; i++) {
                    if (thread_list[i].thread_status == -1) {
                        strcpy(thread_list[i].session_pipe_name, session_pipe_name);
                        thread_list[i].thread_status = 1;
                        pthread_create(&thread_list[i].thread_id, NULL, thread_handler, (void*)&thread_list[i]);
                        break;
                    }
                }
                pthread_mutex_unlock(&thread_list_mutex);

                switch ((int)sv_order_msg[0]){
            /* ---------------------------------------------- LINHA APENAS PARA AJUDA VISUAL -------------------------------------------------*/

                case 1:
                    char dir1[33];
                    int x1;
                    int flag_1 = 0;
                    memset(dir1,0,33);
                    memcpy(dir1,"/",sizeof(char));
                    memcpy(dir1+sizeof(char),box_name,sizeof(box_name));

                    int session_pipe_1 = open(session_pipe_name, O_RDONLY);
                    if (session_pipe_1 < 0) {
                        printf("Error opening session pipe\n");
                    return -1;
                    }
                    ssize_t flg_1 = read(session_pipe_1,msg,1024);
                    if(flg_1 == -1){
                        return -1;
                    }
                    printf("A Mensagem do user foi: %s \n", msg);

                    for (x1=0; x1 < boxes_list.size; x1++){
                        if(strcmp(boxes_list.boxes[x1].id,box_name)==0){
                            if(boxes_list.boxes[x1].pub_num == 0){
                                flag_1 = 1;
                            }
                        }
                    }

                    char new_msg[1024];
                    memset(new_msg,0,1024);
                    strcpy(new_msg,msg_pub(msg));
                    
                    if(flag_1 == 0){
                        printf("Box does not exist or have already a publisher \n");
                        close(session_pipe_1);
                        unlink(session_pipe_name);
                        return -1;
                    }

                    int flg_tfs_open1 = tfs_open(dir1, TFS_O_APPEND|O_WRONLY);
                    if(flg_tfs_open1 == -1){
                            printf("Erro ao abrir a caixa no TFS \n");
                            return -1;
                    }

                    int flg_tfs_write1 = (int)tfs_write(flg_tfs_open1,new_msg,sizeof(new_msg));
                    for (x1=0; x1 < boxes_list.size; x1++){
                        if(strcmp(boxes_list.boxes[x1].id,box_name)==0){
                            boxes_list.boxes[x1].size += (uint64_t)strlen(new_msg);
                            boxes_list.boxes[x1].pub_num = 1;
                        }
                    }
                    if(flg_tfs_write1 == -1){
                            printf("Erro ao escrever na caixa \n");
                            return -1;
                    }

                    close(session_pipe_1);
                    tfs_close(flg_tfs_open1);
                    unlink(session_pipe_name);
                    break;

                /* ---------------------------------------------- LINHA APENAS PARA AJUDA VISUAL -------------------------------------------------*/

                case 2:
                    char msg_sub[1025];
                    char box_msg[1024];
                    char dir2[33];
                    memset(box_msg, 0, sizeof(box_msg));
                    memset(msg_sub, 0, sizeof(msg_sub));
                    memset(dir2,0,33);
                    memcpy(dir2,"/",sizeof(char));
                    memcpy(dir2+sizeof(char),box_name,sizeof(box_name));

                    i = 10;
                    int x2;
                    int flag_2 = 0;
                    for (x2=0; x2 < boxes_list.size; x2++){
                        if(strcmp(boxes_list.boxes[x2].id,box_name)==0){
                            flag_2 = 1;
                            break;
                        }
                    }
                    if(flag_2 == 0){
                        printf("Box doesnt exist \n");
                        return -1;
                    }
                    int flg_tfs_open2 = tfs_open(dir2, O_RDONLY);
                    if(flg_tfs_open2 == -1){
                            printf("Erro ao abrir a caixa no TFS \n");
                            return -1;
                    }
                    int flg_tfs_read = (int)tfs_read(flg_tfs_open2, box_msg, 1024);
                    if(flg_tfs_read == -1){
                            printf("Erro ao ler a caixa no TFS \n");
                            return -1;
                    }

                    strcpy(msg_sub,msg_to_sub(i, box_msg));

                    int session_pipe_sub = open(session_pipe_name, O_WRONLY);
                    if (session_pipe_sub < 0) {
                        printf("Error opening session pipe\n");
                    return -1;
                    }
                    ssize_t flg2 = write(session_pipe_sub, msg_sub, sizeof(msg_sub));
                    if(flg2 == -1){
                        return -1;
                    }
                    
                    boxes_list.boxes[x2].sub_num += 1;
                    printf("A Mensagem enviada foi: %s \n", msg_sub);
                    
                    close(session_pipe_sub);
                    tfs_close(flg_tfs_open2);
                    unlink(session_pipe_name);
                    break;

                /* ---------------------------------------------- LINHA APENAS PARA AJUDA VISUAL -------------------------------------------------*/

                case 3:
                    flag = create_box(&boxes_list,box_name);
                    i = 4;
                    memset(aswser_msg46,0,1029);
                    if(flag == 0){
                        char dir[33];

                        memset(dir,0,33);
                        memcpy(dir,"/",sizeof(char));
                        memcpy(dir+sizeof(char),box_name,sizeof(box_name));
                        
                        int flg_tfs = tfs_open(dir, TFS_O_CREAT);
                        if(flg_tfs == -1){
                            printf("Erro ao criar a caixa no TFS \n");
                            return -1;
                        }
                    
                        char* error_message = "";
                        strcpy(aswser_msg46,create_manager_msg_4_6(i,flag,error_message));
                    }
                    if(flag == -1){
                        char* error_message = "Não foi possivel a criação da caixa";
                        strcpy(aswser_msg46,create_manager_msg_4_6(i,flag,error_message));
                    }
                    int session_pipe_3 = open(session_pipe_name, O_WRONLY);
                    if (session_pipe_3 < 0) {
                        printf("Error opening session pipe\n");
                        return -1;
                    }
                    ssize_t flg_3 = write(session_pipe_3,aswser_msg46,1029);
                    if(flg_3 == -1){
                        return -1;
                    }
                    printf("A Mensagem enviada foi: %s \n", aswser_msg46);
                    close(session_pipe_3);
                    unlink(session_pipe_name);
                    break;

                /* ---------------------------------------------- LINHA APENAS PARA AJUDA VISUAL -------------------------------------------------*/

                case 5:
                    flag = delete_box(&boxes_list,box_name);
                    i = 6;
                    memset(aswser_msg46,0,1029);
                    if(flag == 0){

                        char dir[sizeof(box_name)+1];

                        memset(dir,0,33);
                        memcpy(dir,"/",sizeof(char));
                        memcpy(dir+sizeof(char),box_name,sizeof(box_name));
                        
                        int flg_tfs = tfs_unlink(dir);
                        if(flg_tfs == -1){
                            printf("Erro ao apagar caixa no TFS \n");
                            return -1;
                        }

                        char* error_message = "";
                        strcpy(aswser_msg46,create_manager_msg_4_6(i,flag,error_message));
                    }
                    if(flag == -1){
                        char* error_message = "Não foi possivel a remoção da caixa";
                        strcpy(aswser_msg46,create_manager_msg_4_6(i,flag,error_message));
                    }
                    int session_pipe_5 = open(session_pipe_name, O_WRONLY);
                    if (session_pipe_5 < 0) {
                        printf("Error opening session pipe\n");
                        return -1;
                    }
                    ssize_t flg_5 = write(session_pipe_5,aswser_msg46,1029);
                    if(flg_5 == -1){
                        return -1;
                    }
                    close(session_pipe_5);
                    unlink(session_pipe_name);
                    break;

                /* ---------------------------------------------- LINHA APENAS PARA AJUDA VISUAL -------------------------------------------------*/

                case 7:
                    int y;
                    uint8_t last;
                    char temp_box_n[32];
                    memset(temp_box_n, 0, 32);
                    memset(aswser_msg8, 0, 58);
                    int fd_7 = open(session_pipe_name, O_WRONLY);
                    i = 8;
                    if (boxes_list.size == 0){
                        printf("session pipe: %s \n",session_pipe_name);
                        last = 1;
                        if(fd_7 < 0){
                            printf("Error opening session pipe %s \n",session_pipe_name);
                            close(fd_7);
                            return -1;
                        }
                        strcpy(aswser_msg8, msg_list_null(i, last, temp_box_n));
                        ssize_t flg_7 = write(fd_7, aswser_msg8, sizeof(aswser_msg8));
                        if(flg_7 == -1){
                            close(fd_7);
                            return -1;
                        }
                        printf("Mensagem enviada:  CODE(%d) LAST(%d) \n",aswser_msg8[0],aswser_msg8[2]);
                        close(fd_7);
                        break;
                    }
                    for (y = 0; y < boxes_list.size; y++){
                        memset(aswser_msg8, 0, 58);
                        if(fd_7 < 0){
                            printf("Error opening session pipe\n");
                            close(fd_7);
                            return -1;
                        }
                        if(y == boxes_list.size-1){
                            last = 1;
                        }
                        else{
                            last = 0;
                        }
                        strcpy(aswser_msg8, msg_list(i, last, box_name, boxes_list.boxes[y].size, boxes_list.boxes[y].pub_num, boxes_list.boxes[y].sub_num));
                        if(write(fd_7, aswser_msg8, sizeof(aswser_msg8)) == -1){
                            close(fd_7);
                            return -1;
                        }
                    }
                    close(fd_7);
                    unlink(session_pipe_name);
                    break;

                /* ---------------------------------------------- LINHA APENAS PARA AJUDA VISUAL -------------------------------------------------*/

                default:
                    break;
                }
            }
        }    
    }

    free_box_array(&boxes_list);
    pthread_exit(NULL);
    unlink(register_pipe_name);
    tfs_destroy();
    return 0;
}
