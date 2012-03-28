#ifndef _TOP_SOCK_H
#define _TOP_SOCK_H
#include "toptype.h"


G_BEGIN_DECLS
/*
 * Define common errors.
 */
#if (defined(TOP_WIN32) && TOP_WIN32!=0)
#  include <winsock2.h>
#   include <ws2tcpip.h>

/* This value specifies the value set in errno by the OS when a non-blocking
 * socket recv() or send() can not return immediately.
 */
#define TOP_BLOCKING_ERROR_VAL       WSAEWOULDBLOCK

/* This value specifies the value set in errno by the OS when a non-blocking
 * socket connect() can not get connected immediately.
 */
#define TOP_BLOCKING_CONNECT_ERROR_VAL   WSAEWOULDBLOCK

#  define OSERR_EWOULDBLOCK    WSAEWOULDBLOCK
#  define OSERR_EINPROGRESS    WSAEINPROGRESS
#  define OSERR_ECONNRESET     WSAECONNRESET
#  define OSERR_ENOTCONN       WSAENOTCONN
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <asm/ioctls.h>		/* FIONBIO	*/
/* This value specifies the value set in errno by the OS when a non-blocking
 * socket recv() can not return immediate daata.
 */
#define TOP_BLOCKING_ERROR_VAL       EAGAIN

/* This value specifies the value set in errno by the OS when a non-blocking
 * socket connect() can not get connected immediately.
 */
#define TOP_BLOCKING_CONNECT_ERROR_VAL   EINPROGRESS
#  define OSERR_EWOULDBLOCK    EWOULDBLOCK
#  define OSERR_EINPROGRESS    EINPROGRESS
#  define OSERR_ECONNRESET     ECONNRESET
#  define OSERR_ENOTCONN       ENOTCONN
#endif
/**
 * Supported address families.
 * APPLICATION MUST USE THESE VALUES INSTEAD OF NORMAL AF_*, BECAUSE
 * THE LIBRARY WILL DO TRANSLATION TO THE NATIVE VALUE.
 */

/** Address family is unspecified. @see top_AF_UNSPEC() */
extern const gushort TOP_AF_UNSPEC;

/** Unix domain socket.	@see top_AF_UNIX() */
extern const gushort TOP_AF_UNIX;

/** POSIX name for AF_UNIX	*/
#define TOP_AF_LOCAL	 TOP_AF_UNIX;

/** Internet IP protocol. @see top_AF_INET() */
extern const gushort TOP_AF_INET;

/** IP version 6. @see top_AF_INET6() */
extern const gushort TOP_AF_INET6;

/** Packet family. @see top_AF_PACKET() */
extern const gushort TOP_AF_PACKET;

/** IRDA sockets. @see top_AF_IRDA() */
extern const gushort TOP_AF_IRDA;

/*
 * Accessor functions for various address family constants. These
 * functions are provided because Symbian doesn't allow exporting
 * global variables from a DLL.
 */

#if defined(TOP_DLL)
    /** Get #TOP_AF_UNSPEC value */
    TOP_DECL(gushort) top_AF_UNSPEC(void);
    /** Get #TOP_AF_UNIX value. */
    TOP_DECL(gushort) top_AF_UNIX(void);
    /** Get #TOP_AF_INET value. */
    TOP_DECL(gushort) top_AF_INET(void);
    /** Get #TOP_AF_INET6 value. */
    TOP_DECL(gushort) top_AF_INET6(void);
    /** Get #TOP_AF_PACKET value. */
    TOP_DECL(gushort) top_AF_PACKET(void);
    /** Get #TOP_AF_IRDA value. */
    TOP_DECL(gushort) top_AF_IRDA(void);
#else
    /* When pjlib is not built as DLL, these accessor functions are
     * simply a macro to get their constants
     */
    /** Get #TOP_AF_UNSPEC value */
#   define top_AF_UNSPEC()   TOP_AF_UNSPEC
    /** Get #TOP_AF_UNIX value. */
#   define top_AF_UNIX()	    TOP_AF_UNIX
    /** Get #TOP_AF_INET value. */
#   define top_AF_INET()	    TOP_AF_INET
    /** Get #TOP_AF_INET6 value. */
#   define top_AF_INET6()    TOP_AF_INET6
    /** Get #TOP_AF_PACKET value. */
#   define top_AF_PACKET()   TOP_AF_PACKET
    /** Get #TOP_AF_IRDA value. */
#   define top_AF_IRDA()	    TOP_AF_IRDA
#endif


/**
 * Supported types of sockets.
 * APPLICATION MUST USE THESE VALUES INSTEAD OF NORMAL SOCK_*, BECAUSE
 * THE LIBRARY WILL TRANSLATE THE VALUE TO THE NATIVE VALUE.
 */

/** Sequenced, reliable, connection-based byte streams.
 *  @see top_SOCK_STREAM() */
extern const gushort TOP_SOCK_STREAM;

/** Connectionless, unreliable datagrams of fixed maximum lengths.
 *  @see top_SOCK_DGRAM() */
extern const gushort TOP_SOCK_DGRAM;

/** Raw protocol interface. @see top_SOCK_RAW() */
extern const gushort TOP_SOCK_RAW;

/** Reliably-delivered messages.  @see top_SOCK_RDM() */
extern const gushort TOP_SOCK_RDM;


/*
 * Accessor functions for various constants. These functions are provided
 * because Symbian doesn't allow exporting global variables from a DLL.
 */

#if defined(TOP_DLL)
    /** Get #TOP_SOCK_STREAM constant */
    TOP_DECL(int) top_SOCK_STREAM(void);
    /** Get #TOP_SOCK_DGRAM constant */
    TOP_DECL(int) top_SOCK_DGRAM(void);
    /** Get #TOP_SOCK_RAW constant */
    TOP_DECL(int) top_SOCK_RAW(void);
    /** Get #TOP_SOCK_RDM constant */
    TOP_DECL(int) top_SOCK_RDM(void);
#else
    /** Get #TOP_SOCK_STREAM constant */
#   define top_SOCK_STREAM() TOP_SOCK_STREAM
    /** Get #TOP_SOCK_DGRAM constant */
#   define top_SOCK_DGRAM()  TOP_SOCK_DGRAM
    /** Get #TOP_SOCK_RAW constant */
