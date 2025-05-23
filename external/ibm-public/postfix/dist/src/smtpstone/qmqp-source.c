/*	$NetBSD$	*/

/*++
/* NAME
/*	qmqp-source 1
/* SUMMARY
/*	multi-threaded QMQP test generator
/* SYNOPSIS
/* .fi
/*	\fBqmqp-source\fR [\fIoptions\fR] [\fBinet:\fR]\fIhost\fR[:\fIport\fR]
/*
/*	\fBqmqp-source\fR [\fIoptions\fR] \fBunix:\fIpathname\fR
/* DESCRIPTION
/*	\fBqmqp-source\fR connects to the named host and TCP port (default 628)
/*	and sends one or more messages to it, either sequentially
/*	or in parallel. The program speaks the QMQP protocol.
/*	Connections can be made to UNIX-domain and IPv4 or IPv6 servers.
/*	IPv4 and IPv6 are the default.
/*
/*	Note: this is an unsupported test program. No attempt is made
/*	to maintain compatibility between successive versions.
/*
/*	Arguments:
/* .IP \fB-4\fR
/*	Connect to the server with IPv4. This option has no effect when
/*	Postfix is built without IPv6 support.
/* .IP \fB-6\fR
/*	Connect to the server with IPv6. This option is not available when
/*	Postfix is built without IPv6 support.
/* .IP \fB-c\fR
/*	Display a running counter that is incremented each time
/*	a delivery completes.
/* .IP "\fB-C \fIcount\fR"
/*	When a host sends RESET instead of SYN|ACK, try \fIcount\fR times
/*	before giving up. The default count is 1. Specify a larger count in
/*	order to work around a problem with TCP/IP stacks that send RESET
/*	when the listen queue is full.
/* .IP "\fB-f \fIfrom\fR"
/*	Use the specified sender address (default: <foo@myhostname>).
/* .IP "\fB-l \fIlength\fR"
/*	Send \fIlength\fR bytes as message payload. The length
/*	includes the message headers.
/* .IP "\fB-m \fImessage_count\fR"
/*	Send the specified number of messages (default: 1).
/* .IP "\fB-M \fImyhostname\fR"
/*	Use the specified hostname or [address] in the default
/*	sender and recipient addresses, instead of the machine
/*	hostname.
/* .IP "\fB-r \fIrecipient_count\fR"
/*	Send the specified number of recipients per transaction (default: 1).
/*	Recipient names are generated by prepending a number to the
/*	recipient address.
/* .IP "\fB-s \fIsession_count\fR"
/*	Run the specified number of QMQP sessions in parallel (default: 1).
/* .IP "\fB-t \fIto\fR"
/*	Use the specified recipient address (default: <foo@myhostname>).
/* .IP "\fB-R \fIinterval\fR"
/*	Wait for a random period of time 0 <= n <= interval between messages.
/*	Suspending one thread does not affect other delivery threads.
/* .IP \fB-v\fR
/*	Make the program more verbose, for debugging purposes.
/* .IP "\fB-w \fIinterval\fR"
/*	Wait a fixed time between messages.
/*	Suspending one thread does not affect other delivery threads.
/* SEE ALSO
/*	qmqp-sink(1), QMQP message dump
/* LICENSE
/* .ad
/* .fi
/*	The Secure Mailer license must be distributed with this software.
/* AUTHOR(S)
/*	Wietse Venema
/*	IBM T.J. Watson Research
/*	P.O. Box 704
/*	Yorktown Heights, NY 10598, USA
/*--*/

/* System library. */

#include <sys_defs.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <string.h>

/* Utility library. */

#include <msg.h>
#include <msg_vstream.h>
#include <vstring.h>
#include <vstream.h>
#include <get_hostname.h>
#include <split_at.h>
#include <connect.h>
#include <mymalloc.h>
#include <events.h>
#include <iostuff.h>
#include <netstring.h>
#include <sane_connect.h>
#include <host_port.h>
#include <myaddrinfo.h>
#include <inet_proto.h>
#include <valid_hostname.h>
#include <valid_mailhost_addr.h>

/* Global library. */

