/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/* Description: 在版本文件头上添加一个文件标签                               */
/*                                                                           */
/* Others:                                                                   */
/*                                                                           */
/* History:                                                                  */
/* 1. Date:                                                                  */
/*    Author:                                                                */
/*    Modification: Create this file                                         */
/*    2005-03-22 eric 增加一种文件类型(book-6,电话号段表)                    */
/*                                                                           */
/*****************************************************************************/



#ifdef _MSC_VER
#include <winsock2.h>
#include <windows.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <io.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>
#define strcasecmp stricmp
#define strncasecmp  strnicmp 
#else
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <time.h>
#endif

#define LOAD_FILE_EXT_LEN   3
#define LOAD_FILE_LEN       8
#define LOAD_FILENAME_LEN   20
#define LOAD_TMP_BUFFER     (10*1024*1024)

unsigned char buffer[LOAD_TMP_BUFFER];

typedef  struct FileTagStruct{
    unsigned long    udwFileLength;    /*文件长度*/
    unsigned long    udwCompressTag;     /*是否经过压缩*/
    unsigned long    udwFileType;        /*文件类型/软件类型*/
    unsigned long    udwDate;          /*日期*/
    unsigned long    udwTime;          /*时间*/
    unsigned long    udwVersionID;     /*版本*/
    unsigned long    udwCrc;           /*CRC校验码*/
    char     pszFileName[LOAD_FILENAME_LEN]; /*版本文件名*/
}LOAD_FILETAG;

typedef struct DATESTURCT{
    unsigned short  year;
    char  month;
    char  date;
}DATES;

typedef struct TIMESTURCT{
    char hour;
    char minute;
    char second;
}TIME;

enum
{
    LOAD_OS_TYPE_LINUX =0,
    LOAD_OS_TYPE_VXWORKS,
    LOAD_OS_TYPE_WINDOWS,
    LOAD_OS_TYPE_UCLINUX,
};

enum
{
    LOAD_CPU_TYPE_ARM =0,
    LOAD_CPU_TYPE_X86,
    LOAD_CPU_TYPE_MIPS,
    LOAD_CPU_TYPE_PPC,
};

enum
{
    LOAD_PRODUCT_MTG600 =0,
    LOAD_PRODUCT_MTG200 =1,
    LOAD_PRODUCT_MTG1000 =2,
    LOAD_PRODUCT_MTG1000C= 3,
    LOAD_PRODUCT_SIM_BOX = 30,
    LOAD_PRODUCT_SIM_BANK = 31,
    LOAD_PRODUCT_SIM_SERVER = 32,    
    LOAD_PRODUCT_AG = 100,
    LOAD_PRODUCT_WG = 150,
};

#define LOAD_GET_OS_TYPE_FLAG(a) ((a)>>24)
#define LOAD_GET_CPU_TYPE_FLAG(a) (((a)>>16)&0xff)
#define LOAD_GET_PRODUCT_TYPE_FLAG(a) ((a)&0xffff)

#define LOAD_SET_OS_TYPE_FLAG(a, b) do{a |= ((b)<<24);}while(0)
#define LOAD_SET_CPU_TYPE_FLAG(a, b) do{a |= ((b)<<16);}while(0)
#define LOAD_SET_PRODUCT_TYPE_FLAG(a, b) do{a |= ((b)&0xffff);}while(0)


unsigned long getfilesize(char * filename);
unsigned long iscompress(char * filename);
unsigned long calfilecrc(char * filename,unsigned short * crc);
char * getfilename(char * fullpath);
unsigned long genverfile(char * filename,char filetype,unsigned long verid
                            , unsigned long os, unsigned long cpu, unsigned long product);
void  ConverFileHeadHtoN(LOAD_FILETAG * pFileHead);
void  ConverFileHeadNtoH(LOAD_FILETAG * pFileHead);

#define a_isdigit(c)				((c) >= '0' && (c) <= '9')
#define stricmp  strcasecmp

/*========================================================================*/
/*                                                                        */
/*    Function Name    : CalCRC                                           */
/*    Function Func.   : Calculate the CRC                                */
/*                                                                        */
/*========================================================================*/
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