#   define top_SOCK_RAW()    TOP_SOCK_RAW
    /** Get #TOP_SOCK_RDM constant */
#   define top_SOCK_RDM()    TOP_SOCK_RDM
#endif


/**
 * Socket level specified in #top_sock_setsockopt() or #top_sock_getsockopt().
 * APPLICATION MUST USE THESE VALUES INSTEAD OF NORMAL SOL_*, BECAUSE
 * THE LIBRARY WILL TRANSLATE THE VALUE TO THE NATIVE VALUE.
 */
/** Socket level. @see top_SOL_SOCKET() */
extern const gushort TOP_SOL_SOCKET;
/** IP level. @see top_SOL_IP() */
extern const gushort TOP_SOL_IP;
/** TCP level. @see top_SOL_TCP() */
extern const gushort TOP_SOL_TCP;
/** UDP level. @see top_SOL_UDP() */
extern const gushort TOP_SOL_UDP;
/** IP version 6. @see top_SOL_IPV6() */
extern const gushort TOP_SOL_IPV6;

/*
 * Accessor functions for various constants. These functions are provided
 * because Symbian doesn't allow exporting global variables from a DLL.
 */

#if defined(TOP_DLL)
    /** Get #TOP_SOL_SOCKET constant */
    TOP_DECL(gushort) top_SOL_SOCKET(void);
    /** Get #TOP_SOL_IP constant */
    TOP_DECL(gushort) top_SOL_IP(void);
    /** Get #TOP_SOL_TCP constant */
    TOP_DECL(gushort) top_SOL_TCP(void);
    /** Get #TOP_SOL_UDP constant */
    TOP_DECL(gushort) top_SOL_UDP(void);
    /** Get #TOP_SOL_IPV6 constant */
    TOP_DECL(gushort) top_SOL_IPV6(void);
#else
    /** Get #TOP_SOL_SOCKET constant */
#   define top_SOL_SOCKET()  TOP_SOL_SOCKET
    /** Get #TOP_SOL_IP constant */
#   define top_SOL_IP()	    TOP_SOL_IP
    /** Get #TOP_SOL_TCP constant */
#   define top_SOL_TCP()	    TOP_SOL_TCP
    /** Get #TOP_SOL_UDP constant */
#   define top_SOL_UDP()	    TOP_SOL_UDP
    /** Get #TOP_SOL_IPV6 constant */
#   define top_SOL_IPV6()    TOP_SOL_IPV6
#endif


/* IP_TOS
 *
 * Note:
 *  TOS CURRENTLY DOES NOT WORK IN Windows 2000 and above!
 *  See http://support.microsoft.com/kb/248611
 */
/** IP_TOS optname in setsockopt(). @see top_IP_TOS() */
extern const gushort TOP_IP_TOS;

/*
 * IP TOS related constats.
 *
 * Note:
 *  TOS CURRENTLY DOES NOT WORK IN Windows 2000 and above!
 *  See http://support.microsoft.com/kb/248611
 */
/** Minimize delays. @see top_IPTOS_LOWDELAY() */
extern const gushort TOP_IPTOS_LOWDELAY;

/** Optimize throughput. @see top_IPTOS_THROUGHPUT() */
extern const gushort TOP_IPTOS_THROUGHPUT;

/** Optimize for reliability. @see top_IPTOS_RELIABILITY() */
extern const gushort TOP_IPTOS_RELIABILITY;

/** "filler data" where slow transmission does't matter.
 *  @see top_IPTOS_MINCOST() */
extern const gushort TOP_IPTOS_MINCOST;


#if defined(TOP_DLL)
    /** Get #TOP_IP_TOS constant */
    TOP_DECL(int) top_IP_TOS(void);

    /** Get #TOP_IPTOS_LOWDELAY constant */
    TOP_DECL(int) top_IPTOS_LOWDELAY(void);

    /** Get #TOP_IPTOS_THROUGHPUT constant */
    TOP_DECL(int) top_IPTOS_THROUGHPUT(void);

    /** Get #TOP_IPTOS_RELIABILITY constant */
    TOP_DECL(int) top_IPTOS_RELIABILITY(void);

    /** Get #TOP_IPTOS_MINCOST constant */
    TOP_DECL(int) top_IPTOS_MINCOST(void);
#else
    /** Get #TOP_IP_TOS constant */
#   define top_IP_TOS()		TOP_IP_TOS

    /** Get #TOP_IPTOS_LOWDELAY constant */
#   define top_IPTOS_LOWDELAY()	TOP_IP_TOS_LOWDELAY

    /** Get #TOP_IPTOS_THROUGHPUT constant */
#   define top_IPTOS_THROUGHPUT() TOP_IP_TOS_THROUGHPUT

    /** Get #TOP_IPTOS_RELIABILITY constant */
#   define top_IPTOS_RELIABILITY() TOP_IP_TOS_RELIABILITY

    /** Get #TOP_IPTOS_MINCOST constant */
#   define top_IPTOS_MINCOST()	TOP_IP_TOS_MINCOST
#endif


/**
 * Values to be specified as \c optname when calling #top_sock_setsockopt()
 * or #top_sock_getsockopt().
 */

/** Socket type. @see top_SO_TYPE() */
extern const gushort TOP_SO_TYPE;

/** Buffer size for receive. @see top_SO_RCVBUF() */
extern const gushort TOP_SO_RCVBUF;

/** Buffer size for send. @see top_SO_SNDBUF() */
extern const gushort TOP_SO_SNDBUF;

/** Disables the Nagle algorithm for send coalescing. @see top_TCP_NODELAY */
extern const gushort TOP_TCP_NODELAY;

/** Allows the socket to be bound to an address that is already in use.
 *  @see top_SO_REUSEADDR */
extern const gushort TOP_SO_REUSEADDR;

/** Set the protocol-defined priority for all packets to be sent on socket.
 */
extern const gushort TOP_SO_PRIORITY;

/** IP multicast interface. @see top_IP_MULTICAST_IF() */
extern const gushort TOP_IP_MULTICAST_IF;

/** IP multicast ttl. @see top_IP_MULTICAST_TTL() */
extern const gushort TOP_IP_MULTICAST_TTL;

/** IP multicast loopback. @see top_IP_MULTICAST_LOOP() */
extern const gushort TOP_IP_MULTICAST_LOOP;