#include <mail_date.h>
#include <qmqp_proto.h>
#include <mail_version.h>

/* Application-specific. */

 /*
  * Per-session data structure with state.
  * 
  * This software can maintain multiple parallel connections to the same QMQP
  * server. However, it makes no more than one connection request at a time
  * to avoid overwhelming the server with SYN packets and having to back off.
  * Back-off would screw up the benchmark. Pending connection requests are
  * kept in a linear list.
  */
typedef struct SESSION {
    int     xfer_count;			/* # of xfers in session */
    int     rcpt_done;			/* # of recipients done */
    int     rcpt_count;			/* # of recipients to go */
    VSTREAM *stream;			/* open connection */
    int     connect_count;		/* # of connect()s to retry */
    struct SESSION *next;		/* connect() queue linkage */
} SESSION;

static SESSION *last_session;		/* connect() queue tail */

static VSTRING *buffer;
static int var_line_limit = 10240;
static int var_timeout = 300;
static const char *var_myhostname;
static int session_count;
static int message_count = 1;
static struct sockaddr_storage ss;

#undef sun
static struct sockaddr_un sun;
static struct sockaddr *sa;
static int sa_length;
static int recipients = 1;
static char *defaddr;
static char *recipient;
static char *sender;
static int message_length = 1024;
static int count = 0;
static int counter = 0;
static int connect_count = 1;
static int random_delay = 0;
static int fixed_delay = 0;
static const char *mydate;
static int mypid;

static void enqueue_connect(SESSION *);
static void start_connect(SESSION *);
static void connect_done(int, char *);

static void send_data(SESSION *);
static void receive_reply(int, char *);

static VSTRING *message_buffer;
static VSTRING *sender_buffer;
static VSTRING *recipient_buffer;

/* Silly little macros. */

#define STR(x)	vstring_str(x)
#define	LEN(x)	VSTRING_LEN(x)

/* random_interval - generate a random value in 0 .. (small) interval */

static int random_interval(int interval)
{
    return (rand() % (interval + 1));
}

/* socket_error - look up and reset the last socket error */

static int socket_error(int sock)
{
    int     error;
    SOCKOPT_SIZE error_len;

    /*
     * Some Solaris 2 versions have getsockopt() itself return the error,
     * instead of returning it via the parameter list.
     */
    error = 0;
    error_len = sizeof(error);
    if (getsockopt(sock, SOL_SOCKET, SO_ERROR, (char *) &error, &error_len) < 0)
	return (-1);
    if (error) {
	errno = error;
	return (-1);
    }

    /*
     * No problems.
     */
    return (0);
}

/* exception_text - translate exceptions from the netstring module */

static char *exception_text(int except)
{
    ;

    switch (except) {
    case NETSTRING_ERR_EOF:
	return ("lost connection");
    case NETSTRING_ERR_TIME:
	return ("timeout");
    case NETSTRING_ERR_FORMAT:
	return ("netstring format error");
    case NETSTRING_ERR_SIZE:
	return ("netstring size exceeds limit");
    default:
	msg_panic("exception_text: unknown exception %d", except);
    }
    /* NOTREACHED */
}

/* startup - connect to server but do not wait */

static void startup(SESSION *session)
{
    if (message_count-- <= 0) {
	myfree((char *) session);
	session_count--;
	return;
    }
    enqueue_connect(session);
}

/* start_event - invoke startup from timer context */

static void start_event(int unused_event, char *context)
{
    SESSION *session = (SESSION *) context;

    startup(session);
}

/* start_another - start another session */

static void start_another(SESSION *session)
{
    if (random_delay > 0) {
	event_request_timer(start_event, (char *) session,
			    random_interval(random_delay));
    } else if (fixed_delay > 0) {
	event_request_timer(start_event, (char *) session, fixed_delay);
    } else {
	startup(session);
    }
}

/* enqueue_connect - queue a connection request */

static void enqueue_connect(SESSION *session)
{
    session->next = 0;
    if (last_session == 0) {
	last_session = session;
	start_connect(session);
    } else {
	last_session->next = session;
	last_session = session;
    }
}

/* dequeue_connect - connection request completed */

