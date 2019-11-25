#ifndef __HE_UTILS_LIST_H__
#define __HE_UTILS_LIST_H__

#include "types.h"

/*
 * Architectures might want to move the poison pointer offset
 * into some well-recognized area such as 0xdead000000000000,
 * that is also not mappable by user-space exploits:
 */
#define HE_UTILS_POISON_POINTER 0

/*
 * These are non-NULL pointers that will result in page faults
 * under normal circumstances, used to verify that nobody uses
 * non-initialized list entries.
 */
#define HE_UTILS_LIST_POISON1  ((void *) (0x100 + HE_UTILS_POISON_POINTER))
#define HE_UTILS_LIST_POISON2  ((void *) (0x200 + HE_UTILS_POISON_POINTER))

struct he_utils_list_head_s {
    struct he_utils_list_head_s *next, *prev;
};
typedef struct he_utils_list_head_s he_utils_list_head_t;

struct he_utils_hlist_node_s {
    struct he_utils_hlist_node_s *next, **pprev;
};
typedef struct he_utils_hlist_node_s he_utils_hlist_node_t;

struct he_utils_hlist_head_s {
    struct he_utils_hlist_node_s *first;
};
typedef struct he_utils_hlist_head_s he_utils_hlist_head_t;


/*
 * Simple doubly linked list implementation.
 *
 * Some of the internal functions ("__xxx") are useful when
 * manipulating whole lists rather than single entries, as
 * sometimes we already know the next/prev entries and we can
 * generate better code by using them directly rather than
 * using the generic single-entry routines.
 */

#define HE_UTILS_LIST_HEAD_INIT(name) { &(name), &(name) }

#define HE_UTILS_LIST_HEAD(name) \
    he_utils_list_head_t name = HE_UTILS_LIST_HEAD_INIT(name)

static inline void HE_UTILS_INIT_LIST_HEAD(he_utils_list_head_t *list) {
    list->next = list;
    list->prev = list;
}

/*
 * Insert a new entry between two known consecutive entries.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __list_add(he_utils_list_head_t *node,
                              he_utils_list_head_t *prev,
                              he_utils_list_head_t *next) {
    next->prev = node;
    node->next = next;
    node->prev = prev;
    prev->next = node;
}

/**
 * he_utils_list_add - add a new entry
 * @node: new entry to be added
 * @head: list head to add it after
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 */
static inline void he_utils_list_add(he_utils_list_head_t *node,
                            he_utils_list_head_t *head) {
    __list_add(node, head, head->next);
}


/**
 * he_utils_list_add_tail - add a new entry
 * @node: new entry to be added
 * @head: list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
static inline void he_utils_list_add_tail(he_utils_list_head_t *node,
                                 he_utils_list_head_t *head) {
    __list_add(node, head->prev, head);
}

/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __list_del(he_utils_list_head_t * prev,
                              he_utils_list_head_t * next) {
    next->prev = prev;
    prev->next = next;
}

/**
 * he_utils_list_del - deletes entry from list.
 * @entry: the element to delete from the list.
 * Note: he_utils_list_empty() on entry does not return true after this,
 * the entry is in an undefined state.
 */
static inline void __list_del_entry(he_utils_list_head_t *entry) {
    __list_del(entry->prev, entry->next);
}

static inline void he_utils_list_del(he_utils_list_head_t *entry) {
    __list_del(entry->prev, entry->next);
    entry->next = (he_utils_list_head_t *)HE_UTILS_LIST_POISON1;
    entry->prev = (he_utils_list_head_t *)HE_UTILS_LIST_POISON2;
}

/**
 * he_utils_list_replace - replace old entry by new one
 * @oldnode : the element to be replaced
 * @newnode : the new element to insert
 *
 * If @old was empty, it will be overwritten.
 */
static inline void he_utils_list_replace(he_utils_list_head_t *oldnode,
                                he_utils_list_head_t *newnode) {
    newnode->next = oldnode->next;
    newnode->next->prev = newnode;
    newnode->prev = oldnode->prev;
    newnode->prev->next = newnode;
}

static inline void he_utils_list_replace_init(he_utils_list_head_t *oldnode,
                                     he_utils_list_head_t *newnode) {
    he_utils_list_replace(oldnode, newnode);
    HE_UTILS_INIT_LIST_HEAD(oldnode);
}

