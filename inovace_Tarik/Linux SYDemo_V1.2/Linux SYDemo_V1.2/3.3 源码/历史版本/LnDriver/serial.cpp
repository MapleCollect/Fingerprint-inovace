/*
 * Program: 	serial.c
 * Author:  	Paul Dean
 * Version:     0.0.3
 * Date:    	2002-02-19
 * Description: To provide underlying serial port function,
 *              for high level applications.
 *
*/

#include <termios.h>            /* tcgetattr, tcsetattr */
#include <stdio.h>              /* perror, printf, puts, fprintf, fputs */
#include <unistd.h>             /* read, write, close */
#include <fcntl.h>              /* open */
#include <sys/signal.h>
#include <sys/types.h>
#include <string.h>             /* bzero, memcpy */
#include <limits.h>             /* CHAR_MAX */

#include "vardef.h"             /* INT32, INT16, INT8, UINT32, UINT16, UINT8 */
#include "serial.h"

/* 
 * Decription for TIMEOUT_SEC(buflen,baud);
 * baud bits per second, buflen bytes to send.
 * buflen*20 (20 means sending an octect-bit data by use of the maxim bits 20)
 * eg. 9600bps baudrate, buflen=1024B, then TIMEOUT_SEC = 1024*20/9600+1 = 3 
 * don't change the two lines below unless you do know what you are doing.
*/
#define TIMEOUT_SEC(buflen,baud) (buflen*20/baud+2)
#define TIMEOUT_USEC 0

#define BUFFER_LEN	1024    /* sendfile() */

static INT32    fd;             //File descriptor for the port
static struct termios termios_old, termios_new;
static fd_set   fs_read, fs_write;
static struct timeval tv_timeout;
static struct sigaction sigaction_io;   /* definition of signal action */

static int      SetPortSig ();
static void     signal_handler_IO (int status);
static void     SetBaudrate (INT32);
static INT32    GetBaudrate ();
static void     SetDataBit (INT32 databit);
static INT32    BAUDRATE (INT32 baudrate);
static INT32    _BAUDRATE (INT32 baudrate);
static INT32    SetPortAttr (INT32 baudrate, INT32 databit,
                             const char *stopbit, char parity);
static void     SetStopBit (const char *stopbit);
static void     SetParityCheck (char parity);

