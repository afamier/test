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

#define ________MD5_H________
/* md5 definitions */
#ifndef MD5_H  
#define MD5_H  
  
typedef struct  
{  
    unsigned int count[2];  
    unsigned int state[4];  
    unsigned char buffer[64];     
}MD5_CTX;  
  
#define F(x,y,z) ((x & y) | (~x & z))  
#define G(x,y,z) ((x & z) | (y & ~z))  
#define H(x,y,z) (x^y^z)  
#define I(x,y,z) (y ^ (x | ~z))  
#define ROTATE_LEFT(x,n) ((x << n) | (x >> (32-n)))  
#define FF(a,b,c,d,x,s,ac) \  
{ \  
    a += F(b,c,d) + x + ac; \  
    a = ROTATE_LEFT(a,s); \  
    a += b; \  
}  
#define GG(a,b,c,d,x,s,ac) \  
{ \  
    a += G(b,c,d) + x + ac; \  
    a = ROTATE_LEFT(a,s); \  
    a += b; \  
}  
#define HH(a,b,c,d,x,s,ac) \  
{ \  
    a += H(b,c,d) + x + ac; \  
    a = ROTATE_LEFT(a,s); \  
    a += b; \  
}  
#define II(a,b,c,d,x,s,ac) \  
{ \  
    a += I(b,c,d) + x + ac; \  
    a = ROTATE_LEFT(a,s); \  
    a += b; \  
}                                              
void MD5Init(MD5_CTX *context);  
void MD5Update(MD5_CTX *context,unsigned char *input,unsigned int inputlen);  
void MD5Final(MD5_CTX *context,unsigned char digest[16]);  
void MD5Transform(unsigned int state[4],unsigned char block[64]);  
void MD5Encode(unsigned char *output,unsigned int *input,unsigned int len);  
void MD5Decode(unsigned int *output,unsigned char *input,unsigned int len);  

#endif

#define ________MD5_C________

/* md5 functions */
unsigned char PADDING[]={0x80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};  
  
