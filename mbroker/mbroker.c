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


// Struct for the boxes used in the program
typedef struct box{
    char id[32];
    uint64_t size;
    uint64_t pub_num;
    uint64_t sub_num;
}Box;

/* Struct that contains the boxes used in the program
and how many */

struct box_array{
    struct box * boxes;
    int size;
};

/* Struct that contains the threads used in the program
and their information */

struct thread_data {
    pthread_t thread_id;
    char session_pipe_name[256];
    int thread_status;
};


/* GLOBAL VARIABLES */
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


/*This function takes a pointer to a "struct box_array" and a 
"size_t" as its arguments. If the size argument is 0, it sets the "boxes" 
field of the struct to NULL and the "size" field to 0. If the size argument 
is not 0, it allocates memory for an array of "size" "struct box" elements and 
assigns the address of the first element of the array to the "boxes" field of 
the struct and the size of the array to the "size" field.*/

void create_box_array(struct box_array *arr, size_t  x) {
    if(x == 0) {
        arr->boxes = NULL;
        arr->size = 0;
    } else {
        arr->boxes = malloc(x * sizeof(struct box));
        arr->size = (int)x;
    }
}


/* This function takes a pointer to a "struct box_array" as its argument. 
It first checks if the "boxes" field is NULL and if it is, it allocates memory 
for one "struct box" element. Otherwise, it reallocates memory to fit an additional 
"struct box" element and increases the "size" field by 1.*/

void increment_box_array(struct box_array *arr) {
    if(arr->boxes == NULL) {
        arr->boxes = malloc(sizeof(struct box));
    } else {
        arr->boxes = realloc(arr->boxes, (size_t)(arr->size + 1) * sizeof(struct box));
    }
    arr->size++;
}


/* This function takes a pointer to a "struct box_array" as its argument. 
It first checks if the "size" field is greater than 0. If it is, it decreases the 
"size" field by 1 and reallocates memory to fit one less "struct box" element. If the 
"size" field is 0, it frees the memory allocated for the "boxes" field and sets it to NULL.*/

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


/*This function takes a pointer to a "struct box_array" as its argument. 
It frees the memory allocated for the "boxes" field and sets it to NULL and 
the "size" field to 0.*/

void free_box_array(struct box_array *arr) {
    free(arr->boxes);
    arr->boxes = NULL;
    arr->size = 0;
}


/* This function takes a pointer to a "struct box_array" and a string 
as its arguments. It first checks if a box with the given string as name 
already exists in the array. If it does, it prints an error message and returns -1. 
If it does not exist, it calls the increment_box_array() function to allocate memory 
for an additional element in the array, copies the string to the last element of the 
array and prints a success message.*/

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


/*This function takes a pointer to a "struct box_array" and a string as its arguments.
It first checks if a box with the given string as name exists in the array. If it does,
it removes the box from the array by shifting the elements after it and calls the 
decrement_box_array() function to resize the array and prints a success message. 
If it does not exist, it prints an error message.*/

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


/* This function takes a pointer to a "struct box_array" as its argument. 
It sorts the boxes in the array by their name using the bubble sort algorithm.*/

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


/*This function is a thread handler. It takes a pointer to a "struct thread_data" 
as its argument. It sets the "thread_status" field of the struct to 0 and returns 
a null pointer.*/

void* thread_handler(void* thread_data) {
    struct thread_data* data = (struct thread_data*) thread_data;
    data->thread_status = 0;
    return NULL;
    }


/* Function that processes register pipe requests */

int get_order(int fd){

    // Clear several buffers
    memset(sv_order_msg,0,sizeof(sv_order_msg));
    memset(session_pipe_name,0,sizeof(session_pipe_name));
    memset(box_name,0,sizeof(box_name));
    memset(msg,0,sizeof(msg));

    char *token;
    // Read data from file descriptor into sv_order_msg
    ssize_t n = read(fd, sv_order_msg, sizeof(sv_order_msg)-1);
    if(n <= 0){
        return (int)n;
    }

    // printf("The message received by the mbroker was: %s\n", sv_order_msg);
    // printf("CODE: %i\n", (int)sv_order_msg[0]);

    // Check the value of the first character of the message
    if(sv_order_msg[0] != 7 && (int)n > 1){
    // Tokenize the message and extract the second and third tokens
    token = strtok(sv_order_msg," ");
    token = strtok(NULL, " "); // skipping the first token
    strcpy(session_pipe_name, token);
    token = strtok(NULL, " ");
    strcpy(box_name, token);
    }
    
    if(sv_order_msg[0] == 7 && (int)n > 1){
    // Tokenize the message and extract the second token
    token = strtok(sv_order_msg," ");
    token = strtok(NULL, " "); // skipping the first token
    strcpy(session_pipe_name, token);
    }

    // Return the number of bytes read from the file descriptor
    return (int)n;
}



