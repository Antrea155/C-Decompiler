#include "ll.h"



/**
 * Allocates or initializes a new list
 * @param list {List *} (optional) - A pointer to a list or NULL, if not supplied a new list is allocated.
 * @return {List *} A pointer to the supplied list or the newly allocated list. On error it returns NULL
 */
List * List_new( List * list ) {
    if( list == NULL ) {
        xmalloc( list, sizeof(List), List *, NULL );
        list->_reserved.allocated = true;
    }
    else {
        list->_reserved.allocated = false;
    }

    List_destroy(list);
    dlprintf(VERB_DEBUG, "Initialized List at %p", list);
    return list;
}

void List_free( List * list ) {
    if( list->_reserved.allocated == false ) {
        error_print("Attempting to free a non-allocated list");
        return;
    }
    free(list);
}

void List_destroy( List * list ) {
    if( list == NULL ) {
        error_print("NULL value given");
        return;
    }

    list->current = NULL;
    list->head = NULL;
    list->tail = NULL;
    list->numItems = 0;
}


bool _List_pushElement( List * list, ListElement * listElement ) {
    if( list == NULL ) return_error_print( false, "Null list given" );
    if( listElement == NULL ) return_error_print( false, "Null listElement given" );

    if( list->head == NULL ) {
        list->head = listElement;
        listElement->next = NULL;
        listElement->prev = NULL;
    }
    else {
 
        listElement->next = list->head;
        listElement->prev = NULL;
        list->head->prev = listElement;
        list->head = listElement;
    }
    
    if( list->tail == NULL ) {
            list->tail = list->head;
        }
    
    list->numItems++;
    return true;
}

bool _List_pushElement_back( List * list, ListElement * listElement ) {
    if( list == NULL ) return_error_print( false, "Null list given" );
    if( listElement == NULL ) return_error_print( false, "Null listElement given" );

    if( list->tail == NULL ) {
        list->tail = listElement;
        listElement->next = NULL;
        listElement->prev = NULL;
    }
    else {
 
        listElement->prev = list->tail;
        listElement->next = NULL;
        list->tail->next = listElement;
        list->tail = listElement;
    }
    
    if( list->head == NULL ) {
            list->head = list->tail;
    }
    
    list->numItems++;
    return true;
}


ListElement * List_popElement( List * list ) {
    ListElement * ret;

    if( list == NULL ) return_error_print( NULL, "Null list given" );
    if( list->head == NULL ) return NULL;

    ret = list->head;

    if( ret->next != NULL ) {
        ret->next->prev = NULL;
    }
    list->head = ret->next;
    ret->next = NULL;
    list->numItems--;
    return ret;
}

ListElement * List_getHead( List * list ) {
    return list->head;
}

ListElement * List_getTail( List * list ) {
    return list->tail;
}

ListElement * List_getNextElement( List * list ) {
    if( list == NULL ) return_error_print( NULL, "Null list given" );
    if( list->head == NULL ) return NULL;

    if( list->current != NULL ) {
        list->current = list->current->next;
    }
    else {
        list->current = list->head;
    }

    return list->current;
}


ListElement * List_getPrevElement( List * list ) {
    if( list == NULL ) return_error_print( NULL, "Null list given" );

    if( list->current != NULL ) {
        list->current = list->current->prev;
    }
    else if( list->tail != NULL ) {
        list->current = list->tail;
    }
    else if( list->head != NULL ) {
        list->current = list->head;
    }
    else {
        return NULL;
    }
    return list->current;
}

/**
 * Resets the list back to the first item.
 * @param list {List *} - Pointer to list
 */
void List_reset( List * list ) {
    if( list == NULL ) { error_print( "Null list given" ); return; }
    list->current = NULL;
}


bool _List_remove( List * list, ListElement * listElement ) {
    if( list == NULL ) return_error_print( false, "Null list given" );
    if( listElement == NULL ) return_error_print( false, "Null listElement given");
    
    if (list->head == listElement) {
        list->head = listElement->next;
    }
    
    if (list->tail == listElement) {
        list->tail = listElement->prev ;
    }

    if( listElement->prev ) {
        listElement->prev->next = listElement->next;
    }
    if( listElement->next ) {
        listElement->next->prev = listElement->prev;
    }

    listElement->next = NULL;
    listElement->prev = NULL;
    list->numItems--;
    return true;
}


/**
 * returns true if list is empty.
 * @param list {List *} - Pointer to list
 */
bool List_is_empty( List * list ) {
    return (list->numItems == 0) ;
}