#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "enqstr.h"

/*
* TODO - rever o 'esclose' em todos os casos
* para (char*), (ENQSTR*), (struct _EnqStr *)
*/

/*
* Expressao usada frequentemente ao longo do codigo
* So existe um elemento
*/
#define is_single(x) ((*(x))->next == NULL && (*(x))->prev == NULL)

static void write(ENQSTR *enqstrings, const char *str);
static void create(ENQSTR *enqstrings);
static void strlinker(ENQSTR *enqstrings, const char *str);
static void insert_one_not_read(ENQSTR *enqstrings, const char *str);
static void insert_all_read(ENQSTR *enqstrings, const char *str);
static void replace_ptr( ENQSTR * __ptr1, ENQSTR * __ptr2 );
static void replace_strs( char ** str1, char ** str2 );
static void rewind_one( ENQSTR * enqstring );
static void rewind_loop( ENQSTR * enqstring );

/* Func que pode ser usada de maneira recursiva */
static void write(ENQSTR *enqstrings, const char *str){
    if(*enqstrings == NULL){
        create(enqstrings);
        strlinker(enqstrings, str);
    }
    else{ /* Procure o ultimo elemento */
        /* Diferentemente do outro insert so existe um caso em que 'next' == 'next->next',
           entao pode ultilizalo dentro da funcao recursiva */
        if( (*enqstrings)->next != NULL && (*enqstrings)->next == (*enqstrings)->next->next ){
            /* caso do ultimo elemento nao lido, insira um novo bloco e faca parecer que
               sempre esteve ali */
            insert_one_not_read(enqstrings, str);
            return;
        }
        write( &(*enqstrings)->next, str );

        /* Faz o elo entre o proximo elemtento e este */
        if((*enqstrings)->next->prev == NULL)
            (*enqstrings)->next->prev = *enqstrings;
    }
}

static void create(ENQSTR *enqstrings){
    /* Em momento nenhum e' declarado um objeto 'struct _EnqStr',
       tudo que faco e' alocar a memoria nescessaria (28 bytes) */
    *enqstrings = (struct _EnqStr *) malloc( sizeof(struct _EnqStr) );

    /* Por segurança */
    (*enqstrings)->str  = NULL;
    (*enqstrings)->next = NULL;
    (*enqstrings)->prev = NULL;
}

static void strlinker(ENQSTR *enqstrings, const char *str){
    char *aux;

    /*
    * Aloca memoria necessaria
    * sizeof(char) so para garantir, sem esquecer o '\0'
    */
    aux = (char *) malloc( (strlen(str) + 1) * sizeof(char) );
    strcpy(aux, str); /* TODO (strcpy) is considered unsafe, substituir por strcpy_s */
    (*enqstrings)->str = aux;
}

static void insert_one_not_read(ENQSTR *enqstrings, const char *str){
    ENQSTR *newslot;

    newslot = esopen();
    create(newslot);

    (*newslot)->prev = (*enqstrings)->next;
    strlinker(newslot, str);

    (*enqstrings)->next = *newslot;
}

static void insert_all_read(ENQSTR *enqstrings, const char *str){
    ENQSTR *newslot;

    newslot = esopen();
    create(newslot);

    (*newslot)->next = *newslot; /* o ultimo elemento ja foi lido */
    (*newslot)->prev = (*enqstrings)->next;
    (*newslot)->str  = (*enqstrings)->str;

    (*enqstrings)->next = *newslot;
    (*enqstrings)->prev = NULL; /* este nao foi lido ainda */
    strlinker(enqstrings, str);
}

static void replace_ptr( ENQSTR * __ptr1, ENQSTR * __ptr2 ){
    ENQSTR tmp;

    tmp = *__ptr1;
    *__ptr1 = *__ptr2;
    *__ptr2 = tmp;
}

static void replace_strs( char ** str1, char ** str2 ){
    char *tmp = NULL;

    tmp = *str1;
    *str1 = *str2;
    *str2 = tmp;
}