static void dequeue_connect(SESSION *session)
{
    if (session == last_session) {
	if (session->next != 0)
	    msg_panic("dequeue_connect: queue ends after last");
	last_session = 0;
    } else {
	if (session->next == 0)
	    msg_panic("dequeue_connect: queue ends before last");
	start_connect(session->next);
    }
}

/* fail_connect - handle failed startup */

static void fail_connect(SESSION *session)
{
    if (session->connect_count-- == 1)
	msg_fatal("connect: %m");
    msg_warn("connect: %m");
    event_disable_readwrite(vstream_fileno(session->stream));
    vstream_fclose(session->stream);
    session->stream = 0;
#ifdef MISSING_USLEEP
    doze(10);
#else
    usleep(10);
#endif
    start_connect(session);
}

/* start_connect - start TCP handshake */

static void start_connect(SESSION *session)
{
    int     fd;
    struct linger linger;

    /*
     * Some systems don't set the socket error when connect() fails early
     * (loopback) so we must deal with the error immediately, rather than
     * retrieving it later with getsockopt(). We can't use MSG_PEEK to
     * distinguish between server disconnect and connection refused.
     */
    if ((fd = socket(sa->sa_family, SOCK_STREAM, 0)) < 0)
	msg_fatal("socket: %m");
    (void) non_blocking(fd, NON_BLOCKING);
    linger.l_onoff = 1;
    linger.l_linger = 0;
    if (setsockopt(fd, SOL_SOCKET, SO_LINGER, (char *) &linger,
		   sizeof(linger)) < 0)
	msg_warn("setsockopt SO_LINGER %d: %m", linger.l_linger);
    session->stream = vstream_fdopen(fd, O_RDWR);
    event_enable_write(fd, connect_done, (char *) session);
    netstring_setup(session->stream, var_timeout);
    if (sane_connect(fd, sa, sa_length) < 0 && errno != EINPROGRESS)
	fail_connect(session);
}

/* connect_done - send message sender info */

static void connect_done(int unused_event, char *context)
{
    SESSION *session = (SESSION *) context;
    int     fd = vstream_fileno(session->stream);

    /*
     * Try again after some delay when the connection failed, in case they
     * run a Mickey Mouse protocol stack.
     */
    if (socket_error(fd) < 0) {
	fail_connect(session);
    } else {
	dequeue_connect(session);
	non_blocking(fd, BLOCKING);
	event_disable_readwrite(fd);
	/* Avoid poor performance when TCP MSS > VSTREAM_BUFSIZE. */
	if (sa->sa_family == AF_INET
#ifdef AF_INET6
	    || sa->sa_family == AF_INET6
#endif
	    )
	    vstream_tweak_tcp(session->stream);
	send_data(session);
    }
}

/* send_data - send message+sender+recipients */

static void send_data(SESSION *session)
{
    int     fd = vstream_fileno(session->stream);
    int     except;

    /*
     * Prepare for disaster.
     */
    if ((except = vstream_setjmp(session->stream)) != 0)
	msg_fatal("%s while sending message", exception_text(except));

    /*
     * Send the message content, by wrapping three netstrings into an
     * over-all netstring.
     * 
     * XXX This should be done more carefully to avoid blocking when sending
     * large messages over slow networks.
     */
    netstring_put_multi(session->stream,
			STR(message_buffer), LEN(message_buffer),
			STR(sender_buffer), LEN(sender_buffer),
			STR(recipient_buffer), LEN(recipient_buffer),
			(char *) 0);
    netstring_fflush(session->stream);

    /*
     * Wake me up when the server replies or when something bad happens.
     */
    event_enable_read(fd, receive_reply, (char *) session);
}

/* receive_reply - read server reply */

