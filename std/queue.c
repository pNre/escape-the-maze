//
//  queue.c
//  Reloaded
//
//  Created by Pierluigi D'Andrea on 16/07/13.
//  Copyright (c) 2013 Pierluigi D'Andrea. All rights reserved.
//

#include <stdio.h>

#include "std/queue.h"

//  Inserimento in coda senza la copia del valore
void queue_push(queue * queue, void * element) {
    
    list_insert(queue, element, INSERT_MODE_TAIL, 0);

}

//  Estrazione dalla testa
void * queue_pop(queue * queue) {
    
    list_node * head = queue->head;

    if (!head)
        return NULL;
    
    //  valore contenuto nel nodo
    void * value = head->value;
    
    //  si elimina il nodo
    list_remove_node(queue, head);
    
    return value;
    
}