int main( int argc, char *argv[])
{
    unsigned long verid;
    char * tempstr;
    char * left;
    unsigned long temp2;
    int   i;
    int   fileType = 0;
	int	  strlength = 0;
    char  ver_info[32] = { 0 };
	unsigned long ostype = LOAD_OS_TYPE_LINUX, cputype, producttype;


    if ( argc != 7 )
    {
        printf("please input parameter as follow.\n\n");
        printf("	addfiletag    | filename | | version | | filetype | cpu | product | ostype\n\n");
		printf("	filetype: program data web driver msp voice\r\n");
		printf("	cpu     : arm mips ppc x86\n");
		printf("	product : mtg200 mtg600 mtg1000b mtg1000 ag wg simbox simbank simsrv\n");
		printf("	ostype  : linux vxworks windows uclinux\n");
        
        exit(0);
    }

    printf( "addhead %s %s %s %s %s %s \n",
            argv[1], argv[2], argv[3], argv[4], argv[5], argv[6] );

    /*参数合法性检测*/
    left = strrchr(argv[1],'.');
    if ( left == NULL )
    {
        printf("filename no extend name.\n");
        exit(0);
    }
    else
    {
        if ( strlen(left) < 1 )
        {
            printf("extend name length should be larger than 1\n");
            exit(0);
        }
    }

    strncpy( ver_info, argv[2], 32 );
    for ( i = 0; i < (int)strlen( ver_info ); i ++ )
    {
        /* special for 02320450-64bit */
        if ( ver_info[i] == '-' )
        {
            ver_info[i]  = '\0';
            break;
        }

        if (!a_isdigit(ver_info[i]))
        {
            printf("add filetag fail 1.\n");
            return -1; 
        }
    }

    temp2 = strtoul(ver_info,&tempstr,10);

    verid    = temp2;

	strlength = strlen(argv[3]);

	if(strlength < 1 || strlength > 7)
	{
		printf("filetype should be: program data web driver msp voice\n");
		return -1;
	}

	if(1 == strlength && 'p' == argv[3][0])
	{
		printf("filetype is program or patch ?\n");
		return -1;
	}

	if(0 == strncmp("program",argv[3],strlength))
	{
		fileType = 1;
	}

	else if(0 == strncmp("patch",argv[3],strlength))
	{
		fileType = 2;
	}

	else if(0 == strncmp("data",argv[3],strlength))
	{
		fileType = 3;
	}

	else if(0 == strncmp("logic",argv[3],strlength))
	{
		fileType = 4;
	}

	else if(0 == strncmp("book",argv[3],strlength))
	{
		fileType = 6;/*类型5要保留给MIB做立即加载使用*/
	}
	
	else if(0 == strncmp("web",argv[3],strlength))
	{
		fileType = 7;/*etg web*/
	}

	else if(0 == strncmp("route",argv[3],strlength))
	{
		fileType = 8;/*国际话务路由*/
	}
	else if(0 == strncmp("voice",argv[3],strlength))
	{
		fileType = 12;/*国际话务路由*/
	}
	else if(0 == strncmp("driver",argv[3],strlength))
	{
		fileType = 14;/*国际话务路由*/
	}
	else if(0 == strncmp("msp",argv[3],strlength))
	{
		fileType = 15;/*国际话务路由*/
	}

	else
	{
		printf("filetype should be program or patch or data or logic or book or driver\n");
		return -1;
	}

    strlength = strlen(argv[4]);
	if(0 == strncmp("arm",argv[4],strlength))
	{
		cputype = LOAD_CPU_TYPE_ARM;
	}
	else if(0 == strncmp("mips",argv[4],strlength))
	{
		cputype = LOAD_CPU_TYPE_MIPS;
	}
	else if(0 == strncmp("ppc",argv[4],strlength))
	{
		cputype = LOAD_CPU_TYPE_PPC;
	}
	else if(0 == strncmp("x86",argv[4],strlength))
	{
		cputype = LOAD_CPU_TYPE_X86;
	}

	else
	{
		printf("cpu should be cpu: arm or mips or ppc or x86\n");
		return -1;
	}
	
    strlength = strlen(argv[5]);
	if(0 == strncmp("mtg200",argv[5],strlength))
	{
		producttype = LOAD_PRODUCT_MTG200;
	}
	else if(0 == strncmp("mtg600",argv[5],strlength))
	{
		producttype = LOAD_PRODUCT_MTG600;
	}
	else if(0 == strncmp("mtg1000",argv[5],strlength))
	{
		producttype = LOAD_PRODUCT_MTG1000C;
	}
	else if(0 == strncmp("mtg1000b",argv[5],strlength))
	{
		producttype = LOAD_PRODUCT_MTG1000;
	}
	else if(0 == strncmp("ag",argv[5],strlength))
	{
		producttype = LOAD_PRODUCT_AG;
	}
	else if(0 == strncmp("wg",argv[5],strlength))
	{
		producttype = LOAD_PRODUCT_WG;
	}
	else if(0 == strncmp("simbank",argv[5],strlength))
	{
		producttype = LOAD_PRODUCT_SIM_BANK;
	}
	else if(0 == strncmp("simbox",argv[5],strlength))
	{
		producttype = LOAD_PRODUCT_SIM_BOX;
	}
	else if(0 == strncmp("simsrv",argv[5],strlength))
	{
		producttype = LOAD_PRODUCT_SIM_SERVER;
	}
	else
	{
		printf("product should be product: mtg200 or mtg600 or mtg1000 or ag\n");
		return -1;
	}

    strlength = strlen(argv[6]);
	if(0 == strncmp("linux",argv[6],strlength))
	{
		ostype = LOAD_OS_TYPE_LINUX;
	}
	else if(0 == strncmp("vxworks",argv[6],strlength))
	{
		ostype = LOAD_OS_TYPE_VXWORKS;
	}
	else if(0 == strncmp("windows",argv[6],strlength))
	{
		ostype = LOAD_OS_TYPE_WINDOWS;
	}
	else if(0 == strncmp("uclinux",argv[6],strlength))
	{
		ostype = LOAD_OS_TYPE_UCLINUX;
	}
	else
	{
		printf("os should be: linux vxworks windows uclinux\n");
		return -1;
	}

	
    if ( 0xffffffff == genverfile(argv[1],fileType,verid, ostype, cputype, producttype) )
    {
        printf("add filetag fail 2.\n");
    }
    else
    {
        printf("add filetag success.\n");
    }

    return 0;

}

