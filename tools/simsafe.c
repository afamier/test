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
#include <fcntl.h>
#include <syslog.h>
#include <errno.h>

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
#define U32_BUTT ((U32)-1)
#define VA_START            va_start
#define VA_ARG              va_arg
#define VA_END              va_end

#define db_strncpy          strncpy
#define aos_memcpy          memcpy
#define aos_memset          memset
#define aos_strlen          strlen
#define aos_strcpy          strcpy
#define aos_strncpy         strncpy
#define aos_sscanf          sscanf
#define aos_sprintf         sprintf
#define aos_stricmp         strcasecmp
#define aos_vsnprintf       vsnprintf

#define AOS_HTONL           htonl
#define AOS_HTONS           htons

#define AOS_ASSERT( para )  if ( para == FALSE ) printf( "ASSERT!!! \n" )

#define ROOT_MPE            1
#define MPE_SIMSRV_DBO      1
#define ROOT_DEBUG( IM_DEBUG_MUST ) 1
#define TMP_LICENSE_PASSWORD            "dinstar2011"

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

#define MAX_SCP_MSG_SIZE        4000                // AOS Msg Limitation <= 4096 Bytes, 仅用于MTU连通性探测, 必须大于1500
#define MAX_SPLIT_MSG_SIZE      1300                // UDP分片情况下的网络连通性不好, 限制为小于MTU 1400 - ( MAC + IP + UDP + delta )
                                                    // GW_PORT_INFO, msgLen=1284, with 32 ports
                                                    // BANK_PORT_INFO, msgLen=1092, with 32 ports
#define REGISTER_MSG_LEN        MAX_SPLIT_MSG_SIZE  // UDP分片情况下的网络连通性不好, 改为 MAX_SPLIT_MSG_SIZE
#define REGISTER_ACK_MSG_LEN    MAX_SPLIT_MSG_SIZE  // UDP分片情况下的网络连通性不好, 改为 MAX_SPLIT_MSG_SIZE

#define MODULE_NO_SIGNAL        99              // 无线信号强度:   0(null) ~ 31(full), 99 - error;

/*sip server地址最大长度*/
#define  MAX_SIP_SERVER_LEN     127

/* -------------------------------- enums -----------------------------------*/

typedef enum im_license_class
{
    IM_LICENSE_CLASS_NULL       = 0,

    IM_LICENSE_CLASS_SERVER     = 1,        /* for server license */
    IM_LICENSE_CLASS_DOMAIN     = 2,        /* for domain license */

    IM_LICENSE_CLASS_BUTT
}IM_LICENSE_CLASS_TYPE;

typedef enum im_license_sign
{
    IM_LICENSE_SIGN_NULL        = 0,

    IM_LICENSE_SIGN_TRIAL       = 1,        /* for trial */
    IM_LICENSE_SIGN_OFFICIAL    = 2,        /* for official */

    IM_LICENSE_SIGN_PERMANENT   = 9,        /* for permanent */

    IM_LICENSE_SIGN_BUTT
}IM_LICENSE_SIGN_TYPE;

typedef enum im_license_status
{
    IM_LICENSE_STATUS_NULL          = 0,        /* no license */

    IM_LICENSE_STATUS_PENDING       = 1,        /* pending */
    IM_LICENSE_STATUS_APPROVED      = 2,        /* approved */
    IM_LICENSE_STATUS_EXPIRED       = 3,        /* expired */

    IM_LICENSE_STATUS_INVALID       = 4,        /* invalid */

    IM_LICENSE_STATUS_BUTT
}IM_LICENSE_STATUS;

