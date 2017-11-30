//
//  buffer_test.c
//  hwc1
//
//  Created by Federico Yusteenappar on 26/11/17.
//  Copyright © 2017 Federico Yusteenappar. All rights reserved.
//

#include <stdio.h>
#include "/usr/local/opt/cunit/include/CUnit/Basic.h"
#include <string.h>
#include <unistd.h>
#include "buffer.h"
static buffer_t* buffer_unitario_pieno = NULL;
static buffer_t* buffer_unitario_vuoto = NULL;
static buffer_t* buffer_non_unitario_pieno = NULL;
static buffer_t* buffer_non_unitario_vuoto = NULL;
static char messaggio[]= "EXPECTED_MSG";
static msg_t* messaggio1 = NULL;
static msg_t* messaggio2 = NULL;
static msg_t* messaggio3 = NULL;



/* --------------SETUP e CLEAN di un buffer unitario pieno --------------*/
int setup_buffer_unitario_pieno(void){
    buffer_unitario_pieno = buffer_init(1);
    
    if(buffer_unitario_pieno!= NULL){
        msg_t* msg = msg_init(messaggio);
        buffer_unitario_pieno->array[buffer_unitario_pieno->index_d] = msg;
        buffer_unitario_pieno->index_d = (buffer_unitario_pieno->index_d)%(buffer_unitario_pieno->maxsize);
        buffer_unitario_pieno->k = 1;
        return 0;
    }
    else
        return -1;
}

int clean_buffer_unitario_pieno(void){
    buffer_destroy(buffer_unitario_pieno);
    return 0;

}
/*-----------------------------------------------------------------------------*/

/*--------------SETUP e CLEAN di un buffer unitario vuoto --------------*/
int setup_buffer_unitario_vuoto(void){
    buffer_unitario_vuoto = buffer_init(1);
    if(buffer_unitario_vuoto!= NULL){
        return 0;
    }
    else return -1;
}
int clean_buffer_unitario_vuoto(void){
    buffer_destroy(buffer_unitario_vuoto);
    return 0;
}
void* thread_put_bloccante_buffer_unitario_vuoto(msg_t* messaggio_da_inserire){
    msg_t* messaggio = put_bloccante(buffer_unitario_vuoto, messaggio_da_inserire);
    return (void*)messaggio;
}
/*-----------------------------------------------------------------------------*/
void* thread_put_bloccante_buffer_non_unitario_vuoto(msg_t* messaggio_da_inserire){
    msg_t* messaggio = put_bloccante(buffer_non_unitario_vuoto, messaggio_da_inserire);
    return (void*)messaggio;
}
void* thread_put_non_bloccante_buffer_non_unitario_vuoto(msg_t* messaggio_da_inserire){
    msg_t* messaggio = put_non_bloccante(buffer_non_unitario_vuoto, messaggio_da_inserire);
    return (void*)messaggio;
}
void* thread_non_put_bloccante_buffer_non_unitario_vuoto(msg_t* messaggio_da_inserire){
    msg_t* messaggio = put_non_bloccante(buffer_non_unitario_vuoto, messaggio_da_inserire);
    return (void*)messaggio;
}
/*-------------------------------------------------------------------------------*/
/* (P=1; C=0; N=1) Produzione di un solo messaggio in un buffer vuoto  VERSIONE NON BLOCCANTE*/
void test_produzione_non_bloccante_buffer_unitario_vuoto(){
    clean_buffer_unitario_vuoto();
    setup_buffer_unitario_vuoto();
    msg_t* messaggio_da_inserire = msg_init("prima_produzione");
    msg_t* messaggio_ritornato = put_non_bloccante(buffer_unitario_vuoto, messaggio_da_inserire);
    CU_ASSERT_EQUAL(buffer_unitario_vuoto->k, buffer_unitario_vuoto->maxsize);
    CU_ASSERT_STRING_EQUAL(messaggio_ritornato->content,messaggio_da_inserire->content);
}

/* (P=1; C=0; N=1) Produzione di un solo messaggio in un buffer vuoto VERSIONE BLOCCANTE*/
void test_produzione_bloccante_buffer_unitario_vuoto(){
    msg_t* messaggio_da_inserire = msg_init("prova");
    msg_t* messaggio_ritornato = put_bloccante(buffer_unitario_vuoto, messaggio_da_inserire);
    CU_ASSERT_EQUAL(buffer_unitario_vuoto->k, buffer_unitario_vuoto->maxsize);
    CU_ASSERT_STRING_EQUAL(messaggio_da_inserire->content,messaggio_ritornato->content);

}