/** Add an IP group membership. @see top_IP_ADD_MEMBERSHIP() */
extern const gushort TOP_IP_ADD_MEMBERSHIP;

/** Drop an IP group membership. @see top_IP_DROP_MEMBERSHIP() */
extern const gushort TOP_IP_DROP_MEMBERSHIP;


#if defined(TOP_DLL)
    /** Get #TOP_SO_TYPE constant */
    TOP_DECL(gushort) top_SO_TYPE(void);

    /** Get #TOP_SO_RCVBUF constant */
    TOP_DECL(gushort) top_SO_RCVBUF(void);

    /** Get #TOP_SO_SNDBUF constant */
    TOP_DECL(gushort) top_SO_SNDBUF(void);

    /** Get #TOP_TCP_NODELAY constant */
    TOP_DECL(gushort) top_TCP_NODELAY(void);

    /** Get #TOP_SO_REUSEADDR constant */
    TOP_DECL(gushort) top_SO_REUSEADDR(void);

    /** Get #TOP_SO_PRIORITY constant */
    TOP_DECL(gushort) top_SO_PRIORITY(void);

    /** Get #TOP_IP_MULTICAST_IF constant */
    TOP_DECL(gushort) top_IP_MULTICAST_IF(void);

    /** Get #TOP_IP_MULTICAST_TTL constant */
    TOP_DECL(gushort) top_IP_MULTICAST_TTL(void);

    /** Get #TOP_IP_MULTICAST_LOOP constant */
    TOP_DECL(gushort) top_IP_MULTICAST_LOOP(void);

    /** Get #TOP_IP_ADD_MEMBERSHIP constant */
    TOP_DECL(gushort) top_IP_ADD_MEMBERSHIP(void);

    /** Get #TOP_IP_DROP_MEMBERSHIP constant */
    TOP_DECL(gushort) top_IP_DROP_MEMBERSHIP(void);
#else
    /** Get #TOP_SO_TYPE constant */
#   define top_SO_TYPE()	    TOP_SO_TYPE

    /** Get #TOP_SO_RCVBUF constant */
#   define top_SO_RCVBUF()   TOP_SO_RCVBUF

    /** Get #TOP_SO_SNDBUF constant */
#   define top_SO_SNDBUF()   TOP_SO_SNDBUF

    /** Get #TOP_TCP_NODELAY constant */
#   define top_TCP_NODELAY() TOP_TCP_NODELAY

    /** Get #TOP_SO_REUSEADDR constant */
#   define top_SO_REUSEADDR() TOP_SO_REUSEADDR

    /** Get #TOP_SO_PRIORITY constant */
#   define top_SO_PRIORITY() TOP_SO_PRIORITY

    /** Get #TOP_IP_MULTICAST_IF constant */
#   define top_IP_MULTICAST_IF()    TOP_IP_MULTICAST_IF

    /** Get #TOP_IP_MULTICAST_TTL constant */
#   define top_IP_MULTICAST_TTL()   TOP_IP_MULTICAST_TTL

    /** Get #TOP_IP_MULTICAST_LOOP constant */
#   define top_IP_MULTICAST_LOOP()  TOP_IP_MULTICAST_LOOP

    /** Get #TOP_IP_ADD_MEMBERSHIP constant */
#   define top_IP_ADD_MEMBERSHIP()  TOP_IP_ADD_MEMBERSHIP

    /** Get #TOP_IP_DROP_MEMBERSHIP constant */
#   define top_IP_DROP_MEMBERSHIP() TOP_IP_DROP_MEMBERSHIP
#endif


/*
 * Flags to be specified in #top_sock_recv, #top_sock_send, etc.
 */

/** Out-of-band messages. @see top_MSG_OOB() */
extern const int TOP_MSG_OOB;

/** Peek, don't remove from buffer. @see top_MSG_PEEK() */
extern const int TOP_MSG_PEEK;

/** Don't route. @see top_MSG_DONTROUTE() */
extern const int TOP_MSG_DONTROUTE;


#if defined(TOP_DLL)
    /** Get #TOP_MSG_OOB constant */
    TOP_DECL(int) top_MSG_OOB(void);

    /** Get #TOP_MSG_PEEK constant */
    TOP_DECL(int) top_MSG_PEEK(void);

    /** Get #TOP_MSG_DONTROUTE constant */
    TOP_DECL(int) top_MSG_DONTROUTE(void);
#else
    /** Get #TOP_MSG_OOB constant */
#   define top_MSG_OOB()		TOP_MSG_OOB

    /** Get #TOP_MSG_PEEK constant */
#   define top_MSG_PEEK()	TOP_MSG_PEEK

    /** Get #TOP_MSG_DONTROUTE constant */
#   define top_MSG_DONTROUTE()	TOP_MSG_DONTROUTE
#endif


/**
 * Flag to be specified in #top_sock_shutdown().
 */
typedef enum top_socket_sd_type
{
    TOP_SD_RECEIVE   = 0,    /**< No more receive.	    */
    TOP_SHUT_RD	    = 0,    /**< Alias for SD_RECEIVE.	    */
    TOP_SD_SEND	    = 1,    /**< No more sending.	    */
    TOP_SHUT_WR	    = 1,    /**< Alias for SD_SEND.	    */
    TOP_SD_BOTH	    = 2,    /**< No more send and receive.  */
    TOP_SHUT_RDWR    = 2     /**< Alias for SD_BOTH.	    */
} top_socket_sd_type;



/** Address to accept any incoming messages. */
#define TOP_INADDR_ANY	    ((guint)0)

/** Address indicating an error return */
#define TOP_INADDR_NONE	    ((guint)0xffffffff)

/** Address to send to all hosts. */
#define TOP_INADDR_BROADCAST ((guint)0xffffffff)


/**
 * Maximum length specifiable by #top_sock_listen().
 * If the build system doesn't override this value, then the lowest
 * denominator (five, in Win32 systems) will be used.
 */
#if !defined(TOP_SOMAXCONN)
#  define TOP_SOMAXCONN	5
#endif


/**
 * Constant for invalid socket returned by #top_sock_socket() and
 * #top_sock_accept().
 */
#define TOP_INVALID_SOCKET   (-1)

/* Must undefine s_addr because of top_in_addr below */
#undef s_addr

/**
 * This structure describes Internet address.
 */