unsigned long getfilesize(char * filename)
{
	struct stat buf;  
    if(stat(filename, &buf)<0)  
    {  
        return 0xffffffff; 
    }  
    return (unsigned long)buf.st_size; 
}

unsigned long iscompress(char * filename)
{
    int len;
    char extName[5];
	char * p;

    len = strlen(filename);

    if ( len < 4 )
    {
        return 0xffffffff;
    }

	p = strchr(filename, '.');
	if(NULL == p)
	{
		return 0xffffffff;
	}
	
    strcpy(extName,(p));

    if ( stricmp(extName,".arj") == 0 || 0 == stricmp(extName,".gz"))
    {
        return 1;
    }
    else
    {
        return 0;
    }

}

/*注意，缓冲区一次性得能打开文件*/
unsigned long calfilecrc(char * filename,unsigned short * crc)
{
    FILE * fh;
    unsigned long readlen, filelen;
    unsigned int  count  =0;
    unsigned short ulCrc;
    unsigned char * cTmp ;

	filelen = getfilesize(filename);

    fh = fopen(filename, "rb");
    if ( NULL == fh)
    {
        return 0xffffffff;
    }

    readlen = fread(buffer,filelen,1, fh);
    if (1 != readlen)
    {
        fclose(fh);
        return 0xffffffff;
    }
    fclose(fh);

    cTmp = buffer;
    count = filelen;
    ulCrc = CalCRC(cTmp, count);

    *crc = ( ulCrc & 0xFFFF );

    return 0;
   
}

