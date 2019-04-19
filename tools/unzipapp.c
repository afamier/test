

/*  ������Ź��������ͻ��ˣ����������˷���������Ϣ
�����һ��ʱ���û���յ�������Ϣ������Ϊ�����Ѿ�����������˻�����*/

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/resource.h>
#include <signal.h>
#include <arpa/inet.h>
#include <errno.h>
#include "fcntl.h"

#define AOS_ATTRIBUTE  __attribute__ ((packed,aligned(1)))


typedef char          		S8;
typedef short          		S16;
typedef long		   		S32;

typedef unsigned char   	U8;
typedef unsigned short  	U16;
typedef unsigned long   	U32;

#define  MAX_FILENAME_LEN      20//�ú궨����Ҫ��addhead����һ��

typedef  struct tagFileHeadStruct{
    U32    ulFileLength;                    /*�ļ�����,����tag*/
    U32    ulCompressTag;                   /*�Ƿ񾭹�ѹ��*/
    U32    ulFileType;                      /*�ļ�����/�������*/
    U32    ulDate;                          /*����*/
    U32    ulTime;                          /*ʱ��*/
    U32    ulVersionID;                     /*�汾*/
    U32    usCRC;                           /*CRCУ����*/
    S8     szFileName[MAX_FILENAME_LEN];   /*�汾�ļ���*/
}AOS_ATTRIBUTE LOAD_FILE_TAG;


void  convert_filehead_n2h(LOAD_FILE_TAG * pFileHead)
{
    pFileHead->ulFileLength  = ntohl(pFileHead->ulFileLength);
    pFileHead->ulCompressTag = ntohl(pFileHead->ulCompressTag);
    pFileHead->ulFileType    = ntohl(pFileHead->ulFileType);
    pFileHead->ulDate        = ntohl(pFileHead->ulDate);
    pFileHead->ulTime        = ntohl(pFileHead->ulTime);
    pFileHead->ulVersionID   = ntohl(pFileHead->ulVersionID);
    pFileHead->usCRC=          ntohl(pFileHead->usCRC);

}

unsigned short CalCRC ( unsigned char *ptr, int count )
{
    unsigned short ulCrc;
    unsigned short iTmp ;
    unsigned char * cTmp ;

    ulCrc = 0 ;
    cTmp = ptr ;

    while ( --count >= 0 )
    {
    	ulCrc = ulCrc ^ ( ( unsigned short )( *cTmp++ ) << 8  );
    	for ( iTmp = 0 ; iTmp < 8 ; ++iTmp )
    		if ( ulCrc & 0x8000 )
    			ulCrc = ( ulCrc << 1 ) ^ 0x1021 ;
    		else
    			ulCrc = ulCrc << 1 ;
    }

    return ( ulCrc & 0xFFFF ) ;
}

void catching_sigs(int signo)
{
    int x;
    switch(signo)
    {
        case SIGINT:
            printf("catching_sigs()->SIGINT\n");
            break;
        case SIGSEGV:
            printf("catching_sigs()->SIGSEGV\n");
            break;
        case SIGQUIT:
            printf("catching_sigs()->SIGQUIT\n");
            break;
        case SIGTERM:
            printf("catching_sigs()->SIGTERM\n");
            break;
        case SIGKILL:
            printf("catching_sigs()->SIGKILL\n");
            break;
        case SIGPIPE:
            printf("catching_sigs()->SIGPIPE\n");
            break;
        default :
            printf("catching_sigs()->SIG(%d)\n",signo);
            break;
    }
}


int unzip_program(S8 * pszSrcName, S8 * pszDstName, int blverbose)
{
    FILE * fl, *fldst;
    LOAD_FILE_TAG stFileTag;
    S32 ret = 0;
    S32 filelen;
    U16 usCrc;
    U32 i;
    S8 szBuf[1024];
    int inlen, outlen;

    fl = fopen(pszSrcName, "rb");
    if(NULL == fl)
    {
        if(blverbose)
            printf("\r\n open %s file fail", pszSrcName);
        //�����ڣ��Ǿ�ȥ����һ�ݱ��õĹ���
        goto err_proc;
    }

    fseek(fl, 0, SEEK_END);
    filelen = ftell(fl);
    fseek(fl, 0, SEEK_SET);
    ret = fread(&stFileTag, sizeof(stFileTag), 1, fl);
    if(1 != ret)
    {
        if(blverbose)
            printf("\r\n read %s file fail", pszSrcName);
        goto err0_proc;
    }
    convert_filehead_n2h(&stFileTag);

    if(stFileTag.ulFileLength + sizeof(stFileTag) != filelen)
    {
        if(blverbose)
            printf("\r\n file %s len is not correct", pszSrcName);
        goto err0_proc;
    }

    //��ɾ��ԭ����
    unlink(pszDstName);
    
    //����ѹ���ļ�
    fldst = fopen(pszDstName, "wb+");
    if(NULL == fldst)
    {
        if(blverbose)
            printf("\r\n create file %s fail", pszDstName);
        goto err0_proc;
    }

    for(i = 0; i < stFileTag.ulFileLength/sizeof(szBuf); i++)
    {
        inlen = fread(szBuf, 1, sizeof(szBuf), fl);
        outlen = fwrite(szBuf, 1, inlen, fldst);
        if(outlen != inlen)
        {
            goto err1_proc;
        }
    }

    if(stFileTag.ulFileLength%sizeof(szBuf))
    {
        inlen = fread(szBuf, 1, stFileTag.ulFileLength-(i*sizeof(szBuf)), fl);
        outlen = fwrite(szBuf, 1, inlen, fldst);
        if(outlen != inlen)
        {
            goto err1_proc;
        }        
    }
    
    fclose(fl);
    fclose(fldst);

    if(blverbose)
        printf("\r\n unzip file %s to %s succ\r\n", pszSrcName, pszDstName);
    

    return 0;
    
err1_proc:
    fclose(fldst);
    unlink(pszDstName);
err0_proc:
    fclose(fl);
err_proc:
    return -1;
}


int main(int argc, char *argv[])
{
    int ret;
    char szBuf[126];
    int ulTimeOutCount = 0;
    int ulMaxTimeoutCount = 5;
    int blverbose = 0;
    struct sigaction act;
    FILE * fl;
    

    if(argc < 2)
    {
        printf("\r\n useage: unzipapp srcname dstname [verbose]\r\n");
        return -1;
    }

    if(argc > 3)
    {
        if(strncmp(argv[3], "v", 1))
        {
            printf("\r\n useage: unzipapp srcname dstname [verbose]\r\n");
            return -1;               
        }
        blverbose = 1;
    }

    if(blverbose)
            printf("\r\n srcname %s, dstname:%s", argv[1], argv[2]);

    memset(&act, 0, sizeof(struct sigaction));
    act.sa_handler = catching_sigs;
    act.sa_flags   = 0;
    sigemptyset(&act.sa_mask);       //��ʼ��һ���յ��źż���sa_mask

    if(sigaction(SIGINT, &act, NULL) < 0)
    {
        if(blverbose)
        printf("catch SIGINT failed!\n");
        return -1;
    }
    if(sigaction(SIGQUIT, &act, NULL) < 0)
    {
        if(blverbose)
        printf("catch SIGQUIT failed!\n");
        return -1;
    }
    if(sigaction(SIGTERM, &act, NULL) < 0)
    {
        if(blverbose)
        printf("catch SIGTERM failed!\n");
        return -1;
    }


    return unzip_program(argv[1], argv[2], blverbose);
}