typedef struct top_in_addr
{
    guint	s_addr;		/**< The 32bit IP address.	    */
} top_in_addr;


/**
 * Maximum length of text representation of an IPv4 address.
 */
#define TOP_INET_ADDRSTRLEN	16

/**
 * Maximum length of text representation of an IPv6 address.
 */
#define TOP_INET6_ADDRSTRLEN	46


/**
 * This structure describes Internet socket address.
 * If TOP_SOCKADDR_HAS_LEN is not zero, then sin_zero_len member is added
 * to this struct. As far the application is concerned, the value of
 * this member will always be zero. Internally, TOPLIB may modify the value
 * before calling OS socket API, and reset the value back to zero before
 * returning the struct to application.
 */
typedef struct top_sockaddr_in
{
#if defined(TOP_SOCKADDR_HAS_LEN) && TOP_SOCKADDR_HAS_LEN!=0
    guchar  sin_zero_len;	/**< Just ignore this.		    */
    guchar  sin_family;	/**< Address family.		    */
#else
    gushort	sin_family;	/**< Address family.		    */
#endif
    gushort	sin_port;	/**< Transport layer port number.   */
    top_in_addr	sin_addr;	/**< IP address.		    */
    char	sin_zero[8];	/**< Padding.			    */
}top_sockaddr_in;


#undef s6_addr

/**
 * This structure describes IPv6 address.
 */
typedef union top_in6_addr
{
    /* This is the main entry */
    guchar  s6_addr[16];   /**< 8-bit array */

    /* While these are used for proper alignment */
    guint	u6_addr32[4];

    /* Do not use this with Winsock2, as this will align top_sockaddr_in6
     * to 64-bit boundary and Winsock2 doesn't like it!
     * Update 26/04/2010:
     *  This is now disabled, see http://trac.pjsip.org/repos/ticket/1058
     */
#if 0 && defined(TOP_HAS_INT64) && TOP_HAS_INT64!=0 && \
    (!defined(TOP_WIN32) || TOP_WIN32==0)
    top_int64_t	u6_addr64[2];
#endif

} top_in6_addr;


/** Initializer value for top_in6_addr. */
#define TOP_IN6ADDR_ANY_INIT { { { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } } }

/** Initializer value for top_in6_addr. */
#define TOP_IN6ADDR_LOOPBACK_INIT { { { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 } } }

/**
 * This structure describes IPv6 socket address.
 * If TOP_SOCKADDR_HAS_LEN is not zero, then sin_zero_len member is added
 * to this struct. As far the application is concerned, the value of
 * this member will always be zero. Internally, TOPLIB may modify the value
 * before calling OS socket API, and reset the value back to zero before
 * returning the struct to application.
 */
typedef struct top_sockaddr_in6
{
#if defined(TOP_SOCKADDR_HAS_LEN) && TOP_SOCKADDR_HAS_LEN!=0
    guchar  sin6_zero_len;	    /**< Just ignore this.	   */
    guchar  sin6_family;	    /**< Address family.	   */
#else
    gushort	sin6_family;	    /**< Address family		    */
#endif
    gushort	sin6_port;	    /**< Transport layer port number. */
    guint	sin6_flowinfo;	    /**< IPv6 flow information	    */
    top_in6_addr sin6_addr;	    /**< IPv6 address.		    */
    guint sin6_scope_id;	    /**< Set of interfaces for a scope	*/
} top_sockaddr_in6;


/**
 * This structure describes common attributes found in transport addresses.
 * If TOP_SOCKADDR_HAS_LEN is not zero, then sa_zero_len member is added
 * to this struct. As far the application is concerned, the value of
 * this member will always be zero. Internally, TOPLIB may modify the value
 * before calling OS socket API, and reset the value back to zero before
 * returning the struct to application.
 */
typedef struct top_addr_hdr
{
#if defined(TOP_SOCKADDR_HAS_LEN) && TOP_SOCKADDR_HAS_LEN!=0
    guchar  sa_zero_len;
    guchar  sa_family;
#else
    gushort	sa_family;	/**< Common data: address family.   */
#endif
} top_addr_hdr;


/**
 * This union describes a generic socket address.
 */
typedef union top_sockaddr
{
    top_addr_hdr	    addr;	/**< Generic transport address.	    */
    top_sockaddr_in  ipv4;	/**< IPv4 transport address.	    */
    top_sockaddr_in6 ipv6;	/**< IPv6 transport address.	    */
} top_sockaddr;


/**
 * This structure provides multicast group information for IPv4 addresses.
 */
typedef struct top_ip_mreq {
    top_in_addr imr_multiaddr;	/**< IP multicast address of group. */
    top_in_addr imr_interface;	/**< local IP address of interface. */
} top_ip_mreq;


/*****************************************************************************
 *
 * SOCKET ADDRESS MANIPULATION.
 *
 *****************************************************************************
 */

/**
 * Convert 16-bit value from network byte order to host byte order.
 *
 * @param netshort  16-bit network value.
 * @return	    16-bit host value.
 */
TOP_DECL(gushort) top_ntohs(gushort netshort);

/**
 * Convert 16-bit value from host byte order to network byte order.
 *
 * @param hostshort 16-bit host value.
 * @return	    16-bit network value.
 */
TOP_DECL(gushort) top_htons(gushort hostshort);

/**
 * Convert 32-bit value from network byte order to host byte order.
 *
 * @param netlong   32-bit network value.
 * @return	    32-bit host value.
 */
TOP_DECL(guint) top_ntohl(guint netlong);

/**
 * Convert 32-bit value from host byte order to network byte order.
 *
 * @param hostlong  32-bit host value.
 * @return	    32-bit network value.
 */
TOP_DECL(guint) top_htonl(guint hostlong);

/**
 * Convert an Internet host address given in network byte order
 * to string in standard numbers and dots notation.
 *
 * @param inaddr    The host address.
 * @return	    The string address.
 */
TOP_DECL(char*) top_inet_ntoa(top_in_addr inaddr);

/**
 * This function converts the Internet host address cp from the standard
 * numbers-and-dots notation into binary data and stores it in the structure
 * that inp points to.
 *
 * @param cp	IP address in standard numbers-and-dots notation.
 * @param inp	Structure that holds the output of the conversion.
 *
 * @return	nonzero if the address is valid, zero if not.
 */