int main(int argc, char **argv) {

    // Initialize all
    tfs_init(NULL);
    struct thread_data thread_list[max_sessions];
    pthread_mutex_t thread_list_mutex = PTHREAD_MUTEX_INITIALIZER;
    
    // Checks if the received arguments are valid
    if (argc < 3) {
        printf("Invalid arguments \n"); 
        return -1;
    }
    
    // put the arguments in the respective places
    register_pipe_name = argv[1];
    max_sessions = atoi(argv[2]);
    if(max_sessions <= 0){
       return -1;
    }

    // Iterate through the thread_list array
    for (i = 0; i < max_sessions; i++) {
        // Set the thread_status for each element in the array to -1
        thread_list[i].thread_status = -1;
    }


    // initializes the boxes structure to zero and with the name ' boxes_list '
    struct box_array boxes_list;
    create_box_array(&boxes_list, 0);

    /* print to check that all components are being stored in the right places */
    // printf("mbroker.register_pipe = '%s' max_sessions = '%d' \n",register_pipe_name, max_sessions);

    /* see if the pipe exists and if it exists, delete it */
    if (access(register_pipe_name, F_OK) == 0){
        unlink(register_pipe_name); 
    }
    
    /* creates the pipe where all requests will be received */
    if (mkfifo(register_pipe_name, 0666) < 0) {
        printf("Error creating register pipe\n"); 
        return -1;
    }

    /* open the requests pipe for reading */
    int fd = open(register_pipe_name, O_RDONLY); 
    if (fd < 0) {
        printf("Error opening register pipe\n"); 
        return -1;
    }

    // Main program logic for processing client messages and handling message box operations
    while (1) {

        int bytes_read = get_order(fd);
        if (bytes_read == -1) {
            perror("Error reading from register pipe");
        } else {

            // Logic to process the message and perform message box operations
            if (bytes_read > 0) {
                // Logic to read the session pipe name from the client and store it in the thread's data structure
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
            /* ---------------------------------------------- PUBLISHER REGISTRATION REQUEST -------------------------------------------------*/

                case 1:
                    char dir1[33];
                    int x1;
                    int flag_1 = 0;

                    /* code for building the directory used in TFS */
                    memset(dir1,0,33); 
                    memcpy(dir1,"/",sizeof(char));
                    memcpy(dir1+sizeof(char),box_name,sizeof(box_name));

                    /* open the session_pipe for reading */
                    int session_pipe_1 = open(session_pipe_name, O_RDONLY);
                    if (session_pipe_1 < 0) {
                        printf("Error opening session pipe\n");
                    return -1;
                    }
                    
                     //logic for processing publisher messages
                    while(1){
                        
                         // opes the session_pipes and reads the string inside
                        ssize_t flg_1 = read(session_pipe_1,msg,1024);
                        if(flg_1 <= 0){
                            perror("Error reading from session pipe");
                            break;
                        }
                        else{

                            // print for visual effect of the message
                            printf("The user message was: %s \n", msg);

                            // updates the number of publishers in the box
                            for (x1=0; x1 < boxes_list.size; x1++){
                                if(strcmp(boxes_list.boxes[x1].id,box_name)==0){
                                    if(boxes_list.boxes[x1].pub_num == 0){
                                        flag_1 = 1;
                                    }
                                }
                            }

                            // reset the message to send
                            char new_msg[1024];
                            memset(new_msg,0,1024);

                            // build the message to send
                            strcpy(new_msg,msg_pub(msg));
                            
                            // if the box does not exist or already has a publisher
                            if(flag_1 == 0){
                                printf("Box does not exist or have already a publisher \n");
                                close(session_pipe_1);
                                unlink(session_pipe_name);
                                return -1;
                            }

                            int flg_tfs_open1 = tfs_open(dir1, TFS_O_APPEND|O_WRONLY);
                            if(flg_tfs_open1 == -1){
                                    printf("Error opening box in TFS \n");
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
                                    printf("Error writing in box \n");
                                    return -1;
                            }
                        tfs_close(flg_tfs_open1);
                        }
                    }
                close(session_pipe_1);
                unlink(session_pipe_name);
                break;

                /* ---------------------------------------------- SUBSCRIBER REGISTRATION REQUEST -------------------------------------------------*/

                case 2:
                    char msg_sub[1028];
                    char box_msg[1024];
                    char dir2[33];
                    //Sets the memory of all the variables needed to 0 emptying them
                    memset(box_msg, 0, sizeof(box_msg));
                    memset(msg_sub, 0, sizeof(msg_sub));

                    /* code for building the directory used in TFS */
                    memset(dir2,0,33);
                    memcpy(dir2,"/",sizeof(char));
                    memcpy(dir2+sizeof(char),box_name,sizeof(box_name));
                    

                    i = 10;
                    int x2;
                    int flag_2 = 0;
                    // Goes through all the boxes
                    for (x2=0; x2 < boxes_list.size; x2++){
                        //If it finds the box desired is sets the flag to 1
                        if(strcmp(boxes_list.boxes[x2].id,box_name)==0){
                            flag_2 = 1;
                            break;
                        }
                    }
                    //if the flag is still 0 it meandsthe desired box doesen't exist so it returns -1
                    if(flag_2 == 0){
                        printf("Box doesnt exist \n");
                        return -1;
                    }
                    //Opens the box for reading
                    int flg_tfs_open2 = tfs_open(dir2, O_RDONLY);
                    //If the program fails to open the box it returns -1
                    if(flg_tfs_open2 == -1){
                        
                            printf("Erro ao abrir a caixa no TFS \n");
                            return -1;
                    }

                    //Reads the content of the box
                    int flg_tfs_read = (int)tfs_read(flg_tfs_open2, box_msg, 1024);
                    //if it fails to open the box it returns -1
                    if(flg_tfs_read == -1){ 
                            printf("Erro ao ler a caixa no TFS \n");
                            return -1;
                    }
                    
                    //Builds the message to send to the subscriber's pipe and saves it to the variable "msg_sub"
                    sprintf(msg_sub,"%d %s",i,box_msg);
                    //Opens the subscriber's pipe for writing only
                    int session_pipe_sub = open(session_pipe_name, O_WRONLY);
                    //If it fails to open the pipe it returns 
                    if (session_pipe_sub < 0) {
                        printf("Error opening session pipe\n");
                        return -1;
                    }
                    // Writes the message to the subscriber's pipe
                    ssize_t flg2 = write(session_pipe_sub, msg_sub, sizeof(msg_sub));
                    if(flg2 == -1){
                        return -1;
                    }
                    //Increments the number os subscribers that read from the box                
                    boxes_list.boxes[x2].sub_num += 1;
                    printf("A Mensagem enviada foi: %s \n", msg_sub);
                    //Closes the pipe                    
                    close(session_pipe_sub);
                    //Closes the box
                    tfs_close(flg_tfs_open2);
                    //Unlinks the subscriber's pipe
                    unlink(session_pipe_name);

                    break;

                /* ---------------------------------------------- BOX CREATION REQUEST -------------------------------------------------*/

                case 3:
                    flag = create_box(&boxes_list,box_name);
                    i = 4;
                    memset(aswser_msg46,0,1029);
                    if(flag == 0){
                        char dir[33];

                        /* code for building the directory used in TFS */
                        memset(dir,0,33);
                        memcpy(dir,"/",sizeof(char));
                        memcpy(dir+sizeof(char),box_name,sizeof(box_name));

                        //Creates the box in the desired directory                     
                        int flg_tfs = tfs_open(dir, TFS_O_CREAT);
                        //If it fails to create returns -1
                        if(flg_tfs == -1){
                            printf("Error creating box in TFS \n");
                            return -1;
                        }
                    
                        //Creates the message to send to the manager and saves it to the "aswer_msg46" variable
                        char* error_message = "";
                        strcpy(aswser_msg46,create_manager_msg_4_6(i,flag,error_message));

                    }
                    //If it failed to create the box returns -1
                    if(flag == -1){
                        //Creates the message to send to the manager and saves it to the "aswer_msg46" variable
                        char* error_message = "Failed to create box";
                        strcpy(aswser_msg46,create_manager_msg_4_6(i,flag,error_message));

                    }
                    //Opens the manager's pipe
                    int session_pipe_3 = open(session_pipe_name, O_WRONLY);
                    //If it fails to open the manager's pipe it returns -1
                    if (session_pipe_3 < 0) {

                        printf("Error opening session pipe\n");
                        return -1;
                    }
                    //CWrites the message to the manager's pipe
                    ssize_t flg_3 = write(session_pipe_3,aswser_msg46,1029);
                    //If it fails to write it returns -1 
                    if(flg_3 == -1){

                        return -1;
                    }
                    //Closes the managrer's pipe
                    close(session_pipe_3);
                    //unlinks the manager's pipe
                    unlink(session_pipe_name);

                    break;

                /* ---------------------------------------------- BOX REMOVAL REQUEST -------------------------------------------------*/

                case 5:
                    flag = delete_box(&boxes_list,box_name);
                    i = 6;
                    memset(aswser_msg46,0,1029);
                    if(flag == 0){

                        char dir[sizeof(box_name)+1];

                        memset(dir,0,33);
                        memcpy(dir,"/",sizeof(char));
                        memcpy(dir+sizeof(char),box_name,sizeof(box_name));

                         // Deletes the box                       
                        int flg_tfs = tfs_unlink(dir);
                        //If it fails to delete the box returns -1
                        if(flg_tfs == -1){

                            printf("Error deleting box in TFS \n");
                            return -1;
                        }

                        char* error_message = "";
                        //Builds and thaves the messsage to send to the manager in the "aswer_msg48" variable
                        strcpy(aswser_msg46,create_manager_msg_4_6(i,flag,error_message));

                    }
                    //if it fails to remove the box returns -1
                    if(flag == -1){

                        char* error_message = "It was not possible to remove the box";
                        //Builds and thaves the messsage to send to the manager in the "aswer_msg48" variable
                        strcpy(aswser_msg46,create_manager_msg_4_6(i,flag,error_message));
                    }
                    //Opens the maneger's pipe for reading only
                    int session_pipe_5 = open(session_pipe_name, O_WRONLY);
                    //If it fails to open the manager's pipe it returns -1
                    if (session_pipe_5 < 0) {
                        printf("Error opening session pipe\n");
                        return -1;
                    }
                    //Writes the message to the manager's pipe
                    ssize_t flg_5 = write(session_pipe_5,aswser_msg46,1029);
                    //if it fails to write the message to the manager's pipe it returns -1
                    if(flg_5 == -1){
                        return -1;
                    }
                    //Closes the manager's pipe
                    close(session_pipe_5);
                    //Unlinks the manager's pipe
                    unlink(session_pipe_name);
                    break;

                /* ---------------------------------------------- BOX LISTING REQUEST -------------------------------------------------*/

                case 7:
                    int y;
                    uint8_t last;
                    char temp_box_n[32];
                    // Empties the temp_box putting all of it's bites as 0
                    memset(temp_box_n, 0, 32); 
                    // Empties the aswer_msg8 putting all of it's bites as 0
                    memset(aswser_msg8, 0, 58);
                    //Opens the manager's pipe for reading only
                    int fd_7 = open(session_pipe_name, O_WRONLY);
                    i = 8;
                    //If there are no boxes
                    if (boxes_list.size == 0){  
                        printf("session pipe: %s \n",session_pipe_name);
                        // Sets "last" as 1 for the final message 
                        last = 1;
                        //If the pipe does not exist
                        if(fd_7 < 0){
                            printf("Error opening session pipe %s \n",session_pipe_name);
                            close(fd_7);
                            return -1;
                        }
                        // Buids the message to send to the manager
                        strcpy(aswser_msg8, msg_list_null(i, last, temp_box_n));
                        //Sends the message to the manager's pipe
                        ssize_t flg_7 = write(fd_7, aswser_msg8, sizeof(aswser_msg8));
                        if(flg_7 == -1){
                            close(fd_7);
                            return -1;
                        }
                        //closes the pipe
                        close(fd_7);
                        break;
                    }
                        //Goes through all the boxes 
                    for (y = 0; y < boxes_list.size; y++){
                        //If the pipe does not exist
                        if(fd_7 < 0){
                            printf("Error opening session pipe\n");
                            close(fd_7);
                            return -1;
                        }
                        //If it is the last box of the server it will set the "last" variable to 1
                        if(y == boxes_list.size-1){
                            last = 1;
                        }
                         //If it isn't the last box of the server it will set the "last" variable to 0
                        else{
                            last = 0;
                        }
                        // Builds the message to send to the manager's pipe and save it to the "aswer_msg8" varianle
                        sprintf(aswser_msg8, "%u %u %s %lu %lu %lu", i, last, boxes_list.boxes[y].id, boxes_list.boxes[y].size, boxes_list.boxes[y].pub_num, boxes_list.boxes[y].sub_num);
                        //writes the message to the manager's pipe and if it fails returns -1
                        if(write(fd_7, aswser_msg8, sizeof(aswser_msg8)) == -1){
                            close(fd_7);
                            return -1;
                        }
                    }
                    //closes the manager's pipe
                    close(fd_7);
                    //unlinkes the session pipe
                    unlink(session_pipe_name);
                    
                    break;

                /* ---------------------------------------------- DEFAULT -------------------------------------------------*/

                default:
                    break;
                }
            }
        }    
    }

    // Ends the program by clearing everything

    // Frees the box array
    free_box_array(&boxes_list);
    // Ends the threads
    pthread_exit(NULL);
    //Unlinks the mbroker's pipe
    unlink(register_pipe_name);
    //Stops tfs and cleans all the data
    tfs_destroy();

    return 0;
}