void MD5Init(MD5_CTX *context)  
{  
    context->count[0] = 0;  
    context->count[1] = 0;  
    context->state[0] = 0x67452301;  
    context->state[1] = 0xEFCDAB89;  
    context->state[2] = 0x98BADCFE;  
    context->state[3] = 0x10325476;  
}  
void MD5Update(MD5_CTX *context,unsigned char *input,unsigned int inputlen)  
{  
    unsigned int i = 0,index = 0,partlen = 0;  
    index = (context->count[0] >> 3) & 0x3F;  
    partlen = 64 - index;  
    context->count[0] += inputlen << 3;  
    if(context->count[0] < (inputlen << 3))  
        context->count[1]++;  
    context->count[1] += inputlen >> 29;  
  
    if(inputlen >= partlen)  
    {  
        memcpy(&context->buffer[index],input,partlen);  
        MD5Transform(context->state,context->buffer);  
        for(i = partlen;i+64 <= inputlen;i+=64)  
            MD5Transform(context->state,&input[i]);  
        index = 0;          
    }    
    else  
    {  
        i = 0;  
    }  
    memcpy(&context->buffer[index],&input[i],inputlen-i);  
}  
void MD5Final(MD5_CTX *context,unsigned char digest[16])  
{  
    unsigned int index = 0,padlen = 0;  
    unsigned char bits[8];  
    index = (context->count[0] >> 3) & 0x3F;  
    padlen = (index < 56)?(56-index):(120-index);  
    MD5Encode(bits,context->count,8);  
    MD5Update(context,PADDING,padlen);  
    MD5Update(context,bits,8);  
    MD5Encode(digest,context->state,16);  
}  
void MD5Encode(unsigned char *output,unsigned int *input,unsigned int len)  
{  
    unsigned int i = 0,j = 0;  
    while(j < len)  
    {  
        output[j] = input[i] & 0xFF;    
        output[j+1] = (input[i] >> 8) & 0xFF;  
        output[j+2] = (input[i] >> 16) & 0xFF;  
        output[j+3] = (input[i] >> 24) & 0xFF;  
        i++;  
        j+=4;  
    }  
}  
void MD5Decode(unsigned int *output,unsigned char *input,unsigned int len)  
{  
    unsigned int i = 0,j = 0;  
    while(j < len)  
    {  
        output[i] = (input[j]) |  
            (input[j+1] << 8) |  
            (input[j+2] << 16) |  
            (input[j+3] << 24);  
        i++;  
        j+=4;   
    }  
}  
void MD5Transform(unsigned int state[4],unsigned char block[64])  
{  
    unsigned int a = state[0];  
    unsigned int b = state[1];  
    unsigned int c = state[2];  
    unsigned int d = state[3];  
    unsigned int x[64];  
    MD5Decode(x,block,64);  
    FF(a, b, c, d, x[ 0], 7, 0xd76aa478); /* 1 */  
    FF(d, a, b, c, x[ 1], 12, 0xe8c7b756); /* 2 */  
    FF(c, d, a, b, x[ 2], 17, 0x242070db); /* 3 */  
    FF(b, c, d, a, x[ 3], 22, 0xc1bdceee); /* 4 */  
    FF(a, b, c, d, x[ 4], 7, 0xf57c0faf); /* 5 */  
    FF(d, a, b, c, x[ 5], 12, 0x4787c62a); /* 6 */  
    FF(c, d, a, b, x[ 6], 17, 0xa8304613); /* 7 */  
    FF(b, c, d, a, x[ 7], 22, 0xfd469501); /* 8 */  
    FF(a, b, c, d, x[ 8], 7, 0x698098d8); /* 9 */  
    FF(d, a, b, c, x[ 9], 12, 0x8b44f7af); /* 10 */  
    FF(c, d, a, b, x[10], 17, 0xffff5bb1); /* 11 */  
    FF(b, c, d, a, x[11], 22, 0x895cd7be); /* 12 */  
    FF(a, b, c, d, x[12], 7, 0x6b901122); /* 13 */  
    FF(d, a, b, c, x[13], 12, 0xfd987193); /* 14 */  
    FF(c, d, a, b, x[14], 17, 0xa679438e); /* 15 */  
    FF(b, c, d, a, x[15], 22, 0x49b40821); /* 16 */  
  
    /* Round 2 */  
    GG(a, b, c, d, x[ 1], 5, 0xf61e2562); /* 17 */  
    GG(d, a, b, c, x[ 6], 9, 0xc040b340); /* 18 */  
    GG(c, d, a, b, x[11], 14, 0x265e5a51); /* 19 */  
    GG(b, c, d, a, x[ 0], 20, 0xe9b6c7aa); /* 20 */  
    GG(a, b, c, d, x[ 5], 5, 0xd62f105d); /* 21 */  
    GG(d, a, b, c, x[10], 9,  0x2441453); /* 22 */  
    GG(c, d, a, b, x[15], 14, 0xd8a1e681); /* 23 */  
    GG(b, c, d, a, x[ 4], 20, 0xe7d3fbc8); /* 24 */  
    GG(a, b, c, d, x[ 9], 5, 0x21e1cde6); /* 25 */  
    GG(d, a, b, c, x[14], 9, 0xc33707d6); /* 26 */  
    GG(c, d, a, b, x[ 3], 14, 0xf4d50d87); /* 27 */  
    GG(b, c, d, a, x[ 8], 20, 0x455a14ed); /* 28 */  
    GG(a, b, c, d, x[13], 5, 0xa9e3e905); /* 29 */  
    GG(d, a, b, c, x[ 2], 9, 0xfcefa3f8); /* 30 */  
    GG(c, d, a, b, x[ 7], 14, 0x676f02d9); /* 31 */  
    GG(b, c, d, a, x[12], 20, 0x8d2a4c8a); /* 32 */  
  
    /* Round 3 */  
    HH(a, b, c, d, x[ 5], 4, 0xfffa3942); /* 33 */  
    HH(d, a, b, c, x[ 8], 11, 0x8771f681); /* 34 */  
    HH(c, d, a, b, x[11], 16, 0x6d9d6122); /* 35 */  
    HH(b, c, d, a, x[14], 23, 0xfde5380c); /* 36 */  
    HH(a, b, c, d, x[ 1], 4, 0xa4beea44); /* 37 */  
    HH(d, a, b, c, x[ 4], 11, 0x4bdecfa9); /* 38 */  
    HH(c, d, a, b, x[ 7], 16, 0xf6bb4b60); /* 39 */  
    HH(b, c, d, a, x[10], 23, 0xbebfbc70); /* 40 */  
    HH(a, b, c, d, x[13], 4, 0x289b7ec6); /* 41 */  
    HH(d, a, b, c, x[ 0], 11, 0xeaa127fa); /* 42 */  
    HH(c, d, a, b, x[ 3], 16, 0xd4ef3085); /* 43 */  
    HH(b, c, d, a, x[ 6], 23,  0x4881d05); /* 44 */  
    HH(a, b, c, d, x[ 9], 4, 0xd9d4d039); /* 45 */  
    HH(d, a, b, c, x[12], 11, 0xe6db99e5); /* 46 */  
    HH(c, d, a, b, x[15], 16, 0x1fa27cf8); /* 47 */  
    HH(b, c, d, a, x[ 2], 23, 0xc4ac5665); /* 48 */  
  
    /* Round 4 */  
    II(a, b, c, d, x[ 0], 6, 0xf4292244); /* 49 */  
    II(d, a, b, c, x[ 7], 10, 0x432aff97); /* 50 */  
    II(c, d, a, b, x[14], 15, 0xab9423a7); /* 51 */  
    II(b, c, d, a, x[ 5], 21, 0xfc93a039); /* 52 */  
    II(a, b, c, d, x[12], 6, 0x655b59c3); /* 53 */  
    II(d, a, b, c, x[ 3], 10, 0x8f0ccc92); /* 54 */  
    II(c, d, a, b, x[10], 15, 0xffeff47d); /* 55 */  
    II(b, c, d, a, x[ 1], 21, 0x85845dd1); /* 56 */  
    II(a, b, c, d, x[ 8], 6, 0x6fa87e4f); /* 57 */  
    II(d, a, b, c, x[15], 10, 0xfe2ce6e0); /* 58 */  
    II(c, d, a, b, x[ 6], 15, 0xa3014314); /* 59 */  
    II(b, c, d, a, x[13], 21, 0x4e0811a1); /* 60 */  
    II(a, b, c, d, x[ 4], 6, 0xf7537e82); /* 61 */  
    II(d, a, b, c, x[11], 10, 0xbd3af235); /* 62 */  
    II(c, d, a, b, x[ 2], 15, 0x2ad7d2bb); /* 63 */  
    II(b, c, d, a, x[ 9], 21, 0xeb86d391); /* 64 */  
    state[0] += a;  
    state[1] += b;  
    state[2] += c;  
    state[3] += d;  
}  