/**
 * he_utils_list_del_init - deletes entry from list and reinitialize it.
 * @entry: the element to delete from the list.
 */
static inline void he_utils_list_del_init(he_utils_list_head_t *entry) {
    __list_del_entry(entry);
    HE_UTILS_INIT_LIST_HEAD(entry);
}

/**
 * he_utils_list_move - delete from one list and add as another's head
 * @list: the entry to move
 * @head: the head that will precede our entry
 */
static inline void he_utils_list_move(he_utils_list_head_t *list,
                             he_utils_list_head_t *head) {
    __list_del_entry(list);
    he_utils_list_add(list, head);
}

/**
 * he_utils_list_move_tail - delete from one list and add as another's tail
 * @list: the entry to move
 * @head: the head that will follow our entry
 */
static inline void he_utils_list_move_tail(he_utils_list_head_t *list,
                                  he_utils_list_head_t *head) {
    __list_del_entry(list);
    he_utils_list_add_tail(list, head);
}

/**
 * he_utils_list_is_last - tests whether @list is the last entry in list @head
 * @list: the entry to test
 * @head: the head of the list
 */
static inline int he_utils_list_is_last(const he_utils_list_head_t *list,
                               const he_utils_list_head_t *head) {
    return list->next == head;
}

/**
 * he_utils_list_empty - tests whether a list is empty
 * @head: the list to test.
 */
static inline int he_utils_list_empty(const he_utils_list_head_t *head) {
    return head->next == head;
}

/**
 * he_utils_list_empty_careful - tests whether a list is empty and not being modified
 * @head: the list to test
 *
 * Description:
 * tests whether a list is empty _and_ checks that no other CPU might be
 * in the process of modifying either member (next or prev)
 *
 * NOTE: using he_utils_list_empty_careful() without synchronization
 * can only be safe if the only activity that can happen
 * to the list entry is he_utils_list_del_init(). Eg. it cannot be used
 * if another CPU could re-he_utils_list_add() it.
 */
static inline int he_utils_list_empty_careful(const he_utils_list_head_t *head) {
    he_utils_list_head_t *next = head->next;
    return (next == head) && (next == head->prev);
}

/**
 * he_utils_list_rotate_left - rotate the list to the left
 * @head: the head of the list
 */
static inline void he_utils_list_rotate_left(he_utils_list_head_t *head) {
    he_utils_list_head_t *first;

    if (!he_utils_list_empty(head)) {
        first = head->next;
        he_utils_list_move_tail(first, head);
    }
}

/**
 * he_utils_list_is_singular - tests whether a list has just one entry.
 * @head: the list to test.
 */
static inline int he_utils_list_is_singular(const he_utils_list_head_t *head) {
    return !he_utils_list_empty(head) && (head->next == head->prev);
}

static inline void __list_cut_position(he_utils_list_head_t *list,
                                       he_utils_list_head_t *head,
                                       he_utils_list_head_t *entry) {
    he_utils_list_head_t *new_first = entry->next;
    list->next = head->next;
    list->next->prev = list;
    list->prev = entry;
    entry->next = list;
    head->next = new_first;
    new_first->prev = head;
}

/**
 * he_utils_list_cut_position - cut a list into two
 * @list: a new list to add all removed entries
 * @head: a list with entries
 * @entry: an entry within head, could be the head itself
 *  and if so we won't cut the list
 *
 * This helper moves the initial part of @head, up to and
 * including @entry, from @head to @list. You should
 * pass on @entry an element you know is on @head. @list
 * should be an empty list or a list you do not care about
 * losing its data.
 *
 */
static inline void he_utils_list_cut_position(he_utils_list_head_t *list,
                                     he_utils_list_head_t *head,
                                     he_utils_list_head_t *entry) {
    if (he_utils_list_empty(head))
        return;
    if (he_utils_list_is_singular(head) &&
            (head->next != entry && head != entry))
        return;
    if (entry == head)
        HE_UTILS_INIT_LIST_HEAD(list);
    else
        __list_cut_position(list, head, entry);
}

static inline void __list_splice(const he_utils_list_head_t *list,
                                 he_utils_list_head_t *prev,
                                 he_utils_list_head_t *next) {
    he_utils_list_head_t *first = list->next;
    he_utils_list_head_t *last = list->prev;

    first->prev = prev;
    prev->next = first;

    last->next = next;
    next->prev = last;
}