static void rewind_one( ENQSTR * enqstring )
{
    if((*enqstring)->prev != NULL) /* (*enqstring)->next tem q ser Null */
        replace_ptr(& (*enqstring)->prev, & (*enqstring)->next);
    else{
        if((*enqstring)->next != NULL){
            if((*enqstring)->next == (*enqstring)->next->next){ /* um elemento nao lido */
                /* a ordem importa */
                replace_strs(& (*enqstring)->str, & (*enqstring)->next->str);
                (*enqstring)->next->next = NULL;
            }
            else
                if(*enqstring != (*enqstring)->next->prev){ /* evita o loop infinito */
                    /* a ordem importa */
                    replace_ptr(& (*enqstring)->next, & (*enqstring)->next->prev->next); /* LOL */
                    replace_strs(& (*enqstring)->str, & (*enqstring)->next->str);
                }
        }
    }
}

static void rewind_loop( ENQSTR * enqstring ) /* FIXME */
{
    /** TODO teve ter um jeito mais facil de dizer q o loop acabou **/
    while( (*enqstring)->prev != NULL || (*enqstring)->next->prev != *enqstring )
        rewind_one(enqstring);
}

/* TODO mudar pra esinit? */
ENQSTR *esopen( void ){
    ENQSTR *mem;
    mem = (ENQSTR *) malloc(sizeof(ENQSTR));
    return mem;
}

/* Funcao nao recursiva */
void eswrite(ENQSTR *enqstrings, const char *str){
    /*
    * Se o elemento ainda existe (elemento ja aberto pois 'enqtrings != NULL') e
    * Se todos os elementos ja foram lidos
    */
    if( *enqstrings != NULL && (*enqstrings)->prev != NULL )
        insert_all_read(enqstrings, str);
    else
        write(enqstrings, str);
}

char * esread( ENQSTR * enqstrings ){
    char *aux = NULL;

    if( *enqstrings == NULL )
        return NULL;

    /* Verifica se ainda há elentos para serem lidos
       isso é dado pelo 'prev' */
    if((*enqstrings)->prev == NULL)
        aux = (*enqstrings)->str;
    else
        return aux; /* NULL */

    /* Se so existe um elemento */
    if( is_single(enqstrings) ){
        (*enqstrings)->prev = *enqstrings;
    } else {
        if( (*enqstrings)->next != NULL && (*enqstrings)->next != (*enqstrings)->next->next ){
            replace_strs( &(*enqstrings)->str, &(*enqstrings)->next->str );

            if( (*enqstrings)->next->next != NULL )
                replace_ptr( &(*enqstrings)->next, &(*enqstrings)->next->next );
            else
                (*enqstrings)->next->next = (*enqstrings)->next;
        } else {
            replace_ptr( &(*enqstrings)->next, &(*enqstrings)->prev ); /* Feche o sistema */
        }
    }

    return aux;
}

void esrewind( ENQSTR *enqstrings ){
    if( *enqstrings == NULL && is_single(enqstrings) )
        return;
    else{
        if( (*enqstrings)->prev == *enqstrings ){
            (*enqstrings)->prev = NULL;
            return;
        }
        else
            rewind_loop(enqstrings);
    }
}

/* O ponteiro continua apontando para o mesmo local
   mas nao tem o controle sobre ele */
void esclose(ENQSTR *enqstring){
    esrewind(enqstring);
    /* sempre liberar a string primeiro */
    if( !is_single(enqstring) ){ /* Se nao e single concerteza tem next */
        struct _EnqStr *tmp; /* guarda o ponteiro antes de perder o acesso */

        while( (*enqstring)->next != NULL && (*enqstring)->next->prev != *enqstring ){
            tmp = (*enqstring)->next->next;

            free((*enqstring)->str);
            free((*enqstring)->next);

            (*enqstring)->next = tmp;
        }
    }

    free((*enqstring)->str);
    free(*enqstring);
}
