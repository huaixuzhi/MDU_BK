/*
 * ioqueue_base.c
 *
 * This contains common functionalities to emulate proactor pattern with
 * various event dispatching mechanisms (e.g. select, epoll).
 *
 * This file will be included by the appropriate ioqueue implementation.
 * This file is NOT supposed to be compiled as stand-alone source.
 */
//#include "toptype.h"
//#include <string.h>
#define PENDING_RETRY	2

static void topioque_init( top_ioqueue_t *ioqueue )
{
    ioqueue->lock = NULL;
    ioqueue->auto_delete_lock = 0;
    ioqueue->default_concurrency = TOP_IOQUEUE_DEFAULT_ALLOW_CONCURRENCY;
}

static gint topioque_destroy(top_ioqueue_t *ioqueue)
{
    if (ioqueue->auto_delete_lock && ioqueue->lock ) {
	    top_lock_release(ioqueue->lock);
        return top_lock_destroy(ioqueue->lock);
    }

    return TOP_SUCCESS;
}

/*
 * top_ioqueue_set_lock()
 */
gint top_ioqueue_set_lock( top_ioqueue_t *ioqueue,
					 top_lock_t *lock,
					 gboolean auto_delete )
{
    TOP_ASSERT_RETURN(ioqueue && lock, TOP_EINVAL);

    if (ioqueue->auto_delete_lock && ioqueue->lock) {
        top_lock_destroy(ioqueue->lock);
    }

    ioqueue->lock = lock;
    ioqueue->auto_delete_lock = auto_delete;

    return TOP_SUCCESS;
}

static gint topque_init_key( top_ioqueue_t *ioqueue,
                                     top_ioqueue_key_t *key,
                                     top_sock_t sock,
                                     void *user_data,
                                     const top_ioqueue_callback *cb)
{
    gint rc;
    int optlen;

 key->ioqueue = ioqueue;
    key->fd = sock;
    key->user_data = user_data;
    top_list_init(&key->read_list);
    top_list_init(&key->write_list);
#if TOP_HAS_TCP
    top_list_init(&key->accept_list);
    key->connecting = 0;
#endif

    /* Save callback. */
    top_memcpy(&key->cb, cb, sizeof(top_ioqueue_callback));

#if TOP_IOQUEUE_HAS_SAFE_UNREG
    /* Set initial reference count to 1 */
    g_assert(key->ref_count == 0);
    ++key->ref_count;

    key->closing = 0;
#endif

    rc = top_ioqueue_set_concurrency(key, ioqueue->default_concurrency);
    if (rc != TOP_SUCCESS)
	return rc;

    /* Get socket type. When socket type is datagram, some optimization
     * will be performed during send to allow parallel send operations.
     */
    optlen = sizeof(key->fd_type);
    rc = top_sock_getsockopt(sock, top_SOL_SOCKET(), top_SO_TYPE(),
                            &key->fd_type, &optlen);
    if (rc != TOP_SUCCESS)
        key->fd_type = top_SOCK_STREAM();

    /* Create mutex for the key. */
#if !TOP_IOQUEUE_HAS_SAFE_UNREG
    g_static_rec_mutex_init(&key->mutex);
#endif

    return rc;
}

/*
 * top_ioqueue_get_user_data()
 *
 * Obtain value associated with a key.
 */
void* top_ioqueue_get_user_data( top_ioqueue_key_t *key )
{
    TOP_ASSERT_RETURN(key != NULL, NULL);
    return key->user_data;
}

/*
 * top_ioqueue_set_user_data()
 */
gint top_ioqueue_set_user_data( top_ioqueue_key_t *key,
                                              void *user_data,
                                              void **old_data)
{
    TOP_ASSERT_RETURN(key, TOP_EINVAL);

    if (old_data)
        *old_data = key->user_data;
    key->user_data = user_data;

    return TOP_SUCCESS;
}

int top_key_has_pending_write(top_ioqueue_key_t *key)
{
    return !top_list_empty(&key->write_list);
}

int top_key_has_pending_read(top_ioqueue_key_t *key)
{
    return !top_list_empty(&key->read_list);
}

int top_key_has_pending_accept(top_ioqueue_key_t *key)
{
#if TOP_HAS_TCP
    return !top_list_empty(&key->accept_list);
#else
    TOP_UNUSED_ARG(key);
    return 0;
#endif
}

int key_has_pending_connect(top_ioqueue_key_t *key)
{
    return key->connecting;
}


#if TOP_IOQUEUE_HAS_SAFE_UNREG
#   define TOP_IS_CLOSING(key)  (key->closing)
#else
#   define TOP_IS_CLOSING(key)  (0)
#endif


/*
 * ioqueue_dispatch_event()
 *
 * Report occurence of an event in the key to be processed by the
 * framework.
 */