typedef enum im_server_mode
{
    IM_SERVER_MODE_NULL         = 0,

    IM_SERVER_MODE_LOCAL        = 1,        /* for local server */

    IM_SERVER_MODE_CLOUD        = 9,        /* for public cloud */

    IM_SERVER_MODE_BUTT
}IM_SERVER_MODE_TYPE;

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
#define SMS_STR_LEN                 ( SMS_LEN + 1 )
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
    S8                      strMysqlDb[ MAX_NAME_STR_LEN ];         /* simserver */
    S8                      strMysqlUser[ MAX_USERNAME_STR_LEN ];   /* dinstar */
    S8                      strMysqlPwd[ MAX_PASSWORD_STR_LEN ];    /* 123456 */

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
    U8              ucLicenseStatus;                    /* IM_LICENSE_STATUS */
    U32             ulSrvUuid;                          /* cloud allocation */
    U8              ucSrvMode;                          /* IM_SERVER_MODE_TYPE: local, cloud */
    U32             ulMaxSimCard;
    U8              ucHbmFeatures;
    U8              ucServiceApi;
    U32             ulSerialNo;
    U8              ucSignType;                         /* IM_LICENSE_SIGN_TYPE: trial, official, permanent */
    SYS_TIME_S      stSignDate;
    SYS_TIME_S      stExpiredDate;
    U16             usAvailableDays;

    /* original info */
    S8              strVersion[ MAX_NAME_STR_LEN ];
    S8              strClass[ MAX_NAME_STR_LEN ];       /* IM_LICENSE_CLASS_TYPE: server, domain */
    S8              strSrvUuid[ MAX_NAME_STR_LEN ];     /* IM_LICENSE_CLASS_TYPE: server, domain */
    S8              strSrvMode[ MAX_NAME_STR_LEN ];     /* IM_LICENSE_CLASS_TYPE: server, domain */
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

    S8              strAuthInfo[ MD5_HASH_STR_LEN ];
}SYS_LICENSE_S;