unsigned int md5_calc( char * pszInput, char * pszResult, unsigned int ulOutLen )
{
    MD5_CTX md5;  
    int i;  
    unsigned char decrypt[16];
    
    if ( ulOutLen < 32 )
    {
        return 1;
    }      

    MD5Init(&md5);           
    MD5Update(&md5,pszInput,strlen((char *)pszInput));  
    MD5Final(&md5,decrypt);
    
    memset( pszResult, 0x00, ulOutLen );
    for(i=0;i<16;i++)  
    {  
        sprintf( pszResult + strlen( pszResult ), "%02x", decrypt[i] );  
    }

    return 0;
}


#define ________TEST_H________
/* test definitions */
#define FALSE               0
#define TRUE                1

#define ASSERT( para )  if ( para == FALSE ) printf( "ASSERT!!! \n" )

#define MAX_NAME_STR_LEN    32
#define SERVER_XML_PORT     3030
#define MAX_XML_TEXT_LEN    64000
#define MAX_XML_NAME_LEN    128
#define MAX_XML_VAL_LEN     128
#define MAX_MD5_INFO_LEN    1280
#define MAX_MD5_HASH_LEN    128

#define LOCAL_TEST_PORT     0

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


/* format-1: msg_type="request" */
unsigned int get_xml_val_by_param( const char * pszLineStr, const char * pszParamStr, char * pszOutVal, unsigned int ulOutLen )
{
    char *  pszTempStr;
    char *  pszBeginPos;
    char *  pszEndPos;

    /* check name field */
    pszTempStr  = strstr( pszLineStr, pszParamStr );
    if ( pszTempStr == NULL )
    {
        /* discard */
        return 1;
    }

    pszTempStr  += strlen( pszParamStr );

    pszBeginPos = strstr( pszTempStr, "\"" );
    if ( pszBeginPos == NULL )
    {
        pszBeginPos = strstr( pszTempStr, "\'" );
    }
    
    if ( pszBeginPos == NULL )
    {
        ASSERT( 0 );
        printf( "[ERROR]get_xml_val_by_name() name:%s, val:NULL! \r\n", pszParamStr );
        return 1;
    }

    pszBeginPos++;
    pszEndPos   = strstr( pszBeginPos, "\"" );
    if ( pszEndPos == NULL )
    {
        pszEndPos   = strstr( pszBeginPos, "\'" );
    }
        
    if ( pszEndPos == NULL )
    {
        ASSERT( 0 );
        printf( "[ERROR]get_xml_val_by_name() name:%s, val:NULL! \r\n", pszParamStr );
        return 1;
    }

    memset( pszOutVal, 0x00, ulOutLen );
    strncpy( pszOutVal, pszBeginPos, ( pszEndPos - pszBeginPos ) );
    return 0;
}