void topque_dispatch_write_event(top_ioqueue_t *ioqueue, top_ioqueue_key_t *h)
{
    /* Lock the key. */
    g_static_rec_mutex_lock(&h->mutex);

    if (TOP_IS_CLOSING(h)) {
	g_static_rec_mutex_unlock(&h->mutex);
	return;
    }

#if defined(TOP_HAS_TCP) && TOP_HAS_TCP!=0
    if (h->connecting) {
	/* Completion of connect() operation */
	gint status;
	gboolean has_lock;

	/* Clear operation. */
	h->connecting = 0;

        ioqueue_remove_from_set(ioqueue, h, WRITEABLE_EVENT);
        ioqueue_remove_from_set(ioqueue, h, EXCEPTION_EVENT);


#if (defined(TOP_HAS_SO_ERROR) && TOP_HAS_SO_ERROR!=0)
	/* from connect(2):
	 * On Linux, use getsockopt to read the SO_ERROR option at
	 * level SOL_SOCKET to determine whether connect() completed
	 * successfully (if SO_ERROR is zero).
	 */
	{
	  int value;
	  int vallen = sizeof(value);
	  int gs_rc = top_sock_getsockopt(h->fd, SOL_SOCKET, SO_ERROR,
					 &value, &vallen);
	  if (gs_rc != 0) {
	    /* Argh!! What to do now???
	     * Just indicate that the socket is connected. The
	     * application will get error as soon as it tries to use
	     * the socket to send/receive.
	     */
	      status = TOP_SUCCESS;
	  } else {
	      status = TOP_STATUS_FROM_OS(value);
	  }
 	}
#elif defined(TOP_WIN32) && TOP_WIN32!=0
	status = TOP_SUCCESS; /* success */
#else
	/* Excellent information in D.J. Bernstein page:
	 * http://cr.yp.to/docs/connect.html
	 *
	 * Seems like the most portable way of detecting connect()
	 * failure is to call getpeername(). If socket is connected,
	 * getpeername() will return 0. If the socket is not connected,
	 * it will return ENOTCONN, and read(fd, &ch, 1) will produce
	 * the right errno through error slippage. This is a combination
	 * of suggestions from Douglas C. Schmidt and Ken Keys.
	 */
	{
	    struct sockaddr_in addr;
	    int addrlen = sizeof(addr);

	    status = top_sock_getpeername(h->fd, (struct sockaddr*)&addr,
				         &addrlen);
	}
#endif

        /* Unlock; from this point we don't need to hold key's mutex
	 * (unless concurrency is disabled, which in this case we should
	 * hold the mutex while calling the callback) */
	if (h->allow_concurrent) {
	    /* concurrency may be changed while we're in the callback, so
	     * save it to a flag.
	     */
	    has_lock = TOP_FALSE;
	    g_static_rec_mutex_unlock(&h->mutex);
	} else {
	    has_lock = TOP_TRUE;
	}

	/* Call callback. */
        if (h->cb.on_connect_complete && !TOP_IS_CLOSING(h))
	    (*h->cb.on_connect_complete)(h, status);

	/* Unlock if we still hold the lock */
	if (has_lock) {
	    g_static_rec_mutex_unlock(&h->mutex);
	}

        /* Done. */

    } else
#endif /* TOP_HAS_TCP */
    if (top_key_has_pending_write(h)) {
	/* Socket is writable. */
        struct write_operation *write_op;
        gssize sent;
        gint send_rc;

        /* Get the first in the queue. */
        write_op = h->write_list.next;

        /* For datagrams, we can remove the write_op from the list
         * so that send() can work in parallel.
         */
        if (h->fd_type == top_SOCK_DGRAM()) {
            top_list_erase(write_op);

            if (top_list_empty(&h->write_list))
                ioqueue_remove_from_set(ioqueue, h, WRITEABLE_EVENT);

        }

        /* Send the data.
         * Unfortunately we must do this while holding key's mutex, thus
         * preventing parallel write on a single key.. :-((
         */
        sent = write_op->size - write_op->written;
        if (write_op->op == TOP_IOQUEUE_OP_SEND) {
            send_rc = top_sock_send(h->fd, write_op->buf+write_op->written,
                                   &sent, write_op->flags);
	    /* Can't do this. We only clear "op" after we're finished sending
	     * the whole buffer.
	     */
	    //write_op->op = 0;
        } else if (write_op->op == TOP_IOQUEUE_OP_SEND_TO) {
            //g_message("topque_dispatch_write_event");
            send_rc = top_sock_sendto(h->fd,
                                     write_op->buf+write_op->written,
                                     &sent, write_op->flags,
                                     &write_op->rmt_addr,
                                     write_op->rmt_addrlen);
	    /* Can't do this. We only clear "op" after we're finished sending
	     * the whole buffer.
	     */
	    //write_op->op = 0;
        } else {
            //top_assert(!"Invalid operation type!");
	    write_op->op = TOP_IOQUEUE_OP_NONE;
            send_rc = TOP_EBUG;
        }

        if (send_rc == TOP_SUCCESS) {
            write_op->written += sent;
        } else {
            g_assert(send_rc > 0);
            write_op->written = -send_rc;
        }

        /* Are we finished with this buffer? */
        if (send_rc!=TOP_SUCCESS ||
            write_op->written == (gssize)write_op->size ||
            h->fd_type == top_SOCK_DGRAM())
        {
	    gboolean has_lock;

	    write_op->op = TOP_IOQUEUE_OP_NONE;

            if (h->fd_type != top_SOCK_DGRAM()) {
                /* Write completion of the whole stream. */
                top_list_erase(write_op);

                /* Clear operation if there's no more data to send. */
                if (top_list_empty(&h->write_list))
                    ioqueue_remove_from_set(ioqueue, h, WRITEABLE_EVENT);

            }

	    /* Unlock; from this point we don't need to hold key's mutex
	     * (unless concurrency is disabled, which in this case we should
	     * hold the mutex while calling the callback) */
	    if (h->allow_concurrent) {
		/* concurrency may be changed while we're in the callback, so
		 * save it to a flag.
		 */
		has_lock = TOP_FALSE;
		g_static_rec_mutex_unlock(&h->mutex);
	    } else {
		has_lock = TOP_TRUE;
	    }

	    /* Call callback. */
            if (h->cb.on_write_complete && !TOP_IS_CLOSING(h)) {
	        (*h->cb.on_write_complete)(h,
                                           (top_ioqueue_op_key_t*)write_op,
                                           write_op->written);
            }

	    if (has_lock) {
		g_static_rec_mutex_unlock(&h->mutex);
	    }

        } else {
            g_static_rec_mutex_unlock(&h->mutex);
        }

        /* Done. */
    } else {
        /*
         * This is normal; execution may fall here when multiple threads
         * are signalled for the same event, but only one thread eventually
         * able to process the event.
         */
        g_static_rec_mutex_unlock(&h->mutex);
    }
}