static void receive_reply(int unused_event, char *context)
{
    SESSION *session = (SESSION *) context;
    int     except;

    /*
     * Prepare for disaster.
     */
    if ((except = vstream_setjmp(session->stream)) != 0)
	msg_fatal("%s while receiving server reply", exception_text(except));

    /*
     * Receive and process the server reply.
     */
    netstring_get(session->stream, buffer, var_line_limit);
    if (msg_verbose)
	vstream_printf("<< %.*s\n", (int) LEN(buffer), STR(buffer));
    if (STR(buffer)[0] != QMQP_STAT_OK)
	msg_fatal("%s error: %.*s",
		  STR(buffer)[0] == QMQP_STAT_RETRY ? "recoverable" :
		  STR(buffer)[0] == QMQP_STAT_HARD ? "unrecoverable" :
		  "unknown", (int) LEN(buffer) - 1, STR(buffer) + 1);

    /*
     * Update the optional running counter.
     */
    if (count) {
	counter++;
	vstream_printf("%d\r", counter);
	vstream_fflush(VSTREAM_OUT);
    }

    /*
     * Finish this session. QMQP sends only one message per session.
     */
    event_disable_readwrite(vstream_fileno(session->stream));
    vstream_fclose(session->stream);
    session->stream = 0;
    start_another(session);
}

/* usage - explain */

static void usage(char *myname)
{
    msg_fatal("usage: %s -cv -s sess -l msglen -m msgs -C count -M myhostname -f from -t to -R delay -w delay host[:port]", myname);
}

MAIL_VERSION_STAMP_DECLARE;

/* main - parse JCL and start the machine */