unsigned long getfiledate(char * filename, DATES * date,TIME * time)
{
	struct tm *p;
	struct stat buf;  

    if(stat(filename, &buf)<0)  
    {  
        return 0xffffffff; 
    }  

	p = localtime(&buf.st_ctime);

    date->year  = 1900+p->tm_year;
    date->month = (unsigned char)(1+p->tm_mon);
    date->date  = (unsigned char)p->tm_mday;

    time->hour   = (unsigned char)p->tm_hour;
    time->minute = (unsigned char)p->tm_min;
    time->second = (unsigned char)p->tm_sec;

    return 0;
}

//char buffer[LOAD_TMP_BUFFER];


unsigned long cp_file(char * sourfile, char * destfile, LOAD_FILETAG * pfiletag)
{
    long filelen, len;
    unsigned char szBuf[2048];
    unsigned long i;
    FILE * ffrom, *fto;
    
    ffrom =fopen(sourfile, "rb");
    if(NULL == ffrom)
    {
        return 0xffffffff;;
    }
    fseek(ffrom, 0, SEEK_END);
    filelen = ftell(ffrom);
    fseek(ffrom, 0, SEEK_SET);
    
    fto =fopen(destfile, "wb+");
    if(NULL == fto)
    {
        goto end_proc0;
    }
    fseek(fto, 0, SEEK_SET);

	fwrite(pfiletag, sizeof(LOAD_FILETAG), 1, fto);

    i = 0;
    while(i++ < (filelen/sizeof(szBuf)))
    {
        fread(szBuf, sizeof(szBuf), 1, ffrom);
        fwrite(szBuf, sizeof(szBuf), 1, fto);
    }

    if(filelen % sizeof(szBuf))
    {
        fread(szBuf, filelen % sizeof(szBuf), 1, ffrom);
        fwrite(szBuf, filelen % sizeof(szBuf), 1, fto);
    }
    fflush(fto);
    fclose(fto);

    fclose(ffrom);
	return 0;
end_proc0:
    fclose(ffrom);
	return 0xffffffff;;
}

char * getfilename(char * fullpath)
{
    char * temp1;
    char * temp2;
    char * temp3;

    temp1 = strrchr(fullpath,':');

    if ( temp1 == NULL )
    {
        temp2 = strrchr(fullpath,'\\');
        temp3 = strrchr(fullpath,'/');

        if ( temp2 == NULL && temp3 == NULL )
        {
            return fullpath;
        }
        else
        {
            if ( temp2 == NULL )
            {
                return temp3 + 1;
            }
            else
            {
                return temp2 + 1;
            }
        }
    }
    else
    {
        temp2 = strrchr(temp1,'\\');
        temp3 = strrchr(temp1,'/');

        if ( temp2 == NULL && temp3 == NULL )
        {
            return temp1;
        }
        else
        {
            if ( temp2 == NULL )
            {
                return temp3 + 1;
            }
            else
            {
                return temp2 + 1;
            }
        }
    }
}

unsigned long gendestfilename(char * filename, char *destfilename)
{
    char * right;
    int    len;

    destfilename[0] = '\0';

    right = strrchr(filename,'.');
    if ( right == NULL )
    {
        return 0xffffffff;
    }
    len = right - filename + 1;
    strncpy(destfilename,filename,len);
    destfilename[len] = '\0';
    strcat(destfilename,"ldf");
    destfilename[len + 3] = '\0';

    printf("filename = %s, destfilename = %s\n",filename,destfilename);

    return 0;
}