/* (P=0; C=1; N=1) Consumazione di un solo messaggio da un buffer pieno VERSIONE BLOCCANTE*/

void test_consumazione_bloccante_buffer_unitario_pieno(){
    msg_t* message = get_bloccante(buffer_unitario_pieno);
    CU_ASSERT(buffer_unitario_pieno->k == 0);
    CU_ASSERT_STRING_EQUAL(messaggio,message->content);
    
}
/* (P=0; C=1; N=1) Consumazione di un solo messaggio da un buffer pieno VERSIONE NON BLOCCANTE*/
void test_consumazione_non_bloccante_buffer_unitario_pieno(){
    clean_buffer_unitario_pieno();
    setup_buffer_unitario_pieno();
    msg_t* message = get_non_bloccante(buffer_unitario_pieno);
    CU_ASSERT(buffer_unitario_pieno->k == 0);
    CU_ASSERT_STRING_EQUAL(messaggio,message->content);
}


/* (P=1; C=0; N=1) Produzione in un buffer pieno  VERSIONE NON BLOCCANTE */
void test_produzione_non_bloccante_buffer_unitario_pieno() {
   
    setup_buffer_unitario_pieno();
    msg_t* messaggio_inserito = msg_init("salve");
    msg_t* messaggio_ritornato = put_non_bloccante(buffer_unitario_pieno, messaggio_inserito);
    CU_ASSERT_EQUAL(buffer_unitario_pieno->k, buffer_unitario_pieno->maxsize);
    CU_ASSERT_PTR_NOT_EQUAL(messaggio_ritornato,messaggio_inserito->content);
    CU_ASSERT_PTR_EQUAL(messaggio_ritornato, BUFFER_ERROR);
    msg_destroy(messaggio_inserito);
    msg_destroy(messaggio_ritornato);
    
}

/* (P=1; C=0; N=1) Produzione in un buffer pieno  VERSIONE BLOCCANTE */
void* thread_put_bloccante(msg_t* messaggio_da_inserire){
    msg_t* messaggio = put_bloccante(buffer_unitario_pieno, messaggio_da_inserire);
    return (void*)messaggio;
}
void test_produzione_bloccante_buffer_unitario_pieno(){
    msg_t* msg_da_inserire = msg_init("buonasera");
    pthread_t produttore;
    pthread_create(&produttore, NULL, (void*)&thread_put_bloccante, msg_da_inserire);
    sleep(5);
    CU_ASSERT_EQUAL(buffer_unitario_pieno->k, buffer_unitario_pieno->maxsize);
    CU_ASSERT_PTR_NOT_EQUAL(buffer_unitario_pieno->array[0]->content,msg_da_inserire->content);
    pthread_kill(produttore, 0);
    msg_destroy(msg_da_inserire);
}
/* (P=0; C=1; N=1) Consumazione di un solo messaggio da un buffer vuoto VERSIONE NON BLOCCANTE*/
void test_consumazione_non_bloccante_buffer_unitario_vuoto(){
    msg_t* messaggio_ricevuto = get_non_bloccante(buffer_unitario_vuoto);
    CU_ASSERT_EQUAL(buffer_unitario_vuoto->k, 0);
    CU_ASSERT_PTR_EQUAL(messaggio_ricevuto, BUFFER_ERROR);
}
    
/* (P=0; C=1; N=1) Consumazione di un solo messaggio da un buffer vuoto VERSIONE BLOCCANTE*/
void* thread_get_bloccante(buffer_t* buffer){
    msg_t* message = get_bloccante(buffer);
    return (void*)message;
}

void test_consumazione_bloccante_buffer_unitario_vuoto(){
    pthread_t consumatore,produttore;
    msg_t* GO_MSG = msg_init("messaggio");
    msg_t* messaggio_letto = NULL;
    pthread_create(&consumatore, NULL, (void*)&thread_get_bloccante, buffer_unitario_vuoto);
    sleep(5);
    pthread_create(&produttore, NULL, (void*)&thread_put_bloccante_buffer_unitario_vuoto, GO_MSG);
    pthread_join(consumatore, (void*)&messaggio_letto);
    CU_ASSERT_EQUAL(buffer_unitario_vuoto->k, 0);
    CU_ASSERT_STRING_EQUAL (GO_MSG->content,messaggio_letto->content);
    msg_destroy(GO_MSG);
    
}