/**
 * he_utils_list_splice - join two lists, this is designed for stacks
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 */
static inline void he_utils_list_splice(const he_utils_list_head_t *list,
                               he_utils_list_head_t *head) {
    if (!he_utils_list_empty(list))
        __list_splice(list, head, head->next);
}

/**
 * he_utils_list_splice_tail - join two lists, each list being a queue
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 */
static inline void he_utils_list_splice_tail(he_utils_list_head_t *list,
                                    he_utils_list_head_t *head) {
    if (!he_utils_list_empty(list))
        __list_splice(list, head->prev, head);
}

/**
 * he_utils_list_splice_init - join two lists and reinitialise the emptied list.
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 *
 * The list at @list is reinitialised
 */
static inline void he_utils_list_splice_init(he_utils_list_head_t *list,
                                    he_utils_list_head_t *head) {
    if (!he_utils_list_empty(list)) {
        __list_splice(list, head, head->next);
        HE_UTILS_INIT_LIST_HEAD(list);
    }
}

/**
 * he_utils_list_splice_tail_init - join two lists and reinitialise the emptied list
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 *
 * Each of the lists is a queue.
 * The list at @list is reinitialised
 */
static inline void he_utils_list_splice_tail_init(he_utils_list_head_t *list,
        he_utils_list_head_t *head) {
    if (!he_utils_list_empty(list)) {
        __list_splice(list, head->prev, head);
        HE_UTILS_INIT_LIST_HEAD(list);
    }
}

/**
 * he_utils_list_entry - get the struct for this entry
 * @ptr:    the &he_utils_list_head_t pointer.
 * @type:   the type of the struct this is embedded in.
 * @member: the name of the he_utils_list_head within the struct.
 */
#define he_utils_list_entry(ptr, type, member) \
    container_of(ptr, type, member)

/**
 * he_utils_list_first_entry - get the first element from a list
 * @ptr:    the list head to take the element from.
 * @type:   the type of the struct this is embedded in.
 * @member: the name of the he_utils_list_head within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define he_utils_list_first_entry(ptr, type, member) \
    he_utils_list_entry((ptr)->next, type, member)

/**
 * he_utils_list_last_entry - get the last element from a list
 * @ptr:    the list head to take the element from.
 * @type:   the type of the struct this is embedded in.
 * @member: the name of the he_utils_list_head within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define he_utils_list_last_entry(ptr, type, member) \
    he_utils_list_entry((ptr)->prev, type, member)

/**
 * he_utils_list_first_entry_or_null - get the first element from a list
 * @ptr:    the list head to take the element from.
 * @type:   the type of the struct this is embedded in.
 * @member: the name of the he_utils_list_head within the struct.
 *
 * Note that if the list is empty, it returns NULL.
 */
#define he_utils_list_first_entry_or_null(ptr, type, member) \
    (!he_utils_list_empty(ptr) ? he_utils_list_first_entry(ptr, type, member) : NULL)

/**
 * he_utils_list_next_entry - get the next element in list
 * @pos:    the type * to cursor
 * @member: the name of the he_utils_list_head within the struct.
 */
#define he_utils_list_next_entry(pos, member) \
    he_utils_list_entry((pos)->member.next, typeof(*(pos)), member)

/**
 * he_utils_list_prev_entry - get the prev element in list
 * @pos:    the type * to cursor
 * @member: the name of the he_utils_list_head within the struct.
 */
#define he_utils_list_prev_entry(pos, member) \
    he_utils_list_entry((pos)->member.prev, typeof(*(pos)), member)

/**
 * he_utils_list_for_each    -   iterate over a list
 * @pos:    the &he_utils_list_head_t to use as a loop cursor.
 * @head:   the head for your list.
 */
#define he_utils_list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

/**
 * he_utils_list_for_each_prev   -   iterate over a list backwards
 * @pos:    the &he_utils_list_head_t to use as a loop cursor.
 * @head:   the head for your list.
 */
#define he_utils_list_for_each_prev(pos, head) \
    for (pos = (head)->prev; pos != (head); pos = pos->prev)

/**
 * he_utils_list_for_each_safe - iterate over a list safe against removal of list entry
 * @pos:    the &he_utils_list_head_t to use as a loop cursor.
 * @n:      another &he_utils_list_head_t to use as temporary storage
 * @head:   the head for your list.
 */