void topque_dispatch_read_event( top_ioqueue_t *ioqueue, top_ioqueue_key_t *h )
{
    gint rc;

    /* Lock the key. */
    g_static_rec_mutex_lock(&h->mutex);

    if (TOP_IS_CLOSING(h)) {
	g_static_rec_mutex_unlock(&h->mutex);
	return;
    }
    //g_message("topque_dispatch_read_event socket is %d",h->fd);
#   if TOP_HAS_TCP
    if (!top_list_empty(&h->accept_list)) {
        //g_message("Accept");
        struct accept_operation *accept_op;
	gboolean has_lock;

        /* Get one accept operation from the list. */
	accept_op = h->accept_list.next;
        top_list_erase(accept_op);
        accept_op->op = TOP_IOQUEUE_OP_NONE;

	/* Clear bit in fdset if there is no more pending accept */
        if (top_list_empty(&h->accept_list))
            ioqueue_remove_from_set(ioqueue, h, READABLE_EVENT);

	rc=top_sock_accept(h->fd, accept_op->accept_fd,
                          accept_op->rmt_addr, accept_op->addrlen);
	if (rc==TOP_SUCCESS && accept_op->local_addr) {
	    rc = top_sock_getsockname(*accept_op->accept_fd,
                                     accept_op->local_addr,
				     accept_op->addrlen);
	}

	/* Unlock; from this point we don't need to hold key's mutex
	 * (unless concurrency is disabled, which in this case we should
	 * hold the mutex while calling the callback) */
	if (h->allow_concurrent) {
	    /* concurrency may be changed while we're in the callback, so
	     * save it to a flag.
	     */
	    has_lock = TOP_FALSE;
	    g_static_rec_mutex_unlock(&h->mutex);
	} else {
	    has_lock = TOP_TRUE;
	}

	/* Call callback. */
        if (h->cb.on_accept_complete && !TOP_IS_CLOSING(h)) {
	    (*h->cb.on_accept_complete)(h,
                                        (top_ioqueue_op_key_t*)accept_op,
                                        *accept_op->accept_fd, rc);
	}

	if (has_lock) {
	    g_static_rec_mutex_unlock(&h->mutex);
	}
    }
    else
#   endif
    if (top_key_has_pending_read(h)) {
        //g_message("top_key_has_pending_read");
        struct read_operation *read_op;
        gssize bytes_read;
	gboolean has_lock;

        /* Get one pending read operation from the list. */
        read_op = h->read_list.next;
        top_list_erase(read_op);

        /* Clear fdset if there is no pending read. */
        if (top_list_empty(&h->read_list))
            ioqueue_remove_from_set(ioqueue, h, READABLE_EVENT);

        bytes_read = read_op->size;

	if ((read_op->op == TOP_IOQUEUE_OP_RECV_FROM)) {
	    read_op->op = TOP_IOQUEUE_OP_NONE;
	    rc = top_sock_recvfrom(h->fd, read_op->buf, &bytes_read,
				  read_op->flags,
				  read_op->rmt_addr,
                                  read_op->rmt_addrlen);
	} else if ((read_op->op == TOP_IOQUEUE_OP_RECV)) {
	    read_op->op = TOP_IOQUEUE_OP_NONE;
	    rc = top_sock_recv(h->fd, read_op->buf, &bytes_read,
			      read_op->flags);
        //g_message("TCP recv len --> %d rc-->%d",bytes_read,rc);
        } else {
            g_assert(read_op->op == TOP_IOQUEUE_OP_READ);
	    read_op->op = TOP_IOQUEUE_OP_NONE;
            /*
             * User has specified top_ioqueue_read().
             * On Win32, we should do ReadFile(). But because we got
             * here because of select() anyway, user must have put a
             * socket descriptor on h->fd, which in this case we can
             * just call top_sock_recv() instead of ReadFile().
             * On Unix, user may put a file in h->fd, so we'll have
             * to call read() here.
             * This may not compile on systems which doesn't have
             * read(). That's why we only specify TOP_LINUX here so
             * that error is easier to catch.
             */
#	    if defined(TOP_WIN32) && TOP_WIN32 != 0 || \
	       defined(TOP_WIN32_WINCE) && TOP_WIN32_WINCE != 0
                rc = top_sock_recv(h->fd, read_op->buf, &bytes_read,
				  read_op->flags);
                //rc = ReadFile((HANDLE)h->fd, read_op->buf, read_op->size,
                //              &bytes_read, NULL);
#           else
                bytes_read = read(h->fd, read_op->buf, bytes_read);
                rc = (bytes_read >= 0) ? TOP_SUCCESS : top_get_os_error();
#           endif
        }

	if (rc != TOP_SUCCESS) {
#	    if defined(TOP_WIN32) && TOP_WIN32 != 0
	    /* On Win32, for UDP, WSAECONNRESET on the receive side
	     * indicates that previous sending has triggered ICMP Port
	     * Unreachable message.
	     * But we wouldn't know at this point which one of previous
	     * key that has triggered the error, since UDP socket can
	     * be shared!
	     * So we'll just ignore it!
	     */

	    if (rc == TOP_STATUS_FROM_OS(WSAECONNRESET)) {
		//TOP_LOG(4,(THIS_FILE,
                //          "Ignored ICMP port unreach. on key=%p", h));
	    }
#	    endif

            /* In any case we would report this to caller. */
            bytes_read = -rc;

#if defined(TOP_IPHONE_OS_HAS_MULTITASKING_SUPPORT) && \
    TOP_IPHONE_OS_HAS_MULTITASKING_SUPPORT!=0
	    /* Special treatment for dead UDP sockets here, see ticket #1107 */
	    if (rc == TOP_STATUS_FROM_OS(ENOTCONN) && !TOP_IS_CLOSING(h) &&
		h->fd_type==top_SOCK_DGRAM())
	    {
		replace_udp_sock(h);
	    }
#endif
	}

	/* Unlock; from this point we don't need to hold key's mutex
	 * (unless concurrency is disabled, which in this case we should
	 * hold the mutex while calling the callback) */
	if (h->allow_concurrent) {
	    /* concurrency may be changed while we're in the callback, so
	     * save it to a flag.
	     */
	    has_lock = TOP_FALSE;
	    g_static_rec_mutex_unlock(&h->mutex);
	} else {
	    has_lock = TOP_TRUE;
	}

	/* Call callback. */
        if (h->cb.on_read_complete && !TOP_IS_CLOSING(h)) {
	    (*h->cb.on_read_complete)(h,
                                      (top_ioqueue_op_key_t*)read_op,
                                      bytes_read);
        }

	if (has_lock) {
	    g_static_rec_mutex_unlock(&h->mutex);
	}

    } else {
        /*
         * This is normal; execution may fall here when multiple threads
         * are signalled for the same event, but only one thread eventually
         * able to process the event.
         */
        g_static_rec_mutex_unlock(&h->mutex);
    }
}


