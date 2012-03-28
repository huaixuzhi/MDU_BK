#ifndef _IO_QUEUE_BASE_H
#define _IO_QUEUE_BASE_H


/* ioqueue_base.h
 *
 * This file contains private declarations for abstracting various
 * event polling/dispatching mechanisms (e.g. select, poll, epoll)
 * to the ioqueue.
 */

#include "toplist.h"

/*
 * The select ioqueue relies on socket functions (top_sock_xxx()) to return
 * the correct error code.
 */
#if TOP_RETURN_OS_ERROR(100) != TOP_STATUS_FROM_OS(100)
#   error "Proper error reporting must be enabled for ioqueue to work!"
#endif


struct generic_operation
{
    TOP_DECL_LIST_MEMBER(struct generic_operation);
    top_ioqueue_operation_e  op;
};

struct read_operation
{
    TOP_DECL_LIST_MEMBER(struct read_operation);
    top_ioqueue_operation_e  op;

    void		   *buf;
    gsize		    size;
    unsigned                flags;
    top_sockaddr_t	   *rmt_addr;
    int			   *rmt_addrlen;
};

struct write_operation
{
    TOP_DECL_LIST_MEMBER(struct write_operation);
    top_ioqueue_operation_e  op;

    char		   *buf;
    gsize		    size;
    gssize              written;
    unsigned                flags;
    top_sockaddr_in	    rmt_addr;
    int			    rmt_addrlen;
};

struct accept_operation
{
    TOP_DECL_LIST_MEMBER(struct accept_operation);
    top_ioqueue_operation_e  op;

    top_sock_t              *accept_fd;
    top_sockaddr_t	   *local_addr;
    top_sockaddr_t	   *rmt_addr;
    int			   *addrlen;
};

union operation_key
{
    struct generic_operation generic;
    struct read_operation    read;
    struct write_operation   write;
#if TOP_HAS_TCP
    struct accept_operation  accept;
#endif
};

#if TOP_IOQUEUE_HAS_SAFE_UNREG
#   define UNREG_FIELDS			\
	unsigned	    ref_count;	\
	gboolean	    closing;	\
	GTimeVal	    free_time;	\

#else
#   define UNREG_FIELDS
#endif

#define DECLARE_COMMON_KEY                          \
    TOP_DECL_LIST_MEMBER(struct top_ioqueue_key_t);   \
    top_ioqueue_t           *ioqueue;                \
    GStaticRecMutex         mutex;                  \
    gboolean		    inside_callback;	    \
    gboolean		    destroy_requested;	    \
    gboolean		    allow_concurrent;	    \
    top_sock_t		    fd;                     \
    int                     fd_type;                \
    void		   *user_data;              \
    top_ioqueue_callback	    cb;                     \
    int                     connecting;             \
    struct read_operation   read_list;              \
    struct write_operation  write_list;             \
    struct accept_operation accept_list;	    \
    UNREG_FIELDS


#define DECLARE_COMMON_IOQUEUE                      \
    top_lock_t          *lock;                       \
    gboolean           auto_delete_lock;	    \
    gboolean		default_concurrency;


enum ioqueue_event_type
{
    NO_EVENT,
    READABLE_EVENT,
    WRITEABLE_EVENT,
    EXCEPTION_EVENT,
};

static void ioqueue_add_to_set( top_ioqueue_t *ioqueue,
                                top_ioqueue_key_t *key,
                                enum ioqueue_event_type event_type );
static void ioqueue_remove_from_set( top_ioqueue_t *ioqueue,
                                     top_ioqueue_key_t *key,
                                     enum ioqueue_event_type event_type);


#endif