typedef struct record_sys_s
{
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
    MD5_CTX context;

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

/* ============================= function ================================*/

U32 task_delay_ms( U32 timeout )
{
    struct timeval tv;

    if( !timeout )
    {
        AOS_ASSERT( 0 );
        return -1;
    }

    tv.tv_sec  = timeout / 1000;
    tv.tv_usec = (timeout % 1000)*1000;

    select(0, NULL, NULL, NULL, &tv);
    return 0;
}

/* 增加空指针判断 ,适用于十进制*/
U32 db_atou( S8 * pszBuff )
{
    if ( pszBuff == NULL )
    {
        return 0;
    }

    return (U32)strtoul( pszBuff, NULL, 10 );
}

/* 增加空指针判断 */
S32 db_atoi( S8 * pszBuff )
{
    if ( pszBuff == NULL )
    {
        return 0;
    }

    return atoi( pszBuff );
}

U32 db_str2float( S8 *strBuf, F32 * pVal )
{
    if ( strBuf == NULL )
    {
        *pVal = 0;
        return AOS_FAIL;
    }

    *pVal   = atof( strBuf );
    return AOS_SUCC;
}


/* 增加空指针判断, 标准格式: YYYY-MM-DD hh:mm:ss */
U32 db_str2time( SYS_TIME_S *pTime, const S8 *strTime )
{
    int year,month,day,hour,min,sec;

    if ( strTime == NULL
      || aos_strlen( strTime ) == 0 )
    {
        aos_memset( pTime, 0x00, sizeof( SYS_TIME_S ) );
        return AOS_FAIL;
    }

    (VOID)sscanf(strTime,"%d-%d-%d %d:%d:%d",
                   &year,&month,&day,
                   &hour,&min,&sec );

    pTime->year = (U16)year;
    pTime->month = (U8)month;
    pTime->date  = (U8)day;
    pTime->hour  = (U8)hour;
    pTime->minute = (U8)min;
    pTime->second = (U8)sec;

    pTime->week     = 0;
    pTime->millisec = 0;

    return AOS_SUCC;
}

S8 *db_time2str( S8 *strBuf,SYS_TIME_S *pTime )
{

    sprintf(strBuf,"%04d-%02d-%02d %02d:%02d:%02d",
            pTime->year,pTime->month,pTime->date,pTime->hour,pTime->minute,pTime->second);

    return strBuf;
}

/* 增加空指针判断, 标准格式: YYYY-MM-DD */
U32 db_str2date( SYS_TIME_S *pDate, const S8 *strDate )
{
    int year,month,day;

    if ( strDate == NULL
      || aos_strlen( strDate ) == 0 )
    {
        aos_memset( pDate, 0x00, sizeof( SYS_TIME_S ) );
        return AOS_FAIL;
    }

    (VOID)sscanf(strDate,"%d-%d-%d",
                   &year,&month,&day );

    pDate->year  = (U16)year;
    pDate->month = (U8)month;
    pDate->date  = (U8)day;
    pDate->hour   = 0;
    pDate->minute = 0;
    pDate->second = 0;

    pDate->week     = 0;
    pDate->millisec = 0;

    return AOS_SUCC;
}

S8 *db_date2str( S8 *strBuf,SYS_TIME_S *pDate )
{
    if ( pDate->month == 0 || pDate->year == 1970 )
    {
        /* patch: special for old license check */
        sprintf( strBuf,"0000-00-00" );
    }
    else
    {
        sprintf( strBuf,"%04d-%02d-%02d", pDate->year,pDate->month,pDate->date);
    }

    return strBuf;
}

S8 * eid_get_bin_str( U8 * pucBytes, U32 ulByteNum, S8 * pcSpit, S8 * pc4Split, S8 * pszOutStr )
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

        if ( ( ( ulIndex + 1 ) % 4 ) == 0 )
        {
            /* 4字节分隔符 */
            if ( pc4Split != NULL )
            {
                aos_sprintf( pszOutStr + ulLen, "%02x%s", pucBytes[ ulIndex ], pc4Split );
            }
            else if ( pcSpit != NULL )
            {
                aos_sprintf( pszOutStr + ulLen, "%02x%s", pucBytes[ ulIndex ], pcSpit );
            }
            else
            {
                aos_sprintf( pszOutStr + ulLen, "%02x", pucBytes[ ulIndex ] );
            }
        }
        else
        {
            /* 普通分隔符 */
            if ( pcSpit != NULL )
            {
                aos_sprintf( pszOutStr + ulLen, "%02x%s", pucBytes[ ulIndex ], pcSpit );
            }
            else
            {
                aos_sprintf( pszOutStr + ulLen, "%02x", pucBytes[ ulIndex ] );
            }
        }
    }

    ulLen   = aos_strlen( pszOutStr );
    aos_sprintf( pszOutStr + ulLen, "%02x", pucBytes[ ulIndex ] );

    return pszOutStr;
}


VOID eid_print_mem_buffer2( U8 * pucBuff, U32 ulBuffSize, U8 ucLineBytes )
{
    S8          strCharLine[ 512 ];
    S8          strHexLine[ 512 ];
    U8          ucVal;
    U32         ulIndex;
    U32         ulOffset;
    U32         ulLeft;
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
                printf( "%s - %s", strHexLine, strCharLine );
            }

            /* 每行首字符 */
            aos_sprintf( strHexLine, "%02x", ucVal );
            if ( ucVal >= 32 && ucVal <= 125 )
            {
                aos_sprintf( strCharLine, "%c", (S8)ucVal );
            }
            else
            {
                aos_sprintf( strCharLine, "." );
            }
        }
        else
        {
            /* 每行的后续字符 */
            aos_sprintf( strHexLine + aos_strlen( strHexLine ), " %02x", ucVal );

            if ( ucVal >= 32 && ucVal <= 125 )
            {
                aos_sprintf( strCharLine + aos_strlen( strCharLine ), "%c", (S8)ucVal );
            }
            else
            {
                aos_sprintf( strCharLine + aos_strlen( strCharLine ), "." );
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
            aos_sprintf( strHexLine + aos_strlen( strHexLine ), " " );
        }
    }

    /* 输出尾行 */
    printf( "%s - %s \r\n", strHexLine, strCharLine );

}


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
    while ( *pszSrc == ' ' )
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
    strncpy( pszStr, strBuff, aos_strlen( pszStr ) );
    return pszStr;
}