void topque_dispatch_exception_event( top_ioqueue_t *ioqueue,
                                       top_ioqueue_key_t *h )
{
    gboolean has_lock;

    g_static_rec_mutex_lock(&h->mutex);

    if (!h->connecting) {
        /* It is possible that more than one thread was woken up, thus
         * the remaining thread will see h->connecting as zero because
         * it has been processed by other thread.
         */
        g_static_rec_mutex_unlock(&h->mutex);
        return;
    }

    if (TOP_IS_CLOSING(h)) {
	g_static_rec_mutex_unlock(&h->mutex);
	return;
    }

    /* Clear operation. */
    h->connecting = 0;

    ioqueue_remove_from_set(ioqueue, h, WRITEABLE_EVENT);
    ioqueue_remove_from_set(ioqueue, h, EXCEPTION_EVENT);

    /* Unlock; from this point we don't need to hold key's mutex
     * (unless concurrency is disabled, which in this case we should
     * hold the mutex while calling the callback) */
    if (h->allow_concurrent) {
	/* concurrency may be changed while we're in the callback, so
	 * save it to a flag.
	 */
	has_lock = TOP_FALSE;
	g_static_rec_mutex_unlock(&h->mutex);
    } else {
	has_lock = TOP_TRUE;
    }

    /* Call callback. */
    if (h->cb.on_connect_complete && !TOP_IS_CLOSING(h)) {
	gint status = -1;
#if (defined(TOP_HAS_SO_ERROR) && TOP_HAS_SO_ERROR!=0)
	int value;
	int vallen = sizeof(value);
	int gs_rc = top_sock_getsockopt(h->fd, SOL_SOCKET, SO_ERROR,
				       &value, &vallen);
	if (gs_rc == 0) {
	    status = TOP_RETURN_OS_ERROR(value);
	}
#endif

	(*h->cb.on_connect_complete)(h, status);
    }

    if (has_lock) {
	g_static_rec_mutex_unlock(&h->mutex);
    }
}

/*
 * top_ioqueue_recv()
 *
 * Start asynchronous recv() from the socket.
 */
