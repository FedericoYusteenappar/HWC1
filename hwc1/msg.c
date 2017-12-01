//
//  msg.c
//  hwc1
//
//  Created by Federico Yusteenappar on 25/11/17.
//  Copyright © 2017 Federico Yusteenappar. All rights reserved.
//

#include <stdio.h>
#include "msg.h"
#include <stdlib.h>
#include <string.h>

msg_t* msg_init_string(void* content) {
    //viene creata una copia "privata"
    msg_t* new_msg = (msg_t*)malloc( sizeof(msg_t) );
    char* string = (char*)content;
    char* new_content = (char*)malloc(strlen(string)+1); // +1 per \0 finale
    strcpy(new_content, string);
    new_msg->content = new_content;
    new_msg->msg_init = msg_init_string;
    new_msg->msg_destroy = msg_destroy_string;
    new_msg->msg_copy = msg_copy_string;
    return new_msg;
}

void msg_destroy_string(msg_t* msg) {
    free(msg->content); // free copia privata
    free(msg); // free struct
}
msg_t* msg_copy_string(msg_t* msg) {
    return msg_init_string(msg->content);
}