#define he_utils_list_for_each_safe(pos, n, head) \
    for (pos = (head)->next, n = pos->next; pos != (head); \
        pos = n, n = pos->next)

/**
 * he_utils_list_for_each_prev_safe - iterate over a list backwards safe against removal of list entry
 * @pos:    the &he_utils_list_head_t to use as a loop cursor.
 * @n:      another &he_utils_list_head_t to use as temporary storage
 * @head:   the head for your list.
 */
#define he_utils_list_for_each_prev_safe(pos, n, head) \
    for (pos = (head)->prev, n = pos->prev; \
         pos != (head); \
         pos = n, n = pos->prev)

/**
 * he_utils_list_for_each_entry  -   iterate over list of given type
 * @pos:    the type * to use as a loop cursor.
 * @head:   the head for your list.
 * @member: the name of the he_utils_list_head within the struct.
 */
#define he_utils_list_for_each_entry(pos, head, member)              \
    for (pos = he_utils_list_first_entry(head, typeof(*pos), member);    \
         &pos->member != (head);                    \
         pos = he_utils_list_next_entry(pos, member))

/**
 * he_utils_list_for_each_entry_reverse - iterate backwards over list of given type.
 * @pos:    the type * to use as a loop cursor.
 * @head:   the head for your list.
 * @member: the name of the he_utils_list_head within the struct.
 */
#define he_utils_list_for_each_entry_reverse(pos, head, member)          \
    for (pos = he_utils_list_last_entry(head, typeof(*pos), member);     \
         &pos->member != (head);                    \
         pos = he_utils_list_prev_entry(pos, member))

/**
 * he_utils_list_prepare_entry - prepare a pos entry for use in he_utils_list_for_each_entry_continue()
 * @pos:    the type * to use as a start point
 * @head:   the head of the list
 * @member: the name of the he_utils_list_head within the struct.
 *
 * Prepares a pos entry for use as a start point in he_utils_list_for_each_entry_continue().
 */
#define he_utils_list_prepare_entry(pos, head, member) \
    ((pos) ? : he_utils_list_entry(head, typeof(*pos), member))

/**
 * he_utils_list_for_each_entry_continue - continue iteration over list of given type
 * @pos:    the type * to use as a loop cursor.
 * @head:   the head for your list.
 * @member: the name of the he_utils_list_head within the struct.
 *
 * Continue to iterate over list of given type, continuing after
 * the current position.
 */
#define he_utils_list_for_each_entry_continue(pos, head, member)         \
    for (pos = he_utils_list_next_entry(pos, member);            \
         &pos->member != (head);                    \
         pos = he_utils_list_next_entry(pos, member))

/**
 * he_utils_list_for_each_entry_continue_reverse - iterate backwards from the given point
 * @pos:    the type * to use as a loop cursor.
 * @head:   the head for your list.
 * @member: the name of the he_utils_list_head within the struct.
 *
 * Start to iterate over list of given type backwards, continuing after
 * the current position.
 */
#define he_utils_list_for_each_entry_continue_reverse(pos, head, member)     \
    for (pos = he_utils_list_prev_entry(pos, member);            \
         &pos->member != (head);                    \
         pos = he_utils_list_prev_entry(pos, member))

/**
 * he_utils_list_for_each_entry_from - iterate over list of given type from the current point
 * @pos:    the type * to use as a loop cursor.
 * @head:   the head for your list.
 * @member: the name of the he_utils_list_head within the struct.
 *
 * Iterate over list of given type, continuing from current position.
 */
#define he_utils_list_for_each_entry_from(pos, head, member)             \
    for (; &pos->member != (head);                  \
         pos = he_utils_list_next_entry(pos, member))

/**
 * he_utils_list_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @pos:    the type * to use as a loop cursor.
 * @n:      another type * to use as temporary storage
 * @head:   the head for your list.
 * @member: the name of the he_utils_list_head within the struct.
 */
#define he_utils_list_for_each_entry_safe(pos, n, head, member)          \
    for (pos = he_utils_list_first_entry(head, typeof(*pos), member),    \
        n = he_utils_list_next_entry(pos, member);           \
         &pos->member != (head);                    \
         pos = n, n = he_utils_list_next_entry(n, member))