gint top_ioqueue_recv(  top_ioqueue_key_t *key,
                                      top_ioqueue_op_key_t *op_key,
				      void *buffer,
				      gssize *length,
				      unsigned flags )
{
    struct read_operation *read_op;

    TOP_ASSERT_RETURN(key && op_key && buffer && length, TOP_EINVAL);

    /* Check if key is closing (need to do this first before accessing
     * other variables, since they might have been destroyed. See ticket
     * #469).
     */
    if (TOP_IS_CLOSING(key))
        return TOP_ECANCELLED;

    read_op = (struct read_operation*)op_key;
    read_op->op = TOP_IOQUEUE_OP_NONE;

    /* Try to see if there's data immediately available.
     */
    if ((flags & TOP_IOQUEUE_ALWAYS_ASYNC) == 0) {
    	gint status;
    	gssize size;

    	size = *length;
    	status = top_sock_recv(key->fd, buffer, &size, flags);
    	if (status == TOP_SUCCESS) {
    	    /* Yes! Data is available! */
    	    *length = size;
    	    return TOP_SUCCESS;
    	} else {
    	    /* If error is not EWOULDBLOCK (or EAGAIN on Linux), report
    	     * the error to caller.
    	     */
    	    if (status != TOP_STATUS_FROM_OS(TOP_BLOCKING_ERROR_VAL))
                return status;
    	}
    }

    flags &= ~(TOP_IOQUEUE_ALWAYS_ASYNC);

    /*
     * No data is immediately available.
     * Must schedule asynchronous operation to the ioqueue.
     */
    read_op->op = TOP_IOQUEUE_OP_RECV;
    read_op->buf = buffer;
    read_op->size = *length;
    read_op->flags = flags;

    g_static_rec_mutex_lock(&key->mutex);
    /* Check again. Handle may have been closed after the previous check
     * in multithreaded app. If we add bad handle to the set it will
     * corrupt the ioqueue set. See #913
     */
    if (TOP_IS_CLOSING(key)) {
    	g_static_rec_mutex_unlock(&key->mutex);
    	return TOP_ECANCELLED;
    }
    top_list_insert_before(&key->read_list, read_op);
    ioqueue_add_to_set(key->ioqueue, key, READABLE_EVENT);
    g_static_rec_mutex_unlock(&key->mutex);

    return TOP_EPENDING;
}

/*
 * top_ioqueue_recvfrom()
 *
 * Start asynchronous recvfrom() from the socket.
 */
gint top_ioqueue_recvfrom( top_ioqueue_key_t *key,
                                         top_ioqueue_op_key_t *op_key,
				         void *buffer,
				         gssize *length,
                                         unsigned flags,
				         top_sockaddr_t *addr,
				         int *addrlen)
{
    struct read_operation *read_op;

    TOP_ASSERT_RETURN(key && op_key && buffer && length, TOP_EINVAL);

    /* Check if key is closing. */
    if (TOP_IS_CLOSING(key))
	return TOP_ECANCELLED;

    read_op = (struct read_operation*)op_key;
    read_op->op = TOP_IOQUEUE_OP_NONE;

    /* Try to see if there's data immediately available.
     */
    if ((flags & TOP_IOQUEUE_ALWAYS_ASYNC) == 0) {
	gint status;
	gssize size;

	size = *length;
    //g_message("top_sock_recvfrom call");
	status = top_sock_recvfrom(key->fd, buffer, &size, flags,
				  addr, addrlen);
    //g_message("top_sock_recvfrom return");
	if (status == TOP_SUCCESS) {
	    /* Yes! Data is available! */
	    *length = size;
	    return TOP_SUCCESS;
	} else {
	    /* If error is not EWOULDBLOCK (or EAGAIN on Linux), report
	     * the error to caller.
	     */
	    if (status != TOP_STATUS_FROM_OS(TOP_BLOCKING_ERROR_VAL))
		return status;
	}
    }
    //g_message("No data is immediately available");
    flags &= ~(TOP_IOQUEUE_ALWAYS_ASYNC);

    /*
     * No data is immediately available.
     * Must schedule asynchronous operation to the ioqueue.
     */
    read_op->op = TOP_IOQUEUE_OP_RECV_FROM;
    read_op->buf = buffer;
    read_op->size = *length;
    read_op->flags = flags;
    read_op->rmt_addr = addr;
    read_op->rmt_addrlen = addrlen;

    g_static_rec_mutex_lock(&key->mutex);
    /* Check again. Handle may have been closed after the previous check
     * in multithreaded app. If we add bad handle to the set it will
     * corrupt the ioqueue set. See #913
     */
    if (TOP_IS_CLOSING(key)) {
	g_static_rec_mutex_unlock(&key->mutex);
	return TOP_ECANCELLED;
    }
    top_list_insert_before(&key->read_list, read_op);
    ioqueue_add_to_set(key->ioqueue, key, READABLE_EVENT);
    g_static_rec_mutex_unlock(&key->mutex);

    return TOP_EPENDING;
}

/*
 * top_ioqueue_send()
 *
 * Start asynchronous send() to the descriptor.
 */