/*(P=1; C=1; N=1) Consumazione e produzione concorrente di un messaggio da un buffer unitario; prima il consumatore  VERSIONE BLOCCANTE*/
static void setup_consumazione_produzione_buffer_unitario(msg_t **messaggio_da_inserire, msg_t **messaggio_letto_dal_consumatore, msg_t **messaggio_ritornato_al_produttore) {
    *messaggio_da_inserire = msg_init("produzione");
    *messaggio_letto_dal_consumatore = NULL;
    *messaggio_ritornato_al_produttore = NULL;
}

void test_consumazione_produzione_concorrente_bloccante_buffer_unitario_prima_consumatore(){
    msg_t * messaggio_da_inserire;
    msg_t * messaggio_letto_dal_consumatore;
    msg_t * messaggio_ritornato_al_produttore;
    setup_consumazione_produzione_buffer_unitario(&messaggio_da_inserire, &messaggio_letto_dal_consumatore, &messaggio_ritornato_al_produttore);
    pthread_t consumatore, produttore;
    pthread_create(&consumatore, NULL, (void*)&thread_get_bloccante, buffer_unitario_pieno);
    pthread_join(consumatore,(void*)&messaggio_letto_dal_consumatore);
    sleep(5);
    pthread_create(&produttore, NULL, (void*)&thread_put_bloccante, messaggio_da_inserire);
    pthread_join(produttore,(void*)&messaggio_ritornato_al_produttore);
    sleep(3);
    
    CU_ASSERT_EQUAL(buffer_unitario_pieno->k, 1);
    CU_ASSERT_STRING_EQUAL(messaggio_ritornato_al_produttore->content,buffer_unitario_pieno->array[0]->content);
    pthread_kill(consumatore, 0);
    pthread_kill(produttore, 0);

}
/*(P=1; C=1; N=1) Consumazione e produzione concorrente di un messaggio da un buffer unitario; prima il consumatore  VERSIONE NON BLOCCANTE*/

void* thread_get_non_bloccante(buffer_t* buffer){
    msg_t* message = get_non_bloccante(buffer);
    return (void*)message;
}
void* thread_put_non_bloccante(msg_t* messaggio_da_inserire){
    msg_t* messaggio = put_non_bloccante(buffer_unitario_pieno, messaggio_da_inserire);
    return (void*)messaggio;
}

void test_consumazione_produzione_concorrente_non_bloccante_buffer_unitario_prima_consumatore(){
    msg_t * messaggio_da_inserire;
    msg_t * messaggio_letto_dal_consumatore;
    msg_t * messaggio_ritornato_al_produttore;
    setup_consumazione_produzione_buffer_unitario(&messaggio_da_inserire, &messaggio_letto_dal_consumatore, &messaggio_ritornato_al_produttore);
    pthread_t consumatore, produttore;
    pthread_create(&consumatore, NULL, (void*)&thread_get_non_bloccante, buffer_unitario_pieno);
    pthread_join(consumatore,(void*)&messaggio_letto_dal_consumatore);
    sleep(5);
    pthread_create(&produttore, NULL, (void*)&thread_put_non_bloccante, messaggio_da_inserire);
    pthread_join(produttore,(void*)&messaggio_ritornato_al_produttore);
    sleep(3);
    
    CU_ASSERT_EQUAL(buffer_unitario_pieno->k, 1);
    CU_ASSERT_STRING_EQUAL(messaggio_ritornato_al_produttore->content,buffer_unitario_pieno->array[0]->content);
    pthread_kill(consumatore, 0);
    pthread_kill(produttore, 0);
    
}

/* (P>1; C=0; N=1) Produzione concorrente di molteplici messaggi in un buffer unitario vuoto VERSIONE BLOCCANTE */
void* thread_put_non_bloccante_buffer_unitario_vuoto(msg_t* messaggio_da_inserire){
    msg_t* messaggio = put_non_bloccante(buffer_unitario_vuoto, messaggio_da_inserire);
    return (void*)messaggio;
}