U32 aos_time_get( SYS_TIME_S *pSysTime )
{
    time_t timep;
    struct tm *p;
    time(&timep);
    p=gmtime(&timep);

    pSysTime->date = p->tm_mday;
    pSysTime->hour = p->tm_hour;
    pSysTime->minute = p->tm_min;
    pSysTime->month = p->tm_mon+1;
    pSysTime->second = p->tm_sec;
    pSysTime->week = p->tm_wday;
    pSysTime->year = p->tm_year+1900;
    pSysTime->millisec = 0;
    return AOS_SUCC;
}

/* priority: LOG_INFO */
VOID sys_syslog_printf( const S8 *fmt, ... )
{
    SYS_TIME_S  stCurTime;
    S8          strTimeBuff[ 32 ];
    S8          strOutBuff[ 256 ];

	va_list args;
	S32 i;

	VA_START(args, fmt);
	aos_vsnprintf( strOutBuff, 256, fmt, args );
	VA_END(args);

    aos_time_get( &stCurTime );
    db_time2str( strTimeBuff, &stCurTime );

	syslog( LOG_INFO, "[%s]%s", strTimeBuff, strOutBuff );
}


U32 sys_do_shell_cmd( S8 * pszCmd, S8 * pszOut, U32 ulOutLen )
{
    FILE *      fp;
    U32         ulSize;

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
    ulSize = fread( pszOut, 1, ulOutLen, fp );
    pclose( fp );

    return ulSize;
}

/*
# -------------------------------------------------
# get one param value from xml file:
#     <param name="param01" value="1"/>
# param_name MUST be unique one
# return: PARAM_VAL2
# -------------------------------------------------
*/
VOID get_xml_param_val2( const S8 * pszFile, const S8 * pszParam, S8 * pszVal, U32 ulOutLen )
{
    S8      strCmd[ 256 ];

    aos_sprintf( strCmd, "cat %s | grep %s | awk '{print $3}' | cut -f 2 -d '\"'",
                 pszFile, pszParam );

    sys_do_shell_cmd( strCmd, pszVal, ulOutLen );
    db_str2trim( pszVal );
}

/*
# -------------------------------------------------
# get process info by shell cmd
# -------------------------------------------------
# ps -ef info:
# UID        PID  PPID  C STIME TTY          TIME CMD
# -------------------------------------------------
# ps aux info:
# USER       PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME COMMAND
# -------------------------------------------------
*/
typedef struct
{
    BOOL_T      bMonitorFlag;           // monitor flag

    S8          strProcCmd[ 128 ];      // process cmd with path

    S8          strPid[ 8 ];
    S8          strCpu[ 8 ];
    S8          strMem[ 8 ];

    U32         ulPid;                  // 0 - means stopped !!!
    F32         flCpu;                  // %#
    F32         flMem;                  // %#

    U32         ulMaxCpu;               // max limitation
    U32         ulMaxMem;               // max limitation
    U32         ulMaxErrorCount;        // max error count
    U32         ulMaxLostCount;         // max lost count

    U32         ulCurErrorCount;        // process was high cpu/mem
    U32         ulCurLostCount;         // process was stopped
}PROCESS_INFO_S;

U32 get_cpu_cores_num( void )
{
    S8      strCmd[ 256 ];
    S8      strValue[ 256 ];
    U32     ulNum;


    /* get cpu cores */
    //aos_sprintf( strCmd, "cat /proc/cpuinfo | grep cores | awk '{if(NR==1)print}' | awk '{print $4}'" );
    aos_sprintf( strCmd, "cat /proc/cpuinfo | grep cores | awk 'BEGIN{CoreCnt=0} $0 ~ /^cpu cores/ {CoreCnt+=$4} END{print CoreCnt}'" );
    sys_do_shell_cmd( strCmd, strValue, 256 );
    ulNum   = db_atou( strValue );
    return ulNum;
}