gint top_ioqueue_send( top_ioqueue_key_t *key,
                                     top_ioqueue_op_key_t *op_key,
			             const void *data,
			             gssize *length,
                                     unsigned flags)
{
    struct write_operation *write_op;
    gint status;
    unsigned retry;
    gssize sent;

    TOP_ASSERT_RETURN(key && op_key && data && length, TOP_EINVAL);

    /* Check if key is closing. */
    if (TOP_IS_CLOSING(key))
	return TOP_ECANCELLED;

    /* We can not use TOP_IOQUEUE_ALWAYS_ASYNC for socket write. */
    flags &= ~(TOP_IOQUEUE_ALWAYS_ASYNC);

    /* Fast track:
     *   Try to send data immediately, only if there's no pending write!
     * Note:
     *  We are speculating that the list is empty here without properly
     *  acquiring ioqueue's mutex first. This is intentional, to maximize
     *  performance via parallelism.
     *
     *  This should be safe, because:
     *      - by convention, we require caller to make sure that the
     *        key is not unregistered while other threads are invoking
     *        an operation on the same key.
     *      - top_list_empty() is safe to be invoked by multiple threads,
     *        even when other threads are modifying the list.
     */
    if (top_list_empty(&key->write_list)) {
        /*
         * See if data can be sent immediately.
         */
        sent = *length;
        status = top_sock_send(key->fd, data, &sent, flags);
        if (status == TOP_SUCCESS) {
            /* Success! */
            *length = sent;
            return TOP_SUCCESS;
        } else {
            /* If error is not EWOULDBLOCK (or EAGAIN on Linux), report
             * the error to caller.
             */
            if (status != TOP_STATUS_FROM_OS(TOP_BLOCKING_ERROR_VAL)) {
                return status;
            }
        }
    }

    /*
     * Schedule asynchronous send.
     */
    write_op = (struct write_operation*)op_key;

    /* Spin if write_op has pending operation */
    for (retry=0; write_op->op != 0 && retry<PENDING_RETRY; ++retry)
	g_usleep(0);

    /* Last chance */
    if (write_op->op) {
	/* Unable to send packet because there is already pending write in the
	 * write_op. We could not put the operation into the write_op
	 * because write_op already contains a pending operation! And
	 * we could not send the packet directly with send() either,
	 * because that will break the order of the packet. So we can
	 * only return error here.
	 *
	 * This could happen for example in multithreads program,
	 * where polling is done by one thread, while other threads are doing
	 * the sending only. If the polling thread runs on lower priority
	 * than the sending thread, then it's possible that the pending
	 * write flag is not cleared in-time because clearing is only done
	 * during polling.
	 *
	 * Aplication should specify multiple write operation keys on
	 * situation like this.
	 */
	//top_assert(!"ioqueue: there is pending operation on this key!");
	return TOP_EBUSY;
    }

    write_op->op = TOP_IOQUEUE_OP_SEND;
    write_op->buf = (char*)data;
    write_op->size = *length;
    write_op->written = 0;
    write_op->flags = flags;

    g_static_rec_mutex_lock(&key->mutex);
    /* Check again. Handle may have been closed after the previous check
     * in multithreaded app. If we add bad handle to the set it will
     * corrupt the ioqueue set. See #913
     */
    if (TOP_IS_CLOSING(key)) {
	g_static_rec_mutex_unlock(&key->mutex);
	return TOP_ECANCELLED;
    }
    top_list_insert_before(&key->write_list, write_op);
    ioqueue_add_to_set(key->ioqueue, key, WRITEABLE_EVENT);
    g_static_rec_mutex_unlock(&key->mutex);

    return TOP_EPENDING;
}


/*
 * top_ioqueue_sendto()
 *
 * Start asynchronous write() to the descriptor.
 */
