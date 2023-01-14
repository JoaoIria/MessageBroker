#include <stdio.h>
#include <string.h>



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


char* create_manager_msg_4_6(__uint8_t i, int32_t return_code, const char* error_message) {
    static char msg[1029];
    memset(msg, 0, 1029);
        
    // copy the value of i to the first byte of sv_order_msg
    memcpy(msg, &i, sizeof(__uint8_t));  

    // add a space after i
    memcpy(msg + sizeof(__uint8_t), " ", sizeof(char));

    // add return_code
    memcpy(msg + sizeof(__uint8_t) + sizeof(char), &return_code, sizeof(return_code));  

    // add a space after return_code
    memcpy(msg + sizeof(__uint8_t) + sizeof(char) + sizeof(return_code), " ", sizeof(char)); 

    // add error message
    memcpy(msg + sizeof(__uint8_t) + sizeof(char) * 2 + sizeof(return_code), error_message, sizeof(char) * strlen(error_message)); 

    // add null terminator
    msg[sizeof(__uint8_t) + sizeof(char) * 2 + sizeof(return_code) + strlen(error_message)] = '\0'; 
    return msg;

}