/* format-1: name="SN" value="0" */
/* format-2: name='SN' value='0' */
unsigned int get_xml_val_by_name( const char * pszLineStr, const char * pszNameStr, char * pszOutVal, unsigned int ulOutLen )
{
    char *  pszTempStr;
    char *  pszBeginPos;
    char *  pszEndPos;
    char    strNameStr2[ 64 ];
    char    strNameStr3[ 64 ];

    /* check name field */
    snprintf( strNameStr2, 64, "\"%s\"", pszNameStr );
    snprintf( strNameStr3, 64, "\'%s\'", pszNameStr );
    pszTempStr  = strstr( pszLineStr, strNameStr2 );
    if ( pszTempStr == NULL )
    {
        pszTempStr  = strstr( pszLineStr, strNameStr3 );
    }

    if ( pszTempStr == NULL )
    {
        /* discard */
        return 1;
    }

    pszTempStr  += strlen( pszNameStr ) + 2;

    pszBeginPos = strstr( pszTempStr, "\"" );
    if ( pszBeginPos == NULL )
    {
        pszBeginPos = strstr( pszTempStr, "\'" );
    }
    
    if ( pszBeginPos == NULL )
    {
        ASSERT( 0 );
        printf( "[ERROR]get_xml_val_by_name() name:%s, val:NULL! \r\n", pszNameStr );
        return 1;
    }

    pszBeginPos++;
    pszEndPos   = strstr( pszBeginPos, "\"" );
    if ( pszEndPos == NULL )
    {
        pszEndPos   = strstr( pszBeginPos, "\'" );
    }
        
    if ( pszEndPos == NULL )
    {
        ASSERT( 0 );
        printf( "[ERROR]get_xml_val_by_name() name:%s, val:NULL! \r\n", pszNameStr );
        return 1;
    }

    memset( pszOutVal, 0x00, ulOutLen );
    strncpy( pszOutVal, pszBeginPos, ( pszEndPos - pszBeginPos ) );
    return 0;
}