void test_produzione_concorrente_bloccante_molteplici_messaggi_buffer_vuoto(){
    pthread_t produttore1, produttore2, produttore3;
    msg_t * messaggio1 =  msg_init("produttore1");;
    msg_t * messaggio2 =  msg_init("produttore1");;
    msg_t * messaggio3 =  msg_init("produttore1");;
    msg_t * risposta1 = NULL;

    pthread_create(&produttore1, NULL, (void*)&thread_put_bloccante_buffer_unitario_vuoto, messaggio1);
    pthread_create(&produttore2, NULL, (void*)&thread_put_bloccante_buffer_unitario_vuoto, messaggio2);
    pthread_create(&produttore3, NULL, (void*)&thread_put_bloccante_buffer_unitario_vuoto, messaggio3);
    sleep(5);
    pthread_join(produttore1, (void*)&risposta1);
    CU_ASSERT_EQUAL(buffer_unitario_vuoto->k, 1);
    CU_ASSERT_STRING_EQUAL(risposta1->content,buffer_unitario_vuoto->array[0]->content);
    msg_destroy(messaggio1);
    msg_destroy(messaggio2);
    msg_destroy(messaggio3);
    pthread_kill(produttore2, 0);
    pthread_kill(produttore3, 0);
    
}
/* (P>1; C=0; N=1) Produzione concorrente di molteplici messaggi in un buffer unitario vuoto VERSIONE  NON BLOCCANTE */
void test_produzione_concorrente_non_bloccante_molteplici_messaggi_buffer_vuoto(){
    clean_buffer_unitario_vuoto();
    setup_buffer_unitario_vuoto();
    pthread_t produttore1, produttore2, produttore3;
    msg_t * messaggio1 =  msg_init("produttore1");;
    msg_t * messaggio2 =  msg_init("produttore2");;
    msg_t * messaggio3 =  msg_init("produttore3");;
    msg_t * risposta1;
    msg_t * risposta2;
    msg_t * risposta3;

    pthread_create(&produttore1, NULL, (void*)&thread_put_non_bloccante_buffer_unitario_vuoto, messaggio1);
    pthread_create(&produttore2, NULL, (void*)&thread_put_non_bloccante_buffer_unitario_vuoto, messaggio2);
    pthread_create(&produttore3, NULL, (void*)&thread_put_non_bloccante_buffer_unitario_vuoto, messaggio3);
    sleep(5);
    pthread_join(produttore1, (void*)&risposta1);
    pthread_join(produttore2, (void*)&risposta2);
    pthread_join(produttore3, (void*)&risposta3);
    
    CU_ASSERT_EQUAL(buffer_unitario_vuoto->k, 1);
    CU_ASSERT_STRING_EQUAL(risposta1->content,buffer_unitario_vuoto->array[0]->content);
    CU_ASSERT_PTR_EQUAL(risposta2, BUFFER_ERROR);
    CU_ASSERT_PTR_EQUAL(risposta3, BUFFER_ERROR);
    
    msg_destroy(messaggio1);
    msg_destroy(messaggio2);
    msg_destroy(messaggio3);
    
}

int setup_buffer_non_unitario_pieno(){
    buffer_non_unitario_pieno = buffer_init(3);
    if(buffer_non_unitario_pieno!= NULL){
        messaggio1 = msg_init("messaggio1");
        messaggio2 = msg_init("messaggio2");
        messaggio3 = msg_init("messaggio3");
        put_bloccante(buffer_non_unitario_pieno, messaggio1);
        put_bloccante(buffer_non_unitario_pieno, messaggio2);
        put_bloccante(buffer_non_unitario_pieno, messaggio3);
        return 0;
    }else return -1;
    
}
int clean_buffer_non_unitario_pieno(){
    buffer_destroy(buffer_non_unitario_pieno);
    msg_destroy(messaggio1);
    msg_destroy(messaggio2);
    msg_destroy(messaggio3);
    return 0;
}
/*(P>1; C=0; N>1) Produzione concorrente di molteplici messaggi in un buffer vuoto; il buffer si satura in corso VERSIONE BLOCCANTE */
void test_produzione_concorrente_bloccante_molteplici_messaggi_buffer_non_unitario_vuoto(){
    pthread_t produttore1,produttore2, produttore3;
    messaggio1 = msg_init("messaggio1");
    messaggio2 = msg_init("messaggio2");
    messaggio3 = msg_init("messaggio3");
    pthread_create(&produttore1, NULL, (void*)&thread_put_bloccante_buffer_non_unitario_vuoto,messaggio1);
    pthread_create(&produttore2, NULL, (void*)&thread_put_bloccante_buffer_non_unitario_vuoto,messaggio2);
    pthread_create(&produttore3, NULL, (void*)&thread_put_bloccante_buffer_non_unitario_vuoto,messaggio3);
    pthread_join(produttore1, NULL);
    pthread_join(produttore2, NULL);
    pthread_join(produttore3, NULL);
    CU_ASSERT_EQUAL(buffer_non_unitario_vuoto->k, 3);
    
}

