#ifndef __TOP_LIST_H__
#define __TOP_LIST_H__

#include "toptype.h"

G_BEGIN_DECLS

/*
 * @defgroup TOP_DS Data Structure.
 */

/**
 * @defgroup TOP_LIST Linked List
 * @ingroup TOP_DS
 * @{
 *
 * List in PJLIB is implemented as doubly-linked list, and it won't require
 * dynamic memory allocation (just as all PJLIB data structures). The list here
 * should be viewed more like a low level C list instead of high level C++ list
 * (which normally are easier to use but require dynamic memory allocations),
 * therefore all caveats with C list apply here too (such as you can NOT put
 * a node in more than one lists).
 *
 * \section top_list_example_sec Examples
 *
 * See below for examples on how to manipulate linked list:
 *  - @ref page_pjlib_samples_list_c
 *  - @ref page_pjlib_list_test
 */


/**
 * Use this macro in the start of the structure declaration to declare that
 * the structure can be used in the linked list operation. This macro simply
 * declares additional member @a prev and @a next to the structure.
 * @hideinitializer
 */
#define TOP_DECL_LIST_MEMBER(type)                       \
                                   /** List @a prev. */ \
                                   type *prev;          \
                                   /** List @a next. */ \
                                   type *next 


/**
 * This structure describes generic list node and list. The owner of this list
 * must initialize the 'value' member to an appropriate value (typically the
 * owner itself).
 */
struct top_list
{
    TOP_DECL_LIST_MEMBER(void);
};


/**
 * Initialize the list.
 * Initially, the list will have no member, and function top_list_empty() will
 * always return nonzero (which indicates TRUE) for the newly initialized 
 * list.
 *
 * @param node The list head.
 */
void top_list_init(top_list_type * node)
{
    ((top_list*)node)->next = ((top_list*)node)->prev = node;
}


/**
 * Check that the list is empty.
 *
 * @param node	The list head.
 *
 * @return Non-zero if the list is empty, or zero if it is not empty.
 *
 */
int top_list_empty(const top_list_type * node)
{
    return ((top_list*)node)->next == node;
}


/**
 * Insert the node to the list before the specified element position.
 *
 * @param pos	The element to which the node will be inserted before. 
 * @param node	The element to be inserted.
 *
 * @return void.
 */
void	top_list_insert_before(top_list_type *pos, top_list_type *node);


/**
 * Insert the node to the back of the list. This is just an alias for
 * #top_list_insert_before().
 *
 * @param list	The list. 
 * @param node	The element to be inserted.
 */
void top_list_push_back(top_list_type *list, top_list_type *node)
{
    top_list_insert_before(list, node);
}


/**
 * Inserts all nodes in \a nodes to the target list.
 *
 * @param lst	    The target list.
 * @param nodes	    Nodes list.
 */
void top_list_insert_nodes_before(top_list_type *lst,
					   top_list_type *nodes);

/**
 * Insert a node to the list after the specified element position.
 *
 * @param pos	    The element in the list which will precede the inserted 
 *		    element.
 * @param node	    The element to be inserted after the position element.
 *
 * @return void.
 */
void top_list_insert_after(top_list_type *pos, top_list_type *node);


/**
 * Insert the node to the front of the list. This is just an alias for
 * #top_list_insert_after().
 *
 * @param list	The list. 
 * @param node	The element to be inserted.
 */
void top_list_push_front(top_list_type *list, top_list_type *node)
{
    top_list_insert_after(list, node);
}


/**
 * Insert all nodes in \a nodes to the target list.
 *
 * @param lst	    The target list.
 * @param nodes	    Nodes list.
 */
void top_list_insert_nodes_after(top_list_type *lst,
					  top_list_type *nodes);


/**
 * Remove elements from the source list, and insert them to the destination
 * list. The elements of the source list will occupy the
 * front elements of the target list. Note that the node pointed by \a list2
 * itself is not considered as a node, but rather as the list descriptor, so
 * it will not be inserted to the \a list1. The elements to be inserted starts
 * at \a list2->next. If \a list2 is to be included in the operation, use
 * \a top_list_insert_nodes_before.
 *
 * @param list1	The destination list.
 * @param list2	The source list.
 *
 * @return void.
 */
void top_list_merge_first(top_list_type *list1, top_list_type *list2);


/**
 * Remove elements from the second list argument, and insert them to the list 
 * in the first argument. The elements from the second list will be appended
 * to the first list. Note that the node pointed by \a list2
 * itself is not considered as a node, but rather as the list descriptor, so
 * it will not be inserted to the \a list1. The elements to be inserted starts
 * at \a list2->next. If \a list2 is to be included in the operation, use
 * \a top_list_insert_nodes_before.
 *
 * @param list1	    The element in the list which will precede the inserted 
 *		    element.
 * @param list2	    The element in the list to be inserted.
 *
 * @return void.
 */
