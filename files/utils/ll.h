#ifndef DDE_LL_H
#define DDE_LL_H
#include "../includes/includes.h"


typedef struct _ListElement {
    struct _ListElement * next;
    struct _ListElement * prev;
    struct {
        bool allocated;
    } _reserved;
} ListElement;

typedef struct {
    ListElement * head;
    ListElement * tail;
    ListElement * current;
    int numItems;
    struct {
        bool allocated;
    } _reserved;
} List;

/**
 * Allocates or initializes a new list. When you pre-allocate a list always call this function with the list as an
 * argument since there are internal information that needs to be populated.
 * @param list {List *} (optional) - A pointer to a list or NULL, if not supplied a new list is allocated.
 * @return {List *} A pointer to the supplied list or the newly allocated list. On error it returns NULL
 */
List * List_new( List * list );

/**
 * Destroys a list and resets its values
 * @param list {List *} - The list
 */
void List_destroy( List * list );

/**
 * Frees a locally allocated list.
 * @param list {List *} - A list that was allocated by the List_new() function
 */
void List_free( List * list );

/**
 * Pushes a struct element that contains `ListElement *` as it's first field as first in the list. Use `List_pushElement` to automatically
 * cast the element.
 * @param list {List *} - The list
 * @param listElement {ListElement *} - A pointer to the list element or struct that contains ListElement at its start
 * @return {bool} - true on success, false on error
 */
bool _List_pushElement( List * list, ListElement * listElement );
#define List_pushElement( list, listElement ) _List_pushElement( list, (ListElement *)(listElement) )

/**
 * Pushes a struct element that contains `ListElement *` as it's first field as last in the list. Use `List_pushElement` to automatically
 * cast the element.
 * @param list {List *} - The list
 * @param listElement {ListElement *} - A pointer to the list element or struct that contains ListElement at its start
 * @return {bool} - true on success, false on error
 */
bool _List_pushElement_back( List * list, ListElement * listElement );
#define List_pushElement_back( list, listElement ) _List_pushElement_back( list, (ListElement *)(listElement) )


/**
 * Pops and removes an element from the top of the list
 * @param list {List *} - The list
 * @return {ListElement *} - The list element to cast to whatever datatype you wish.
 */
ListElement * List_popElement( List * list );

/**
 * Gets head element
 * @param list  {List *} - The list
 * @return {ListElement *} - The head list element
 */
ListElement * List_getHead( List * list );

/**
 * Gets tail element
 * @param list  {List *} - The list
 * @return {ListElement *} - The tail list element
 */
ListElement * List_getTail( List * list );

/**
 * Gets the next element from the list. If this is the first time this function is called then the first element
 * is returned. If the last element is reached then next item is NULL and the list is reset
 * @param list {List *} - Pointer to the list
 * @return {ListElement *} - The list element or NULL
 */
ListElement * List_getNextElement( List * list );

/**
 * Gets the previous element from the list. This starts walking the list backwards. If the last (in this case first)
 * element is reached while doing prev, then NULL is returned and the list is reset.
 * @param list {List *} - Pointer to the list
 * @return {ListElement *} - The list element or NULL at end
 */
ListElement * List_getPrevElement( List * list );

/**
 * Resets the list back to the first item.
 * @param list {List *} - Pointer to list
 */
void List_reset( List * list );

/**
 * Removes a list element from the list. Use `List_remove` to automatically cast element
 * @param list {List *} - Pointer to list
 * @param listElement {ListElement *} - Pointer to element
 * @return {bool} - true on success, false on error
 */
bool _List_remove( List * list, ListElement * listElement );
#define List_remove( list, listElement )  _List_remove( list, (ListElement *)listElement )

/**
 * returns true is list is empty.
 * @param list {List *} - Pointer to list
 */
bool List_is_empty( List * list );


#endif //DDE_LL_H
