#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <netinet/if_ether.h>

#include <sys/ioctl.h>
#include <net/if_arp.h>
#include <net/if.h>
#include <netinet/in.h>


#define DEBUG_FLAG          0

/* global definitions */
typedef void                VOID;
typedef char                S8;
typedef short               S16;
typedef int                 S32;
typedef float               F32;
typedef unsigned char       U8;
typedef unsigned short int  U16;
typedef unsigned int        U32;
typedef U32                 BOOL_T;

#define AOS_ATTRIBUTE  __attribute__ ((packed,aligned(1)))

/* struct definitions */
typedef struct tagINADDR
{
    U32 s_ulAddr;
}AOS_ATTRIBUTE  INADDR_S;

typedef struct tagSOCKADDRIN
{
    U16  sin_chFamily;
    U16 sin_usPort;
    INADDR_S sin_stAddr;
    S8            sin_chZero_a[8];
}AOS_ATTRIBUTE   SOCKADDRIN_S;

typedef fd_set FD_SET_S;

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
/* -------------------------------- macro ----------------------------------*/

#define SCP_VERSION             0x01
#define SCP_FLAG_ENCRYPT        0x01            // bit0
#define SCP_FLAG_MULTI_MSG      0x02            // bit1

#define DEVICE_ID_LEN           8
#define MAC_LEN                 6
#define HASH_LEN                16
#define SCP_KEY_LEN             16
#define SCP_TEA_KEY_LEN         4

#define NAME_STR_LEN            31
#define VER_MOD_LEN             15
#define VER_STR_LEN             31
#define VER_BUILD_TIME_LEN      31

#define VER_MOD_PACKAGE         "package"       // version info: 02300501, 02-vendor id; 30-product id; 0501-version id;

#define ICC_LEN                 31
#define ATR_LEN                 32
#define IMSI_LEN                15
#define IMEI_LEN                15              // be used for GSM, 十进制
#define MEID_LEN                IMEI_LEN        // be used for CDMA, 16进制
#define PIN_LEN                 8
#define PUK_LEN                 8
#define MAX_NUMBER_LEN          24
#define MAX_MODULE_PARAM_LEN    32
#define MAX_TIME_LEN            14              // 时间定义为 20120101183005   YYYYMMDDHHMMSS

#define SCP_MAX_GSM_OPERATOR_CODE_LEN  6

#define MAX_URL_LEN             127
#define IP_ADDRESS_LEN          31
#define MAX_USERNAME_LEN        63
#define MAX_PASSWORD_LEN        31
#define MAX_NTP_SERVER_LEN      127

#define SMS_LEN                 2047            // ( ref. 单条: 160字符, 或者 70汉字 )
#define USSD_LEN                255             // ( ref. ??? )
#define SMSC_LEN                31              // e.g. +8613800100500
#define MAX_NUMBER_COUNT        64              // 群发号码最大数目

#define MAX_SCP_MSG_SIZE        4000                // AOS Msg Limitation <= 4096 Bytes
#define REGISTER_MSG_LEN        MAX_SCP_MSG_SIZE    // 用于MTU连通性探测, 必须大于1500
#define REGISTER_ACK_MSG_LEN    MAX_SCP_MSG_SIZE    // 用于MTU连通性探测, 必须大于1500

#define MODULE_NO_SIGNAL        99              // 无线信号强度:   0(null) ~ 31(full), 99 - error;

/*sip server地址最大长度*/
#define  MAX_SIP_SERVER_LEN     127

/* -------------------------------- enums -----------------------------------*/

/* -------------------------------- macro ----------------------------------*/

/* sim-server max specifications */
#define MAX_HASH_K                  2  /* HASH表尺寸相对于对象数目的倍数 */

#define MAX_SYS_HASH_NUM            ( MAX_HASH_K * 100 ) /* 100台服务器 */
#define MAX_PROC_HASH_NUM           ( MAX_HASH_K * 1000 ) /* 每台服务器最大10个处理线程 */
#define MAX_NODE_HASH_NUM           ( MAX_HASH_K * 100 ) /* 100台服务器 */
#define MAX_DOMAIN_HASH_NUM         ( MAX_HASH_K * 100 ) /* 每台服务器管理的domain最大数目 */
#define MAX_USER_HASH_NUM           ( MAX_HASH_K * 1000 ) /* 每台服务器管理的user最大数目 */
#define MAX_NE_HASH_NUM             ( MAX_HASH_K * 1000 ) /* 每台服务器管理的ne最大数目 */
#define MAX_ZONE_HASH_NUM           ( MAX_HASH_K * 500 ) /* 每台服务器管理的zone最大数目 */
#define MAX_SITE_HASH_NUM           ( MAX_HASH_K * 1000 ) /* 每台服务器管理的site最大数目 */
#define MAX_POLICY_HASH_NUM         ( MAX_HASH_K * 1000 ) /* 每台服务器管理的policy最大数目 */
#define MAX_RULE_HASH_NUM           ( MAX_HASH_K * 5000 ) /* 每台服务器管理的rule最大数目 */
#define MAX_GRP_HASH_NUM            ( MAX_HASH_K * 5000 ) /* 每台服务器管理的grp最大数目 */
#define MAX_SIM_HASH_NUM            ( MAX_HASH_K * 50000 ) /* 每台服务器管理的sim最大数目 */
#define MAX_PORT_HASH_NUM           ( MAX_HASH_K * 50000 ) /* 每台服务器管理的port最大数目 */

