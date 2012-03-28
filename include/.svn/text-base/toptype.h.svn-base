#ifndef _TOPTYPE_H
#define _TOPTYPE_H

G_BEGIN_DECLS
#ifdef G_OS_WIN32
#define TOP_WIN32         1
#endif
#define top_memcpy        memcpy
#define TOP_IOQUEUE_KEY_FREE_DELAY    500
/**
 * Get the total time value in miliseconds. This is the same as
 * multiplying the second part with 1000 and then add the miliseconds
 * part to the result.
 *
 * @param t     The time value.
 * @return      Total time in miliseconds.
 * @hideinitializer
 */
#define TOP_TIME_VAL_MSEC(t)	((t).tv_sec * 1000 + (t).tv_usec)
/**
 * This macro will check if \a t1 is equal to \a t2.
 *
 * @param t1    The first time value to compare.
 * @param t2    The second time value to compare.
 * @return      Non-zero if both time values are equal.
 * @hideinitializer
 */
#define TOP_TIME_VAL_EQ(t1, t2)  ((t1).tv_sec==(t2).tv_sec && (t1).tv_usec==(t2).tv_usec)

/**
 * This macro will check if \a t1 is greater than \a t2
 *
 * @param t1    The first time value to compare.
 * @param t2    The second time value to compare.
 * @return      Non-zero if t1 is greater than t2.
 * @hideinitializer
 */
#define TOP_TIME_VAL_GT(t1, t2)  ((t1).tv_sec>(t2).tv_sec || \
                                ((t1).tv_sec==(t2).tv_sec && (t1).tv_usec>(t2).tv_usec))

/**
 * This macro will check if \a t1 is greater than or equal to \a t2
 *
 * @param t1    The first time value to compare.
 * @param t2    The second time value to compare.
 * @return      Non-zero if t1 is greater than or equal to t2.
 * @hideinitializer
 */
#define TOP_TIME_VAL_GTE(t1, t2) (TOP_TIME_VAL_GT(t1,t2) || \
                                 TOP_TIME_VAL_EQ(t1,t2))
/**
 * Is native platform error positive number?
 * Default: 1 (yes)
 */
#ifndef TOP_NATIVE_ERR_POSITIVE
#   define TOP_NATIVE_ERR_POSITIVE   1
#endif

/**
 * Include error message string in the library (top_strerror()).
 * This is very much desirable!
 *
 * Default: 1
 */
#ifndef TOP_HAS_ERROR_STRING
#   define TOP_HAS_ERROR_STRING	    1
#endif
/**
 * If TOP_IOQUEUE_HAS_SAFE_UNREG macro is defined, then ioqueue
 * will do more things to ensure thread safety of handle
 * unregistration operation by employing reference counter to
 * each handle.
 *
 * In addition, the ioqueue will preallocate memory for the handles,
 * according to the maximum number of handles that is specified during
 * ioqueue creation.
 *
 * All applications would normally want this enabled, but you may disable
 * this if:
 *  - there is no dynamic unregistration to all ioqueues.
 *  - there is no threading, or there is no preemptive multitasking.
 *
 * Default: 1
 */
#ifndef TOP_IOQUEUE_HAS_SAFE_UNREG
#   define TOP_IOQUEUE_HAS_SAFE_UNREG	1
#endif
/**
 * Default concurrency setting for sockets/handles registered to ioqueue.
 * This controls whether the ioqueue is allowed to call the key's callback
 * concurrently/in parallel. The default is yes, which means that if there
 * are more than one pending operations complete simultaneously, more
 * than one threads may call the key's callback at the same time. This
 * generally would promote good scalability for application, at the
 * expense of more complexity to manage the concurrent accesses.
 *
 * Please see the ioqueue documentation for more info.
 */
#ifndef TOP_IOQUEUE_DEFAULT_ALLOW_CONCURRENCY
#   define TOP_IOQUEUE_DEFAULT_ALLOW_CONCURRENCY   1
#endif
/**
 * Maximum hostname length.
 * Libraries sometimes needs to make copy of an address to stack buffer;
 * the value here affects the stack usage.
 *
 * Default: 128
 */
#ifndef TOP_MAX_HOSTNAME
#  define TOP_MAX_HOSTNAME	    (128)
#endif
/**
 * Support TCP in the library.
 * Disabling TCP will reduce the footprint slightly (about 6KB).
 *
 * Default: 1
 */
#ifndef TOP_HAS_TCP
#  define TOP_HAS_TCP		    1
#endif

/** Status is OK. */
#define TOP_SUCCESS         0
#define TOP_FAIL            1

#define TOP_FALSE           0
#define TOP_TRUE            1

#define TOP_DECL(type)	    type
#define TOP_ASSERT_RETURN(expr,retval)    g_assert(expr)

/** Generic socket address. */
typedef void    top_sockaddr_t;

/** Socket handle. */
typedef long top_sock_t;
typedef void top_list_type;
/**
 * Opaque data type for I/O Queue structure.
 */
typedef struct top_ioqueue_t top_ioqueue_t;

/**
 * Opaque data type for key that identifies a handle registered to the
 * I/O queue framework.
 */
typedef struct top_ioqueue_key_t top_ioqueue_key_t;
/** Lock object. */
typedef struct top_lock_t top_lock_t;

/**
 * List.
 */
typedef struct top_list top_list;

void  time_normalize(GTimeVal *t);

G_END_DECLS

#endif
