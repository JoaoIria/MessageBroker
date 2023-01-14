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