/* definitions based on scp_def.h */
#define PRODUCT_SN_LEN              ( DEVICE_ID_LEN )
#define MAX_VER_STR_LEN             ( VER_STR_LEN + 1 )
#define MAX_NAME_STR_LEN            ( NAME_STR_LEN + 1 )
#define MAX_ALIAS_STR_LEN           ( NAME_STR_LEN + 1 )
#define IMEI_STR_LEN                ( IMEI_LEN + 1 )
#define MEID_STR_LEN                ( MEID_LEN + 1 )
#define IMSI_STR_LEN                ( IMSI_LEN + 1 )
#define SMSC_STR_LEN                ( SMSC_LEN + 1 )
#define PUK_STR_LEN                 ( PUK_LEN + 1 )
#define ICC_STR_LEN                 ( ICC_LEN + 1 )
#define PIN_STR_LEN                 ( PIN_LEN + 1 )
#define MAX_NTP_SERVER_STR_LEN      ( MAX_NTP_SERVER_LEN + 1 )
#define VER_MOD_STR_LEN             ( VER_MOD_LEN + 1 )
#define MAX_URL_STR_LEN             ( MAX_URL_LEN + 1 )
#define MAX_USERNAME_STR_LEN        ( MAX_USERNAME_LEN + 1 )
#define MAX_PASSWORD_STR_LEN        ( MAX_PASSWORD_LEN + 1 )
#define MAX_TIME_STR_LEN            ( 32 )                          // MAX_TIME_LEN is too short, e.g. 2012-05-30 00:00:00
#define IP_ADDRESS_STR_LEN          ( IP_ADDRESS_LEN + 1 )
#define SIM_OPERATOR_STR_LEN        ( MAX_NAME_STR_LEN )
#define MAX_SIP_SERVER_STR_LEN      ( MAX_SIP_SERVER_LEN + 1 )
#define MD5_HASH_STR_LEN            ( 2 * HASH_LEN + 1 )            // 32 + zero

/* sim-server definitions */
#define MAX_LONG_ALIAS_STR_LEN      64

#define NE_AUTH_LOCKED_TIMELEN      60      /* 认证失败, 锁定时长, 不允许频繁认证 */
#define NE_STATUS_REPORT_INTERVAL   6       // 秒
#define NE_REG_AUTH_WAIT_COUNT      2       // 2 * TASK_DOMAIN_CHECK_TMR_LEN, 过滤重复注册请求
#define PASSWORD_MD5_STR_LEN        32      /* 密码MD5长度 */
#define MAX_CLI_PROMPT_LEN          32
#define DETAIL_DESC_STR_LEN         256
#define SMS_STR_LEN                 ( SMS_LEN + 1 ) //push alarm by sms, so limit the length of SMS_STR_LEN must large than ALM_MAX_MAIL_CMD_LEN and ALM_PUSH_SMS_CONTENT_LEN
#define USSD_STR_LEN                ( USSD_LEN + 1 )

#define SMS_AUTO_STR_LEN            128
#define USSD_AUTO_STR_LEN           SMS_AUTO_STR_LEN
#define PAID_LIST_STR_LEN           64
#define DTMF_TEST_STR_LEN           8
#define MAX_NUMBER_STR_LEN         ( MAX_NUMBER_LEN + 1 )
#define MAX_NUMBER_PREFIX_LEN       8

#define MAX_KEYS_STR_LEN            64
#define MAX_KEYS_COUNT              10
#define MAX_FLOAT_STR_LEN           32
#define MAX_FAIL_STR_LEN            64

#define MAX_NE_PORT_COUNT           128     // TODO: only for debugging!!!
#define MAX_NODE_LIST_NUM           16

#define MAX_ALARM_NAME_STR_LEN      64

#define MAX_AUTH_STR_LEN            64
#define MAX_RSA_KEY_STR_LEN         256
#define MAX_LICENSE_STR_LEN         256

#define MAX_XML_BUFFER_LEN          32768     /* text mode, be enough for RECORD_SML_S, RECORD_USSL_S */
#define MAX_XML_GET_COUNT           128       /* MaxGetCount */
#define XML_NA                      ""        /* NA or EMPTY */
/* -------------------------------- enum ----------------------------------*/

/* ============================= structures ================================*/
typedef struct sys_time_s
{
	U16    year;
	U8     month;
	U8     date;
	U8     hour;
	U8     minute;
	U8     second;
	U8     week;/*1~7*/
	U32   millisec;/*time in milliseconds that less than 1 second*/
} SYS_TIME_S;

typedef struct sys_conf_s
{
    /* --------------- system conf info --------------- */
    U32                     ulSysUuid;                              /* 系统标识, 本地编号 */
    S8                      strSysAlias[ MAX_ALIAS_STR_LEN ];       /* 系统别名, 本地命名 */
    S8                      strPubIpAddr[ IP_ADDRESS_STR_LEN ];     /* 公网IP */
    S8                      strEthIpAddr[ IP_ADDRESS_STR_LEN ];     /* 内网IP */
    S8                      strStunListenEth[ MAX_NAME_STR_LEN ];   /* eth0/eth1 */
    U16                     usStunUdpPort;                          /* stun: 13478 */
    U16                     usStunTcpPort;                          /* stun: 13478 */
    U16                     usStunUdpPort2;                         /* stun: 18478 */
    U16                     usStunTcpPort2;                         /* stun: 18478 */
    U16                     usTransListenPort;                      /* sctp: 2020 */
    U16                     usWebProxyPort;                         /* web proxy port: 8080 */
    U16                     usServiceApiPort;                       /* service api port: 3030 */
    U16                     usSysResvPort;                          /* sys resv port: unused */

    S8                      strMysqlUrl[ MAX_URL_STR_LEN ];         /* localhost */
    U16                     usMysqlPort;                            /* 3060 */
    S8                      strMysqlUser[ MAX_USERNAME_STR_LEN ];   /* root */
    S8                      strMysqlPwd[ MAX_PASSWORD_STR_LEN ];    /* 111111 */

    S8                      strProvUrl[ MAX_URL_STR_LEN ];          /* localhost */
    S8                      strProvUser[ MAX_USERNAME_STR_LEN ];    /* ftpuser */
    S8                      strProvPwd[ MAX_PASSWORD_STR_LEN ];     /* 111111 */

    /* --------------- internal conf info --------------- */
    S8                      strMysqlUrl02[ MAX_URL_STR_LEN ];       /* localhost */
    U16                     usMysqlPort02;                          /* 3060 */
    U16                     usIceIdleTimelen;                       /* 10 */
    U16                     usSrvCheckTimelen;                      /* 3600 */
    S8                      strLogServer[ MAX_URL_STR_LEN ];        /* localhost */
    S8                      strDnsDomain01[ MAX_NAME_STR_LEN ];     /* dimiccs.com */
    S8                      strDnsDomain02[ MAX_NAME_STR_LEN ];     /* changyoumifi.com */
} SYS_CONF_S;