/*(P>1; C=0; N>1) Produzione concorrente di molteplici messaggi in un buffer vuoto; il buffer si satura in corso VERSIONE NON BLOCCANTE */
void test_produzione_concorrente_non_bloccante_molteplici_messaggi_buffer_non_unitario_vuoto(){
    pthread_t produttore1,produttore2, produttore3;
    messaggio1 = msg_init("messaggio1");
    messaggio2 = msg_init("messaggio2");
    messaggio3 = msg_init("messaggio3");
    pthread_create(&produttore1, NULL, (void*)&thread_put_non_bloccante_buffer_non_unitario_vuoto,messaggio1);
    pthread_create(&produttore2, NULL, (void*)&thread_put_non_bloccante_buffer_non_unitario_vuoto,messaggio2);
    pthread_create(&produttore3, NULL, (void*)&thread_put_non_bloccante_buffer_non_unitario_vuoto,messaggio3);
    pthread_join(produttore1, NULL);
    pthread_join(produttore2, NULL);
    pthread_join(produttore3, NULL);
    CU_ASSERT_EQUAL(buffer_non_unitario_vuoto->k, 3);
}
/*(P=0; C>1; N=1) Consumazione concorrente di molteplici messaggi da un buffer unitario pieno VERSIONE BLOCCANTE*/
void test_consumazione_concorrente_bloccante_molteplici_messaggi_buffer_unitario_pieno(){
    pthread_t consumatore1,consumatore2,consumatore3;
    msg_t* risposta1;
    pthread_create(&consumatore1, NULL, (void*)&thread_get_bloccante, buffer_unitario_pieno);
    pthread_create(&consumatore2, NULL, (void*)&thread_get_bloccante, buffer_unitario_pieno);
    pthread_create(&consumatore3, NULL, (void*)&thread_get_bloccante, buffer_unitario_pieno);
    pthread_join(consumatore1, (void*)&risposta1);
    CU_ASSERT_EQUAL(buffer_unitario_pieno->k, 0);
    pthread_kill(consumatore2, 0);
    pthread_kill(consumatore3, 0);
}

/*(P=0; C>1; N=1) Consumazione concorrente di molteplici messaggi da un buffer unitario pieno VERSIONE NON BLOCCANTE*/
void test_consumazione_concorrente_non_bloccante_molteplici_messaggi_buffer_unitario_pieno(){
    clean_buffer_unitario_pieno();
    setup_buffer_unitario_pieno();
    pthread_t consumatore1,consumatore2,consumatore3;
    msg_t* risposta1;
    msg_t* risposta2;
    msg_t* risposta3;
    pthread_create(&consumatore1, NULL, (void*)&thread_get_non_bloccante, buffer_unitario_pieno);
    pthread_create(&consumatore2, NULL, (void*)&thread_get_non_bloccante, buffer_unitario_pieno);
    pthread_create(&consumatore3, NULL, (void*)&thread_get_non_bloccante, buffer_unitario_pieno);
    pthread_join(consumatore1, (void*)&risposta1);
    pthread_join(consumatore2, (void*)&risposta2);
    pthread_join(consumatore3, (void*)&risposta3);
    
    CU_ASSERT_EQUAL(buffer_unitario_pieno->k, 0);
    CU_ASSERT_PTR_EQUAL(risposta2, BUFFER_ERROR);
    CU_ASSERT_PTR_EQUAL(risposta3, BUFFER_ERROR);
}