int convert_xml_content( char * pszContent, char * pszOutBuff, int ulOutLen )
{
    unsigned int        ulIndex;
    unsigned int        ulOffset;
    unsigned int        ulSpaceIndex;
    unsigned int        ulSpaceCount;
    unsigned int        ulTabCount;
    unsigned int        ulLineBeginFlag;
    unsigned int        ulLineTabFlag;
    
    memset( pszOutBuff, 0x00, ulOutLen );
    
    ulOffset        = 0;
    ulTabCount      = 0;
    ulLineBeginFlag = 0;
    ulLineTabFlag   = 0;
    for ( ulIndex = 0; ulIndex < strlen( pszContent ); ulIndex++ )
    {
        if ( ulOffset > 0
          && pszContent[ ulIndex ] == '<'
          && ulLineBeginFlag == 0 )
        {
            ulLineBeginFlag = 1;

            if ( pszContent[ ulIndex + 1 ] == '/' )
            {
                ulLineTabFlag   = 0;
            }
            else
            {
                ulLineTabFlag   = 1;
                ulTabCount++;
            }
            
            pszOutBuff[ ulOffset++ ] = '\r';
            pszOutBuff[ ulOffset++ ] = '\n';

            if ( ulTabCount > 1 )
            {
                ulSpaceCount    = ( ulTabCount - 1 ) * 4;
            }
            else
            {
                ulSpaceCount    = 0;
            }

            for ( ulSpaceIndex = 0; ulSpaceIndex < ulSpaceCount; ulSpaceIndex++ )
            {
                pszOutBuff[ ulOffset++ ] = ' ';
            }

            pszOutBuff[ ulOffset++ ] = '<';
        }
        else if ( pszContent[ ulIndex ] == '/'
               && pszContent[ ulIndex + 1 ] == '>' )
        {
            if ( ulTabCount > 0 )
            {
                ulTabCount--;
            }
            pszOutBuff[ ulOffset++ ] = pszContent[ ulIndex ];
        }
        else if ( pszContent[ ulIndex ] == '>'
               && ulLineBeginFlag == 1 )
        {
            /* end of line */
            ulLineBeginFlag = 0;
            if ( ulLineTabFlag == 0 )
            {
                if ( ulTabCount > 0 )
                {
                    ulTabCount--;
                }
            }
            ulLineTabFlag   = 0;
            pszOutBuff[ ulOffset++ ] = pszContent[ ulIndex ];
        }
        else if ( pszContent[ ulIndex ] == ' '
               && ulLineBeginFlag == 0 )
        {
            /* filter [SPACE] at line header */
            continue;
        }
        else if ( pszContent[ ulIndex ] == '\r'
               || pszContent[ ulIndex ] == '\n' )
        {
            /* replace inline \r\n */
            pszOutBuff[ ulOffset++ ] = ' ';
        }
        else
        {
            pszOutBuff[ ulOffset++ ] = pszContent[ ulIndex ];
        }

        if ( ulOffset >= MAX_XML_TEXT_LEN )
        {
            printf( "[ERROR]too long response, len:%d, limit:%d \r\n", strlen( pszContent ), MAX_XML_TEXT_LEN );
            return -1;
        }
    }

    return ulOffset;
}