TOP_DECL(int) top_inet_aton(const GString *cp, struct top_in_addr *inp);

/**
 * This function converts an address in its standard text presentation form
 * into its numeric binary form. It supports both IPv4 and IPv6 address
 * conversion.
 *
 * @param af	Specify the family of the address.  The TOP_AF_INET and
 *		TOP_AF_INET6 address families shall be supported.
 * @param src	Points to the string being passed in.
 * @param dst	Points to a buffer into which the function stores the
 *		numeric address; this shall be large enough to hold the
 *		numeric address (32 bits for TOP_AF_INET, 128 bits for
 *		TOP_AF_INET6).
 *
 * @return	TOP_SUCCESS if conversion was successful.
 */
TOP_DECL(gint) top_inet_pton(int af, const GString *src, void *dst);

/**
 * This function converts a numeric address into a text string suitable
 * for presentation. It supports both IPv4 and IPv6 address
 * conversion.
 * @see top_sockaddr_print()
 *
 * @param af	Specify the family of the address. This can be TOP_AF_INET
 *		or TOP_AF_INET6.
 * @param src	Points to a buffer holding an IPv4 address if the af argument
 *		is TOP_AF_INET, or an IPv6 address if the af argument is
 *		TOP_AF_INET6; the address must be in network byte order.
 * @param dst	Points to a buffer where the function stores the resulting
 *		text string; it shall not be NULL.
 * @param size	Specifies the size of this buffer, which shall be large
 *		enough to hold the text string (TOP_INET_ADDRSTRLEN characters
 *		for IPv4, TOP_INET6_ADDRSTRLEN characters for IPv6).
 *
 * @return	TOP_SUCCESS if conversion was successful.
 */
TOP_DECL(gint) top_inet_ntop(int af, const void *src,
				  char *dst, int size);

/**
 * Converts numeric address into its text string representation.
 * @see top_sockaddr_print()
 *
 * @param af	Specify the family of the address. This can be TOP_AF_INET
 *		or TOP_AF_INET6.
 * @param src	Points to a buffer holding an IPv4 address if the af argument
 *		is TOP_AF_INET, or an IPv6 address if the af argument is
 *		TOP_AF_INET6; the address must be in network byte order.
 * @param dst	Points to a buffer where the function stores the resulting
 *		text string; it shall not be NULL.
 * @param size	Specifies the size of this buffer, which shall be large
 *		enough to hold the text string (TOP_INET_ADDRSTRLEN characters
 *		for IPv4, TOP_INET6_ADDRSTRLEN characters for IPv6).
 *
 * @return	The address string or NULL if failed.
 */
TOP_DECL(char*) top_inet_ntop2(int af, const void *src,
			     char *dst, int size);

/**
 * Print socket address.
 *
 * @param addr	The socket address.
 * @param buf	Text buffer.
 * @param size	Size of buffer.
 * @param flags	Bitmask combination of these value:
 *		  - 1: port number is included.
 *		  - 2: square bracket is included for IPv6 address.
 *
 * @return	The address string.
 */
TOP_DECL(char*) top_sockaddr_print(const top_sockaddr_t *addr,
				 char *buf, int size,
				 unsigned flags);

/**
 * Convert address string with numbers and dots to binary IP address.
 *
 * @param cp	    The IP address in numbers and dots notation.
 * @return	    If success, the IP address is returned in network
 *		    byte order. If failed, TOP_INADDR_NONE will be
 *		    returned.
 * @remark
 * This is an obsolete interface to #top_inet_aton(); it is obsolete
 * because -1 is a valid address (255.255.255.255), and #top_inet_aton()
 * provides a cleaner way to indicate error return.
 */
TOP_DECL(top_in_addr) top_inet_addr(const GString *cp);

/**
 * Convert address string with numbers and dots to binary IP address.
 *
 * @param cp	    The IP address in numbers and dots notation.
 * @return	    If success, the IP address is returned in network
 *		    byte order. If failed, TOP_INADDR_NONE will be
 *		    returned.
 * @remark
 * This is an obsolete interface to #top_inet_aton(); it is obsolete
 * because -1 is a valid address (255.255.255.255), and #top_inet_aton()
 * provides a cleaner way to indicate error return.
 */
TOP_DECL(top_in_addr) top_inet_addr2(const char *cp);

/**
 * Initialize IPv4 socket address based on the address and port info.
 * The string address may be in a standard numbers and dots notation or
 * may be a hostname. If hostname is specified, then the function will
 * resolve the host into the IP address.
 *
 * @see top_sockaddr_init()
 *
 * @param addr	    The IP socket address to be set.
 * @param cp	    The address string, which can be in a standard
 *		    dotted numbers or a hostname to be resolved.
 * @param port	    The port number, in host byte order.
 *
 * @return	    Zero on success.
 */
TOP_DECL(gint) top_sockaddr_in_init( top_sockaddr_in *addr,
				          const gchar *cp,
					  gushort port);

/**
 * Initialize IP socket address based on the address and port info.
 * The string address may be in a standard numbers and dots notation or
 * may be a hostname. If hostname is specified, then the function will
 * resolve the host into the IP address.
 *
 * @see top_sockaddr_in_init()
 *
 * @param af	    Internet address family.
 * @param addr	    The IP socket address to be set.
 * @param cp	    The address string, which can be in a standard
 *		    dotted numbers or a hostname to be resolved.
 * @param port	    The port number, in host byte order.
 *
 * @return	    Zero on success.
 */
TOP_DECL(gint) top_sockaddr_init(int af,
				      top_sockaddr *addr,
				      const GString *cp,
				      gushort port);

/**
 * Compare two socket addresses.
 *
 * @param addr1	    First address.
 * @param addr2	    Second address.
 *
 * @return	    Zero on equal, -1 if addr1 is less than addr2,
 *		    and +1 if addr1 is more than addr2.
 */
TOP_DECL(int) top_sockaddr_cmp(const top_sockaddr_t *addr1,
			     const top_sockaddr_t *addr2);

/**
 * Get pointer to the address part of a socket address.
 *
 * @param addr	    Socket address.
 *
 * @return	    Pointer to address part (sin_addr or sin6_addr,
 *		    depending on address family)
 */
void* top_sockaddr_get_addr(const top_sockaddr_t *addr);