/**
 * he_utils_list_for_each_entry_safe_continue - continue list iteration safe against removal
 * @pos:    the type * to use as a loop cursor.
 * @n:      another type * to use as temporary storage
 * @head:   the head for your list.
 * @member: the name of the he_utils_list_head within the struct.
 *
 * Iterate over list of given type, continuing after current point,
 * safe against removal of list entry.
 */
#define he_utils_list_for_each_entry_safe_continue(pos, n, head, member)         \
    for (pos = he_utils_list_next_entry(pos, member),                \
        n = he_utils_list_next_entry(pos, member);               \
         &pos->member != (head);                        \
         pos = n, n = he_utils_list_next_entry(n, member))

/**
 * he_utils_list_for_each_entry_safe_from - iterate over list from current point safe against removal
 * @pos:    the type * to use as a loop cursor.
 * @n:      another type * to use as temporary storage
 * @head:   the head for your list.
 * @member: the name of the he_utils_list_head within the struct.
 *
 * Iterate over list of given type from current point, safe against
 * removal of list entry.
 */
#define he_utils_list_for_each_entry_safe_from(pos, n, head, member)             \
    for (n = he_utils_list_next_entry(pos, member);                  \
         &pos->member != (head);                        \
         pos = n, n = he_utils_list_next_entry(n, member))

/**
 * he_utils_list_for_each_entry_safe_reverse - iterate backwards over list safe against removal
 * @pos:    the type * to use as a loop cursor.
 * @n:      another type * to use as temporary storage
 * @head:   the head for your list.
 * @member: the name of the he_utils_list_head within the struct.
 *
 * Iterate backwards over list of given type, safe against removal
 * of list entry.
 */
#define he_utils_list_for_each_entry_safe_reverse(pos, n, head, member)      \
    for (pos = he_utils_list_last_entry(head, typeof(*pos), member),     \
        n = he_utils_list_prev_entry(pos, member);           \
         &pos->member != (head);                    \
         pos = n, n = he_utils_list_prev_entry(n, member))

/**
 * he_utils_list_safe_reset_next - reset a stale he_utils_list_for_each_entry_safe loop
 * @pos:    the loop cursor used in the he_utils_list_for_each_entry_safe loop
 * @n:      temporary storage used in he_utils_list_for_each_entry_safe
 * @member: the name of the he_utils_list_head within the struct.
 *
 * he_utils_list_safe_reset_next is not safe to use in general if the list may be
 * modified concurrently (eg. the lock is dropped in the loop body). An
 * exception to this is if the cursor element (pos) is pinned in the list,
 * and he_utils_list_safe_reset_next is called after re-taking the lock and before
 * completing the current iteration of the loop body.
 */
#define he_utils_list_safe_reset_next(pos, n, member)                \
    n = he_utils_list_next_entry(pos, member)

/*
 * Double linked lists with a single pointer list head.
 * Mostly useful for hash tables where the two pointer list head is
 * too wasteful.
 * You lose the ability to access the tail in O(1).
 */

#define HE_UTILS_HLIST_HEAD_INIT { .first = NULL }
#define HE_UTILS_HLIST_HEAD(name) he_utils_hlist_head_t name = {  .first = NULL }
#define HE_UTILS_INIT_HLIST_HEAD(ptr) ((ptr)->first = NULL)
static inline void HE_UTILS_INIT_HLIST_NODE(he_utils_hlist_node_t *h) {
    h->next = NULL;
    h->pprev = NULL;
}

static inline int he_utils_hlist_unhashed(const he_utils_hlist_node_t *h) {
    return !h->pprev;
}

static inline int he_utils_hlist_empty(const he_utils_hlist_head_t *h) {
    return !h->first;
}

static inline void __he_utils_hlist_del(he_utils_hlist_node_t *n) {
    he_utils_hlist_node_t *next = n->next;
    he_utils_hlist_node_t **pprev = n->pprev;

    *pprev = next;
    if (next)
        next->pprev = pprev;
}

static inline void he_utils_hlist_del(he_utils_hlist_node_t *n) {
    __he_utils_hlist_del(n);
    n->next = (he_utils_hlist_node_t *)HE_UTILS_LIST_POISON1;
    n->pprev = (he_utils_hlist_node_t **)HE_UTILS_LIST_POISON2;
}

static inline void he_utils_hlist_del_init(he_utils_hlist_node_t *n) {
    if (!he_utils_hlist_unhashed(n)) {
        __he_utils_hlist_del(n);
        HE_UTILS_INIT_HLIST_NODE(n);
    }
}