int main( int argc, char * argv[] )
{
    int                 slSockFd;  /* socket fd, based on usServiceApiPort */
    SOCKADDRIN_S        stSockAddr;
    int                 slSockAddrLen;
    unsigned int        ulLocalIpAddr;
    unsigned int        ulServerIpAddr;
    unsigned short int  usServerPortNo;
    const char *        pszAuthPwd;
    unsigned int        ulSerialNo;
    unsigned int        ulIndex;
    unsigned int        ulOffset;
    unsigned int        ulSpaceIndex;
    unsigned int        ulSpaceCount;
    unsigned int        ulTabCount;
    unsigned int        ulLineBeginFlag;
    unsigned int        ulLineEndFlag;
    const char *        pszFileName;
    const char *        pszTempStr;
    FILE *              hdlFile;
    char                strLine[ MAX_XML_TEXT_LEN ];
    FD_SET_S            slFdSet;
    TIMEVAL_S           stTimeout;

    char                strXmlName[ MAX_XML_NAME_LEN ];
    char                strXmlVal[ MAX_XML_VAL_LEN ];
    char                strMd5Info[ MAX_MD5_INFO_LEN ];
    char                strMd5Result[ MAX_MD5_HASH_LEN ];
    
    char                strXmlTextReq[ MAX_XML_TEXT_LEN ];
    char                strXmlTextResp[ MAX_XML_TEXT_LEN ];
    char                strXmlTextBuff[ 2 * MAX_XML_TEXT_LEN ];
    
    char                strCmdName[ MAX_XML_NAME_LEN ];
    int                 slRet;

#if 1
    /* check argc */
    if ( argc < 5 )
    {
        printf( "Usage: %s <server_ip> <server_port> <auth_pwd> <xml_file> [SN] \r\n", argv[ 0 ] );
        printf( "       server_ip   - ip address of sim server \r\n" );
        printf( "       server_port - default port is 3030 \r\n" );
        printf( "       auth_pwd    - be set in api settings of user account \r\n" );
        printf( "       xml_file    - file path of xml request \r\n" );
        printf( "       SN          - xml request SN, plus 1 each time \r\n" );
        printf( "                     Notes: auth_info = md5_32{ \"msg_type\",\"SN\",\"Domain\",\"User\",\"Cmd\",\"Timestamp\",\"auth_pwd\"} \r\n" );
        return 1;
    }

    strtoipaddr( argv[ 1 ], &ulServerIpAddr );
    usServerPortNo  = atoi( argv[ 2 ] );
    pszAuthPwd      = argv[ 3 ];
    pszFileName     = argv[ 4 ];
    if ( argc < 6 )
    {
        ulSerialNo  = 0;
    }
    else
    {
        ulSerialNo  = atoi( argv[ 5 ] );
    }
#else
    strtoipaddr( "127.0.0.1", &ulServerIpAddr );
    usServerPortNo  = 3030;
    pszAuthPwd      = "123456";
    pszFileName     = "./Heartbeat.xml";
    ulSerialNo      = 0;
#endif

    /* load xml_req_file */
    {
        memset( strXmlTextReq, 0x00, MAX_XML_TEXT_LEN );
        memset( strMd5Info, 0x00, MAX_MD5_INFO_LEN );

        /* open orig file */
        hdlFile = fopen( pszFileName, "r");
        if( hdlFile == NULL )
        {
            ASSERT( 0 );
            printf( "[ERROR]open file fail, filename:%s \r\n", pszFileName );
            return 1;
        }

        while( !feof( hdlFile ) )
        {
            /* read line */
            memset( strLine, 0x00, MAX_XML_TEXT_LEN );
            fgets( strLine, ( MAX_XML_TEXT_LEN - 1 ), hdlFile );
            
            // printf( "Get Line: %s \r\n", strLine );
            
            /* md5{ msg_type, SN, Domain, User, Cmd, Timestamp, api_auth_pwd } */
            if ( 0 == get_xml_val_by_param( strLine, "msg_type", strXmlVal, MAX_XML_VAL_LEN ) )
            {
                sprintf( strMd5Info + strlen( strMd5Info ), strXmlVal );
                sprintf( strXmlTextReq + strlen( strXmlTextReq ), "%s", strLine );
            }
            else if ( 0 == get_xml_val_by_name( strLine, "SN", strXmlVal, MAX_XML_VAL_LEN ) )
            {
                sprintf( strXmlVal, "%u", ulSerialNo );
                sprintf( strMd5Info + strlen( strMd5Info ), strXmlVal );
                sprintf( strXmlTextReq + strlen( strXmlTextReq ), "        <param name=\"SN\" value=\"%u\" />\r\n", ulSerialNo );
            }
            else if ( 0 == get_xml_val_by_name( strLine, "Domain", strXmlVal, MAX_XML_VAL_LEN ) )
            {
                sprintf( strMd5Info + strlen( strMd5Info ), strXmlVal );
                sprintf( strXmlTextReq + strlen( strXmlTextReq ), "%s", strLine );
            }
            else if ( 0 == get_xml_val_by_name( strLine, "User", strXmlVal, MAX_XML_VAL_LEN ) )
            {
                sprintf( strMd5Info + strlen( strMd5Info ), strXmlVal );
                sprintf( strXmlTextReq + strlen( strXmlTextReq ), "%s", strLine );
            }
            else if ( 0 == get_xml_val_by_name( strLine, "Cmd", strXmlVal, MAX_XML_VAL_LEN ) )
            {
                sprintf( strMd5Info + strlen( strMd5Info ), strXmlVal );
                sprintf( strXmlTextReq + strlen( strXmlTextReq ), "%s", strLine );

                // record cmd name
                sprintf( strCmdName, strXmlVal );
            }
            else if ( 0 == get_xml_val_by_name( strLine, "Retries", strXmlVal, MAX_XML_VAL_LEN ) )
            {
                // sprintf( strMd5Info + strlen( strMd5Info ), strXmlVal );
                sprintf( strXmlTextReq + strlen( strXmlTextReq ), "%s", strLine );
            }
            else if ( 0 == get_xml_val_by_name( strLine, "\"Timeout\"", strXmlVal, MAX_XML_VAL_LEN ) )
            {
                // sprintf( strMd5Info + strlen( strMd5Info ), strXmlVal );
                sprintf( strXmlTextReq + strlen( strXmlTextReq ), "%s", strLine );
            }
            else if ( 0 == get_xml_val_by_name( strLine, "Timestamp", strXmlVal, MAX_XML_VAL_LEN ) )
            {
                sprintf( strMd5Info + strlen( strMd5Info ), strXmlVal );
                sprintf( strXmlTextReq + strlen( strXmlTextReq ), "%s", strLine );
            }
            else if ( 0 == get_xml_val_by_name( strLine, "AuthInfo", strXmlVal, MAX_XML_VAL_LEN ) )
            {
                sprintf( strMd5Info + strlen( strMd5Info ), pszAuthPwd );
                md5_calc( strMd5Info, strMd5Result, MAX_MD5_HASH_LEN );
                // printf( "Request MD5 Info: %s >> %s \r\n", strMd5Info, strMd5Result );

                sprintf( strXmlTextReq + strlen( strXmlTextReq ), "        <param name=\"AuthInfo\" value=\"%s\" />\r\n", strMd5Result );
            }
            else
            {
                sprintf( strXmlTextReq + strlen( strXmlTextReq ), "%s", strLine );
            }
        }

        /* close file */
        fclose( hdlFile );
    }

    /* init socket */
    slSockFd     = -1;
    slSockFd = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
    if ( slSockFd <= 0 )
    {
        ASSERT( 0 );
        printf( "[ERROR]open socket fail, port:%u, errcode:%d \r\n",
                LOCAL_TEST_PORT, slSockFd );
        return 1;
    }

    strtoipaddr( "0.0.0.0", &ulLocalIpAddr );

    stSockAddr.sin_chFamily         = AF_INET;
    stSockAddr.sin_stAddr.s_ulAddr  = htonl( ulLocalIpAddr );
    stSockAddr.sin_usPort           = htons( LOCAL_TEST_PORT );
    slSockAddrLen                   = sizeof( SOCKADDRIN_S );
    slRet   = bind( slSockFd, (struct sockaddr *)&stSockAddr, slSockAddrLen );
    if( slRet < 0 )
    {
        ASSERT( 0 );
        printf( "[ERROR]bind socket fail, port:%u, errcode:%d \r\n",
                LOCAL_TEST_PORT, slSockFd );
        close( slSockFd );
        return 1;
    }

    /* send xml_req to server */
    stSockAddr.sin_chFamily         = AF_INET;
    stSockAddr.sin_stAddr.s_ulAddr  = htonl( ulServerIpAddr );
    stSockAddr.sin_usPort           = htons( (unsigned short int)usServerPortNo );
    slSockAddrLen                   = sizeof( SOCKADDRIN_S );
    slRet = sendto( slSockFd, strXmlTextReq, strlen( strXmlTextReq ),
                    0, (struct sockaddr*)&stSockAddr, sizeof( SOCKADDRIN_S ) );
    if ( slRet < 0 )
    {
        ASSERT( 0 );
        printf( "[ERROR]send fail, ret:%d \r\n", slRet );
        close( slSockFd );
        return 1;
    }

    printf( "\r\nSend XML Request To: >>> %s:%d, len:%d, MD5:\"%s\" \r\n",
            ipaddrtostr( ulServerIpAddr, strLine ), (unsigned short int)usServerPortNo, 
            strlen( strXmlTextReq ), strMd5Info );

    printf( "----------------------------------------------------------------------\r\n" );
    printf( "%s\r\n", strXmlTextReq );

    /* check packet arrived? */
    stTimeout.tv_sec    = 5;    /* timeout after 5 seconds */
    stTimeout.tv_usec   = 0;

    FD_ZERO( &slFdSet );
    FD_SET( slSockFd, &slFdSet );

    slRet = select( slSockFd + 1, &slFdSet, NULL, NULL, (struct timeval *)&stTimeout );
    if ( slRet <= 0 )
    {
        ASSERT( 0 );
        printf( "[ERROR]recv timeout, len: %d sec, ret:%d \r\n", (unsigned int)stTimeout.tv_sec, slRet );
        close( slSockFd );
        return 1;
    }

    /* recv response */
    memset( strXmlTextResp, 0x00, MAX_XML_TEXT_LEN );
    memset( &stSockAddr, 0x00, sizeof( SOCKADDRIN_S ) );
    slSockAddrLen   = sizeof( SOCKADDRIN_S );
    slRet           = recvfrom( slSockFd, strXmlTextResp, MAX_XML_TEXT_LEN, 0,
                                (struct sockaddr *)&stSockAddr, (socklen_t *)&slSockAddrLen );
    if ( slRet <= 0 )
    {
        ASSERT( 0 );
        printf( "[ERROR]recv fail, ret:%d \r\n", slRet );
        close( slSockFd );
        return 1;
    }

#if 0
    strcpy( strXmlTextBuff, strXmlTextResp );
#else
    /* convert and output response */
    convert_xml_content( strXmlTextResp, strXmlTextBuff, 2 * MAX_XML_TEXT_LEN );
#endif

    printf( "Recv XML Response From: <<< %s:%d, len:%d \r\n",
            ipaddrtostr( htonl( stSockAddr.sin_stAddr.s_ulAddr ), strLine ), htons( stSockAddr.sin_usPort ),
            slRet );
    printf( "----------------------------------------------------------------------\r\n" );
    printf( "%s\r\n", strXmlTextBuff );
    printf( "----------------------------------------------------------------------\r\n" );

    // =======================================================================================================
    
    if ( strcmp( strCmdName, "SendSpecSms" ) == 0
      || strcmp( strCmdName, "SendSpecUssd" ) == 0
      || strcmp( strCmdName, "SendSpecCall" ) == 0 )
    {
        printf( "Wait Trap Packet ... press [ Ctrl + C ] to quit \r\n" );

        /* recv response */
        memset( strXmlTextResp, 0x00, MAX_XML_TEXT_LEN );
        memset( &stSockAddr, 0x00, sizeof( SOCKADDRIN_S ) );
        slSockAddrLen   = sizeof( SOCKADDRIN_S );
        slRet           = recvfrom( slSockFd, strXmlTextResp, MAX_XML_TEXT_LEN, 0,
                                    (struct sockaddr *)&stSockAddr, (socklen_t *)&slSockAddrLen );
        if ( slRet <= 0 )
        {
            ASSERT( 0 );
            printf( "[ERROR]recv fail, ret:%d \r\n", slRet );
            close( slSockFd );
            return 1;
        }

#if 0
        strcpy( strXmlTextBuff, strXmlTextResp );
#else
        /* convert and output response */
        convert_xml_content( strXmlTextResp, strXmlTextBuff, 2 * MAX_XML_TEXT_LEN );
#endif
        printf( "Recv XML Trap From: <<< %s:%d, len:%d \r\n",
                ipaddrtostr( htonl( stSockAddr.sin_stAddr.s_ulAddr ), strLine ), htons( stSockAddr.sin_usPort ),
                slRet );
        printf( "----------------------------------------------------------------------\r\n" );
        printf( "%s\r\n", strXmlTextBuff );
        printf( "----------------------------------------------------------------------\r\n" );
    }

    close( slSockFd );
    return 0;
}