/**
 * Check that a socket address contains a non-zero address part.
 *
 * @param addr	    Socket address.
 *
 * @return	    Non-zero if address is set to non-zero.
 */
TOP_DECL(gboolean) top_sockaddr_has_addr(const top_sockaddr_t *addr);

/**
 * Get the address part length of a socket address, based on its address
 * family. For TOP_AF_INET, the length will be sizeof(top_in_addr), and
 * for TOP_AF_INET6, the length will be sizeof(top_in6_addr).
 *
 * @param addr	    Socket address.
 *
 * @return	    Length in bytes.
 */
TOP_DECL(unsigned) top_sockaddr_get_addr_len(const top_sockaddr_t *addr);

/**
 * Get the socket address length, based on its address
 * family. For TOP_AF_INET, the length will be sizeof(top_sockaddr_in), and
 * for TOP_AF_INET6, the length will be sizeof(top_sockaddr_in6).
 *
 * @param addr	    Socket address.
 *
 * @return	    Length in bytes.
 */
TOP_DECL(unsigned) top_sockaddr_get_len(const top_sockaddr_t *addr);

/**
 * Copy only the address part (sin_addr/sin6_addr) of a socket address.
 *
 * @param dst	    Destination socket address.
 * @param src	    Source socket address.
 *
 * @see @top_sockaddr_cp()
 */
void top_sockaddr_copy_addr(top_sockaddr *dst,
				    const top_sockaddr *src);
/**
 * Copy socket address. This will copy the whole structure depending
 * on the address family of the source socket address.
 *
 * @param dst	    Destination socket address.
 * @param src	    Source socket address.
 *
 * @see @top_sockaddr_copy_addr()
 */
void top_sockaddr_cp(top_sockaddr_t *dst, const top_sockaddr_t *src);

/**
 * Get the IP address of an IPv4 socket address.
 * The address is returned as 32bit value in host byte order.
 *
 * @param addr	    The IP socket address.
 * @return	    32bit address, in host byte order.
 */
TOP_DECL(top_in_addr) top_sockaddr_in_get_addr(const top_sockaddr_in *addr);

/**
 * Set the IP address of an IPv4 socket address.
 *
 * @param addr	    The IP socket address.
 * @param hostaddr  The host address, in host byte order.
 */
void top_sockaddr_in_set_addr(top_sockaddr_in *addr,
				      guint hostaddr);

/**
 * Set the IP address of an IP socket address from string address,
 * with resolving the host if necessary. The string address may be in a
 * standard numbers and dots notation or may be a hostname. If hostname
 * is specified, then the function will resolve the host into the IP
 * address.
 *
 * @see top_sockaddr_set_str_addr()
 *
 * @param addr	    The IP socket address to be set.
 * @param cp	    The address string, which can be in a standard
 *		    dotted numbers or a hostname to be resolved.
 *
 * @return	    TOP_SUCCESS on success.
 */
TOP_DECL(gint) top_sockaddr_in_set_str_addr( top_sockaddr_in *addr,
					          const GString *cp);

/**
 * Set the IP address of an IPv4 or IPv6 socket address from string address,
 * with resolving the host if necessary. The string address may be in a
 * standard IPv6 or IPv6 address or may be a hostname. If hostname
 * is specified, then the function will resolve the host into the IP
 * address according to the address family.
 *
 * @param af	    Address family.
 * @param addr	    The IP socket address to be set.
 * @param cp	    The address string, which can be in a standard
 *		    IP numbers (IPv4 or IPv6) or a hostname to be resolved.
 *
 * @return	    TOP_SUCCESS on success.
 */
TOP_DECL(gint) top_sockaddr_set_str_addr(int af,
					      top_sockaddr *addr,
					      const GString *cp);

/**
 * Get the port number of a socket address, in host byte order.
 * This function can be used for both IPv4 and IPv6 socket address.
 *
 * @param addr	    Socket address.
 *
 * @return	    Port number, in host byte order.
 */
TOP_DECL(gushort) top_sockaddr_get_port(const top_sockaddr_t *addr);

/**
 * Get the transport layer port number of an Internet socket address.
 * The port is returned in host byte order.
 *
 * @param addr	    The IP socket address.
 * @return	    Port number, in host byte order.
 */
TOP_DECL(gushort) top_sockaddr_in_get_port(const top_sockaddr_in *addr);

/**
 * Set the port number of an Internet socket address.
 *
 * @param addr	    The socket address.
 * @param hostport  The port number, in host byte order.
 */
TOP_DECL(gint) top_sockaddr_set_port(top_sockaddr *addr,
					  gushort hostport);

/**
 * Set the port number of an IPv4 socket address.
 *
 * @see top_sockaddr_set_port()
 *
 * @param addr	    The IP socket address.
 * @param hostport  The port number, in host byte order.
 */
void top_sockaddr_in_set_port(top_sockaddr_in *addr,
				      gushort hostport);

/**
 * Parse string containing IP address and optional port into socket address,
 * possibly also with address family detection. This function supports both
 * IPv4 and IPv6 parsing, however IPv6 parsing may only be done if IPv6 is
 * enabled during compilation.
 *
 * This function supports parsing several formats. Sample IPv4 inputs and
 * their default results::
 *  - "10.0.0.1:80": address 10.0.0.1 and port 80.
 *  - "10.0.0.1": address 10.0.0.1 and port zero.
 *  - "10.0.0.1:": address 10.0.0.1 and port zero.
 *  - "10.0.0.1:0": address 10.0.0.1 and port zero.
 *  - ":80": address 0.0.0.0 and port 80.
 *  - ":": address 0.0.0.0 and port 0.
 *  - "localhost": address 127.0.0.1 and port 0.
 *  - "localhost:": address 127.0.0.1 and port 0.
 *  - "localhost:80": address 127.0.0.1 and port 80.
 *
 * Sample IPv6 inputs and their default results:
 *  - "[fec0::01]:80": address fec0::01 and port 80
 *  - "[fec0::01]": address fec0::01 and port 0
 *  - "[fec0::01]:": address fec0::01 and port 0
 *  - "[fec0::01]:0": address fec0::01 and port 0
 *  - "fec0::01": address fec0::01 and port 0
 *  - "fec0::01:80": address fec0::01:80 and port 0
 *  - "::": address zero (::) and port 0
 *  - "[::]": address zero (::) and port 0
 *  - "[::]:": address zero (::) and port 0
 *  - ":::": address zero (::) and port 0
 *  - "[::]:80": address zero (::) and port 0
 *  - ":::80": address zero (::) and port 80
 *
 * Note: when the IPv6 socket address contains port number, the IP
 * part of the socket address should be enclosed with square brackets,
 * otherwise the port number will be included as part of the IP address
 * (see "fec0::01:80" example above).
 *
 * @param af	    Optionally specify the address family to be used. If the
 *		    address family is to be deducted from the input, specify
 *		    top_AF_UNSPEC() here. Other supported values are
 *		    #top_AF_INET() and #top_AF_INET6()
 * @param options   Additional options to assist the parsing, must be zero
 *		    for now.
 * @param str	    The input string to be parsed.
 * @param addr	    Pointer to store the result.
 *
 * @return	    TOP_SUCCESS if the parsing is successful.
 *
 * @see top_sockaddr_parse2()
 */