void top_list_merge_last( top_list_type *list1, top_list_type *list2);


/**
 * Erase the node from the list it currently belongs.
 *
 * @param node	    The element to be erased.
 */
void top_list_erase(top_list_type *node);


/**
 * Find node in the list.
 *
 * @param list	    The list head.
 * @param node	    The node element to be searched.
 *
 * @return The node itself if it is found in the list, or NULL if it is not 
 *         found in the list.
 */
top_list_type* top_list_find_node(top_list_type *list, 
					  top_list_type *node);


/**
 * Search the list for the specified value, using the specified comparison
 * function. This function iterates on nodes in the list, started with the
 * first node, and call the user supplied comparison function until the
 * comparison function returns ZERO.
 *
 * @param list	    The list head.
 * @param value	    The user defined value to be passed in the comparison 
 *		    function
 * @param comp	    The comparison function, which should return ZERO to 
 *		    indicate that the searched value is found.
 *
 * @return The first node that matched, or NULL if it is not found.
 */
top_list_type* top_list_search(top_list_type *list, void *value,
				       int (*comp)(void *value, 
						   const top_list_type *node)
				       );


/**
 * Traverse the list to get the number of elements in the list.
 *
 * @param list	    The list head.
 *
 * @return	    Number of elements.
 */
gsize top_list_size(const top_list_type *list);


/* Internal */
void top_link_node(top_list_type *prev, top_list_type *next)
{
    ((top_list*)prev)->next = next;
    ((top_list*)next)->prev = prev;
}

void top_list_insert_after(top_list_type *pos, top_list_type *node)
{
    ((top_list*)node)->prev = pos;
    ((top_list*)node)->next = ((top_list*)pos)->next;
    ((top_list*) ((top_list*)pos)->next) ->prev = node;
    ((top_list*)pos)->next = node;
}


void top_list_insert_before(top_list_type *pos, top_list_type *node)
{
    top_list_insert_after(((top_list*)pos)->prev, node);
}


void top_list_insert_nodes_after(top_list_type *pos, top_list_type *lst)
{
    top_list *lst_last = (top_list *) ((top_list*)lst)->prev;
    top_list *pos_next = (top_list *) ((top_list*)pos)->next;

    top_link_node(pos, lst);
    top_link_node(lst_last, pos_next);
}

void top_list_insert_nodes_before(top_list_type *pos, top_list_type *lst)
{
    top_list_insert_nodes_after(((top_list*)pos)->prev, lst);
}

void top_list_merge_last(top_list_type *lst1, top_list_type *lst2)
{
    if (!top_list_empty(lst2)) {
	top_link_node(((top_list*)lst1)->prev, ((top_list*)lst2)->next);
	top_link_node(((top_list*)lst2)->prev, lst1);
	top_list_init(lst2);
    }
}

void top_list_merge_first(top_list_type *lst1, top_list_type *lst2)
{
    if (!top_list_empty(lst2)) {
	top_link_node(((top_list*)lst2)->prev, ((top_list*)lst1)->next);
	top_link_node(((top_list*)lst1), ((top_list*)lst2)->next);
	top_list_init(lst2);
    }
}

void top_list_erase(top_list_type *node)
{
    top_link_node( ((top_list*)node)->prev, ((top_list*)node)->next);

    /* It'll be safer to init the next/prev fields to itself, to
     * prevent multiple erase() from corrupting the list. See
     * ticket #520 for one sample bug.
     */
    top_list_init(node);
}


top_list_type* top_list_find_node(top_list_type *list, top_list_type *node)
{
    top_list *p = (top_list *) ((top_list*)list)->next;
    while (p != list && p != node)
	p = (top_list *) p->next;

    return p==node ? p : NULL;
}


top_list_type* top_list_search(top_list_type *list, void *value,
	       		int (*comp)(void *value, const top_list_type *node))
{
    top_list *p = (top_list *) ((top_list*)list)->next;
    while (p != list && (*comp)(value, p) != 0)
	p = (top_list *) p->next;

    return p==list ? NULL : p;
}


gsize top_list_size(const top_list_type *list)
{
    const top_list *node = (const top_list*) ((const top_list*)list)->next;
    gsize count = 0;

    while (node != list) {
	++count;
	node = (top_list*)node->next;
    }

    return count;
}



G_END_DECLS

#endif	/* __TOP_LIST_H__ */