gint top_ioqueue_sendto( top_ioqueue_key_t *key,
                                       top_ioqueue_op_key_t *op_key,
			               const void *data,
			               gssize *length,
                                       guint flags,
			               const top_sockaddr_t *addr,
			               int addrlen)
{
    struct write_operation *write_op;
    unsigned retry;
    gint status;
    gssize sent;

    TOP_ASSERT_RETURN(key && op_key && data && length, TOP_EINVAL);

    /* Check if key is closing. */
    if (TOP_IS_CLOSING(key))
	return TOP_ECANCELLED;

    /* We can not use TOP_IOQUEUE_ALWAYS_ASYNC for socket write */
    flags &= ~(TOP_IOQUEUE_ALWAYS_ASYNC);

    /* Fast track:
     *   Try to send data immediately, only if there's no pending write!
     * Note:
     *  We are speculating that the list is empty here without properly
     *  acquiring ioqueue's mutex first. This is intentional, to maximize
     *  performance via parallelism.
     *
     *  This should be safe, because:
     *      - by convention, we require caller to make sure that the
     *        key is not unregistered while other threads are invoking
     *        an operation on the same key.
     *      - top_list_empty() is safe to be invoked by multiple threads,
     *        even when other threads are modifying the list.
     */
    if (top_list_empty(&key->write_list)) {
        /*
         * See if data can be sent immediately.
         */
        sent = *length;
        //g_message("top_ioqueue_sendto");
        status = top_sock_sendto(key->fd, data, &sent, flags, addr, addrlen);
        if (status == TOP_SUCCESS) {
            /* Success! */
            *length = sent;
            return TOP_SUCCESS;
        } else {
            /* If error is not EWOULDBLOCK (or EAGAIN on Linux), report
             * the error to caller.
             */
            if (status != TOP_STATUS_FROM_OS(TOP_BLOCKING_ERROR_VAL)) {
                return status;
            }
	    status = status;
        }
    }

    /*
     * Check that address storage can hold the address parameter.
     */
    TOP_ASSERT_RETURN(addrlen <= (int)sizeof(top_sockaddr_in), TOP_EBUG);

    /*
     * Schedule asynchronous send.
     */
    write_op = (struct write_operation*)op_key;

    /* Spin if write_op has pending operation */
    for (retry=0; write_op->op != 0 && retry<PENDING_RETRY; ++retry)
	g_usleep(0);

    /* Last chance */
    if (write_op->op) {
	/* Unable to send packet because there is already pending write on the
	 * write_op. We could not put the operation into the write_op
	 * because write_op already contains a pending operation! And
	 * we could not send the packet directly with sendto() either,
	 * because that will break the order of the packet. So we can
	 * only return error here.
	 *
	 * This could happen for example in multithreads program,
	 * where polling is done by one thread, while other threads are doing
	 * the sending only. If the polling thread runs on lower priority
	 * than the sending thread, then it's possible that the pending
	 * write flag is not cleared in-time because clearing is only done
	 * during polling.
	 *
	 * Aplication should specify multiple write operation keys on
	 * situation like this.
	 */
	//top_assert(!"ioqueue: there is pending operation on this key!");
	return TOP_EBUSY;
    }

    write_op->op = TOP_IOQUEUE_OP_SEND_TO;
    write_op->buf = (char*)data;
    write_op->size = *length;
    write_op->written = 0;
    write_op->flags = flags;
    top_memcpy(&write_op->rmt_addr, addr, addrlen);
    write_op->rmt_addrlen = addrlen;

    g_static_rec_mutex_lock(&key->mutex);
    /* Check again. Handle may have been closed after the previous check
     * in multithreaded app. If we add bad handle to the set it will
     * corrupt the ioqueue set. See #913
     */
    if (TOP_IS_CLOSING(key)) {
	g_static_rec_mutex_unlock(&key->mutex);
	return TOP_ECANCELLED;
    }
    top_list_insert_before(&key->write_list, write_op);
    ioqueue_add_to_set(key->ioqueue, key, WRITEABLE_EVENT);
    g_static_rec_mutex_unlock(&key->mutex);

    return TOP_EPENDING;
}

#if TOP_HAS_TCP
/*
 * Initiate overlapped accept() operation.
 */
gint top_ioqueue_accept( top_ioqueue_key_t *key,
                                       top_ioqueue_op_key_t *op_key,
			               top_sock_t *new_sock,
			               top_sockaddr_t *local,
			               top_sockaddr_t *remote,
			               int *addrlen)
{
    struct accept_operation *accept_op;
    gint status;

    /* check parameters. All must be specified! */
    TOP_ASSERT_RETURN(key && op_key && new_sock, TOP_EINVAL);

    /* Check if key is closing. */
    if (TOP_IS_CLOSING(key))
	return TOP_ECANCELLED;

    accept_op = (struct accept_operation*)op_key;
    accept_op->op = TOP_IOQUEUE_OP_NONE;

    /* Fast track:
     *  See if there's new connection available immediately.
     */
    if (top_list_empty(&key->accept_list)) {
        status = top_sock_accept(key->fd, new_sock, remote, addrlen);
        if (status == TOP_SUCCESS) {
            /* Yes! New connection is available! */
            if (local && addrlen) {
                status = top_sock_getsockname(*new_sock, local, addrlen);
                if (status != TOP_SUCCESS) {
                    top_sock_close(*new_sock);
                    *new_sock = TOP_INVALID_SOCKET;
                    return status;
                }
            }
            return TOP_SUCCESS;
        } else {
            /* If error is not EWOULDBLOCK (or EAGAIN on Linux), report
             * the error to caller.
             */
            if (status != TOP_STATUS_FROM_OS(TOP_BLOCKING_ERROR_VAL)) {
                return status;
            }
        }
    }

    /*
     * No connection is available immediately.
     * Schedule accept() operation to be completed when there is incoming
     * connection available.
     */
    accept_op->op = TOP_IOQUEUE_OP_ACCEPT;
    accept_op->accept_fd = new_sock;
    accept_op->rmt_addr = remote;
    accept_op->addrlen= addrlen;
    accept_op->local_addr = local;

    g_static_rec_mutex_lock(&key->mutex);
    /* Check again. Handle may have been closed after the previous check
     * in multithreaded app. If we add bad handle to the set it will
     * corrupt the ioqueue set. See #913
     */
    if (TOP_IS_CLOSING(key)) {
	g_static_rec_mutex_unlock(&key->mutex);
	return TOP_ECANCELLED;
    }
    top_list_insert_before(&key->accept_list, accept_op);
    ioqueue_add_to_set(key->ioqueue, key, READABLE_EVENT);
    g_static_rec_mutex_unlock(&key->mutex);

    return TOP_EPENDING;
}

/*
 * Initiate overlapped connect() operation (well, it's non-blocking actually,
 * since there's no overlapped version of connect()).
 */