TOP_DECL(gint) top_sockaddr_parse(int af, unsigned options,
				       const GString *str,
				       top_sockaddr *addr);

/**
 * This function is similar to #top_sockaddr_parse(), except that it will not
 * convert the hostpart into IP address (thus possibly resolving the hostname
 * into a #top_sockaddr.
 *
 * Unlike #top_sockaddr_parse(), this function has a limitation that if port
 * number is specified in an IPv6 input string, the IP part of the IPv6 socket
 * address MUST be enclosed in square brackets, otherwise the port number will
 * be considered as part of the IPv6 IP address.
 *
 * @param af	    Optionally specify the address family to be used. If the
 *		    address family is to be deducted from the input, specify
 *		    #top_AF_UNSPEC() here. Other supported values are
 *		    #top_AF_INET() and #top_AF_INET6()
 * @param options   Additional options to assist the parsing, must be zero
 *		    for now.
 * @param str	    The input string to be parsed.
 * @param hostpart  Optional pointer to store the host part of the socket
 *		    address, with any brackets removed.
 * @param port	    Optional pointer to store the port number. If port number
 *		    is not found, this will be set to zero upon return.
 * @param raf	    Optional pointer to store the detected address family of
 *		    the input address.
 *
 * @return	    TOP_SUCCESS if the parsing is successful.
 *
 * @see top_sockaddr_parse()
 */
TOP_DECL(gint) top_sockaddr_parse2(int af, unsigned options,
				        const GString *str,
				        GString *hostpart,
				        gushort *port,
					int *raf);

/*****************************************************************************
 *
 * HOST NAME AND ADDRESS.
 *
 *****************************************************************************
 */

/**
 * Get system's host name.
 *
 * @return	    The hostname, or empty string if the hostname can not
 *		    be identified.
 */
TOP_DECL(const GString*) top_gethostname(void);

/**
 * Get host's IP address, which the the first IP address that is resolved
 * from the hostname.
 *
 * @return	    The host's IP address, TOP_INADDR_NONE if the host
 *		    IP address can not be identified.
 */
TOP_DECL(top_in_addr) top_gethostaddr(void);


/*****************************************************************************
 *
 * SOCKET API.
 *
 *****************************************************************************
 */
void top_init_sock();
/**
 * Create new socket/endpoint for communication.
 *
 * @param family    Specifies a communication domain; this selects the
 *		    protocol family which will be used for communication.
 * @param type	    The socket has the indicated type, which specifies the
 *		    communication semantics.
 * @param protocol  Specifies  a  particular  protocol  to  be used with the
 *		    socket.  Normally only a single protocol exists to support
 *		    a particular socket  type  within  a given protocol family,
 *		    in which a case protocol can be specified as 0.
 * @param sock	    New socket descriptor, or TOP_INVALID_SOCKET on error.
 *
 * @return	    Zero on success.
 */
TOP_DECL(gint) top_sock_socket(int family,
				    int type,
				    int protocol,
				    top_sock_t *sock);

/**
 * Close the socket descriptor.
 *
 * @param sockfd    The socket descriptor.
 *
 * @return	    Zero on success.
 */
TOP_DECL(gint) top_sock_close(top_sock_t sockfd);


/**
 * This function gives the socket sockfd the local address my_addr. my_addr is
 * addrlen bytes long.  Traditionally, this is called assigning a name to
 * a socket. When a socket is created with #top_sock_socket(), it exists in a
 * name space (address family) but has no name assigned.
 *
 * @param sockfd    The socket desriptor.
 * @param my_addr   The local address to bind the socket to.
 * @param addrlen   The length of the address.
 *
 * @return	    Zero on success.
 */
TOP_DECL(gint) top_sock_bind( top_sock_t sockfd,
				   const top_sockaddr_t *my_addr,
				   int addrlen);

/**
 * Bind the IP socket sockfd to the given address and port.
 *
 * @param sockfd    The socket descriptor.
 * @param addr	    Local address to bind the socket to, in host byte order.
 * @param port	    The local port to bind the socket to, in host byte order.
 *
 * @return	    Zero on success.
 */
TOP_DECL(gint) top_sock_bind_in( top_sock_t sockfd,
				      guint addr,
				      gushort port);

#if TOP_HAS_TCP
/**
 * Listen for incoming connection. This function only applies to connection
 * oriented sockets (such as TOP_SOCK_STREAM or TOP_SOCK_SEQPACKET), and it
 * indicates the willingness to accept incoming connections.
 *
 * @param sockfd	The socket descriptor.
 * @param backlog	Defines the maximum length the queue of pending
 *			connections may grow to.
 *
 * @return		Zero on success.
 */
TOP_DECL(gint) top_sock_listen( top_sock_t sockfd,
				     int backlog );

/**
 * Accept new connection on the specified connection oriented server socket.
 *
 * @param serverfd  The server socket.
 * @param newsock   New socket on success, of TOP_INVALID_SOCKET if failed.
 * @param addr	    A pointer to sockaddr type. If the argument is not NULL,
 *		    it will be filled by the address of connecting entity.
 * @param addrlen   Initially specifies the length of the address, and upon
 *		    return will be filled with the exact address length.
 *
 * @return	    Zero on success, or the error number.
 */
TOP_DECL(gint) top_sock_accept( top_sock_t serverfd,
				     top_sock_t *newsock,
				     top_sockaddr_t *addr,
				     int *addrlen);
