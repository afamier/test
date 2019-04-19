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

/* global definitions */
typedef void                VOID;
typedef char                S8;
typedef short               S16;
typedef int                 S32;
typedef float               F32;
typedef unsigned char       U8;
typedef unsigned short int  U16;
typedef unsigned int        U32;
typedef unsigned int        BOOL_T;
typedef long                SINT;
typedef unsigned long       UINT;
typedef unsigned long       UPTR;

typedef struct timeval TIMEVAL_S;

/* struct definitions */
#define AOS_FAIL -1
#define AOS_SUCC 0
#define FALSE 0
#define TRUE 1

#define db_strncpy          strncpy
#define aos_memcpy          memcpy
#define aos_memset          memset
#define aos_strlen          strlen
#define aos_strcpy          strcpy
#define aos_strncpy         strncpy
#define aos_sscanf          sscanf
#define aos_stricmp         strcasecmp
#define aos_sprintf         sprintf
#define aos_snprintf        snprintf

#define AOS_HTONL           htonl
#define AOS_HTONS           htons

#define AOS_ASSERT( para )  if ( para == FALSE ) printf( "ASSERT!!! \n" )

#define ROOT_MPE            1
#define MPE_SIMSRV_DBO      1
#define ROOT_DEBUG( IM_DEBUG_MUST ) 1

#define MAX_SHELL_CMD_STR_LEN       256
#define DEVICE_ID_LEN       8

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

// scp公共消息头
typedef struct
{
    U32 ulHdrPpid;
    U16 usHdrStream;                            // 02 - SIM卡数据
    U16 usHdrResv;
    // -----------------------------------------
    U8  ucVersion;                              // SCP_VERSION
    U8  ucFlag;                                 // SCP_FLAG_ENCRYPT | SCP_FLAG_MULTI_MSG
    U8  ucReserve;
    U8  ucChannel;                              // SCP_CHANNEL_E
                                                // SCTP link: SCP_CHANNEL_1, SCP_CHANNEL_5
                                                // UDP link : * ( no meaning )
    U8  aucProductSn[ DEVICE_ID_LEN ];          // forward FALSE: means local product_sn
    U16 usSequence;                             // SN = 0, no loss check, be used in SCTP reliable channel
                                                // SN = 1 ~ 65535, loss check, be used in SCTP: SCP_CHANNEL_1 or UDP link */
    U16 usLength;                               // excluding SCP_HEADER_S, only data length
    U32 ulCRC;
}SCP_HEADER_S;                    // header size = 4 + 8 + 8 = 20

/* SCP通信消息结构 */
typedef struct scp_user_msg_s
{
    SCP_HEADER_S        stScpHdr;
    U8                  aucScpMsg[ 0 ];
}SCP_USER_MSG_S;

// scp msg_type消息头
typedef struct
{
    U16 usMsgType;                              // SCP_MSG_TYPE_E
    U16 usMsgLen;                               // excluding SCP_MSG_HEAD_S, only content length
    S8  szContent[ 0 ];
}SCP_MSG_S;


/* test definitions */
#define FALSE               0
#define TRUE                1

#define ASSERT( para )  if ( para == FALSE ) printf( "ASSERT!!! \n" )

#define PROXY_LOCAL_PORT    3020
#define PROXY_RELAY_PORT    3021
#define PROXY_RELAY_ADDR    "127.0.0.1"

#define MAX_NAME_STR_LEN    32
#define MAX_PACKET_SIZE     65535


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

S8 * eid_get_product_sn_str( U8 * pucBytes, U32 ulByteNum, S8 * pszOutStr )
{
    U32         ulIndex;
    U32         ulLen;

    pszOutStr[ 0 ]   = 0;

    if ( ulByteNum == 0 )
    {
        AOS_ASSERT( 0 );
        return pszOutStr;
    }

    if ( ulByteNum == 1 )
    {
        aos_sprintf( pszOutStr, "%02x", pucBytes[ 0 ] );
        return pszOutStr;
    }

    for ( ulIndex = 0; ulIndex < ( ulByteNum - 1 ); ulIndex++ )
    {
        ulLen   = aos_strlen( pszOutStr );

        /* SN: 0102-0304-0506-0708 */
        if ( ( ( ulIndex + 1 ) % 2 ) == 0 )
        {
            /* 2字节分隔符 */
            aos_sprintf( pszOutStr + ulLen, "%02x%s", pucBytes[ ulIndex ], "-" );
        }
        else
        {
            aos_sprintf( pszOutStr + ulLen, "%02x", pucBytes[ ulIndex ] );
        }
    }

    ulLen   = aos_strlen( pszOutStr );
    aos_sprintf( pszOutStr + ulLen, "%02x", pucBytes[ ulIndex ] );

    return pszOutStr;
}


