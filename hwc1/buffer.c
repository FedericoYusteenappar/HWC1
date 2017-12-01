//
//  buffer.c
//  hwc1
//
//  Created by Federico Yusteenappar on 25/11/17.
//  Copyright © 2017 Federico Yusteenappar. All rights reserved.
//

#include <stdio.h>
#include "buffer.h"
#include <stdlib.h>
#include <pthread.h>
#include "msg.h"

buffer_t* buffer_init(unsigned int maxsize){
    buffer_t* buffer = malloc(sizeof(buffer_t));
    buffer->array = (msg_t**) malloc(maxsize*sizeof(msg_t*));
    buffer->k = 0;
    buffer->index_d = 0;
    buffer->index_t = 0;
    buffer->maxsize = maxsize;
    pthread_cond_init(&(buffer->not_empyt),NULL);
    pthread_cond_init(&(buffer->not_full),NULL);
    pthread_mutex_init(&(buffer->uso_d), NULL);
    pthread_mutex_init(&(buffer->uso_t), NULL);
    pthread_mutex_init(&(buffer->service), NULL);
    
    return buffer;
}
void buffer_destroy(buffer_t* buffer){
    free(buffer->array);
    pthread_cond_destroy(&(buffer->not_full));
    pthread_cond_destroy(&(buffer->not_empyt));
    pthread_mutex_destroy(&(buffer->uso_d));
    pthread_mutex_destroy(&(buffer->uso_t));
    pthread_mutex_destroy(&(buffer->service));
    free(buffer);
}

msg_t* put_bloccante(buffer_t* buffer, msg_t* msg){
    msg_t* copy = msg_copy_string(msg);
    pthread_mutex_lock(&buffer->service);
    while(buffer->k == buffer->maxsize){
        pthread_cond_wait(&buffer->not_full,&buffer->service);
    }
    pthread_mutex_lock(&buffer->uso_d);
    buffer->array[buffer->index_d] = copy;
    buffer->index_d = (buffer->index_d + 1)%(buffer->maxsize);
    buffer->k++;
    pthread_mutex_unlock(&buffer->uso_d);
    pthread_cond_signal(&buffer->not_empyt);
    pthread_mutex_unlock(&buffer->service);
    
    return msg;
}

msg_t* put_non_bloccante(buffer_t* buffer, msg_t* msg){
    pthread_mutex_lock(&(buffer->service));
    
    if(buffer->k == buffer->maxsize){
        pthread_mutex_unlock(&(buffer->service));
        return BUFFER_ERROR;
    }
    else{
        pthread_mutex_lock(&(buffer->uso_d));
        buffer->array[buffer->index_d] = msg_copy_string(msg);
        buffer->index_d = (buffer->index_d +1)%(buffer->maxsize);
        buffer->k++;
        pthread_mutex_unlock(&(buffer->uso_d));
        pthread_cond_signal(&(buffer->not_empyt));
        pthread_mutex_unlock(&(buffer->service));
        return msg;
    }
}

msg_t* get_non_bloccante(buffer_t* buffer){
    msg_t* message;
    pthread_mutex_lock(&(buffer->service));
    if(buffer->k == 0){
        pthread_mutex_unlock(&(buffer->service));
        return BUFFER_ERROR;
    }
    else{
        pthread_mutex_lock(&(buffer->uso_t));
        message = buffer->array[buffer->index_t];
        buffer->index_t = (buffer->index_t + 1)%(buffer->maxsize);
        buffer->k--;
        pthread_mutex_unlock(&(buffer->uso_t));
        pthread_cond_signal(&(buffer->not_full));
        pthread_mutex_unlock(&(buffer->service));
        return message;
    }
}

msg_t* get_bloccante(buffer_t* buffer){
    msg_t* message;
    pthread_mutex_lock(&(buffer->service));
    while(buffer->k == 0){
        pthread_cond_wait(&(buffer->not_empyt), &(buffer->service));
    }
    pthread_mutex_lock(&(buffer->uso_t));
    message = buffer->array[buffer->index_t];
    buffer->index_t = (buffer->index_t + 1)%(buffer->maxsize);
    buffer->k--;
    pthread_mutex_unlock(&(buffer->uso_t));
    pthread_cond_signal(&(buffer->not_full));
    pthread_mutex_unlock(&(buffer->service));
    return message;
}