#endif

/**
 * The file descriptor sockfd must refer to a socket.  If the socket is of
 * type TOP_SOCK_DGRAM  then the serv_addr address is the address to which
 * datagrams are sent by default, and the only address from which datagrams
 * are received. If the socket is of type TOP_SOCK_STREAM or TOP_SOCK_SEQPACKET,
 * this call attempts to make a connection to another socket.  The
 * other socket is specified by serv_addr, which is an address (of length
 * addrlen) in the communications space of the  socket.  Each  communications
 * space interprets the serv_addr parameter in its own way.
 *
 * @param sockfd	The socket descriptor.
 * @param serv_addr	Server address to connect to.
 * @param addrlen	The length of server address.
 *
 * @return		Zero on success.
 */
TOP_DECL(gint) top_sock_connect( top_sock_t sockfd,
				      const top_sockaddr_t *serv_addr,
				      int addrlen);

/**
 * Return the address of peer which is connected to socket sockfd.
 *
 * @param sockfd	The socket descriptor.
 * @param addr		Pointer to sockaddr structure to which the address
 *			will be returned.
 * @param namelen	Initially the length of the addr. Upon return the value
 *			will be set to the actual length of the address.
 *
 * @return		Zero on success.
 */
TOP_DECL(gint) top_sock_getpeername(top_sock_t sockfd,
					  top_sockaddr_t *addr,
					  int *namelen);

/**
 * Return the current name of the specified socket.
 *
 * @param sockfd	The socket descriptor.
 * @param addr		Pointer to sockaddr structure to which the address
 *			will be returned.
 * @param namelen	Initially the length of the addr. Upon return the value
 *			will be set to the actual length of the address.
 *
 * @return		Zero on success.
 */
TOP_DECL(gint) top_sock_getsockname( top_sock_t sockfd,
					  top_sockaddr_t *addr,
					  int *namelen);

/**
 * Get socket option associated with a socket. Options may exist at multiple
 * protocol levels; they are always present at the uppermost socket level.
 *
 * @param sockfd	The socket descriptor.
 * @param level		The level which to get the option from.
 * @param optname	The option name.
 * @param optval	Identifies the buffer which the value will be
 *			returned.
 * @param optlen	Initially contains the length of the buffer, upon
 *			return will be set to the actual size of the value.
 *
 * @return		Zero on success.
 */
TOP_DECL(gint) top_sock_getsockopt( top_sock_t sockfd,
					 gushort level,
					 gushort optname,
					 void *optval,
					 int *optlen);
/**
 * Manipulate the options associated with a socket. Options may exist at
 * multiple protocol levels; they are always present at the uppermost socket
 * level.
 *
 * @param sockfd	The socket descriptor.
 * @param level		The level which to get the option from.
 * @param optname	The option name.
 * @param optval	Identifies the buffer which contain the value.
 * @param optlen	The length of the value.
 *
 * @return		TOP_SUCCESS or the status code.
 */
TOP_DECL(gint) top_sock_setsockopt( top_sock_t sockfd,
					 gushort level,
					 gushort optname,
					 const void *optval,
					 int optlen);


/**
 * Receives data stream or message coming to the specified socket.
 *
 * @param sockfd	The socket descriptor.
 * @param buf		The buffer to receive the data or message.
 * @param len		On input, the length of the buffer. On return,
 *			contains the length of data received.
 * @param flags		Flags (such as top_MSG_PEEK()).
 *
 * @return		TOP_SUCCESS or the error code.
 */
TOP_DECL(gint) top_sock_recv(top_sock_t sockfd,
				  void *buf,
				  gssize *len,
				  unsigned flags);

/**
 * Receives data stream or message coming to the specified socket.
 *
 * @param sockfd	The socket descriptor.
 * @param buf		The buffer to receive the data or message.
 * @param len		On input, the length of the buffer. On return,
 *			contains the length of data received.
 * @param flags		Flags (such as top_MSG_PEEK()).
 * @param from		If not NULL, it will be filled with the source
 *			address of the connection.
 * @param fromlen	Initially contains the length of from address,
 *			and upon return will be filled with the actual
 *			length of the address.
 *
 * @return		TOP_SUCCESS or the error code.
 */
TOP_DECL(gint) top_sock_recvfrom( top_sock_t sockfd,
				      void *buf,
				      gssize *len,
				      unsigned flags,
				      top_sockaddr_t *from,
				      int *fromlen);

/**
 * Transmit data to the socket.
 *
 * @param sockfd	Socket descriptor.
 * @param buf		Buffer containing data to be sent.
 * @param len		On input, the length of the data in the buffer.
 *			Upon return, it will be filled with the length
 *			of data sent.
 * @param flags		Flags (such as top_MSG_DONTROUTE()).
 *
 * @return		TOP_SUCCESS or the status code.
 */
TOP_DECL(gint) top_sock_send(top_sock_t sockfd,
				  const void *buf,
				  gssize *len,
				  unsigned flags);

/**
 * Transmit data to the socket to the specified address.
 *
 * @param sockfd	Socket descriptor.
 * @param buf		Buffer containing data to be sent.
 * @param len		On input, the length of the data in the buffer.
 *			Upon return, it will be filled with the length
 *			of data sent.
 * @param flags		Flags (such as top_MSG_DONTROUTE()).
 * @param to		The address to send.
 * @param tolen		The length of the address in bytes.
 *
 * @return		TOP_SUCCESS or the status code.
 */
TOP_DECL(gint) top_sock_sendto(top_sock_t sockfd,
				    const void *buf,
				    gssize *len,
				    unsigned flags,
				    const top_sockaddr_t *to,
				    int tolen);

#if TOP_HAS_TCP
/**
 * The shutdown call causes all or part of a full-duplex connection on the
 * socket associated with sockfd to be shut down.
 *
 * @param sockfd	The socket descriptor.
 * @param how		If how is TOP_SHUT_RD, further receptions will be
 *			disallowed. If how is TOP_SHUT_WR, further transmissions
 *			will be disallowed. If how is TOP_SHUT_RDWR, further
 *			receptions andtransmissions will be disallowed.
 *
 * @return		Zero on success.
 */
TOP_DECL(gint) top_sock_shutdown( top_sock_t sockfd,
				       int how);
#endif

/**
 * @}
 */


G_END_DECLS

#endif