int     main(int argc, char **argv)
{
    SESSION *session;
    char   *host;
    char   *port;
    char   *path;
    int     path_len;
    int     sessions = 1;
    int     ch;
    ssize_t len;
    int     n;
    int     i;
    char   *buf;
    const char *parse_err;
    struct addrinfo *res;
    int     aierr;
    const char *protocols = INET_PROTO_NAME_ALL;
    INET_PROTO_INFO *proto_info;

    /*
     * Fingerprint executables and core dumps.
     */
    MAIL_VERSION_STAMP_ALLOCATE;

    signal(SIGPIPE, SIG_IGN);
    msg_vstream_init(argv[0], VSTREAM_ERR);

    /*
     * Parse JCL.
     */
    while ((ch = GETOPT(argc, argv, "46cC:f:l:m:M:r:R:s:t:vw:")) > 0) {
	switch (ch) {
	case '4':
	    protocols = INET_PROTO_NAME_IPV4;
	    break;
	case '6':
	    protocols = INET_PROTO_NAME_IPV6;
	    break;
	case 'c':
	    count++;
	    break;
	case 'C':
	    if ((connect_count = atoi(optarg)) <= 0)
		usage(argv[0]);
	    break;
	case 'f':
	    sender = optarg;
	    break;
	case 'l':
	    if ((message_length = atoi(optarg)) <= 0)
		usage(argv[0]);
	    break;
	case 'm':
	    if ((message_count = atoi(optarg)) <= 0)
		usage(argv[0]);
	    break;
	case 'M':
	    if (*optarg == '[') {
		if (!valid_mailhost_literal(optarg, DO_GRIPE))
		    msg_fatal("bad address literal: %s", optarg);
	    } else {
		if (!valid_hostname(optarg, DO_GRIPE))
		    msg_fatal("bad hostname: %s", optarg);
	    }
	    var_myhostname = optarg;
	    break;
	case 'r':
	    if ((recipients = atoi(optarg)) <= 0)
		usage(argv[0]);
	    break;
	case 'R':
	    if (fixed_delay > 0 || (random_delay = atoi(optarg)) <= 0)
		usage(argv[0]);
	    break;
	case 's':
	    if ((sessions = atoi(optarg)) <= 0)
		usage(argv[0]);
	    break;
	case 't':
	    recipient = optarg;
	    break;
	case 'v':
	    msg_verbose++;
	    break;
	case 'w':
	    if (random_delay > 0 || (fixed_delay = atoi(optarg)) <= 0)
		usage(argv[0]);
	    break;
	default:
	    usage(argv[0]);
	}
    }
    if (argc - optind != 1)
	usage(argv[0]);

    if (random_delay > 0)
	srand(getpid());

    /*
     * Translate endpoint address to internal form.
     */
    proto_info = inet_proto_init("protocols", protocols);
    if (strncmp(argv[optind], "unix:", 5) == 0) {
	path = argv[optind] + 5;
	path_len = strlen(path);
	if (path_len >= (int) sizeof(sun.sun_path))
	    msg_fatal("unix-domain name too long: %s", path);
	memset((char *) &sun, 0, sizeof(sun));
	sun.sun_family = AF_UNIX;
#ifdef HAS_SUN_LEN
	sun.sun_len = path_len + 1;
#endif
	memcpy(sun.sun_path, path, path_len);
	sa = (struct sockaddr *) & sun;
	sa_length = sizeof(sun);
    } else {
	if (strncmp(argv[optind], "inet:", 5) == 0)
	    argv[optind] += 5;
	buf = mystrdup(argv[optind]);
	if ((parse_err = host_port(buf, &host, (char *) 0, &port, "628")) != 0)
	    msg_fatal("%s: %s", argv[optind], parse_err);
	if ((aierr = hostname_to_sockaddr(host, port, SOCK_STREAM, &res)) != 0)
	    msg_fatal("%s: %s", argv[optind], MAI_STRERROR(aierr));
	myfree(buf);
	sa = (struct sockaddr *) & ss;
	if (res->ai_addrlen > sizeof(ss))
	    msg_fatal("address length %d > buffer length %d",
		      (int) res->ai_addrlen, (int) sizeof(ss));
	memcpy((char *) sa, res->ai_addr, res->ai_addrlen);
	sa_length = res->ai_addrlen;
#ifdef HAS_SA_LEN
	sa->sa_len = sa_length;
#endif
	freeaddrinfo(res);
    }

    /*
     * Allocate space for temporary buffer.
     */
    buffer = vstring_alloc(100);

    /*
     * Make sure we have sender and recipient addresses.
     */
    if (var_myhostname == 0)
	var_myhostname = get_hostname();
    if (sender == 0 || recipient == 0) {
	vstring_sprintf(buffer, "foo@%s", var_myhostname);
	defaddr = mystrdup(vstring_str(buffer));
	if (sender == 0)
	    sender = defaddr;
	if (recipient == 0)
	    recipient = defaddr;
    }

    /*
     * Prepare some results that may be used multiple times: the message
     * content netstring, the sender netstring, and the recipient netstrings.
     */
    mydate = mail_date(time((time_t *) 0));
    mypid = getpid();

    message_buffer = vstring_alloc(message_length + 200);
    vstring_sprintf(buffer,
		  "From: <%s>\nTo: <%s>\nDate: %s\nMessage-Id: <%d@%s>\n\n",
		    sender, recipient, mydate, mypid, var_myhostname);
    for (n = 1; LEN(buffer) < message_length; n++) {
	for (i = 0; i < n && i < 79; i++)
	    VSTRING_ADDCH(buffer, 'X');
	VSTRING_ADDCH(buffer, '\n');
    }
    STR(buffer)[message_length - 1] = '\n';
    netstring_memcpy(message_buffer, STR(buffer), message_length);

    len = strlen(sender);
    sender_buffer = vstring_alloc(len);
    netstring_memcpy(sender_buffer, sender, len);

    if (recipients == 1) {
	len = strlen(recipient);
	recipient_buffer = vstring_alloc(len);
	netstring_memcpy(recipient_buffer, recipient, len);
    } else {
	recipient_buffer = vstring_alloc(100);
	for (n = 0; n < recipients; n++) {
	    vstring_sprintf(buffer, "%d%s", n, recipient);
	    netstring_memcat(recipient_buffer, STR(buffer), LEN(buffer));
	}
    }

    /*
     * Start sessions.
     */
    while (sessions-- > 0) {
	session = (SESSION *) mymalloc(sizeof(*session));
	session->stream = 0;
	session->xfer_count = 0;
	session->connect_count = connect_count;
	session->next = 0;
	session_count++;
	startup(session);
    }
    for (;;) {
	event_loop(-1);
	if (session_count <= 0 && message_count <= 0) {
	    if (count) {
		VSTREAM_PUTC('\n', VSTREAM_OUT);
		vstream_fflush(VSTREAM_OUT);
	    }
	    exit(0);
	}
    }
}
