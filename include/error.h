#ifndef __TOP_ERRNO_H__
#define __TOP_ERRNO_H__

/**
 * @file errno.h
 * @brief PJLIB Error Subsystem
 */
#include "toptype.h"
G_BEGIN_DECLS
#if defined(TOP_WIN32) && TOP_WIN32 != 0 || \
    defined(TOP_WIN32_WINCE) && TOP_WIN32_WINCE != 0

    typedef unsigned long top_os_err_type;
#   define top_get_native_os_error()	    GetLastError()
#   define top_get_native_netos_error()	    WSAGetLastError()

#else
#   include <errno.h>
    typedef int top_os_err_type;
#   define top_get_native_os_error()	    (errno)
#   define top_get_native_netos_error()	    (errno)

#endif
/**
 * @defgroup top_errno Error Subsystem
 * @{
 *
 * The PJLIB Error Subsystem is a framework to unify all error codes
 * produced by all components into a single error space, and provide
 * uniform set of APIs to access them. With this framework, any error
 * codes are encoded as gint value. The framework is extensible,
 * application may register new error spaces to be recognized by
 * the framework.
 *
 * @section top_errno_retval Return Values
 *
 * All functions that returns @a gint returns @a TOP_SUCCESS if the
 * operation was completed successfully, or non-zero value to indicate 
 * error. If the error came from operating system, then the native error
 * code is translated/folded into PJLIB's error namespace by using
 * #TOP_STATUS_FROM_OS() macro. The function will do this automatically
 * before returning the error to caller.
 *
 * @section err_services Retrieving and Displaying Error Messages
 *
 * The framework provides the following APIs to retrieve and/or display
 * error messages:
 *
 *   - #top_strerror(): this is the base API to retrieve error string
 *      description for the specified gint error code.
 *
 *   - #TOP_PERROR() macro: use this macro similar to TOP_LOG to format
 *      an error message and display them to the log
 *
 *   - #top_perror(): this function is similar to TOP_PERROR() but unlike
 *      #TOP_PERROR(), this function will always be included in the
 *      link process. Due to this reason, prefer to use #TOP_PERROR()
 *      if the application is concerned about the executable size.
 *
 * Application MUST NOT pass native error codes (such as error code from
 * functions like GetLastError() or errno) to PJLIB functions expecting
 * @a gint.
 *
 * @section err_extending Extending the Error Space
 *
 * Application may register new error space to be recognized by the
 * framework by using #top_register_strerror(). Use the range started
 * from TOP_ERRNO_START_USER to avoid conflict with existing error
 * spaces.
 *
 */

/**
 * Guidelines on error message length.
 */
#define TOP_ERR_MSG_SIZE  80

/**
 * Buffer for title string of #TOP_PERROR().
 */
#ifndef TOP_PERROR_TITLE_BUF_SIZE
#   define TOP_PERROR_TITLE_BUF_SIZE	120
#endif


/**
 * Get the last platform error/status, folded into gint.
 * @return	OS dependent error code, folded into gint.
 * @remark	This function gets errno, or calls GetLastError() function and
 *		convert the code into gint with TOP_STATUS_FROM_OS. Do
 *		not call this for socket functions!
 * @see	top_get_netos_error()
 */
gint top_get_os_error(void);

/**
 * Get the last error from socket operations.
 * @return	Last socket error, folded into gint.
 */
gint top_get_netos_error(void);

/**
 * @hideinitializer
 * Return platform os error code folded into gint code. This is
 * the macro that is used throughout the library for all PJLIB's functions
 * that returns error from operating system. Application may override
 * this macro to reduce size (e.g. by defining it to always return 
 * #TOP_EUNKNOWN).
 *
 * Note:
 *  This macro MUST return non-zero value regardless whether zero is
 *  passed as the argument. The reason is to protect logic error when
 *  the operating system doesn't report error codes properly.
 *
 * @param os_code   Platform OS error code. This value may be evaluated
 *		    more than once.
 * @return	    The platform os error code folded into gint.
 */
#ifndef TOP_RETURN_OS_ERROR
#   define TOP_RETURN_OS_ERROR(os_code)   (os_code ? \
					    TOP_STATUS_FROM_OS(os_code) : -1)
#endif


/**
 * @hideinitializer
 * Fold a platform specific error into an gint code.
 *
 * @param e	The platform os error code.
 * @return	gint
 * @warning	Macro implementation; the syserr argument may be evaluated
 *		multiple times.
 */
#if TOP_NATIVE_ERR_POSITIVE
#   define TOP_STATUS_FROM_OS(e) (e == 0 ? TOP_SUCCESS : e + TOP_ERRNO_START_SYS)
#else
#   define TOP_STATUS_FROM_OS(e) (e == 0 ? TOP_SUCCESS : TOP_ERRNO_START_SYS - e)
#endif

/**
 * @hideinitializer
 * Fold an gint code back to the native platform defined error.
 *
 * @param e	The gint folded platform os error code.
 * @return	top_os_err_type
 * @warning	macro implementation; the statcode argument may be evaluated
 *		multiple times.  If the statcode was not created by 
 *		top_get_os_error or TOP_STATUS_FROM_OS, the results are undefined.
 */
#if TOP_NATIVE_ERR_POSITIVE
#   define TOP_STATUS_TO_OS(e) (e == 0 ? TOP_SUCCESS : e - TOP_ERRNO_START_SYS)
#else
#   define TOP_STATUS_TO_OS(e) (e == 0 ? TOP_SUCCESS : TOP_ERRNO_START_SYS - e)
#endif