U32 get_process_run_info( PROCESS_INFO_S * pstProcInfo )
{
    S8      strCmd[ 256 ];
    S8      strValue[ 256 ];

    /* get process pid */
    aos_sprintf( strCmd, "/bin/ps -ef | grep -v grep | grep \"%s\" | sed -n '1P' | awk '{print $2}'", pstProcInfo->strProcCmd );
    sys_do_shell_cmd( strCmd, strValue, 256 );
    pstProcInfo->ulPid  = db_atou( strValue );
    if ( pstProcInfo->ulPid == 0 )
    {
        pstProcInfo->flCpu  = 0;
        pstProcInfo->flMem  = 0;
        return AOS_FAIL;
    }

    /* get process cpu */
    aos_sprintf( strCmd, "/bin/ps aux | grep -v grep | grep \"%s\" | sed -n '1P' | awk '{print $3}'", pstProcInfo->strProcCmd );
    sys_do_shell_cmd( strCmd, strValue, 256 );
    db_str2trim( strValue );
    db_strncpy( pstProcInfo->strCpu, strValue, 8 );
    db_str2float( strValue, &pstProcInfo->flCpu );

    /* get process mem */
    aos_sprintf( strCmd, "/bin/ps aux | grep -v grep | grep \"%s\" | sed -n '1P' | awk '{print $4}'", pstProcInfo->strProcCmd );
    sys_do_shell_cmd( strCmd, strValue, 256 );
    db_str2trim( strValue );
    db_strncpy( pstProcInfo->strMem, strValue, 8 );
    db_str2float( strValue, &pstProcInfo->flMem );
    return AOS_SUCC;
}

/* ============================= main ================================*/

#define MAX_PROCESS_NUM                 4
#define CHECK_TIMER_LEN                 5                                       // 5 seconds delay per process
#define PROC_CHECK_INTERVAL             ( MAX_PROCESS_NUM * CHECK_TIMER_LEN )   // 15 seconds

