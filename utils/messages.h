#include <stdio.h>
#include <string.h>
#include <stdint.h>


char* create_sv_order_msg(__uint8_t i, const char* session_pipe_name, const char* box_name) {
    static char sv_order_msg[289];
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
    sv_order_msg[sizeof(__uint8_t) + sizeof(char) * 2 + strlen(session_pipe_name) + strlen(box_name)] = '\0'; 
    return sv_order_msg;
}

char* msg_to_sub(__uint8_t i, const char* box_msg){
    static char msg_to_sub[1025];
    memset(msg_to_sub, 0, sizeof(msg_to_sub));

    // copy the value of i to the first byte of sv_order_msg
    memcpy(msg_to_sub, &i, sizeof(__uint8_t)); 

    // add a space after i
    memcpy(msg_to_sub + sizeof(__uint8_t), " ", sizeof(char)); 

    // add session_pipe_name
    memcpy(msg_to_sub + sizeof(__uint8_t) + sizeof(char), box_msg, strlen(box_msg));

    msg_to_sub[sizeof(__uint8_t) + sizeof(char) * 2 + strlen(box_msg)] = '\0'; 
    return msg_to_sub;
}

char* create_manager_msg_4_6(__uint8_t code, int32_t return_code, char* error_message) {
    static char array[1050];
    memset(array, 0, sizeof(array));
    char temp[50];
    // copy the value of code to the first byte of array
    memcpy(array, &code, sizeof(__uint8_t));
    // add a space after code
    memcpy(array + sizeof(__uint8_t), " ", sizeof(char));

    // add the return_code
    int offset = sizeof(__uint8_t) + sizeof(char);
    if(return_code < 0) {
        sprintf(temp,"-%d", -return_code);
    } else {
        sprintf(temp,"%d", return_code);
    }
    memcpy(array + offset, temp, strlen(temp));
    offset += (int)strlen(temp);
    // add a space after return_code
    memcpy(array + offset, " ", sizeof(char));

    // add error_message
    offset += (int)sizeof(char);
    if(return_code == -1) {
        memcpy(array + offset, error_message, strlen(error_message));
        offset += (int)strlen(error_message);
    } else {
        memcpy(array + offset, "\0", sizeof(char));
        offset += 1;
    }
    // add null terminator
    array[offset] = '\0';
    return array;
}


char* msg_pub(char* box_msg){
    /* Declare a static buffer named msg_pub with a fixed size of 1024 */
    static char msg_pub[1024];
    /* Clear the buffer by setting all its elements to 0 */
    memset(msg_pub, 0, 1024);
    /* Search for the first space character in the box_msg string using strstr */
    char * ptr = strstr(box_msg, " ");
    /* Copy the string starting from the character after the space into the msg_pub buffer */
    strcpy(msg_pub,ptr+1);
    /* Return the msg_pub buffer */
    return msg_pub;
}


char* msg_list(__uint8_t i, __uint8_t last, char box_name[32], uint64_t box_size, __uint64_t n_publishers , __uint64_t n_subscribers){
    /* Declare a static buffer named msg_list with a fixed size of 58 */
    static char msg_list[58];

    /* Clear the buffer with memset */
    memset(msg_list, 0, sizeof(msg_list));

    /* Copy the code value to the first byte of the buffer with memcpy */
    memcpy(msg_list, &i, sizeof(i));

    /* Add a space to the buffer*/
    msg_list[sizeof(i)] = ' ';

    /* Copy the last value to the second byte of the buffer with memcpy */
    memcpy(msg_list + sizeof(i) + 1, &last, sizeof(last));

    /* Add a space to the buffer*/
    msg_list[sizeof(i) + sizeof(last) + 1] = ' ';

    /* Copy the box_name value to the third byte of the buffer with memcpy */
    memcpy(msg_list + sizeof(i) + sizeof(last) + 2, &box_name, strlen(box_name));

    /* Add a space to the buffer*/
    msg_list[sizeof(i) + sizeof(last) + strlen(box_name) + 2] = ' ';

    /* Copy the box_size value to the 35th byte of the buffer with memcpy */
    memcpy(msg_list + sizeof(i) + sizeof(last) + strlen(box_name) + 3, &box_size, sizeof(box_size));

    /* Add a space to the buffer*/
    msg_list[sizeof(i) + sizeof(last) + strlen(box_name) + sizeof(box_size) + 3] = ' ';

    /* Copy the n_publishers value to the 43th byte of the buffer with memcpy */
    memcpy(msg_list + sizeof(i) + sizeof(last) + strlen(box_name) + sizeof(box_size) + 4, &n_publishers, sizeof(n_publishers));

    /* Add a space to the buffer*/
    msg_list[sizeof(i) + sizeof(last) + strlen(box_name) + sizeof(box_size) + sizeof(n_publishers) + 4] = ' ';

    /* Copy the n_subscribers value to the 51th byte of the buffer with memcpy */
    memcpy(msg_list + sizeof(i) + sizeof(last) + strlen(box_name) + sizeof(box_size) + sizeof(n_publishers) + 5, &n_subscribers, sizeof(n_subscribers));

    /* Return the msg_list buffer */
    return msg_list;
}


char* msg_list_null(__uint8_t i, __uint8_t last, char box_name[32]){
    /* Declare a static buffer named msg_list with a fixed size of 58 */
    static char msg_list[58];

    /* Clear the buffer with memset */
    memset(msg_list, 0, sizeof(msg_list));

    /* Copy the code value to the first byte of the buffer with memcpy */
    memcpy(msg_list, &i, sizeof(i));

    /* Add a space to the buffer*/
    msg_list[sizeof(i)] = ' ';

    /* Copy the last value to the second byte of the buffer with memcpy */
    memcpy(msg_list + sizeof(i) + 1, &last, sizeof(last));

    /* Add a space to the buffer*/
    msg_list[sizeof(i) + sizeof(last) + 1] = ' ';

    /* Copy the box_name value to the third byte of the buffer with memcpy */
    memcpy(msg_list + sizeof(i) + sizeof(last) + 2, &box_name, strlen(box_name));

    /* Return the msg_list buffer */
    return msg_list;
}



