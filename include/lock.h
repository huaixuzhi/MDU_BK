#ifndef __TOP_LOCK_H__
#define __TOP_LOCK_H__

/**
 * @file lock.h
 * @brief Higher abstraction for locking objects.
 */
#include "toptype.h"

G_BEGIN_DECLS

/**
 * @defgroup TOP_LOCK Lock Objects
 * @ingroup TOP_OS
 * @{
 *
 * <b>Lock Objects</b> are higher abstraction for different lock mechanisms.
 * It offers the same API for manipulating different lock types (e.g.
 * @ref TOP_MUTEX "mutex", @ref TOP_SEM "semaphores", or null locks).
 * Because Lock Objects have the same API for different types of lock
 * implementation, it can be passed around in function arguments. As the
 * result, it can be used to control locking policy for  a particular
 * feature.
 */
gint top_mutex_lock(GMutex* mutex);
gint top_mutex_trylock(GMutex* mutex);
gint top_mutex_unlock(GMutex* mutex);
gint top_mutex_free(GMutex* mutex);

/**
 * Create simple, non recursive mutex lock object.
 *
 * @param lock	    Pointer to store the returned handle.
 *
 * @return	    TOP_SUCCESS or the appropriate error code.
 */
gint top_lock_create_simple_mutex( top_lock_t **lock );

/**
 * Create recursive mutex lock object.
 *
 * @param lock	    Pointer to store the returned handle.
 *
 * @return	    TOP_SUCCESS or the appropriate error code.
 */
gint top_lock_create_recursive_mutex( top_lock_t **lock );


/**
 * Acquire lock on the specified lock object.
 *
 * @param lock	    The lock object.
 *
 * @return	    TOP_SUCCESS or the appropriate error code.
 */
gint top_lock_acquire( top_lock_t *lock );


/**
 * Try to acquire lock on the specified lock object.
 *
 * @param lock	    The lock object.
 *
 * @return	    TOP_SUCCESS or the appropriate error code.
 */
gint top_lock_tryacquire( top_lock_t *lock );


/**
 * Release lock on the specified lock object.
 *
 * @param lock	    The lock object.
 *
 * @return	    TOP_SUCCESS or the appropriate error code.
 */
gint top_lock_release( top_lock_t *lock );


/**
 * Destroy the lock object.
 *
 * @param lock	    The lock object.
 *
 * @return	    TOP_SUCCESS or the appropriate error code.
 */
gint top_lock_destroy( top_lock_t *lock );


/** @} */

G_END_DECLS


#endif	/* __TOP_LOCK_H__ */