gint top_ioqueue_connect( top_ioqueue_key_t *key,
					const top_sockaddr_t *addr,
					int addrlen )
{
    gint status;

    /* check parameters. All must be specified! */
    TOP_ASSERT_RETURN(key && addr && addrlen, TOP_EINVAL);

    /* Check if key is closing. */
    if (TOP_IS_CLOSING(key))
	return TOP_ECANCELLED;

    /* Check if socket has not been marked for connecting */
    if (key->connecting != 0)
        return TOP_EPENDING;

    status = top_sock_connect(key->fd, addr, addrlen);
    if (status == TOP_SUCCESS) {
	/* Connected! */
	return TOP_SUCCESS;
    } else {
	if (status == TOP_STATUS_FROM_OS(TOP_BLOCKING_CONNECT_ERROR_VAL)) {
	    /* Pending! */
            g_static_rec_mutex_lock(&key->mutex);
	    /* Check again. Handle may have been closed after the previous
	     * check in multithreaded app. See #913
	     */
	    if (TOP_IS_CLOSING(key)) {
		g_static_rec_mutex_unlock(&key->mutex);
		return TOP_ECANCELLED;
	    }
	    key->connecting = TOP_TRUE;
            ioqueue_add_to_set(key->ioqueue, key, WRITEABLE_EVENT);
            ioqueue_add_to_set(key->ioqueue, key, EXCEPTION_EVENT);
            g_static_rec_mutex_unlock(&key->mutex);
	    return TOP_EPENDING;
	} else {
	    /* Error! */
	    return status;
	}
    }
}
#endif	/* TOP_HAS_TCP */


void top_ioqueue_op_key_init( top_ioqueue_op_key_t *op_key,
				     gsize size )
{
    memset(op_key,0, size);
}


/*
 * top_ioqueue_is_pending()
 */
gboolean top_ioqueue_is_pending( top_ioqueue_key_t *key,
                                         top_ioqueue_op_key_t *op_key )
{
    struct generic_operation *op_rec;


    op_rec = (struct generic_operation*)op_key;
    return op_rec->op != 0;
}


/*
 * top_ioqueue_post_completion()
 */
gint top_ioqueue_post_completion( top_ioqueue_key_t *key,
                                                top_ioqueue_op_key_t *op_key,
                                                gssize bytes_status )
{
    struct generic_operation *op_rec;

    /*
     * Find the operation key in all pending operation list to
     * really make sure that it's still there; then call the callback.
     */
    g_static_rec_mutex_lock(&key->mutex);

    /* Find the operation in the pending read list. */
    op_rec = (struct generic_operation*)key->read_list.next;
    while (op_rec != (void*)&key->read_list) {
        if (op_rec == (void*)op_key) {
            top_list_erase(op_rec);
            op_rec->op = TOP_IOQUEUE_OP_NONE;
            g_static_rec_mutex_unlock(&key->mutex);

            (*key->cb.on_read_complete)(key, op_key, bytes_status);
            return TOP_SUCCESS;
        }
        op_rec = op_rec->next;
    }

    /* Find the operation in the pending write list. */
    op_rec = (struct generic_operation*)key->write_list.next;
    while (op_rec != (void*)&key->write_list) {
        if (op_rec == (void*)op_key) {
            top_list_erase(op_rec);
            op_rec->op = TOP_IOQUEUE_OP_NONE;
            g_static_rec_mutex_unlock(&key->mutex);

            (*key->cb.on_write_complete)(key, op_key, bytes_status);
            return TOP_SUCCESS;
        }
        op_rec = op_rec->next;
    }

    /* Find the operation in the pending accept list. */
    op_rec = (struct generic_operation*)key->accept_list.next;
    while (op_rec != (void*)&key->accept_list) {
        if (op_rec == (void*)op_key) {
            top_list_erase(op_rec);
            op_rec->op = TOP_IOQUEUE_OP_NONE;
            g_static_rec_mutex_unlock(&key->mutex);

            (*key->cb.on_accept_complete)(key, op_key,
                                          TOP_INVALID_SOCKET,
                                          bytes_status);
            return TOP_SUCCESS;
        }
        op_rec = op_rec->next;
    }

    g_static_rec_mutex_unlock(&key->mutex);

    return TOP_EINVALIDOP;
}

gint top_ioqueue_set_default_concurrency( top_ioqueue_t *ioqueue,
							gboolean allow)
{
    TOP_ASSERT_RETURN(ioqueue != NULL, TOP_EINVAL);
    ioqueue->default_concurrency = allow;
    return TOP_SUCCESS;
}


gint top_ioqueue_set_concurrency(top_ioqueue_key_t *key,
					       gboolean allow)
{
    TOP_ASSERT_RETURN(key, TOP_EINVAL);

    /* TOP_IOQUEUE_HAS_SAFE_UNREG must be enabled if concurrency is
     * disabled.
     */
    TOP_ASSERT_RETURN(allow || TOP_IOQUEUE_HAS_SAFE_UNREG, TOP_EINVAL);

    key->allow_concurrent = allow;
    return TOP_SUCCESS;
}

gint top_ioqueue_lock_key(top_ioqueue_key_t *key)
{
    g_static_rec_mutex_lock(&key->mutex);
    return TOP_SUCCESS;
}

gint top_ioqueue_unlock_key(top_ioqueue_key_t *key)
{
    g_static_rec_mutex_unlock(&key->mutex);
    return TOP_SUCCESS;
}