typedef struct sys_license_s
{
    /* original info */
    S8              strVersion[ MAX_NAME_STR_LEN ];
    S8              strClass[ MAX_NAME_STR_LEN ];       /* IM_LICENSE_CLASS_TYPE: server, domain */
    S8              strSrvUuid[ MAX_NAME_STR_LEN ];
    S8              strSrvMode[ MAX_NAME_STR_LEN ];     /* IM_SERVER_MODE_TYPE: local, cloud */
    S8              strSrvDomain[ MAX_NAME_STR_LEN ];
    S8              strSrvMagic[ MD5_HASH_STR_LEN ];    /* md5{ host-id, mac, sda ... }, based on license version */
    S8              strDnsUrl[ MAX_URL_STR_LEN ];
    S8              strDnsUrl02[ MAX_URL_STR_LEN ];
    S8              strMaxSimCard[ MAX_NAME_STR_LEN ];
    S8              strHbmFeatures[ MAX_NAME_STR_LEN ];
    S8              strServiceApi[ MAX_NAME_STR_LEN ];
    S8              strSerialNo[ MAX_NAME_STR_LEN ];
    S8              strSignType[ MAX_NAME_STR_LEN ];
    S8              strSignAuthor[ MAX_NAME_STR_LEN ];
    S8              strSignDate[ MAX_NAME_STR_LEN ];
    S8              strAvailableDays[ MAX_NAME_STR_LEN ];
    S8              strExpiredDate[ MAX_NAME_STR_LEN ];
    S8              strAuthUrl[ MAX_URL_STR_LEN ];

    S8              strHbmFeatures02[ MAX_NAME_STR_LEN ];
    S8              strHbmFeatures03[ MAX_NAME_STR_LEN ];
    S8              strValidDays[ MAX_NAME_STR_LEN ];

    S8              strAuthInfo[ MD5_HASH_STR_LEN ];
}SYS_LICENSE_S;


typedef struct record_sys_s
{
    BOOL_T                          bDebugFlag;

    SYS_CONF_S                      stConfInfo;         /* sys conf配置信息 */
    SYS_LICENSE_S                   stLicenseInfo;      /* sys license信息 */
} RECORD_SYS_S;

typedef struct sys_s
{
    /* db record info */
    RECORD_SYS_S            stRecordSys;
} SYS_S;


/* 根节点的跟踪管理数据结构 */
typedef struct record_root_s
{
    U32         ulLocalSysUuid;                         /* 本系统sys uuid */
}RECORD_ROOT_S;


/* 根节点的数据结构 */
typedef struct root_s
{
    RECORD_ROOT_S                   stRecordRoot;
} ROOT_S;


/* ============================= md5 ================================*/

#define SNMP_MD5_AUTH_HASHLEN 16
typedef U8 SNMPHASH[SNMP_MD5_AUTH_HASHLEN];
#define SNMP_MD5_AUTH_HASHHEXLEN 32
typedef U8 SNMPHASHHEX[SNMP_MD5_AUTH_HASHHEXLEN+1];

/* MD5 context. */
typedef struct {
  U32 state[4];   /* state (ABCD) */
  U32 count[2];   /* number of bits, modulo 2^64 (lsb first) */
  U8 buffer[64]; /* input buffer */
} MD5_CTX;

VOID MD5Calc(U8 *output, U8 *input,U32 inlen);
VOID MD5Init(MD5_CTX *context);
VOID MD5Update(MD5_CTX *context, U8 *input, U32 inputLen);
VOID MD5Final(U8 digest[16], MD5_CTX *context);
void MD5_cvthex(SNMPHASH Bin,SNMPHASHHEX Hex);

/* Constants for MD5Transform routine.
 */
#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define SS32 11  /*Conflict with data type "S32" , by steven*/
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21



static U8 PADDING[64] = {
  0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* F, G, H and I are basic MD5 functions.
 */
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

/* ROTATE_LEFT rotates x left n bits.
 */
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

/* FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
Rotation is separate from addition to prevent recomputation.
 */
#define FF(a, b, c, d, x, s, ac) { \
 (a) += F ((b), (c), (d)) + (x) + (U32)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
#define GG(a, b, c, d, x, s, ac) { \
 (a) += G ((b), (c), (d)) + (x) + (U32)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
#define HH(a, b, c, d, x, s, ac) { \
 (a) += H ((b), (c), (d)) + (x) + (U32)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
#define II(a, b, c, d, x, s, ac) { \
 (a) += I ((b), (c), (d)) + (x) + (U32)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }

static VOID MD5Transform (U32 [4], U8 [64]);
static VOID Encode (U8 *, U32 *, U32);
static VOID Decode (U32 *, U8 *, U32);
static VOID MD5_memcpy(U8 *, U8 *, U32);
static VOID MD5_memset(U8 *, S32, U32);

