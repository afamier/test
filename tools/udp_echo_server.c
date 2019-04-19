#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <netinet/if_ether.h>
#include <linux/in.h>
#include <memory.h>

#define ________TEST_H________
/* test definitions */
#define FALSE               0
#define TRUE                1

#define ASSERT( para )  if ( para == FALSE ) printf( "ASSERT!!! \n" )

#define LOCAL_TEST_PORT     2019
#define MAX_NAME_STR_LEN    32
#define MAX_PACKET_SIZE     65535


typedef fd_set              FD_SET_S;
typedef struct timeval      TIMEVAL_S;

typedef struct tagINADDR
{
    unsigned int        s_ulAddr;
}INADDR_S;

typedef struct tagSOCKADDRIN
{
    unsigned short int  sin_chFamily;
    unsigned short int  sin_usPort;
    INADDR_S            sin_stAddr;
    char                sin_chZero_a[8];
}SOCKADDRIN_S;


#define ________TEST_C________

/* test functions */
int strtoipaddr(char *szStr, unsigned int *pulIpAddr)
{
    unsigned int a, b, c, d;

    if (sscanf(szStr, "%d.%d.%d.%d", &a, &b, &c, &d) == 4)
    {
        if (a<256 && b<256 && c<256 && d<256)
        {
            *pulIpAddr = (a<<24) | (b<<16) | (c<<8) | d;
            return 0;
        }
    }

    return -1;
}

char  *ipaddrtostr(unsigned int ulAddr, char *str)
{
    sprintf( str, "%d.%d.%d.%d",
        ulAddr >> 24, (ulAddr >> 16) & 0xff,
        (ulAddr >> 8) & 0xff, ulAddr & 0xff);

    return str;
}

static int aos_task_delay( int timeout )
{
    struct timeval tv;
    tv.tv_sec  = timeout / 1000;
    tv.tv_usec = ( timeout % 1000 ) * 1000;
    select(0, NULL, NULL, NULL, &tv);
    return 0;
}


int main( int argc, char * argv[] )
{
    int                 slSockFd;  /* socket fd, based on usServiceApiPort */
    SOCKADDRIN_S        stSockAddr;
    int                 slSockAddrLen;

    unsigned int        ulLocalIpAddr;
    unsigned short int  usLocalPortNo;

    unsigned int        ulPeerIpAddr;
    unsigned short int  usPeerPortNo;

    FD_SET_S            slFdSet;
    TIMEVAL_S           stTimeout;

    int                 slDebugFlag;
    int                 slRecvCount;

    char                strBuff[ MAX_NAME_STR_LEN ];
    char                strRecvBuff[ MAX_PACKET_SIZE ];
    
    int                 slRet;

    /* check argc */
    if ( argc < 2 )
    {
        printf( "Usage: %s <udp_echo_port> [debug] \r\n", argv[ 0 ] );
        printf( "       udp_echo_port - 2019 by default \r\n" );
        return 1;
    }

    if ( argc >= 3
      && strcmp( argv[ 2 ], "debug" ) == 0 )
    {
        slDebugFlag = 1;
    }
    else
    {
        slDebugFlag = 0;
    }

    slRecvCount     = 0;

    strtoipaddr( "0.0.0.0", &ulLocalIpAddr );
    usLocalPortNo  = atoi( argv[ 1 ] );
    if ( usLocalPortNo == 0 )
    {
        usLocalPortNo   = LOCAL_TEST_PORT;
    }

    /* init socket */
    slSockFd     = -1;
    slSockFd = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
    if ( slSockFd <= 0 )
    {
        ASSERT( 0 );
        printf( "[ERROR]open socket fail, port:%u, errcode:%d \r\n",
                usLocalPortNo, slSockFd );
        return 1;
    }

    stSockAddr.sin_chFamily         = AF_INET;
    stSockAddr.sin_stAddr.s_ulAddr  = htonl( ulLocalIpAddr );
    stSockAddr.sin_usPort           = htons( usLocalPortNo );
    slSockAddrLen                   = sizeof( SOCKADDRIN_S );
    slRet   = bind( slSockFd, (struct sockaddr *)&stSockAddr, slSockAddrLen );
    if( slRet < 0 )
    {
        ASSERT( 0 );
        printf( "[ERROR]bind socket fail, port:%u, errcode:%d \r\n",
                usLocalPortNo, slSockFd );
        close( slSockFd );
        return 1;
    }

    printf( "udp_echo_server is running... port:%u, max pkt:%u \r\n",
            usLocalPortNo, MAX_PACKET_SIZE );

    memset( strRecvBuff, 0x00, MAX_PACKET_SIZE );
    
    while ( 1 )
    {
        /* check packet arrived? */
        stTimeout.tv_sec    = 5;    /* timeout after 5 seconds */
        stTimeout.tv_usec   = 0;

        FD_ZERO( &slFdSet );
        FD_SET( slSockFd, &slFdSet );

        slRet = select( slSockFd + 1, &slFdSet, NULL, NULL, (struct timeval *)&stTimeout );
        if ( slRet <= 0 )
        {
            /* timeout */
            // aos_task_delay( 10 );
            continue;
        }

        /* recv response */
        memset( &stSockAddr, 0x00, sizeof( SOCKADDRIN_S ) );
        slSockAddrLen   = sizeof( SOCKADDRIN_S );
        slRet           = recvfrom( slSockFd, strRecvBuff, MAX_PACKET_SIZE, 0,
                                    (struct sockaddr *)&stSockAddr, (socklen_t *)&slSockAddrLen );
        if ( slRet <= 0 )
        {
            ASSERT( 0 );
            printf( "[ERROR]recv fail, ret:%d \r\n", slRet );
            close( slSockFd );
            return 1;
        }

        if ( slDebugFlag )
        {
            slRecvCount++;
            printf( "udp echo packet from %s:%d, count: %d \r\n",
                    ipaddrtostr( htonl( stSockAddr.sin_stAddr.s_ulAddr ), strBuff ), htons( stSockAddr.sin_usPort ),
                    slRecvCount );
        }

        /* send echo packet */
        slRet = sendto( slSockFd, strRecvBuff, slRet,
                        0, (struct sockaddr*)&stSockAddr, sizeof( SOCKADDRIN_S ) );
        if ( slRet < 0 )
        {
            ASSERT( 0 );
            printf( "[ERROR]send fail, ret:%d \r\n", slRet );
            close( slSockFd );
            return 1;
        }
    }

    printf( "udp_echo_server exit! \r\n" );
    close( slSockFd );
    return 0;
}