/* (P=0; C>1; N>1) Consumazione concorrente di molteplici messaggi da un buffer pieno  VERSIONE BLOCCANTE*/
void test_consumazione_concorrente_bloccante_molteplici_messaggi_buffer_non_unitario_pieno(){
    pthread_t consumatore1,consumatore2,consumatore3;
    pthread_create(&consumatore1, NULL, (void*)&thread_get_bloccante, buffer_non_unitario_pieno);
    pthread_create(&consumatore2, NULL, (void*)&thread_get_bloccante, buffer_non_unitario_pieno);
    pthread_create(&consumatore3, NULL, (void*)&thread_get_bloccante, buffer_non_unitario_pieno);
    
    pthread_join(consumatore1, NULL);
    pthread_join(consumatore2, NULL);
    pthread_join(consumatore3, NULL);
    
    CU_ASSERT_EQUAL(buffer_non_unitario_pieno->k, 0);
}
/*(P=0; C>1; N>1) Consumazione concorrente di molteplici messaggi da un buffer pieno VERSIONE NON BLOCCANTE */
void test_consumazione_concorrente_non_bloccante_molteplici_messaggi_buffer_non_unitario_pieno(){
    clean_buffer_non_unitario_pieno();
    setup_buffer_non_unitario_pieno();

    pthread_t consumatore1,consumatore2,consumatore3,consumatore4;
    msg_t* risposta_consumatore4 = NULL;
    
    pthread_create(&consumatore1, NULL, (void*)&thread_get_non_bloccante, buffer_non_unitario_pieno);
    pthread_create(&consumatore2, NULL, (void*)&thread_get_non_bloccante, buffer_non_unitario_pieno);
    pthread_create(&consumatore3, NULL, (void*)&thread_get_non_bloccante, buffer_non_unitario_pieno);
    pthread_create(&consumatore4, NULL, (void*)&thread_get_non_bloccante, buffer_non_unitario_pieno);
    
    pthread_join(consumatore1, NULL);
    pthread_join(consumatore2, NULL);
    pthread_join(consumatore3, NULL);
    pthread_join(consumatore4, (void*)&risposta_consumatore4);
    
    CU_ASSERT_EQUAL(buffer_non_unitario_pieno->k, 0);
    CU_ASSERT_PTR_EQUAL(risposta_consumatore4, BUFFER_ERROR);
}
int setup_buffer_non_unitario_vuoto(){
    buffer_non_unitario_vuoto = buffer_init(3);
    if (buffer_non_unitario_vuoto != NULL){
        return 0;
    }else return -1;
}
int clean_buffer_non_unitario_vuoto(){
    buffer_destroy(buffer_non_unitario_vuoto);
    return 0;
}

