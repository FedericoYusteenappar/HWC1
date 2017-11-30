//
//  buffer.h
//  hwc1
//
//  Created by Federico Yusteenappar on 25/11/17.
//  Copyright © 2017 Federico Yusteenappar. All rights reserved.
//

#ifndef buffer_h
#define buffer_h
#define BUFFER_ERROR (msg_t*) NULL
#include <stdio.h>
#include "msg.h"
#include <pthread.h>


typedef struct buffer {
    int index_t; // indice per le estrazioni
    int index_d; // indice per gli inserimenti
    msg_t* *array; // array di messaggi di tipo msg_t di dimensione non nota
    int k; // numero di messaggi presenti nel buffer
    int maxsize; // dimensione non nota a priori del buffer
    pthread_cond_t not_empyt; //variabile condizione per indicare il buffer non vuoto
    pthread_cond_t not_full; //variabile condizione per indicare il buffer non pieno
    pthread_mutex_t uso_t; //semaforo per la gestione di index_t
    pthread_mutex_t uso_d; //semaforo per la gesione di index_d
    pthread_mutex_t service;// semaforo di servizio
} buffer_t;

/* allocazione / deallocazione buffer */
// creazione di un buffer vuoto di dim. max nota
buffer_t* buffer_init(unsigned int maxsize);

// deallocazione di un buffer
void buffer_destroy(buffer_t*);

/* operazioni sul buffer */
// inserimento bloccante: sospende se pieno, quindi effettua l’inserimento non appena si libera dello spazio e restituisce il messaggio inserito; N.B.: msg!=null
msg_t* put_bloccante(buffer_t*, msg_t*);

// inserimento non bloccante: restituisce BUFFER_ERROR se pieno, altrimenti effettua l’inserimento e restituisce il messaggio inserito; N.B.: msg!=null
msg_t* put_non_bloccante(buffer_t*, msg_t*);

// estrazione bloccante: sospende se vuoto, quindi restituisce il valore estratto non appena disponibile
msg_t* get_bloccante(buffer_t*);

// estrazione non bloccante: restituisce BUFFER_ERROR se vuoto ed il valore estratto in caso contrario
msg_t* get_non_bloccante(buffer_t*);

#endif /* buffer_h */