void eid_print_mem_buffer( int ulMpe, U8 ucLineBytes, U8 * pucBuff, int ulBuffSize )
{
    S8          strCharLine[ 512 ];
    S8          strHexLine[ 512 ];
    U8          ucVal;
    int         ulIndex;
    int         ulOffset;
    int         ulLeft;
    BOOL_T      bFirstLine;

    if ( ucLineBytes > 64
      || ucLineBytes == 0 )
    {
        AOS_ASSERT( 0 );
        return;
    }

    /* 可显示字符: 32 ~ 125 */
    aos_memset( strCharLine, 0x00, 512 );
    aos_memset( strHexLine, 0x00, 512 );
    bFirstLine          = TRUE;

    for ( ulIndex = 0; ulIndex < ulBuffSize; ulIndex++ )
    {
        /* 取对应字节 */
        ucVal   = *( pucBuff + ulIndex );

        /* 控制每行打印的字节数 */
        if ( ulIndex % ucLineBytes == 0 )
        {
            /* 换行处理 */
            if ( bFirstLine )
            {
                bFirstLine  = FALSE;
            }
            else
            {
                printf( "\r\n   %s - %s", strHexLine, strCharLine );
            }

            /* 每行首字符 */
            sprintf( strHexLine, "%02x", ucVal );
            if ( ucVal >= 32 && ucVal <= 125 )
            {
                sprintf( strCharLine, "%c", (S8)ucVal );
            }
            else
            {
                sprintf( strCharLine, "." );
            }
        }
        else
        {
            /* 每行的后续字符 */
            sprintf( strHexLine + aos_strlen( strHexLine ), " %02x", ucVal );

            if ( ucVal >= 32 && ucVal <= 125 )
            {
                sprintf( strCharLine + aos_strlen( strCharLine ), "%c", (S8)ucVal );
            }
            else
            {
                sprintf( strCharLine + aos_strlen( strCharLine ), "." );
            }
        }
    }

    /* 尾行补齐 */
    ulOffset    = ucLineBytes * 3 - 1;
    ulLeft      = aos_strlen( strHexLine );
    if ( ulLeft < ulOffset )
    {
        for ( ulIndex = 0; ulIndex < ( ulOffset - ulLeft ); ulIndex++ )
        {
            sprintf( strHexLine + aos_strlen( strHexLine ), " " );
        }
    }

    /* 输出尾行 */
    printf( "\r\n   %s - %s ", strHexLine, strCharLine );

}