/* Open serial port ComPort at baudrate baud rate. */
INT32 OpenComPort (INT32 ComPort, INT32 baudrate, INT32 databit,
                   const char *stopbit, char parity)
{
    char           *pComPort;
    INT32           retval;

    switch (ComPort) {
    case 0:
        pComPort = "/dev/ttyS0";
        break;
    case 1:
        pComPort = "/dev/ttyS1";
        break;
    case 2:
        pComPort = "/dev/ttyS2";
        break;
    case 3:
        pComPort = "/dev/ttyS3";
        break;
    case 4:
        pComPort = "/dev/ttyS4";
        break;
    case 5:
        pComPort = "/dev/ttyS5";
        break;
    case 6:
        pComPort = "/dev/ttyS6";
        break;
    case 7:
        pComPort = "/dev/ttyS7";
        break;
    default:
        pComPort = "/dev/ttyS0";
        break;
    }

    fd = open (pComPort, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (-1 == fd) {
        fprintf (stderr, "cannot open port %s\n", pComPort);
        return (-1);
    }
    tcgetattr (fd, &termios_old);       /* save old termios value */
    /* 0 on success, -1 on failure */
    retval = SetPortAttr (baudrate, databit, stopbit, parity);
    if (-1 == retval) {
        fprintf (stderr, "\nport %s cannot set baudrate at %d\n", pComPort,
                 baudrate);
    }
    return (retval);
}

/* close serial port by use of file descriptor fd */
void CloseComPort ()
{
    /* flush output data before close and restore old attribute */
    tcsetattr (fd, TCSADRAIN, &termios_old);
    close (fd);
}
void ClearComPort()
{
   tcflush (fd, TCIOFLUSH);

}

INT32 ReadComPort (void *data, INT32 datalength)
{
    INT32           retval = 0;
    FD_ZERO (&fs_read);
    FD_SET (fd, &fs_read);
    tv_timeout.tv_sec = TIMEOUT_SEC (datalength, GetBaudrate ());
    tv_timeout.tv_usec = TIMEOUT_USEC;
    retval = select (fd + 1, &fs_read, NULL, NULL, &tv_timeout);
    if (retval) {
        return (read (fd, data, datalength));
    }
    else
        return (-1);
}

/* 
 * Write datalength bytes in buffer given by UINT8 *data,
 * return value: bytes written
 * Nonblock mode
*/
INT32 WriteComPort (UINT8 * data, INT32 datalength)
{
    INT32           retval, len = 0, total_len = 0;

    FD_ZERO (&fs_write);
    FD_SET (fd, &fs_write);
    tv_timeout.tv_sec = TIMEOUT_SEC (datalength, GetBaudrate ());
    tv_timeout.tv_usec = TIMEOUT_USEC;

    for (total_len = 0, len = 0; total_len < datalength;) {
        retval = select (fd + 1, NULL, &fs_write, NULL, &tv_timeout);
        if (retval) {
            len = write (fd, &data[total_len], datalength - total_len);
            if (len > 0) {
                total_len += len;
            }
        }
        else {
            tcflush (fd, TCOFLUSH);     /* flush all output data */
            break;
        }
    }
    return (total_len);
}

/* get serial port baudrate */
static INT32 GetBaudrate ()
{
    return (_BAUDRATE (cfgetospeed (&termios_new)));
}

/* set serial port baudrate by use of file descriptor fd */
static void SetBaudrate (INT32 baudrate)
{
    termios_new.c_cflag = BAUDRATE (baudrate);  /* set baudrate */
}

static void SetDataBit (INT32 databit)
{
    termios_new.c_cflag &= ~CSIZE;
    switch (databit) {
    case 8:
        termios_new.c_cflag |= CS8;
        break;
    case 7:
        termios_new.c_cflag |= CS7;
        break;
    case 6:
        termios_new.c_cflag |= CS6;
        break;
    case 5:
        termios_new.c_cflag |= CS5;
        break;
    default:
        termios_new.c_cflag |= CS8;
        break;
    }
}

static void SetStopBit (const char *stopbit)
{
    if (0 == strcmp (stopbit, "1")) {
        termios_new.c_cflag &= ~CSTOPB; /* 1 stop bit */
    }
    else if (0 == strcmp (stopbit, "1.5")) {
        termios_new.c_cflag &= ~CSTOPB; /* 1.5 stop bits */
    }
    else if (0 == strcmp (stopbit, "2")) {
        termios_new.c_cflag |= CSTOPB;  /* 2 stop bits */
    }
    else {
        termios_new.c_cflag &= ~CSTOPB; /* 1 stop bit */
    }
}

static void SetParityCheck (char parity)
{
    switch (parity) {
    case 'N':                  /* no parity check */
        termios_new.c_cflag &= ~PARENB;
        break;
    case 'E':                  /* even */
        termios_new.c_cflag |= PARENB;
        termios_new.c_cflag &= ~PARODD;
        break;
    case 'O':                  /* odd */
        termios_new.c_cflag |= PARENB;
        termios_new.c_cflag |= ~PARODD;
        break;
    default:                   /* no parity check */
        termios_new.c_cflag &= ~PARENB;
        break;
    }
}

static INT32 SetPortAttr (INT32 baudrate,
                          INT32 databit, const char *stopbit, char parity)
{
    bzero (&termios_new, sizeof (termios_new));
    cfmakeraw (&termios_new);
    SetBaudrate (baudrate);
    termios_new.c_cflag |= CLOCAL | CREAD;      /* | CRTSCTS */
    SetDataBit (databit);
    SetParityCheck (parity);
    SetStopBit (stopbit);
    termios_new.c_oflag = 0;
    termios_new.c_lflag |= 0;
    termios_new.c_oflag &= ~OPOST;
    termios_new.c_cc[VTIME] = 1;        /* unit: 1/10 second. */
    termios_new.c_cc[VMIN] = 1; /* minimal characters for reading */
    tcflush (fd, TCIFLUSH);
    return (tcsetattr (fd, TCSANOW, &termios_new));
}

static INT32 BAUDRATE (INT32 baudrate)
{
    switch (baudrate) {
    case 0:
        return (B0);
    case 50:
        return (B50);
    case 75:
        return (B75);
    case 110:
        return (B110);
    case 134:
        return (B134);
    case 150:
        return (B150);
    case 200:
        return (B200);
    case 300:
        return (B300);
    case 600:
        return (B600);
    case 1200:
        return (B1200);
    case 2400:
        return (B2400);
    case 9600:
        return (B9600);
    case 19200:
        return (B19200);
    case 38400:
        return (B38400);
    case 57600:
        return (B57600);
    case 115200:
        return (B115200);
    default:
        return (B9600);
    }
}

static INT32 _BAUDRATE (INT32 baudrate)
{
/* reverse baudrate */
    switch (baudrate) {
    case B0:
        return (0);
    case B50:
        return (50);
    case B75:
        return (75);
    case B110:
        return (110);
    case B134:
        return (134);
    case B150:
        return (150);
    case B200:
        return (200);
    case B300:
        return (300);
    case B600:
        return (600);
    case B1200:
        return (1200);
    case B2400:
        return (2400);
    case B9600:
        return (9600);
    case B19200:
        return (19200);
    case B38400:
        return (38400);
    case B57600:
        return (57600);
    case B115200:
        return (115200);
    default:
        return (9600);
    }
}

/* send file const char *pathname */
INT32 SendFile (const char *pathname, INT32 echo)
{
    INT32           fd, buflen, len;    /*  fd for file */
    char            buf[BUFFER_LEN + 1];
    fd = open (pathname, O_RDONLY);
    if (fd < 0) {
        perror (pathname);
        return (-1);
    }

    while (1) {
        bzero (buf, sizeof (buf));
        buflen = read (fd, buf, BUFFER_LEN);
        if (0 == buflen)
            break;
        buf[buflen] = 0;
        echo ? printf ("%s", buf) : 0;
        if ((len = WriteComPort ((UINT8*)buf, buflen)) != buflen) {
            fprintf (stderr, "write %d bytes for %d bytes\n", len, buflen);
            fprintf (stderr, "To CloseComPort()\n");
            CloseComPort ();
            close (fd);
            fprintf (stderr, "CloseComPort completed\n");
            return (-1);
        }
        SetPortSig ();
    }                           /* end of while (1) */

    fflush (stdout);
    fflush (stderr);
    close (fd);
    return (0);
}

static void signal_handler_IO (int status)
{
    char            recvbuf[CHAR_MAX];
    ReadComPort (recvbuf, sizeof (recvbuf));
}

static int SetPortSig ()
{
    /* install the signal handler before making the device asynchronous */
    sigaction_io.sa_handler = signal_handler_IO;
    sigemptyset (&(sigaction_io.sa_mask));
    sigaction_io.sa_flags = 0;
    sigaction_io.sa_restorer = NULL;
    sigaction (SIGIO, &sigaction_io, NULL);
    /* allow the process to receive SIGIO */
    if (-1 == fcntl (fd, F_SETFL, O_ASYNC))
        return (-1);
    if (-1 == fcntl (fd, F_SETOWN, getpid ()))
        return (-1);
    /* Make the file descriptor asynchronous (the manual page says only
       O_APPEND and O_NONBLOCK, will work with F_SETFL...) */
    return (0);
}