/**
 * Use this macro to generate error message text for your error code,
 * so that they look uniformly as the rest of the libraries.
 *
 * @param code	The error code
 * @param msg	The error test.
 */
#ifndef TOP_BUILD_ERR
#   define TOP_BUILD_ERR(code,msg) { code, msg " (" #code ")" }
#endif


/**
 * @hideinitializer
 * Unknown error has been reported.
 */
#define TOP_EUNKNOWN	    (TOP_ERRNO_START_STATUS + 1)	/* 70001 */
/**
 * @hideinitializer
 * The operation is pending and will be completed later.
 */
#define TOP_EPENDING	    (TOP_ERRNO_START_STATUS + 2)	/* 70002 */
/**
 * @hideinitializer
 * Too many connecting sockets.
 */
#define TOP_ETOOMANYCONN	    (TOP_ERRNO_START_STATUS + 3)	/* 70003 */
/**
 * @hideinitializer
 * Invalid argument.
 */
#define TOP_EINVAL	    (TOP_ERRNO_START_STATUS + 4)	/* 70004 */
/**
 * @hideinitializer
 * Name too long (eg. hostname too long).
 */
#define TOP_ENAMETOOLONG	    (TOP_ERRNO_START_STATUS + 5)	/* 70005 */
/**
 * @hideinitializer
 * Not found.
 */
#define TOP_ENOTFOUND	    (TOP_ERRNO_START_STATUS + 6)	/* 70006 */
/**
 * @hideinitializer
 * Not enough memory.
 */
#define TOP_ENOMEM	    (TOP_ERRNO_START_STATUS + 7)	/* 70007 */
/**
 * @hideinitializer
 * Bug detected!
 */
#define TOP_EBUG             (TOP_ERRNO_START_STATUS + 8)	/* 70008 */
/**
 * @hideinitializer
 * Operation timed out.
 */
#define TOP_ETIMEDOUT        (TOP_ERRNO_START_STATUS + 9)	/* 70009 */
/**
 * @hideinitializer
 * Too many objects.
 */
#define TOP_ETOOMANY         (TOP_ERRNO_START_STATUS + 10)/* 70010 */
/**
 * @hideinitializer
 * Object is busy.
 */
#define TOP_EBUSY            (TOP_ERRNO_START_STATUS + 11)/* 70011 */
/**
 * @hideinitializer
 * The specified option is not supported.
 */
#define TOP_ENOTSUP	    (TOP_ERRNO_START_STATUS + 12)/* 70012 */
/**
 * @hideinitializer
 * Invalid operation.
 */
#define TOP_EINVALIDOP	    (TOP_ERRNO_START_STATUS + 13)/* 70013 */
/**
 * @hideinitializer
 * Operation is cancelled.
 */
#define TOP_ECANCELLED	    (TOP_ERRNO_START_STATUS + 14)/* 70014 */
/**
 * @hideinitializer
 * Object already exists.
 */
#define TOP_EEXISTS          (TOP_ERRNO_START_STATUS + 15)/* 70015 */
/**
 * @hideinitializer
 * End of file.
 */
#define TOP_EEOF		    (TOP_ERRNO_START_STATUS + 16)/* 70016 */
/**
 * @hideinitializer
 * Size is too big.
 */
#define TOP_ETOOBIG	    (TOP_ERRNO_START_STATUS + 17)/* 70017 */
/**
 * @hideinitializer
 * Error in gethostbyname(). This is a generic error returned when
 * gethostbyname() has returned an error.
 */
#define TOP_ERESOLVE	    (TOP_ERRNO_START_STATUS + 18)/* 70018 */
/**
 * @hideinitializer
 * Size is too small.
 */
#define TOP_ETOOSMALL	    (TOP_ERRNO_START_STATUS + 19)/* 70019 */
/**
 * @hideinitializer
 * Ignored
 */
#define TOP_EIGNORED	    (TOP_ERRNO_START_STATUS + 20)/* 70020 */
/**
 * @hideinitializer
 * IPv6 is not supported
 */
#define TOP_EIPV6NOTSUP	    (TOP_ERRNO_START_STATUS + 21)/* 70021 */
/**
 * @hideinitializer
 * Unsupported address family
 */
#define TOP_EAFNOTSUP	    (TOP_ERRNO_START_STATUS + 22)/* 70022 */

/** @} */   /* top_errnum */

/** @} */   /* top_errno */


/**
 * TOP_ERRNO_START is where PJLIB specific error values start.
 */
#define TOP_ERRNO_START		20000

/**
 * TOP_ERRNO_SPACE_SIZE is the maximum number of errors in one of 
 * the error/status range below.
 */
#define TOP_ERRNO_SPACE_SIZE	50000

/**
 * TOP_ERRNO_START_STATUS is where PJLIB specific status codes start.
 * Effectively the error in this class would be 70000 - 119000.
 */
#define TOP_ERRNO_START_STATUS	(TOP_ERRNO_START + TOP_ERRNO_SPACE_SIZE)

/**
 * TOP_ERRNO_START_SYS converts platform specific error codes into
 * gint values.
 * Effectively the error in this class would be 120000 - 169000.
 */
#define TOP_ERRNO_START_SYS	(TOP_ERRNO_START_STATUS + TOP_ERRNO_SPACE_SIZE)

/**
 * TOP_ERRNO_START_USER are reserved for applications that use error
 * codes along with PJLIB codes.
 * Effectively the error in this class would be 170000 - 219000.
 */
#define TOP_ERRNO_START_USER	(TOP_ERRNO_START_SYS + TOP_ERRNO_SPACE_SIZE)

G_END_DECLS
#endif	/* __TOP_ERRNO_H__ */