VOID MD5Calc(U8 *output, U8 *input,U32 inlen)
{
	MD5_CTX	context;

	MD5Init(&context);
	MD5Update(&context, input, inlen);
	MD5Final(output, &context);
}

/* MD5 initialization. Begins an MD5 operation, writing a new context.
 */
VOID MD5Init(MD5_CTX *context)
{
  context->count[0] = context->count[1] = 0;
  /* Load magic initialization constants.
*/
  context->state[0] = 0x67452301;
  context->state[1] = 0xefcdab89;
  context->state[2] = 0x98badcfe;
  context->state[3] = 0x10325476;
}

/* MD5 block update operation. Continues an MD5 message-digest
  operation, processing another message block, and updating the
  context.
 */
VOID MD5Update(MD5_CTX *context, U8 *input, U32 inputLen)
{
  U32 i, index, partLen;

  /* Compute number of bytes mod 64 */
  index = (U32)((context->count[0] >> 3) & 0x3F);

  /* Update number of bits */
  if ((context->count[0] += ((U32)inputLen << 3))
   < ((U32)inputLen << 3))
 context->count[1]++;
  context->count[1] += ((U32)inputLen >> 29);

  partLen = 64 - index;

  /* Transform as many times as possible.
*/
  if (inputLen >= partLen) {
 MD5_memcpy
   ((U8 *)&context->buffer[index], (U8 *)input, partLen);
 MD5Transform (context->state, context->buffer);

 for (i = partLen; i + 63 < inputLen; i += 64)
   MD5Transform (context->state, &input[i]);

 index = 0;
  }
  else
 i = 0;

  /* Buffer remaining input */
  MD5_memcpy
 ((U8 *)&context->buffer[index], (U8 *)&input[i],
  inputLen-i);
}

/* MD5 finalization. Ends an MD5 message-digest operation, writing the
  the message digest and zeroizing the context.
 */
VOID MD5Final(U8 digest[16], MD5_CTX *context)
{
  U8 bits[8];
  U32 index, padLen;

  /* Save number of bits */
  Encode (bits, context->count, 8);

  /* Pad out to 56 mod 64.
*/
  index = (U32)((context->count[0] >> 3) & 0x3f);
  padLen = (index < 56) ? (56 - index) : (120 - index);
  MD5Update(context, PADDING, padLen);

  /* Append length (before padding) */
  MD5Update(context, bits, 8);

  /* Store state in digest */
  Encode (digest, context->state, 16);

  /* Zeroize sensitive information.
*/
  MD5_memset ((U8 *)context, 0, sizeof (*context));
}

/* MD5 basic transformation. Transforms state based on block.
 */