/* udp: 00 00 00 00 00 01 00 00 
        02 00 00 01 da 00 00 12 10 00 00 07 00 77 04 48 00 00 00 00 20 00 04 44
*/
int eid_print_scp_msg_header( int ulIndex, U8 * pucBuff, int ulBuffSize )
{
    SCP_USER_MSG_S *        pstUserMsg  = (SCP_USER_MSG_S *)pucBuff;
    SCP_MSG_S *             pstScpMsg   = (SCP_MSG_S *)pstUserMsg->aucScpMsg;
    S8                      strBuff[ 64 ];

    /* udp, udp2 */
    if ( ulIndex == 2
      || ulIndex == 8 )
    {
        /* print scp_msg header */
        if ( ntohs( pstUserMsg->stScpHdr.usHdrStream ) == 2 )
        {
            printf( "\r\n!!![sim] size=%d", ulBuffSize );
            return TRUE;
        }
        else
        {
            if ( pstUserMsg->stScpHdr.usLength == 0 )
            {
                #if 1  /* 打印应答消息 */
                printf( "\r\n@@@[ack] sn=%d, size/len=%d/%d, dev_sn=%s, ver=%d, chn=%d",
                               ntohs( pstUserMsg->stScpHdr.usSequence ),
                               ulBuffSize,
                               ntohs( pstUserMsg->stScpHdr.usLength ),
                               eid_get_product_sn_str( pstUserMsg->stScpHdr.aucProductSn, DEVICE_ID_LEN, strBuff ),
                               pstUserMsg->stScpHdr.ucVersion,
                               pstUserMsg->stScpHdr.ucChannel );
                return TRUE;
                #endif
            }
            else if ( pstUserMsg->stScpHdr.usSequence == 0 )
            {
                #if 0  /* 打印无需应答的消息 */
                printf( "\r\n@@@[###] sn=%d, size/len=%d/%d, dev_sn=%s, ver=%d, chn=%d, msgType=0x%0x, msgLen=0x%0x",
                               ntohs( pstUserMsg->stScpHdr.usSequence ),
                               ulBuffSize,
                               ntohs( pstUserMsg->stScpHdr.usLength ),
                               eid_get_product_sn_str( pstUserMsg->stScpHdr.aucProductSn, DEVICE_ID_LEN, strBuff ),
                               pstUserMsg->stScpHdr.ucVersion,
                               pstUserMsg->stScpHdr.ucChannel,
                               ntohs( pstScpMsg->usMsgType ),
                               ntohs( pstScpMsg->usMsgLen ) );
                return TRUE;
                #endif
            }
            else
            {
                printf( "\r\n@@@[***] sn=%d, size/len=%d/%d, dev_sn=%s, ver=%d, chn=%d, msgType=0x%0x, msgLen=0x%0x",
                               ntohs( pstUserMsg->stScpHdr.usSequence ),
                               ulBuffSize,
                               ntohs( pstUserMsg->stScpHdr.usLength ),
                               eid_get_product_sn_str( pstUserMsg->stScpHdr.aucProductSn, DEVICE_ID_LEN, strBuff ),
                               pstUserMsg->stScpHdr.ucVersion,
                               pstUserMsg->stScpHdr.ucChannel,
                               ntohs( pstScpMsg->usMsgType ),
                               ntohs( pstScpMsg->usMsgLen ) );
                return TRUE;
            }
        }
    }
    else if ( ulIndex == 6 )  // usctp
    {
        printf( "\r\n!!![usctp] size=%d", ulBuffSize );
        return TRUE;
    }
    else if ( ulIndex == 0 )  // sctp
    {
        printf( "\r\n!!![sctp] size=%d", ulBuffSize );
        return TRUE;
    }

    return FALSE;
}