static inline void he_utils_hlist_add_head(he_utils_hlist_node_t *n,
                                  he_utils_hlist_head_t *h) {
    he_utils_hlist_node_t *first = h->first;
    n->next = first;
    if (first)
        first->pprev = &n->next;
    h->first = n;
    n->pprev = &h->first;
}

/* next must be != NULL */
static inline void he_utils_hlist_add_before(he_utils_hlist_node_t *n,
                                    he_utils_hlist_node_t *next) {
    n->pprev = next->pprev;
    n->next = next;
    next->pprev = &n->next;
    *(n->pprev) = n;
}

static inline void he_utils_hlist_add_behind(he_utils_hlist_node_t *n,
                                    he_utils_hlist_node_t *prev) {
    n->next = prev->next;
    prev->next = n;
    n->pprev = &prev->next;

    if (n->next)
        n->next->pprev  = &n->next;
}

/* after that we'll appear to be on some hlist and he_utils_hlist_del will work */
static inline void he_utils_hlist_add_fake(he_utils_hlist_node_t *n) {
    n->pprev = &n->next;
}

static inline int he_utils_hlist_fake(he_utils_hlist_node_t *h) {
    return h->pprev == &h->next;
}

/*
 * Move a list from one list head to another. Fixup the pprev
 * reference of the first entry if it exists.
 */
static inline void he_utils_hlist_move_list(he_utils_hlist_head_t *old,
                                   he_utils_hlist_head_t *node) {
    node->first = old->first;
    if (node->first)
        node->first->pprev = &node->first;
    old->first = NULL;
}

#define he_utils_hlist_entry(ptr, type, member) container_of(ptr,type,member)

#define he_utils_hlist_for_each(pos, head) \
    for (pos = (head)->first; pos ; pos = pos->next)

#define he_utils_hlist_for_each_safe(pos, n, head) \
    for (pos = (head)->first; pos && ({ n = pos->next; 1; }); \
         pos = n)

#define he_utils_hlist_entry_safe(ptr, type, member) \
    ({ typeof(ptr) ____ptr = (ptr); \
       ____ptr ? he_utils_hlist_entry(____ptr, type, member) : NULL; \
    })

/**
 * he_utils_hlist_for_each_entry - iterate over list of given type
 * @pos:    the type * to use as a loop cursor.
 * @head:   the head for your list.
 * @member: the name of the he_utils_hlist_node within the struct.
 */
#define he_utils_hlist_for_each_entry(pos, head, member)             \
    for (pos = he_utils_hlist_entry_safe((head)->first, typeof(*(pos)), member);\
         pos;                           \
         pos = he_utils_hlist_entry_safe((pos)->member.next, typeof(*(pos)), member))

/**
 * he_utils_hlist_for_each_entry_continue - iterate over a hlist continuing after current point
 * @pos:    the type * to use as a loop cursor.
 * @member: the name of the he_utils_hlist_node within the struct.
 */
#define he_utils_hlist_for_each_entry_continue(pos, member)          \
    for (pos = he_utils_hlist_entry_safe((pos)->member.next, typeof(*(pos)), member);\
         pos;                           \
         pos = he_utils_hlist_entry_safe((pos)->member.next, typeof(*(pos)), member))

/**
 * he_utils_hlist_for_each_entry_from - iterate over a hlist continuing from current point
 * @pos:    the type * to use as a loop cursor.
 * @member: the name of the he_utils_hlist_node within the struct.
 */
#define he_utils_hlist_for_each_entry_from(pos, member)              \
    for (; pos;                         \
         pos = he_utils_hlist_entry_safe((pos)->member.next, typeof(*(pos)), member))

/**
 * he_utils_hlist_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @pos:    the type * to use as a loop cursor.
 * @n:      another &he_utils_hlist_node_t to use as temporary storage
 * @head:   the head for your list.
 * @member: the name of the he_utils_hlist_node within the struct.
 */
#define he_utils_hlist_for_each_entry_safe(pos, n, head, member)         \
    for (pos = he_utils_hlist_entry_safe((head)->first, typeof(*pos), member);\
         pos && ({ n = pos->member.next; 1; });         \
         pos = he_utils_hlist_entry_safe(n, typeof(*pos), member))

#endif