int main( int argc, char * argv[] )
{
    U32                 ulCpuCores;
    U32                 ulMaxCpuRate;
    U32                 ulIndex;
    U32                 ulCount;
    PROCESS_INFO_S      stProcInfos[ MAX_PROCESS_NUM ];
    PROCESS_INFO_S *    pstProcInfo;
    S8                  strOutBuff[ 256 ];
    S8                  strOutCmd[ 256 ];
    S8                  strOutVal[ 256 ];

    /* get cpu cores */
    ulCpuCores          = get_cpu_cores_num();
    if ( ulCpuCores == 0 )
    {
        aos_sprintf( strOutBuff, "simsafe get cpu cores fail! \n" );
        sys_syslog_printf( "[simsafe]get cpu cores fail! " );
        ulCpuCores  = 1;
    }

    ulMaxCpuRate    = ulCpuCores * 90;  /* 考虑多核CPU */
    aos_sprintf( strOutBuff, "simsafe cpu cores: %d, max cpu rate: %d%% \n",
                             ulCpuCores, ulMaxCpuRate );
    sys_syslog_printf( "[simsafe]cpu cores: %d, max cpu rate: %d%% ", ulCpuCores, ulMaxCpuRate );

    /* init process info */
    aos_memset( &stProcInfos[ 0 ], 0x00, MAX_PROCESS_NUM * sizeof( PROCESS_INFO_S ) );

    /* 1. /dinstar/bin/simsrv */
    ulIndex = 0;
    pstProcInfo = &stProcInfos[ ulIndex ];
    pstProcInfo->bMonitorFlag    = TRUE;
    aos_sprintf( pstProcInfo->strProcCmd, "/dinstar/bin/simsrv" );
    pstProcInfo->ulMaxCpu        = ulMaxCpuRate;
    pstProcInfo->ulMaxMem        = 80;
    pstProcInfo->ulMaxErrorCount = ( 360 / PROC_CHECK_INTERVAL );
    pstProcInfo->ulMaxLostCount  = ( 120 / PROC_CHECK_INTERVAL );
#if 0
    /* 2. restund */
    ulIndex++;
    pstProcInfo = &stProcInfos[ ulIndex ];
    pstProcInfo->bMonitorFlag    = TRUE;
    aos_sprintf( pstProcInfo->strProcCmd, "/dinstar/app-restund/sbin/restund -f /dinstar/app-restund/etc/app-restund.conf" );
    pstProcInfo->ulMaxCpu        = ulMaxCpuRate;
    pstProcInfo->ulMaxMem        = 90;
    pstProcInfo->ulMaxErrorCount = ( 360 / PROC_CHECK_INTERVAL );
    pstProcInfo->ulMaxLostCount  = ( 120 / PROC_CHECK_INTERVAL );

    /* 3. restund2 */
    ulIndex++;
    pstProcInfo = &stProcInfos[ ulIndex ];
    pstProcInfo->bMonitorFlag    = TRUE;
    aos_sprintf( pstProcInfo->strProcCmd, "/dinstar/app-restund/sbin/restund -f /dinstar/app-restund/etc/app-restund2.conf" );
    pstProcInfo->ulMaxCpu        = ulMaxCpuRate;
    pstProcInfo->ulMaxMem        = 90;
    pstProcInfo->ulMaxErrorCount = ( 360 / PROC_CHECK_INTERVAL );
    pstProcInfo->ulMaxLostCount  = ( 120 / PROC_CHECK_INTERVAL );
#endif
    /* 4. tomcat */
    ulIndex++;
    pstProcInfo = &stProcInfos[ ulIndex ];
    pstProcInfo->bMonitorFlag    = TRUE;
    aos_sprintf( pstProcInfo->strProcCmd, "/usr/lib/jvm/default-java/bin/java" );
    pstProcInfo->ulMaxCpu        = ulMaxCpuRate;
    pstProcInfo->ulMaxMem        = 90;
    pstProcInfo->ulMaxErrorCount = ( 360 / PROC_CHECK_INTERVAL );
    pstProcInfo->ulMaxLostCount  = ( 120 / PROC_CHECK_INTERVAL );

    /* running info */
    printf( "simsafe is running..." );
    for ( ulIndex = 0; ulIndex < MAX_PROCESS_NUM; ulIndex++ )
    {
        pstProcInfo = &stProcInfos[ ulIndex ];

        if ( pstProcInfo->bMonitorFlag == FALSE )
        {
            continue;
        }

        printf( "\nmonitor[%d]: %s ", ulIndex, pstProcInfo->strProcCmd );
    }
    printf( "\n" );

    /* timing monitor */
    ulIndex = 0;
    while( TRUE )
    {
        ulIndex     = ( ulIndex + 1 ) % MAX_PROCESS_NUM;

        pstProcInfo = &stProcInfos[ ulIndex ];
        if ( pstProcInfo->bMonitorFlag == FALSE )
        {
            /* delay a moment */
            task_delay_ms( (U32)CHECK_TIMER_LEN * 1000 );
            continue;
        }

        if ( AOS_SUCC != get_process_run_info( pstProcInfo ) )
        {
            pstProcInfo->ulCurLostCount++;

            if ( pstProcInfo->ulCurLostCount >= 3  )
            {
                /* lost info */
                aos_sprintf( strOutBuff, "@@@@@@@@ simserver was LOST corrupt! caused by %s ",
                                         pstProcInfo->strProcCmd );
                sys_syslog_printf( "[simsafe]%s ", strOutBuff );
                printf( "%s \n", strOutBuff );

                aos_sprintf( strOutBuff, "@@@@@@@@ lost_count = %d/%d ",
                                         pstProcInfo->ulCurLostCount, pstProcInfo->ulMaxLostCount );
                sys_syslog_printf( "[simsafe]%s  ", strOutBuff );
                printf( "%s \n", strOutBuff );
            }
        }
        else if ( pstProcInfo->flCpu >= pstProcInfo->ulMaxCpu
               || pstProcInfo->flMem >= pstProcInfo->ulMaxMem )
        {
            pstProcInfo->ulCurLostCount  = 0;
            if ( ulIndex != 0 )
            {
                pstProcInfo->ulCurErrorCount++;

                if ( pstProcInfo->ulCurErrorCount >= 3  )
                {
                    /* error info */
                    aos_sprintf( strOutBuff, "@@@@@@@@ simserver was ERROR corrupt! caused by %s ",
                                             pstProcInfo->strProcCmd );
                    sys_syslog_printf( "[simsafe]%s  ", strOutBuff );
                    printf( "%s \n", strOutBuff );

                    aos_sprintf( strOutBuff, "@@@@@@@@ cpu = %s mem = %s error_count = %d/%d ",
                                             pstProcInfo->strCpu, pstProcInfo->strMem,
                                             pstProcInfo->ulCurErrorCount, pstProcInfo->ulMaxErrorCount );
                    sys_syslog_printf( "[simsafe]%s  ", strOutBuff );
                    printf( "%s \n", strOutBuff );
                }
            }
        }
        else
        {
            pstProcInfo->ulCurErrorCount  = 0;
            pstProcInfo->ulCurLostCount = 0;
        }

        /* check restart ? */
        if ( pstProcInfo->ulCurErrorCount >= pstProcInfo->ulMaxErrorCount )
        {
            pstProcInfo->ulCurLostCount = 0;

            /* error info */
            aos_sprintf( strOutBuff, "@@@@@@@@ simserver was ERROR corrupt! caused by %s ",
                                     pstProcInfo->strProcCmd );
            sys_syslog_printf( "[simsafe]%s  ", strOutBuff );
            printf( "%s \n", strOutBuff );

            aos_sprintf( strOutBuff, "@@@@@@@@ cpu = %s mem = %s error_time = %d sec ",
                                     pstProcInfo->strCpu, pstProcInfo->strMem,
                                     pstProcInfo->ulMaxErrorCount * PROC_CHECK_INTERVAL );
            sys_syslog_printf( "[simsafe]%s  ", strOutBuff );
            printf( "%s \n", strOutBuff );

            aos_sprintf( strOutBuff, "@@@@@@@@ reboot system!!! " );
            sys_syslog_printf( "[simsafe]%s  ", strOutBuff );
            printf( "%s \n", strOutBuff );

            /* reboot system */
            system( "reboot" );
        }
        else if ( pstProcInfo->ulCurLostCount >= pstProcInfo->ulMaxLostCount )
        {
            pstProcInfo->ulCurLostCount = 0;

            /* lost info */
            aos_sprintf( strOutBuff, "@@@@@@@@ simserver was LOST corrupt! caused by %s ",
                                     pstProcInfo->strProcCmd );
            sys_syslog_printf( "[simsafe]%s  ", strOutBuff );
            printf( "%s \n", strOutBuff );

            aos_sprintf( strOutBuff, "@@@@@@@@ lost_time = %d sec ",
                                     pstProcInfo->ulMaxLostCount * PROC_CHECK_INTERVAL );
            sys_syslog_printf( "[simsafe]%s  ", strOutBuff );
            printf( "%s \n", strOutBuff );

            aos_sprintf( strOutBuff, "@@@@@@@@ restart process!!! " );
            sys_syslog_printf( "[simsafe]%s  ", strOutBuff );
            printf( "%s \n", strOutBuff );

            /* only restart the lost process */
            system( "/dinstar/bin/start_app.sh auto" );
        }

        /* delay a moment */
        task_delay_ms( (U32)CHECK_TIMER_LEN * 1000 );
    }

    return AOS_SUCC;
}