/*(P>1; C>1; N>1) Consumazioni e produzioni concorrenti di molteplici messaggi in un buffer VERSIONE BLOCCANTE*/
void test_produzione_consumazione_concorrente_bloccante_molteplici_messaggi_buffer_non_unitario_vuoto(){
    pthread_t produttore1,produttore2,consumatore1,consumatore2;
    msg_t* messaggio1 = msg_init("produttore1");
    msg_t* messaggio2 = msg_init("produttore2");
    msg_t* risposta1;
    msg_t* risposta2;
    pthread_create(&produttore1, NULL, (void*)&thread_put_bloccante_buffer_non_unitario_vuoto, messaggio1);
    pthread_create(&produttore2, NULL, (void*)&thread_put_bloccante_buffer_non_unitario_vuoto, messaggio2);
    pthread_create(&consumatore1, NULL, (void*)&thread_get_bloccante, buffer_non_unitario_vuoto);
    pthread_create(&consumatore2, NULL, (void*)&thread_get_bloccante, buffer_non_unitario_vuoto);
    pthread_join(produttore1,NULL);
    pthread_join(produttore2,NULL);
    pthread_join(consumatore1, (void*)&risposta1);
    pthread_join(consumatore2, (void*)&risposta2);
    
    CU_ASSERT_STRING_EQUAL(risposta1->content,messaggio1->content);
    CU_ASSERT_STRING_EQUAL(risposta2->content,messaggio2->content);
    CU_ASSERT_EQUAL(buffer_non_unitario_vuoto->k, 0);
    msg_destroy(messaggio1);
    msg_destroy(messaggio2);
    
}
/*(P>1; C>1; N>1) Consumazioni e produzioni concorrenti di molteplici messaggi in un buffer VERSIONE NON BLOCCANTE*/
void test_produzione_consumazione_concorrente_non_bloccante_molteplici_messaggi_buffer_non_unitario_vuoto(){
    pthread_t produttore1,produttore2,consumatore1,consumatore2;
    msg_t* messaggio1 = msg_init("produttore1");
    msg_t* messaggio2 = msg_init("produttore2");
    msg_t* risposta1;
    msg_t* risposta2;
    pthread_create(&produttore1, NULL, (void*)&thread_put_non_bloccante_buffer_non_unitario_vuoto, messaggio1);
    pthread_create(&consumatore1, NULL, (void*)&thread_get_non_bloccante, buffer_non_unitario_vuoto);
    pthread_create(&consumatore2, NULL, (void*)&thread_get_non_bloccante, buffer_non_unitario_vuoto);
    pthread_create(&produttore2, NULL, (void*)&thread_put_non_bloccante_buffer_non_unitario_vuoto, messaggio2);
    pthread_join(produttore1,NULL);
    pthread_join(produttore2,NULL);
    pthread_join(consumatore1, (void*)&risposta1);
    pthread_join(consumatore2, (void*)&risposta2);
    
    CU_ASSERT_STRING_EQUAL(messaggio1->content, risposta1->content);
    CU_ASSERT_PTR_EQUAL(risposta2, BUFFER_ERROR);
    CU_ASSERT_EQUAL(buffer_non_unitario_vuoto->k, 1);
    msg_destroy(messaggio1);
    msg_destroy(messaggio2);

}
int main(){
    
    CU_pSuite pSuite = NULL;
    
    /* initialize the CUnit test registry */
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();
    
    /* ---------- SCENARIO Produzione in un buffer unitario pieno ----------*/
    pSuite = CU_add_suite("Produzione in un buffer unitario pieno", setup_buffer_unitario_pieno,clean_buffer_unitario_pieno);
    if(NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    
    /* add the tests to the suite */
    if ((NULL == CU_add_test(pSuite,"test put_bloccante buffer unitario pieno",test_produzione_bloccante_buffer_unitario_pieno))||(NULL == CU_add_test(pSuite,"test put_non_bloccante buffer unitario pieno",test_produzione_non_bloccante_buffer_unitario_pieno))) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    /*------------------------------------------------------------------------------*/
    
    /* ---------- SCENARIO Produzione in un buffer unitario vuoto ----------*/
    pSuite = CU_add_suite("Produzione in un buffer unitario vuoto", setup_buffer_unitario_vuoto,clean_buffer_unitario_vuoto);
    if(NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    
    /* add the tests to the suite */
    if ((NULL == CU_add_test(pSuite,"test put_bloccante buffer unitario vuoto",test_produzione_bloccante_buffer_unitario_vuoto))||(NULL == CU_add_test(pSuite,"test put_non_bloccante buffer unitario vuoto",test_produzione_non_bloccante_buffer_unitario_vuoto))) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    /*------------------------------------------------------------------------------*/
    
    /* ---------- SCENARIO Consumazione da un buffer unitario pieno ----------*/
    pSuite = CU_add_suite("Consumazione da un buffer unitario pieno", setup_buffer_unitario_pieno,clean_buffer_unitario_pieno);
    if(NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    
    /* add the tests to the suite */
    if ((NULL == CU_add_test(pSuite,"test get_bloccante buffer unitario pieno",test_consumazione_bloccante_buffer_unitario_pieno))||(NULL == CU_add_test(pSuite,"test get_non_bloccante buffer unitario pieno",test_consumazione_non_bloccante_buffer_unitario_pieno))) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    /*------------------------------------------------------------------------------*/
    
    /* ---------- SCENARIO Consumazione da un buffer unitario vuoto ----------*/
    pSuite = CU_add_suite("Consumazione da un buffer unitario vuoto", setup_buffer_unitario_vuoto,clean_buffer_unitario_vuoto);
    if(NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    
    /* add the tests to the suite */
    if ((NULL == CU_add_test(pSuite,"test get_non_bloccante buffer unitario vuoto",test_consumazione_non_bloccante_buffer_unitario_vuoto))||(NULL == CU_add_test(pSuite,"test get_bloccante buffer unitario vuoto",test_consumazione_bloccante_buffer_unitario_vuoto))) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    /*------------------------------------------------------------------------------*/
    
    /* ---------- SCENARIO Consumazione-produzione da un buffer unitario pieno, prima il consumatore ----------*/
    pSuite = CU_add_suite("Consumazione e produzione da/su un buffer unitario pieno, prima il consumatore", setup_buffer_unitario_pieno,clean_buffer_unitario_pieno);
    if(NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    
    /* add the tests to the suite */
    if ((NULL == CU_add_test(pSuite,"test get_bloccante/put_bloccante consumazione e produzione da/su un buffer unitario pieno",test_consumazione_produzione_concorrente_bloccante_buffer_unitario_prima_consumatore))||(NULL == CU_add_test(pSuite,"test get_non_bloccante/put_non_bloccante consumazione e produzione da/su un buffer unitario pieno",test_consumazione_produzione_concorrente_non_bloccante_buffer_unitario_prima_consumatore))) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    /*------------------------------------------------------------------------------*/
    /* ---------- SCENARIO Produzioni concorrenti su un buffer unitario vuoto ----------*/
    pSuite = CU_add_suite("Produzioni concorrenti su un buffer unitario vuoto", setup_buffer_unitario_vuoto,clean_buffer_unitario_vuoto);
    if(NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    
    /* add the tests to the suite */
    if ((NULL == CU_add_test(pSuite,"test put_bloccante da produttori concorrenti su buffer unitario vuoto",test_produzione_concorrente_bloccante_molteplici_messaggi_buffer_vuoto))||(NULL == CU_add_test(pSuite,"test put_non_bloccante da produttori concorrenti buffer unitario vuoto",test_produzione_concorrente_non_bloccante_molteplici_messaggi_buffer_vuoto))) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    
    /*------------------------------------------------------------------------------*/
    /* ---------- SCENARIO Consumazioni concorrenti su un buffer unitario pieno ----------*/
    pSuite = CU_add_suite("Consumazioni concorrenti su un buffer unitario pieno", setup_buffer_unitario_pieno,clean_buffer_unitario_pieno);
    if(NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    /* add the tests to the suite */
    if ((NULL == CU_add_test(pSuite,"test get_bloccante da consumatori concorrenti su buffer unitario pieno",test_consumazione_concorrente_bloccante_molteplici_messaggi_buffer_unitario_pieno))||(NULL == CU_add_test(pSuite,"test get_non_bloccante da consumatori concorrenti su buffer unitario pieno",test_consumazione_concorrente_non_bloccante_molteplici_messaggi_buffer_unitario_pieno))) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    /*------------------------------------------------------------------------------*/
    
    /* ---------- SCENARIO Consumazioni concorrenti su un buffer non unitario pieno ----------*/
    pSuite = CU_add_suite("Consumazioni concorrenti su un buffer non unitario pieno", setup_buffer_non_unitario_pieno,clean_buffer_non_unitario_pieno);
    if(NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    /* add the tests to the suite */
    if ((NULL == CU_add_test(pSuite,"test get_bloccante da consumatori concorrenti su buffer non unitario pieno",test_consumazione_concorrente_bloccante_molteplici_messaggi_buffer_non_unitario_pieno))||(NULL == CU_add_test(pSuite,"test get_non_bloccante da consumatori concorrenti su buffer non unitario pieno",test_consumazione_concorrente_non_bloccante_molteplici_messaggi_buffer_non_unitario_pieno))) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    /*------------------------------------------------------------------------------*/
    
    /* ---------- SCENARIO Consumazioni e produzioni concorrenti su un buffer non unitario vuoto ----------*/
    pSuite = CU_add_suite("Consumazioni e produzioni concorrenti su un buffer non unitario vuoto", setup_buffer_non_unitario_vuoto,clean_buffer_non_unitario_vuoto);
    if(NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    /* add the tests to the suite */
    if ((NULL == CU_add_test(pSuite,"test put_bloccante/get_bloccante da consumatori e produttori concorrenti su buffer non unitario vuoto",test_produzione_consumazione_concorrente_bloccante_molteplici_messaggi_buffer_non_unitario_vuoto))||(NULL == CU_add_test(pSuite,"test put_non_bloccante/get_non_bloccante da consumatori e produttoriconcorrenti su buffer non unitario vuoto",test_produzione_consumazione_concorrente_non_bloccante_molteplici_messaggi_buffer_non_unitario_vuoto))) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    /*------------------------------------------------------------------------------*/
    
    /* ---------- SCENARIO Produzioni concorrenti su un buffer non unitario vuoto; il buffer si satura in corso ----------*/
    pSuite = CU_add_suite("produzioni concorrenti su un buffer non unitario vuoto; il buffer si satura in corso", setup_buffer_non_unitario_vuoto,clean_buffer_non_unitario_vuoto);
    if(NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    /* add the tests to the suite */
    if ((NULL == CU_add_test(pSuite,"test put_bloccante da produttori concorrenti su buffer non unitario vuoto; il buffer si satura in corso",test_produzione_concorrente_bloccante_molteplici_messaggi_buffer_non_unitario_vuoto))||(NULL == CU_add_test(pSuite,"test put_non_bloccante da produttori concorrenti su buffer non unitario vuoto; il buffer si satura in corso",test_produzione_concorrente_non_bloccante_molteplici_messaggi_buffer_non_unitario_vuoto))) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    /*------------------------------------------------------------------------------*/
    
    /* Run all tests using the CUnit Basic interface */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}

