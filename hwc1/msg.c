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

msg_t* msg_init(void* content) {
    //viene creata una copia "privata"
    msg_t* new_msg = (msg_t*)malloc( sizeof(msg_t) );
    new_msg->content = content;
    new_msg->msg_init = msg_init;
    new_msg->msg_destroy = msg_destroy;
    new_msg->msg_copy = msg_copy;
    return new_msg;
}

void msg_destroy(msg_t* msg) {
    free(msg); // free struct
}
msg_t *msg_copy(msg_t* msg) {
    return msg_init(msg->content);
}