unsigned long genverfile(char * filename,char filetype,unsigned long verid
                            , unsigned long os, unsigned long cpu, unsigned long product)
{
    char * tmpname;
    unsigned long  filelen;
    unsigned long  compress;
    unsigned short  crc;
    DATES  date;
    TIME   time;
    char   destfilename[LOAD_FILENAME_LEN];

    LOAD_FILETAG filetag;

    if ( (filelen = getfilesize(filename)) == 0xffffffff )
    {
        printf( "get file size error! filename = %s \n",filename );
        return 0xffffffff;
    }

    if ( (compress = iscompress(filename)) == 0xffffffff )    
    {
        printf( "is compress error! filename = %s \n",filename );
        return 0xffffffff;
    }


    if ( 0xffffffff == getfiledate(filename,&date,&time) )
    {
        printf( "get file date error! filename = %s \n",filename );
        return 0xffffffff;
    }

    
    if ( 0xffffffff == calfilecrc(filename,&crc) )
    {
        printf( "calc file crc error! filename = %s \n",filename );
        return 0xffffffff;
    }

	memset( &filetag, 0, sizeof(filetag) );
    filetag.udwFileLength = filelen;
    filetag.udwVersionID  = verid;
    LOAD_SET_CPU_TYPE_FLAG(filetag.udwCompressTag, cpu);
    LOAD_SET_OS_TYPE_FLAG(filetag.udwCompressTag, os);
    LOAD_SET_PRODUCT_TYPE_FLAG(filetag.udwCompressTag, product);
    filetag.udwCrc        = crc;
    filetag.udwFileType     = filetype;
    /*
    udwDate	VOS_UINT32	4	年16位+月8位+日8位	2000.12.20
    udwTime	VOS_UINT32	4	最高8位保留，时8位+分8位+秒8位。	13:20:40
    */

    filetag.udwDate = (((unsigned long)(date.year)) << 16) + (((unsigned long)(date.month))<< 8) +
                      (unsigned long)(date.date);
    filetag.udwTime = (((unsigned long)(time.hour)) << 16)+ (((unsigned long)(time.minute)) << 8) +
                      (unsigned long)(time.second);

    if ( strlen(filename) > LOAD_FILENAME_LEN )
    {
        printf( "strlen of filename error! filename = %s, len = %d/%d \n", filename, (int)strlen(filename), (int)LOAD_FILENAME_LEN );
        return 0xffffffff;
    }
    
    gendestfilename(filename,destfilename);

    tmpname = getfilename(destfilename);

    if ( tmpname == NULL )
    {
        printf( "temp filename error! \n" );
        return 0xffffffff;
    }

    if ( strlen(tmpname) >= LOAD_FILENAME_LEN ) 
    {
        printf( "strlen of temp filename error! temp filename = %s, len = %d/%d \n", tmpname, (int)strlen(tmpname), (int)LOAD_FILENAME_LEN );
        return 0xffffffff;
    }

    strcpy((char*)filetag.pszFileName,tmpname);
    filetag.pszFileName[LOAD_FILENAME_LEN - 1] = '\0';

   ConverFileHeadHtoN(&filetag);

    if ( 0xffffffff == cp_file(filename,destfilename,&filetag) )
    {
        printf( "cp_file error! filename = %s, destfilename = %s \n", filename, destfilename );
        return 0xffffffff;
    }
    else
    {
/*
        if ( FALSE == DeleteFile(filename) )
        {
            printf("delete file = %s error. \n",filename);
            printf("error : = %d \n",GetLastError());
        }
        
*/
    }

    return 0;
}
void  ConverFileHeadHtoN(LOAD_FILETAG * pFileHead)
{
    pFileHead->udwFileLength  = htonl(pFileHead->udwFileLength);
    pFileHead->udwCompressTag = htonl(pFileHead->udwCompressTag);
    pFileHead->udwFileType    = htonl(pFileHead->udwFileType);
    pFileHead->udwDate        = htonl(pFileHead->udwDate);
    pFileHead->udwTime        = htonl(pFileHead->udwTime);
    pFileHead->udwVersionID   = htonl(pFileHead->udwVersionID);
    pFileHead->udwCrc=          htonl(pFileHead->udwCrc);
}
void  ConverFileHeadNtoH(LOAD_FILETAG * pFileHead)
{
    pFileHead->udwCompressTag = ntohl(pFileHead->udwCompressTag);
    pFileHead->udwCrc=          ntohl(pFileHead->udwCrc);
    pFileHead->udwDate        = ntohl(pFileHead->udwDate);
    pFileHead->udwFileLength  = ntohl(pFileHead->udwFileLength);
    pFileHead->udwFileType    = ntohl(pFileHead->udwFileType);
    pFileHead->udwTime        = ntohl(pFileHead->udwTime);
    pFileHead->udwVersionID   = ntohl(pFileHead->udwVersionID);
}