/*
  2020  - sctp      不支持搭桥转发
  2021  - echo
  2022  - udp
  2026  - usctp     不支持搭桥转发
  2028  - udp2
*/
#define MAX_RELAY_NUM   10
int main( int argc, char * argv[] )
{
    int                 slLocalSockFds[ MAX_RELAY_NUM ];  /* local socket fd */
    int                 slRelaySockFds[ MAX_RELAY_NUM ];  /* relay socket fd */
    int                 slMaxSockFd;

    SOCKADDRIN_S        stLocalFromAddrs[ MAX_RELAY_NUM ];
    int                 slLocalFromLen;
    SOCKADDRIN_S        stRelayFromAddrs[ MAX_RELAY_NUM ];
    int                 slRelayFromLen;
    SOCKADDRIN_S        stPeerSockAddrs[ MAX_RELAY_NUM ];
    int                 slPeerAddrLen;

    unsigned int        ulLocalIpAddr;
    unsigned short int  usLocalPortNos[ MAX_RELAY_NUM ];
    unsigned int        ulRelayIpAddr;
    unsigned short int  usRelayPortNos[ MAX_RELAY_NUM ];
    unsigned int        ulPeerIpAddr;
    unsigned short int  usPeerPortNos[ MAX_RELAY_NUM ];

    FD_SET_S            slFdSet;
    TIMEVAL_S           stTimeout;
    SOCKADDRIN_S        stSockAddr;
    int                 slAddrLen;
    int                 ulIndex;
    unsigned short int  usPortNo;

    int                 slDebugFlag;
    int                 slDebugSize;
    int                 slRecvLocalCount;
    int                 slRecvRelayCount;

    TIMEVAL_S           stCurTime;
    char                strBuff[ MAX_NAME_STR_LEN ];
    char                strRecvBuff[ MAX_PACKET_SIZE ];
    
    int                 slRet;
    

    /* check argc */
    if ( argc < 5 )
    {
        printf( "Usage: %s <local_port> <relay_port> <peer_addr> <peer_port> [debug] [print_size] \r\n", argv[ 0 ] );
        printf( "       local_port - 2080, device -> proxy, 2080~2089 \r\n" );
        printf( "       relay_port - 2090, proxy  -> peer, 2090~2099 \r\n" );
        printf( "       peer_addr  - peer ip address \r\n" );
        printf( "       peer_port  - peer port no \r\n" );
        printf( "       debug      - print debug info \r\n" );
        printf( "       print_size - print packet bytes size \r\n" );
        return 1;
    }

    if ( argc >= 6
      && strcmp( argv[ 5 ], "debug" ) == 0 )
    {
        slDebugFlag = 1;
    }
    else
    {
        slDebugFlag = 0;
    }

    if ( argc >= 7 )
    {
        slDebugSize     = atoi( argv[ 6 ] );
    }
    else
    {
        slDebugSize     = 0;
    }

    slRecvLocalCount     = 0;
    slRecvRelayCount     = 0;

    strtoipaddr( "0.0.0.0", &ulLocalIpAddr );

    /* get local port */
    usPortNo    = atoi( argv[ 1 ] );
    if ( usPortNo == 0 )
    {
        printf( "[ERROR]invalid local port! \r\n" );
        return 1;
    }
    
    for ( ulIndex = 0; ulIndex < MAX_RELAY_NUM; ulIndex++ )
    {
        usLocalPortNos[ ulIndex ]  = usPortNo + ulIndex;
    }

    strtoipaddr( "0.0.0.0", &ulRelayIpAddr );

    /* get relay port */
    usPortNo    = atoi( argv[ 2 ] );
    if ( usPortNo == 0 )
    {
        printf( "[ERROR]invalid relay port! \r\n" );
        return 1;
    }

    for ( ulIndex = 0; ulIndex < MAX_RELAY_NUM; ulIndex++ )
    {
        usRelayPortNos[ ulIndex ]  = usPortNo + ulIndex;
    }

    /* get peer addr */
    if ( -1 == strtoipaddr( argv[ 3 ], &ulPeerIpAddr ) )
    {
        printf( "[ERROR]invalid peer addr! \r\n" );
        return 1;
    }

    /* get peer port */
    usPortNo    = atoi( argv[ 4 ] );
    if ( usPortNo == 0 )
    {
        printf( "[ERROR]invalid peer port! \r\n" );
        return 1;
    }

    for ( ulIndex = 0; ulIndex < MAX_RELAY_NUM; ulIndex++ )
    {
        usPeerPortNos[ ulIndex ]  = usPortNo + ulIndex;
    }
    
    /* init local socket */
    for ( ulIndex = 0; ulIndex < MAX_RELAY_NUM; ulIndex++ )
    {
        slLocalSockFds[ ulIndex ]   = -1;
    }
    
    for ( ulIndex = 0; ulIndex < MAX_RELAY_NUM; ulIndex++ )
    {
        /* new local socket */
        slLocalSockFds[ ulIndex ]   = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
        if ( slLocalSockFds[ ulIndex ] <= 0 )
        {
            printf( "[ERROR]open local socket fail, port:%u, errcode:%d \r\n",
                    usLocalPortNos[ ulIndex ], slLocalSockFds[ ulIndex ] );
            return 1;
        }

        /* bind local socket */
        stSockAddr.sin_chFamily         = AF_INET;
        stSockAddr.sin_stAddr.s_ulAddr  = htonl( ulLocalIpAddr );
        stSockAddr.sin_usPort           = htons( usLocalPortNos[ ulIndex ] );
        slAddrLen                       = sizeof( SOCKADDRIN_S );
        slRet   = bind( slLocalSockFds[ ulIndex ], (struct sockaddr *)&stSockAddr, slAddrLen );
        if( slRet < 0 )
        {
            printf( "[ERROR]bind local socket fail, port:%u, errcode:%d \r\n",
                    usLocalPortNos[ ulIndex ], slLocalSockFds[ ulIndex ] );

            for ( ulIndex = 0; ulIndex < MAX_RELAY_NUM; ulIndex++ )
            {
                close( slLocalSockFds[ ulIndex ] );
            }
            return 1;
        }
    }


    /* init relay socket */
    for ( ulIndex = 0; ulIndex < MAX_RELAY_NUM; ulIndex++ )
    {
        slRelaySockFds[ ulIndex ]   = -1;
    }

    for ( ulIndex = 0; ulIndex < MAX_RELAY_NUM; ulIndex++ )
    {
        /* new relay socket */
        slRelaySockFds[ ulIndex ]   = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
        if ( slRelaySockFds[ ulIndex ] <= 0 )
        {
            printf( "[ERROR]open relay socket fail, port:%u, errcode:%d \r\n",
                    usRelayPortNos[ ulIndex ], slRelaySockFds[ ulIndex ] );
            return 1;
        }

        /* bind relay socket */
        stSockAddr.sin_chFamily         = AF_INET;
        stSockAddr.sin_stAddr.s_ulAddr  = htonl( ulRelayIpAddr );
        stSockAddr.sin_usPort           = htons( usRelayPortNos[ ulIndex ] );
        slAddrLen                       = sizeof( SOCKADDRIN_S );
        slRet   = bind( slRelaySockFds[ ulIndex ], (struct sockaddr *)&stSockAddr, slAddrLen );
        if( slRet < 0 )
        {
            printf( "[ERROR]bind relay socket fail, port:%u, errcode:%d \r\n",
                    usRelayPortNos[ ulIndex ], slRelaySockFds[ ulIndex ] );

            for ( ulIndex = 0; ulIndex < MAX_RELAY_NUM; ulIndex++ )
            {
                close( slRelaySockFds[ ulIndex ] );
            }
            
            for ( ulIndex = 0; ulIndex < MAX_RELAY_NUM; ulIndex++ )
            {
                close( slLocalSockFds[ ulIndex ] );
            }
            return 1;
        }    
    }


    /* set peer sockaddr */
    for ( ulIndex = 0; ulIndex < MAX_RELAY_NUM; ulIndex++ )
    {
        stPeerSockAddrs[ ulIndex ].sin_chFamily         = AF_INET;
        stPeerSockAddrs[ ulIndex ].sin_stAddr.s_ulAddr  = htonl( ulPeerIpAddr );
        stPeerSockAddrs[ ulIndex ].sin_usPort           = htons( usPeerPortNos[ ulIndex ] );
    }

    slPeerAddrLen                             = sizeof( SOCKADDRIN_S );

    /* kickoff */
    printf( "udp_relay_server is running... local_port:%d~%d, relay_port:%d~%d, peer_addr:%s, peer_port:%d~%d \r\n",
            (int)usLocalPortNos[ 0 ], (int)usLocalPortNos[ MAX_RELAY_NUM - 1 ], 
            (int)usRelayPortNos[ 0 ], (int)usRelayPortNos[ MAX_RELAY_NUM - 1 ],
            ipaddrtostr( ulPeerIpAddr, strBuff ), 
            (int)usPeerPortNos[ 0 ], (int)usPeerPortNos[ MAX_RELAY_NUM - 1 ] );

    memset( strRecvBuff, 0x00, MAX_PACKET_SIZE );
    
    while ( 1 )
    {
        /* check packet arrived? */
        stTimeout.tv_sec    = 5;    /* timeout after 5 seconds */
        stTimeout.tv_usec   = 0;

        /* set fd_set */
        FD_ZERO( &slFdSet );
        for ( ulIndex = 0; ulIndex < MAX_RELAY_NUM; ulIndex++ )
        {
            FD_SET( slLocalSockFds[ ulIndex ], &slFdSet );
            FD_SET( slRelaySockFds[ ulIndex ], &slFdSet );
            
        }

        /* get max_sock_fd */
        slMaxSockFd     = 0;
        for ( ulIndex = 0; ulIndex < MAX_RELAY_NUM; ulIndex++ )
        {
            if ( slLocalSockFds[ ulIndex ] > slMaxSockFd )
            {
                slMaxSockFd     = slLocalSockFds[ ulIndex ];
            }

            if ( slRelaySockFds[ ulIndex ] > slMaxSockFd )
            {
                slMaxSockFd     = slRelaySockFds[ ulIndex ];
            }
        }

        /* select */
        slRet = select( slMaxSockFd + 1, &slFdSet, NULL, NULL, (struct timeval *)&stTimeout );
        if ( slRet <= 0 )
        {
            /* timeout */
            // aos_task_delay( 10 );
            continue;
        }

        /* check local socket */
        for ( ulIndex = 0; ulIndex < MAX_RELAY_NUM; ulIndex++ )
        {
    		if ( FD_ISSET( slLocalSockFds[ ulIndex ], &slFdSet ) )
    		{
                /* recv request and update from addr */
                // memset( strRecvBuff, 0x00, 128 );
                memset( &stLocalFromAddrs[ ulIndex ], 0x00, sizeof( SOCKADDRIN_S ) );
                slLocalFromLen   = sizeof( SOCKADDRIN_S );
                slRet           = recvfrom( slLocalSockFds[ ulIndex ], strRecvBuff, MAX_PACKET_SIZE, 0,
                                            (struct sockaddr *)&stLocalFromAddrs[ ulIndex ], (socklen_t *)&slLocalFromLen );
                if ( slRet <= 0 )
                {
                    printf( "\r\n[ERROR]recv local fail, ret:%d \r\n", slRet );
                    #if 0
                    for ( ulIndex = 0; ulIndex < MAX_RELAY_NUM; ulIndex++ )
                    {
                        close( slRelaySockFds[ ulIndex ] );
                    }
                    
                    for ( ulIndex = 0; ulIndex < MAX_RELAY_NUM; ulIndex++ )
                    {
                        close( slLocalSockFds[ ulIndex ] );
                    }
                    #endif
                    aos_task_delay( 1000 );
                    continue;
                }

                gettimeofday( &stCurTime, NULL );

                if ( slDebugFlag )
                {
                    SCP_USER_MSG_S *        pstUserMsg  = (SCP_USER_MSG_S *)strRecvBuff;
                    SCP_MSG_S *             pstScpMsg   = (SCP_MSG_S *)pstUserMsg->aucScpMsg;

                    /* not ack packet */
                    slRecvLocalCount++;
                    printf( "\r\n[ %u.%u ]udp_relay_server local: %3d, peer: %3d, recv local: %s:%d >>> local[ %d ], size: %d ",
                            (int)stCurTime.tv_sec, (int)(stCurTime.tv_usec/1000),
                            slRecvLocalCount, slRecvRelayCount,
                            ipaddrtostr( htonl( stLocalFromAddrs[ ulIndex ].sin_stAddr.s_ulAddr ), strBuff ), htons( stLocalFromAddrs[ ulIndex ].sin_usPort ),
                            usLocalPortNos[ ulIndex ],
                            slRet );

                    if ( TRUE == eid_print_scp_msg_header( ulIndex, (U8 *)strRecvBuff, slRet ) )
                    {
                        if ( slDebugSize > 0 )
                        {
                            if ( slDebugSize > slRet )
                            {
                                eid_print_mem_buffer( 0, 32, (U8 *)strRecvBuff, slRet );
                            }
                            else
                            {
                                eid_print_mem_buffer( 0, 32, (U8 *)strRecvBuff, slDebugSize );
                            }
                        }
                    }
                }

                /* proxy convert */
                #if 0
                if ( ulIndex == 6 )  // usctp header: xx xx [ server port: ** ** ] xx xx ...
                {
                    strRecvBuff[ 2 ]    = ( usPeerPortNos[ ulIndex ] >> 8 ) & 0x00FF;                    
                    strRecvBuff[ 3 ]    = usPeerPortNos[ ulIndex ] & 0x00FF;                    
                }
                #endif
                    
                /* send to peer addr */
                slRet = sendto( slRelaySockFds[ ulIndex ], strRecvBuff, slRet,
                                0, (struct sockaddr*)&stPeerSockAddrs[ ulIndex ], sizeof( SOCKADDRIN_S ) );
                if ( slRet < 0 )
                {
                    printf( "\r\n[ERROR]send to peer fail, ret:%d \r\n", slRet );
                    #if 0
                    for ( ulIndex = 0; ulIndex < MAX_RELAY_NUM; ulIndex++ )
                    {
                        close( slRelaySockFds[ ulIndex ] );
                    }
                    
                    for ( ulIndex = 0; ulIndex < MAX_RELAY_NUM; ulIndex++ )
                    {
                        close( slLocalSockFds[ ulIndex ] );
                    }
                    #endif
                    aos_task_delay( 1000 );
                    continue;
                }
    		}
        }

        /* check relay socket */
        for ( ulIndex = 0; ulIndex < MAX_RELAY_NUM; ulIndex++ )
        {
    		if ( FD_ISSET( slRelaySockFds[ ulIndex ], &slFdSet ) )
    		{
                /* recv request and update from addr */
                memset( strRecvBuff, 0x00, 128 );
                memset( &stRelayFromAddrs[ ulIndex ], 0x00, sizeof( SOCKADDRIN_S ) );
                slRelayFromLen   = sizeof( SOCKADDRIN_S );
                slRet            = recvfrom( slRelaySockFds[ ulIndex ], strRecvBuff, MAX_PACKET_SIZE, 0,
                                            (struct sockaddr *)&stRelayFromAddrs[ ulIndex ], (socklen_t *)&slRelayFromLen );
                if ( slRet <= 0 )
                {
                    printf( "\r\n[ERROR]recv peer fail, ret:%d \r\n", slRet );
                    #if 0
                    for ( ulIndex = 0; ulIndex < MAX_RELAY_NUM; ulIndex++ )
                    {
                        close( slRelaySockFds[ ulIndex ] );
                    }
                    
                    for ( ulIndex = 0; ulIndex < MAX_RELAY_NUM; ulIndex++ )
                    {
                        close( slLocalSockFds[ ulIndex ] );
                    }
                    #endif
                    aos_task_delay( 1000 );
                    continue;
                }

                gettimeofday( &stCurTime, NULL );

                if ( slDebugFlag )
                {
                    SCP_USER_MSG_S *        pstUserMsg  = (SCP_USER_MSG_S *)strRecvBuff;
                    SCP_MSG_S *             pstScpMsg   = (SCP_MSG_S *)pstUserMsg->aucScpMsg;

                    /* not ack packet */
                    slRecvRelayCount++;
                    printf( "\r\n[ %u.%u ]udp_relay_server local: %3d, peer: %3d, recv peer: relay[ %d ] <<< %s:%d, size: %d ",
                            (int)stCurTime.tv_sec, (int)(stCurTime.tv_usec/1000),
                            slRecvRelayCount, slRecvRelayCount,
                            usRelayPortNos[ ulIndex ],
                            ipaddrtostr( htonl( stRelayFromAddrs[ ulIndex ].sin_stAddr.s_ulAddr ), strBuff ), htons( stRelayFromAddrs[ ulIndex ].sin_usPort ),
                            slRet );

                    if ( TRUE == eid_print_scp_msg_header( ulIndex, (U8 *)strRecvBuff, slRet ) )
                    {
                        if ( slDebugSize > 0 )
                        {
                            if ( slDebugSize > slRet )
                            {
                                eid_print_mem_buffer( 0, 32, (U8 *)strRecvBuff, slRet );
                            }
                            else
                            {
                                eid_print_mem_buffer( 0, 32, (U8 *)strRecvBuff, slDebugSize );
                            }
                        }
                    }
                }

                /* proxy convert */
                #if 0
                if ( ulIndex == 6 )  // usctp header: [ server port: ** ** ] xx xx xx xx ...
                {
                    strRecvBuff[ 0 ]    = ( usRelayPortNos[ ulIndex ] >> 8 ) & 0x00FF;                    
                    strRecvBuff[ 1 ]    = usRelayPortNos[ ulIndex ] & 0x00FF;                    
                }
                #endif

                /* send to local addr */
                slRet = sendto( slLocalSockFds[ ulIndex ], strRecvBuff, slRet,
                                0, (struct sockaddr*)&stLocalFromAddrs[ ulIndex ], sizeof( SOCKADDRIN_S ) );
                if ( slRet < 0 )
                {
                    printf( "\r\n[ERROR]send to local fail, ret:%d \r\n", slRet );
                    #if 0
                    for ( ulIndex = 0; ulIndex < MAX_RELAY_NUM; ulIndex++ )
                    {
                        close( slRelaySockFds[ ulIndex ] );
                    }
                    
                    for ( ulIndex = 0; ulIndex < MAX_RELAY_NUM; ulIndex++ )
                    {
                        close( slLocalSockFds[ ulIndex ] );
                    }
                    #endif
                    aos_task_delay( 1000 );
                    continue;
                }
    		}
        }
    }

    printf( "\r\nudp_relay_server exit! \r\n" );
    for ( ulIndex = 0; ulIndex < MAX_RELAY_NUM; ulIndex++ )
    {
        close( slRelaySockFds[ ulIndex ] );
    }

    for ( ulIndex = 0; ulIndex < MAX_RELAY_NUM; ulIndex++ )
    {
        close( slLocalSockFds[ ulIndex ] );
    }
    return 0;
}