static VOID MD5Transform (U32 state[4], U8 block[64])
{
  U32 a = state[0], b = state[1], c = state[2], d = state[3], x[16];

  Decode (x, block, 64);

  /* Round 1 */
  FF (a, b, c, d, x[ 0], S11, 0xd76aa478); /* 1 */
  FF (d, a, b, c, x[ 1], S12, 0xe8c7b756); /* 2 */
  FF (c, d, a, b, x[ 2], S13, 0x242070db); /* 3 */
  FF (b, c, d, a, x[ 3], S14, 0xc1bdceee); /* 4 */
  FF (a, b, c, d, x[ 4], S11, 0xf57c0faf); /* 5 */
  FF (d, a, b, c, x[ 5], S12, 0x4787c62a); /* 6 */
  FF (c, d, a, b, x[ 6], S13, 0xa8304613); /* 7 */
  FF (b, c, d, a, x[ 7], S14, 0xfd469501); /* 8 */
  FF (a, b, c, d, x[ 8], S11, 0x698098d8); /* 9 */
  FF (d, a, b, c, x[ 9], S12, 0x8b44f7af); /* 10 */
  FF (c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
  FF (b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
  FF (a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
  FF (d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
  FF (c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
  FF (b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

 /* Round 2 */
  GG (a, b, c, d, x[ 1], S21, 0xf61e2562); /* 17 */
  GG (d, a, b, c, x[ 6], S22, 0xc040b340); /* 18 */
  GG (c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
  GG (b, c, d, a, x[ 0], S24, 0xe9b6c7aa); /* 20 */
  GG (a, b, c, d, x[ 5], S21, 0xd62f105d); /* 21 */
  GG (d, a, b, c, x[10], S22,  0x2441453); /* 22 */
  GG (c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
  GG (b, c, d, a, x[ 4], S24, 0xe7d3fbc8); /* 24 */
  GG (a, b, c, d, x[ 9], S21, 0x21e1cde6); /* 25 */
  GG (d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
  GG (c, d, a, b, x[ 3], S23, 0xf4d50d87); /* 27 */
  GG (b, c, d, a, x[ 8], S24, 0x455a14ed); /* 28 */
  GG (a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
  GG (d, a, b, c, x[ 2], S22, 0xfcefa3f8); /* 30 */
  GG (c, d, a, b, x[ 7], S23, 0x676f02d9); /* 31 */
  GG (b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

  /* Round 3 */
  HH (a, b, c, d, x[ 5], S31, 0xfffa3942); /* 33 */
  HH (d, a, b, c, x[ 8], SS32, 0x8771f681); /* 34 */
  HH (c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
  HH (b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
  HH (a, b, c, d, x[ 1], S31, 0xa4beea44); /* 37 */
  HH (d, a, b, c, x[ 4], SS32, 0x4bdecfa9); /* 38 */
  HH (c, d, a, b, x[ 7], S33, 0xf6bb4b60); /* 39 */
  HH (b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
  HH (a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
  HH (d, a, b, c, x[ 0], SS32, 0xeaa127fa); /* 42 */
  HH (c, d, a, b, x[ 3], S33, 0xd4ef3085); /* 43 */
  HH (b, c, d, a, x[ 6], S34,  0x4881d05); /* 44 */
  HH (a, b, c, d, x[ 9], S31, 0xd9d4d039); /* 45 */
  HH (d, a, b, c, x[12], SS32, 0xe6db99e5); /* 46 */
  HH (c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
  HH (b, c, d, a, x[ 2], S34, 0xc4ac5665); /* 48 */

  /* Round 4 */
  II (a, b, c, d, x[ 0], S41, 0xf4292244); /* 49 */
  II (d, a, b, c, x[ 7], S42, 0x432aff97); /* 50 */
  II (c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
  II (b, c, d, a, x[ 5], S44, 0xfc93a039); /* 52 */
  II (a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
  II (d, a, b, c, x[ 3], S42, 0x8f0ccc92); /* 54 */
  II (c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
  II (b, c, d, a, x[ 1], S44, 0x85845dd1); /* 56 */
  II (a, b, c, d, x[ 8], S41, 0x6fa87e4f); /* 57 */
  II (d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
  II (c, d, a, b, x[ 6], S43, 0xa3014314); /* 59 */
  II (b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
  II (a, b, c, d, x[ 4], S41, 0xf7537e82); /* 61 */
  II (d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
  II (c, d, a, b, x[ 2], S43, 0x2ad7d2bb); /* 63 */
  II (b, c, d, a, x[ 9], S44, 0xeb86d391); /* 64 */

  state[0] += a;
  state[1] += b;
  state[2] += c;
  state[3] += d;

  /* Zeroize sensitive information.
*/
  MD5_memset ((U8 *)x, 0, sizeof (x));
}

/* Encodes input (U32) into output (U8). Assumes len is
  a multiple of 4.
 */
static VOID Encode (U8 *output, U32 *input, U32 len)
{
  U32 i, j;

  for (i = 0, j = 0; j < len; i++, j += 4) {
 output[j] = (U8)(input[i] & 0xff);
 output[j+1] = (U8)((input[i] >> 8) & 0xff);
 output[j+2] = (U8)((input[i] >> 16) & 0xff);
 output[j+3] = (U8)((input[i] >> 24) & 0xff);
  }
}

/* Decodes input (U8) into output (U32). Assumes len is
  a multiple of 4.
 */
static VOID Decode (U32 *output,U8 *input, U32 len)
{
  U32 i, j;

  for (i = 0, j = 0; j < len; i++, j += 4)
 output[i] = ((U32)input[j]) | (((U32)input[j+1]) << 8) |
   (((U32)input[j+2]) << 16) | (((U32)input[j+3]) << 24);
}

/* Note: Replace "for loop" with standard memcpy if possible.
 */

static VOID MD5_memcpy (U8 * output,U8 * input,U32 len)
{
  U32 i;

  for (i = 0; i < len; i++)
 output[i] = input[i];
}

/* Note: Replace "for loop" with standard memset if possible.
 */
static VOID MD5_memset (U8 * output, S32 value, U32 len)
{
  U32 i;

  for (i = 0; i < len; i++)
 ((S8 *)output)[i] = (S8)value;
}

//added by helen 20040313
void MD5_cvthex(SNMPHASH Bin,SNMPHASHHEX Hex)
{
    U8 i;
    U8 j;

    for (i = 0; i < SNMP_MD5_AUTH_HASHLEN; i++)
    {
        j = (Bin[i] >> 4) & 0xf;
        if (j <= 9)
        Hex[i*2] = (j + '0');
        else
        Hex[i*2] = (j + 'a' - 10);
        j = Bin[i] & 0xf;
        if (j <= 9)
        Hex[i*2+1] = (j + '0');
        else
        Hex[i*2+1] = (j + 'a' - 10);
    }

    Hex[SNMP_MD5_AUTH_HASHHEXLEN] = '\0';
}

/* ============================= md5 ================================*/


const S8 * db_str2trim( S8 *pszStr )
{
    U32         ulLen;
    S8          strBuff[ 256 ];
    S8 *        pszSrc;
    S8 *        pszDst;

    /* length check */
    ulLen       = strlen( pszStr );
    if ( ulLen > 255 )
    {
        AOS_ASSERT( 0 );
        return pszStr;
    }

    /* move to first char */
    pszSrc      = pszStr;
    while ( *pszSrc == ' '
         || *pszSrc == '\t'
         || *pszSrc == 0x0a
         || *pszSrc == 0x0d )
    {
        pszSrc++;

        if ( *pszSrc == '\0' )
        {
            break;
        }
    }

    /* trim source string */
    pszDst = strBuff;
    while ( 1 )
    {
        *pszDst = *pszSrc;

        if ( *pszSrc == '\0' )
        {
            break;
        }
        else if ( *pszSrc == ' ' )
        {
            break;
        }
        else if ( *pszSrc == '\t' )
        {
            break;
        }
        else if ( *pszSrc == 0x0a )
        {
            break;
        }
        else if ( *pszSrc == 0x0d )
        {
            break;
        }

        pszDst++;
        pszSrc++;
    }

    *pszDst = '\0';

    /* overwrite source string */
    aos_snprintf( pszStr, aos_strlen( pszStr ) + 1, "%s", strBuff );
    return pszStr;
}


/* ============================= function ================================*/

U32 sys_do_shell_cmd( S8 * pszCmd, S8 * pszOut, U32 ulOutLen )
{
    FILE *      fp;
    S32         slSize;

    if ( pszOut == NULL
      || ulOutLen <= 1 )
    {
        // AOS_ASSERT( 0 );
        return 0;
    }

    /* clear out buffer */
    memset( pszOut, 0x00, ulOutLen );

    /* 执行shell命令并打开匿名管道 */
    fp = popen( pszCmd, "r" );
    if ( fp == NULL )
    {
        // AOS_ASSERT( 0 );
        printf( "do_shell_cmd() popen fail! cmd = %s \n  err = %s \n",
                pszCmd, strerror( errno ) );
        return 0;
    }

    /* 通过管道读取执行结果 */
    slSize = fread( pszOut, 1, ( ulOutLen - 1 ), fp );
    pclose( fp );

    /* check result */
    if ( slSize < 0 )
    {
        // AOS_ASSERT( 0 );
        printf( "[ERROR]do_shell_cmd() fread fail! cmd = %s \n  err = %s \n",
                pszCmd, strerror( errno ) );
        slSize = 0;
    }

    return slSize;
}


/* check buffer len: INET_ADDRSTRLEN */
U32 sys_get_mac_addr( const S8 * pszEthName, S8 * pszMacBuff, U32 ulBuffLen )
{
    int                     sockFd;
    struct ifreq            ifr;
    struct sockaddr_in *    sin;

    if ( ulBuffLen < INET_ADDRSTRLEN )
    {
        AOS_ASSERT( 0 );
        return AOS_FAIL;
    }

    /* clear mac buffer */
    aos_memset( pszMacBuff, 0x00, ulBuffLen );

    /* init sockFd with interface name */
    sockFd  = socket( AF_INET, SOCK_DGRAM, 0 );
    if( sockFd < 0)
    {
        AOS_ASSERT( 0 );
        return AOS_FAIL;
    }

    /* get interface mac address */
    aos_memset( &ifr, 0x00, sizeof( struct ifreq ) );
    aos_strncpy( ifr.ifr_name, pszEthName, sizeof( ifr.ifr_name ) - 1 );
    if ( ioctl( sockFd, SIOCGIFHWADDR, &ifr ) < 0 )
    {
        /* maybe null */
        // AOS_ASSERT( 0 );
        close( sockFd );
        return AOS_FAIL;
    }

    aos_snprintf( pszMacBuff, ulBuffLen, "%02x:%02x:%02x:%02x:%02x:%02x",
                  (unsigned char)ifr.ifr_hwaddr.sa_data[ 0 ],
                  (unsigned char)ifr.ifr_hwaddr.sa_data[ 1 ],
                  (unsigned char)ifr.ifr_hwaddr.sa_data[ 2 ],
                  (unsigned char)ifr.ifr_hwaddr.sa_data[ 3 ],
                  (unsigned char)ifr.ifr_hwaddr.sa_data[ 4 ],
                  (unsigned char)ifr.ifr_hwaddr.sa_data[ 5 ]
                );

    close( sockFd );
    return AOS_SUCC;
}


U32 sys_calc_local_server_magic1( SYS_S * pstSysObj, S8 * pszOutMagic, U32 ulOutLen, ROOT_S * pstRootObj )
{
    SYS_LICENSE_S *     pstLicenseInfo;

    /* server key info */
    S8                  strHostId[ 256 ];
    S8                  strSdaSn[ 256 ];
    S8                  strEthMacAddr[ 256 ];

    /* md5 calc fields */
    MD5_CTX             Md5Ctx;
    U8                  aucHash[ HASH_LEN ];
    U32                 ulIndex;
    U32                 ulOffset;
    U32                 ulLen;

    pstLicenseInfo          = &pstSysObj->stRecordSys.stLicenseInfo;

    /* get server key info */
    {
        /* ********************************************************* */
        /* warning: don't change algorithm after version released!!! */
        /* ********************************************************* */
        S8      strCmd[ 256 ];

        /* get server host-id */
        #if 0
        aos_sprintf( strCmd, "hostid" );
        sys_do_shell_cmd( strCmd, strHostId, 256 );
        db_str2trim( strHostId );
        #else
        /* notes: use fixed hostid! hostid would be changed by localhost setting */
        aos_sprintf( strHostId, "007f0101" );
        #endif

        /* get server eth0-mac or eth1-mac, as same as get_server_magic, only for license check! */
        if ( AOS_SUCC != sys_get_mac_addr( "eth0", strEthMacAddr, 256 )
          && AOS_SUCC != sys_get_mac_addr( "eth1", strEthMacAddr, 256 ) )
        {
            // maybe null
            // AOS_ASSERT( 0 );
            aos_sprintf( strEthMacAddr, "NA" );
        }

        db_str2trim( strEthMacAddr );

        /* get server sda-id */
        aos_sprintf( strCmd, "ls -S /dev/disk/by-uuid | awk '{if(NR==1)print}' | awk '{print $1}'" );
        ulLen = sys_do_shell_cmd( strCmd, strSdaSn, 256 );
        if ( ulLen == 0 )
        {
            // AOS_ASSERT( 0 );
            aos_sprintf( strSdaSn, "NA" );
        }

        db_str2trim( strSdaSn );

        if ( pstSysObj->stRecordSys.bDebugFlag == TRUE )
        {
            printf(     " %40s : %s \n",
                        "----------------------------------------",
                        "Server Magic Info(v1)" );

            printf(     " %40s : %s \n",
                        "Server Host Id",
                        strHostId );

            printf(     " %40s : %s \n",
                        "Server MAC Addr",
                        strEthMacAddr );

            printf(     " %40s : %s \n",
                        "Server SDA Id",
                        strSdaSn );
        }
    }

    if ( aos_stricmp( pstLicenseInfo->strVersion, "1.0" ) == 0 )
    {
        /* ********************************************************* */
        /* warning: don't change algorithm after version released!!! */
        /* ********************************************************* */
        MD5Init( &Md5Ctx );
        MD5Update( &Md5Ctx, (U8 *)strHostId, aos_strlen( strHostId ) );
        MD5Update( &Md5Ctx, (U8 *)strSdaSn, aos_strlen( strSdaSn ) );
        MD5Update( &Md5Ctx, (U8 *)strEthMacAddr, aos_strlen( strEthMacAddr ) );
        MD5Final( aucHash, &Md5Ctx );

        /* convert md5 to string */
        ulOffset    = 0;
        for ( ulIndex = 0; ulIndex < HASH_LEN; ulIndex++ )
        {
            aos_sprintf( pszOutMagic + ulOffset, "%02x", aucHash[ ulIndex ] );
            ulOffset += 2;
        }
    }
    else
    {
        if ( ROOT_MPE == MPE_SIMSRV_DBO && ROOT_DEBUG( IM_DEBUG_MUST ) )
        {
            AOS_ASSERT( 0 );
        }

        aos_sprintf( pszOutMagic, "[ERROR]doesn't support!" );
        return AOS_FAIL;
    }

    return AOS_SUCC;
}


U32 sys_calc_local_server_magic2( SYS_S * pstSysObj, S8 * pszOutMagic, U32 ulOutLen, ROOT_S * pstRootObj )
{
    SYS_LICENSE_S *     pstLicenseInfo;

    /* server key info */
    S8                  strHostId[ 256 ];
    S8                  strSdaSn[ 256 ];
    S8                  strEthMacAddr[ 256 ];

    /* md5 calc fields */
    MD5_CTX             Md5Ctx;
    U8                  aucHash[ HASH_LEN ];
    U32                 ulIndex;
    U32                 ulOffset;
    U32                 ulLen;

    pstLicenseInfo          = &pstSysObj->stRecordSys.stLicenseInfo;

    /* get server key info */
    {
        /* ********************************************************* */
        /* warning: don't change algorithm after version released!!! */
        /* ********************************************************* */
        S8      strCmd[ 256 ];

        /* get server host-id */
        #if 0
        aos_sprintf( strCmd, "hostid" );
        sys_do_shell_cmd( strCmd, strHostId, 256 );
        db_str2trim( strHostId );
        #else
        /* notes: use fixed hostid! hostid would be changed by localhost setting */
        aos_sprintf( strHostId, "007f0101" );
        #endif

        /* get server eth0-mac or eth1-mac, as same as get_server_magic, only for license check! */
        if ( AOS_SUCC != sys_get_mac_addr( "eth0", strEthMacAddr, 256 )
          && AOS_SUCC != sys_get_mac_addr( "eth1", strEthMacAddr, 256 ) )
        {
            // maybe null
            // AOS_ASSERT( 0 );
            aos_sprintf( strEthMacAddr, "NA" );
        }

        db_str2trim( strEthMacAddr );

        /* get server sda-id */
        aos_sprintf( strCmd, "ls -l -S /dev/disk/by-uuid | grep -v dm | grep -v ram | awk '{if(NR==2)print}' | awk '{print $9}'" );
        ulLen = sys_do_shell_cmd( strCmd, strSdaSn, 256 );
        if ( ulLen == 0 )
        {
            // AOS_ASSERT( 0 );
            aos_sprintf( strSdaSn, "NA" );
        }

        db_str2trim( strSdaSn );

        if ( pstSysObj->stRecordSys.bDebugFlag == TRUE )
        {
            printf(     " %40s : %s \n",
                        "----------------------------------------",
                        "Server Magic Info(v2)" );

            printf(     " %40s : %s \n",
                        "Server Host Id",
                        strHostId );

            printf(     " %40s : %s \n",
                        "Server MAC Addr",
                        strEthMacAddr );

            printf(     " %40s : %s \n",
                        "Server SDA Id",
                        strSdaSn );
        }
    }

    if ( aos_stricmp( pstLicenseInfo->strVersion, "2.0" ) == 0 )
    {
        /* ********************************************************* */
        /* warning: don't change algorithm after version released!!! */
        /* ********************************************************* */
        MD5Init( &Md5Ctx );
        MD5Update( &Md5Ctx, (U8 *)strHostId, aos_strlen( strHostId ) );
        MD5Update( &Md5Ctx, (U8 *)strSdaSn, aos_strlen( strSdaSn ) );
        MD5Update( &Md5Ctx, (U8 *)strEthMacAddr, aos_strlen( strEthMacAddr ) );
        MD5Final( aucHash, &Md5Ctx );

        /* convert md5 to string */
        ulOffset    = 0;
        for ( ulIndex = 0; ulIndex < HASH_LEN; ulIndex++ )
        {
            aos_sprintf( pszOutMagic + ulOffset, "%02x", aucHash[ ulIndex ] );
            ulOffset += 2;
        }
    }
    else
    {
        if ( ROOT_MPE == MPE_SIMSRV_DBO && ROOT_DEBUG( IM_DEBUG_MUST ) )
        {
            AOS_ASSERT( 0 );
        }

        aos_sprintf( pszOutMagic, "[ERROR]doesn't support!" );
        return AOS_FAIL;
    }

    return AOS_SUCC;
}


U32 sys_calc_local_server_magic3( SYS_S * pstSysObj, S8 * pszOutMagic, U32 ulOutLen, ROOT_S * pstRootObj )
{
    SYS_LICENSE_S *     pstLicenseInfo;

    /* server key info */
    S8                  strHostId[ 256 ];
    S8                  strSdaDev[ 256 ];
    S8                  strSdaSn[ 256 ];
    S8                  strEthMacAddr[ 256 ];

    /* md5 calc fields */
    MD5_CTX             Md5Ctx;
    U8                  aucHash[ HASH_LEN ];
    U32                 ulIndex;
    U32                 ulOffset;
    U32                 ulLen;

    pstLicenseInfo          = &pstSysObj->stRecordSys.stLicenseInfo;

    /* get server key info */
    {
        /* ********************************************************* */
        /* warning: don't change algorithm after version released!!! */
        /* ********************************************************* */
        S8      strCmd[ 256 ];

        /* get server host-id */
        #if 0
        aos_sprintf( strCmd, "hostid" );
        sys_do_shell_cmd( strCmd, strHostId, 256 );
        db_str2trim( strHostId );
        #else
        /* notes: use fixed hostid! hostid would be changed by localhost setting */
        aos_sprintf( strHostId, "007f0101" );
        #endif

        /* get server eth0-mac or eth1-mac, as same as get_server_magic, only for license check! */
        if ( AOS_SUCC != sys_get_mac_addr( "eth0", strEthMacAddr, 256 )
          && AOS_SUCC != sys_get_mac_addr( "eth1", strEthMacAddr, 256 ) )
        {
            // maybe null
            // AOS_ASSERT( 0 );
            aos_sprintf( strEthMacAddr, "NA" );
        }

        db_str2trim( strEthMacAddr );

        /* get server boot dev */
        aos_sprintf( strCmd, "fdisk -l | grep Linux | grep '*' | awk '{print $1}'" );
        ulLen = sys_do_shell_cmd( strCmd, strSdaDev, 256 );
        if ( ulLen == 0 )
        {
            // AOS_ASSERT( 0 );
            aos_sprintf( strSdaSn, "NA" );
        }
        else
        {
            /* get server sda-id */
            db_str2trim( strSdaDev );
            aos_sprintf( strCmd, "blkid -s UUID %s | awk '{if(NR==1)print}' | awk '{print $2}' | cut -f 2 -d '\"'", strSdaDev );
            ulLen = sys_do_shell_cmd( strCmd, strSdaSn, 256 );
            if ( ulLen == 0 )
            {
                // AOS_ASSERT( 0 );
                aos_sprintf( strSdaSn, "NA" );
            }
        }

        db_str2trim( strSdaSn );

        if ( pstSysObj->stRecordSys.bDebugFlag == TRUE )
        {
            printf(     " %40s : %s \n",
                        "----------------------------------------",
                        "Server Magic Info(v3)" );

            printf(     " %40s : %s \n",
                        "Server Host Id",
                        strHostId );

            printf(     " %40s : %s \n",
                        "Server MAC Addr",
                        strEthMacAddr );

            printf(     " %40s : %s \n",
                        "Server SDA Id",
                        strSdaSn );
        }
    }

    if ( aos_stricmp( pstLicenseInfo->strVersion, "3.0" ) == 0 )
    {
        /* ********************************************************* */
        /* warning: don't change algorithm after version released!!! */
        /* ********************************************************* */
        MD5Init( &Md5Ctx );
        MD5Update( &Md5Ctx, (U8 *)strHostId, aos_strlen( strHostId ) );
        MD5Update( &Md5Ctx, (U8 *)strSdaSn, aos_strlen( strSdaSn ) );
        MD5Update( &Md5Ctx, (U8 *)strEthMacAddr, aos_strlen( strEthMacAddr ) );
        MD5Final( aucHash, &Md5Ctx );

        /* convert md5 to string */
        ulOffset    = 0;
        for ( ulIndex = 0; ulIndex < HASH_LEN; ulIndex++ )
        {
            aos_sprintf( pszOutMagic + ulOffset, "%02x", aucHash[ ulIndex ] );
            ulOffset += 2;
        }
    }
    else
    {
        if ( ROOT_MPE == MPE_SIMSRV_DBO && ROOT_DEBUG( IM_DEBUG_MUST ) )
        {
            AOS_ASSERT( 0 );
        }

        aos_sprintf( pszOutMagic, "[ERROR]doesn't support!" );
        return AOS_FAIL;
    }

    return AOS_SUCC;
}


/* ============================= main ================================*/


int main( int argc, char * argv[] )
{
    SYS_S       stSysObj;
    ROOT_S      stRootObj;
    S8          strMagicAuth[ MD5_HASH_STR_LEN ];

    if ( argc < 2 )
    {
        stSysObj.stRecordSys.bDebugFlag = FALSE;
    }
    else
    {
        if ( aos_stricmp( argv[ 1 ], "debug" ) == 0 )
        {
            stSysObj.stRecordSys.bDebugFlag = TRUE;
        }
        else
        {
            stSysObj.stRecordSys.bDebugFlag = FALSE;
        }
    }

    printf(     " %40s \n",
                "--------------------------------------------------------------------------- " );

    aos_strncpy( stSysObj.stRecordSys.stLicenseInfo.strVersion, "1.0", MAX_NAME_STR_LEN );
    sys_calc_local_server_magic1( &stSysObj, strMagicAuth, MD5_HASH_STR_LEN, &stRootObj );
    printf(     " %40s : %s \n",
                "Local Server Magic(v1)", strMagicAuth );

    aos_strncpy( stSysObj.stRecordSys.stLicenseInfo.strVersion, "2.0", MAX_NAME_STR_LEN );
    sys_calc_local_server_magic2( &stSysObj, strMagicAuth, MD5_HASH_STR_LEN, &stRootObj );
    printf(     " %40s : %s \n",
                "Local Server Magic(v2)", strMagicAuth );

    aos_strncpy( stSysObj.stRecordSys.stLicenseInfo.strVersion, "3.0", MAX_NAME_STR_LEN );
    sys_calc_local_server_magic3( &stSysObj, strMagicAuth, MD5_HASH_STR_LEN, &stRootObj );
    printf(     " %40s : %s \n",
                "Local Server Magic(v3)", strMagicAuth );

    printf(     " %40s \n",
                "--------------------------------------------------------------------------- " );

    return AOS_SUCC;
}

