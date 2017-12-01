//
//  msg.h
//  hwc1
//
//  Created by Federico Yusteenappar on 25/11/17.
//  Copyright © 2017 Federico Yusteenappar. All rights reserved.
//

#ifndef msg_h
#define msg_h

typedef struct message {
    void* content; // generico contenuto del messaggio
    struct message * (*msg_init)(void*); // creazione msg
    void (*msg_destroy)(struct message *); // deallocazione msg
    struct message * (*msg_copy)(struct message *); // creazione/copia msg
}msg_t;

msg_t* msg_init_string(void*); //viene creata una copia "privata" della stringa

void msg_destroy_string(msg_t*);

msg_t* msg_copy_string(msg_t*);

#endif /* msg_h */
