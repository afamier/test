
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
#include "source/include/xml/mxml.h"

#define AOS_FAIL -1
#define AOS_SUCC 0
#define FALSE 0
#define TRUE 1

mxml_node_t * g_pstXmlRoot;

/*
xml2cfg src dst -v
*/
int main(int argc, char *argv[])
{
    FILE * fp;
    int verbose = FALSE;
    int i;
    char szBuf[24];
    
    mxml_node_t * network, * param, * eth, *vlan, *natfwd, * fwd;
    const char * value;

    if(argc < 3)
    {
        printf("\r\ntoo few param\r\n");
        return AOS_FAIL;
    }

    if(argc > 3)
    {
        if(strcmp(argv[3], "-v"))
        {
            printf("\r\n bad param %s \r\n", argv[2]);
            return AOS_FAIL;
        }

        verbose = TRUE;
    }

    if(NULL == strstr(argv[1], ".xml"))
    {
        if(verbose)
        {
            printf("\r\n not xml file \r\n");
            return AOS_FAIL;
        }
    }
    

    fp = fopen(argv[1], "r");
    if(NULL == fp)
    {
        if(verbose)
                printf("\r\n open file %s fail \r\n", argv[1]);
        return AOS_FAIL;
    }
    else
    {
        g_pstXmlRoot = mxmlLoadFile(NULL, fp,
                            MXML_OPAQUE_CALLBACK);
        fclose(fp);
        
        if(NULL == g_pstXmlRoot)
        {
            if(verbose)
                printf("\r\n load xml file fail \r\n");
            return AOS_FAIL;
        }
    }

    fp = fopen(argv[2], "w");
    if(NULL == fp)
    {
        if(verbose)
                printf("\r\n open file %s fail \r\n", argv[2]);
        goto err_proc0;
    }

    
    fprintf(fp, "#ETHTYPE:bridge,router,mngt\n"
                "#bridge:two interface as same one\n"
                "#router:two interface with nat\n"
                "#mngt:two interface whitout nat, only for mtg\n"
                "#WAN NETMODE:static,dhcp,pppoe\n"
                "#mac style: xx:xx:xx:xx:xx:xx\n"
                "#flag value: on/off\n\n");
    

    network = mxmlFindElement(g_pstXmlRoot, g_pstXmlRoot, "network"
                            , NULL, NULL, MXML_DESCEND);
    if(NULL == network)
    {
        if(verbose)
                printf("\r\n get network fail \r\n");
        goto err_proc1;
    }

    
    param = mxmlFindElement(network, g_pstXmlRoot, "param"
                        , "name", "ethtype", MXML_DESCEND);
    if(NULL == param)
    {
        if(verbose)
                printf("\r\n get ethtype fail \r\n");
        goto err_proc1;
    }    
    value = mxmlElementGetAttr(param, "value");
    fprintf(fp, "ETHTYPE=%s\n", (value&&*value)?value:"");

    /*---------------------wan-------------------*/

    eth = mxmlFindElement(network, g_pstXmlRoot, "eth0"
                            , NULL, NULL, MXML_DESCEND);
    if(NULL == eth)
    {
        goto err_proc1;
    }
    
    param = mxmlFindElement(eth, g_pstXmlRoot, "param"
                        , "name", "ipmode", MXML_DESCEND);
    if(NULL == param)
    {
        goto err_proc1;
    }
    value = mxmlElementGetAttr(param, "value");
    fprintf(fp, "NETMODE=%s\n", (value&&*value)?value:"");

    fprintf(fp, "\n");
    
    param = mxmlFindElement(eth, g_pstXmlRoot, "param"
                        , "name", "ip", MXML_DESCEND);
    if(NULL == param)
    {
        goto err_proc1;
    }
    value = mxmlElementGetAttr(param, "value");
    fprintf(fp, "WANIP=%s\n", (value&&*value)?value:"");

    param = mxmlFindElement(eth, g_pstXmlRoot, "param"
                        , "name", "mask", MXML_DESCEND);
    if(NULL == param)
    {
        goto err_proc1;
    }
    value = mxmlElementGetAttr(param, "value");
    fprintf(fp, "WANMASK=%s\n", (value&&*value)?value:"");

    param = mxmlFindElement(eth, g_pstXmlRoot, "param"
                        , "name", "mtu", MXML_DESCEND);
    if(NULL == param)
    {
        goto err_proc1;
    }
    value = mxmlElementGetAttr(param, "value");
    fprintf(fp, "WANMTU=%s\n", (value&&*value)?value:"");

    param = mxmlFindElement(eth, g_pstXmlRoot, "param"
                        , "name", "gateway", MXML_DESCEND);
    if(NULL == param)
    {
        goto err_proc1;
    }
    value = mxmlElementGetAttr(param, "value");
    fprintf(fp, "WANGATEWAY=%s\n", (value&&*value)?value:"");

    param = mxmlFindElement(eth, g_pstXmlRoot, "param"
                        , "name", "pppuser", MXML_DESCEND);
    if(NULL == param)
    {
        goto err_proc1;
    }
    value = mxmlElementGetAttr(param, "value");
    fprintf(fp, "PPPUSERNAME=%s\n", (value&&*value)?value:"");

    param = mxmlFindElement(eth, g_pstXmlRoot, "param"
                        , "name", "ppppwd", MXML_DESCEND);
    if(NULL == param)
    {
        goto err_proc1;
    }
    value = mxmlElementGetAttr(param, "value");
    fprintf(fp, "PPPPASSWORD=%s\n", (value&&*value)?value:"");

    param = mxmlFindElement(eth, g_pstXmlRoot, "param"
                        , "name", "usepeerdns", MXML_DESCEND);
    if(NULL == param)
    {
        goto err_proc1;
    }
    value = mxmlElementGetAttr(param, "value");
    fprintf(fp, "USEPEERDNS=%s\n", (value&&*value)?value:"");

    param = mxmlFindElement(eth, g_pstXmlRoot, "param"
                        , "name", "dns1", MXML_DESCEND);
    if(NULL == param)
    {
        goto err_proc1;
    }
    value = mxmlElementGetAttr(param, "value");
    fprintf(fp, "WANDNS1=%s\n", (value&&*value)?value:"");

    param = mxmlFindElement(eth, g_pstXmlRoot, "param"
                        , "name", "dns2", MXML_DESCEND);
    if(NULL == param)
    {
        goto err_proc1;
    }
    value = mxmlElementGetAttr(param, "value");
    fprintf(fp, "WANDNS2=%s\n", (value&&*value)?value:"");

    param = mxmlFindElement(eth, g_pstXmlRoot, "param"
                        , "name", "mac", MXML_DESCEND);
    if(NULL == param)
    {
        goto err_proc1;
    }
    value = mxmlElementGetAttr(param, "value");
    fprintf(fp, "WANMAC=%s\n", (value&&*value)?value:"");

    fprintf(fp, "\n");
    /*---------------------data vlan-------------------*/

    vlan = mxmlFindElement(eth, g_pstXmlRoot, "datavlan"
                        , NULL, NULL, MXML_DESCEND);
    if(NULL == vlan)
    {
        goto err_proc1;
    }

    param = mxmlFindElement(vlan, g_pstXmlRoot, "param"
                        , "name", "flag", MXML_DESCEND);
    if(NULL == param)
    {
        goto err_proc1;
    }
    value = mxmlElementGetAttr(param, "value");
    fprintf(fp, "DATAVLANFLAG=%s\n", (value&&*value)?value:"");

    param = mxmlFindElement(vlan, g_pstXmlRoot, "param"
                        , "name", "vlanid", MXML_DESCEND);
    if(NULL == param)
    {
        goto err_proc1;
    }
    value = mxmlElementGetAttr(param, "value");
    fprintf(fp, "DATAVLANID=%s\n", (value&&*value)?value:"");

    param = mxmlFindElement(vlan, g_pstXmlRoot, "param"
                        , "name", "priid", MXML_DESCEND);
    if(NULL == param)
    {
        goto err_proc1;
    }
    value = mxmlElementGetAttr(param, "value");
    fprintf(fp, "DATAVLANPRIID=%s\n", (value&&*value)?value:"");

    fprintf(fp, "\n");
    /*---------------------voice vlan-------------------*/
    vlan = mxmlFindElement(eth, g_pstXmlRoot, "voicevlan"
                        , NULL, NULL, MXML_DESCEND);
    if(NULL == vlan)
    {
        goto err_proc1;
    }

    param = mxmlFindElement(vlan, g_pstXmlRoot, "param"
                        , "name", "flag", MXML_DESCEND);
    if(NULL == param)
    {
        goto err_proc1;
    }
    value = mxmlElementGetAttr(param, "value");
    fprintf(fp, "VOICEVLANFLAG=%s\n", (value&&*value)?value:"");

    param = mxmlFindElement(vlan, g_pstXmlRoot, "param"
                        , "name", "vlanid", MXML_DESCEND);
    if(NULL == param)
    {
        goto err_proc1;
    }
    value = mxmlElementGetAttr(param, "value");
    fprintf(fp, "VOICEVLANID=%s\n", (value&&*value)?value:"");

    param = mxmlFindElement(vlan, g_pstXmlRoot, "param"
                        , "name", "priid", MXML_DESCEND);
    if(NULL == param)
    {
        goto err_proc1;
    }
    value = mxmlElementGetAttr(param, "value");
    fprintf(fp, "VOICEVLANPRIID=%s\n", (value&&*value)?value:"");

    param = mxmlFindElement(vlan, g_pstXmlRoot, "param"
                        , "name", "ipmode", MXML_DESCEND);
    if(NULL == param)
    {
        goto err_proc1;
    }
    value = mxmlElementGetAttr(param, "value");
    fprintf(fp, "VOICENETMODE=%s\n", (value&&*value)?value:"");

    param = mxmlFindElement(vlan, g_pstXmlRoot, "param"
                        , "name", "ip", MXML_DESCEND);
    if(NULL == param)
    {
        goto err_proc1;
    }
    value = mxmlElementGetAttr(param, "value");
    fprintf(fp, "VOICEIP=%s\n", (value&&*value)?value:"");


    param = mxmlFindElement(vlan, g_pstXmlRoot, "param"
                        , "name", "mask", MXML_DESCEND);
    if(NULL == param)
    {
        goto err_proc1;
    }
    value = mxmlElementGetAttr(param, "value");
    fprintf(fp, "VOICEMASK=%s\n", (value&&*value)?value:"");

    param = mxmlFindElement(vlan, g_pstXmlRoot, "param"
                        , "name", "mtu", MXML_DESCEND);
    if(NULL == param)
    {
        goto err_proc1;
    }
    value = mxmlElementGetAttr(param, "value");
    fprintf(fp, "VOICEMTU=%s\n", (value&&*value)?value:"");

    param = mxmlFindElement(vlan, g_pstXmlRoot, "param"
                        , "name", "gateway", MXML_DESCEND);
    if(NULL == param)
    {
        goto err_proc1;
    }
    value = mxmlElementGetAttr(param, "value");
    fprintf(fp, "VOICEGATEWAY=%s\n", (value&&*value)?value:"");

    param = mxmlFindElement(vlan, g_pstXmlRoot, "param"
                        , "name", "usepeerdns", MXML_DESCEND);
    if(NULL == param)
    {
        goto err_proc1;
    }
    value = mxmlElementGetAttr(param, "value");
    fprintf(fp, "VOICEUSEPEERDNS=%s\n", (value&&*value)?value:"");

    param = mxmlFindElement(vlan, g_pstXmlRoot, "param"
                        , "name", "dns1", MXML_DESCEND);
    if(NULL == param)
    {
        goto err_proc1;
    }
    value = mxmlElementGetAttr(param, "value");
    fprintf(fp, "VOICEDNS1=%s\n", (value&&*value)?value:"");

    param = mxmlFindElement(vlan, g_pstXmlRoot, "param"
                        , "name", "dns2", MXML_DESCEND);
    if(NULL == param)
    {
        goto err_proc1;
    }
    value = mxmlElementGetAttr(param, "value");
    fprintf(fp, "VOICEDNS2=%s\n", (value&&*value)?value:"");

    param = mxmlFindElement(vlan, g_pstXmlRoot, "param"
                        , "name", "pppuser", MXML_DESCEND);
    if(NULL == param)
    {
        goto err_proc1;
    }
    value = mxmlElementGetAttr(param, "value");
    fprintf(fp, "VOICEPPPUSERNAME=%s\n", (value&&*value)?value:"");

    param = mxmlFindElement(vlan, g_pstXmlRoot, "param"
                        , "name", "ppppwd", MXML_DESCEND);
    if(NULL == param)
    {
        goto err_proc1;
    }
    value = mxmlElementGetAttr(param, "value");
    fprintf(fp, "VOICEPPPPASSWORD=%s\n", (value&&*value)?value:"");

    fprintf(fp, "\n");
    /*---------------------mngt vlan-------------------*/
    vlan = mxmlFindElement(eth, g_pstXmlRoot, "mngtvlan"
                        , NULL, NULL, MXML_DESCEND);
    if(NULL == vlan)
    {
        goto err_proc1;
    }

    param = mxmlFindElement(vlan, g_pstXmlRoot, "param"
                        , "name", "flag", MXML_DESCEND);
    if(NULL == param)
    {
        goto err_proc1;
    }
    value = mxmlElementGetAttr(param, "value");
    fprintf(fp, "MNGTVLANFLAG=%s\n", (value&&*value)?value:"");

    param = mxmlFindElement(vlan, g_pstXmlRoot, "param"
                        , "name", "vlanid", MXML_DESCEND);
    if(NULL == param)
    {
        goto err_proc1;
    }
    value = mxmlElementGetAttr(param, "value");
    fprintf(fp, "MNGTVLANID=%s\n", (value&&*value)?value:"");

    param = mxmlFindElement(vlan, g_pstXmlRoot, "param"
                        , "name", "priid", MXML_DESCEND);
    if(NULL == param)
    {
        goto err_proc1;
    }
    value = mxmlElementGetAttr(param, "value");
    fprintf(fp, "MNGTVLANPRIID=%s\n", (value&&*value)?value:"");

    param = mxmlFindElement(vlan, g_pstXmlRoot, "param"
                        , "name", "ipmode", MXML_DESCEND);
    if(NULL == param)
    {
        goto err_proc1;
    }
    value = mxmlElementGetAttr(param, "value");
    fprintf(fp, "MNGTNETMODE=%s\n", (value&&*value)?value:"");

    param = mxmlFindElement(vlan, g_pstXmlRoot, "param"
                        , "name", "ip", MXML_DESCEND);
    if(NULL == param)
    {
        goto err_proc1;
    }
    value = mxmlElementGetAttr(param, "value");
    fprintf(fp, "MNGTIP=%s\n", (value&&*value)?value:"");


    param = mxmlFindElement(vlan, g_pstXmlRoot, "param"
                        , "name", "mask", MXML_DESCEND);
    if(NULL == param)
    {
        goto err_proc1;
    }
    value = mxmlElementGetAttr(param, "value");
    fprintf(fp, "MNGTMASK=%s\n", (value&&*value)?value:"");

    param = mxmlFindElement(vlan, g_pstXmlRoot, "param"
                        , "name", "mtu", MXML_DESCEND);
    if(NULL == param)
    {
        goto err_proc1;
    }
    value = mxmlElementGetAttr(param, "value");
    fprintf(fp, "MNGTMTU=%s\n", (value&&*value)?value:"");

    param = mxmlFindElement(vlan, g_pstXmlRoot, "param"
                        , "name", "gateway", MXML_DESCEND);
    if(NULL == param)
    {
        goto err_proc1;
    }
    value = mxmlElementGetAttr(param, "value");
    fprintf(fp, "MNGTGATEWAY=%s\n", (value&&*value)?value:"");

    param = mxmlFindElement(vlan, g_pstXmlRoot, "param"
                        , "name", "usepeerdns", MXML_DESCEND);
    if(NULL == param)
    {
        goto err_proc1;
    }
    value = mxmlElementGetAttr(param, "value");
    fprintf(fp, "MNGTUSEPEERDNS=%s\n", (value&&*value)?value:"");

    param = mxmlFindElement(vlan, g_pstXmlRoot, "param"
                        , "name", "dns1", MXML_DESCEND);
    if(NULL == param)
    {
        goto err_proc1;
    }
    value = mxmlElementGetAttr(param, "value");
    fprintf(fp, "MNGTDNS1=%s\n", (value&&*value)?value:"");

    param = mxmlFindElement(vlan, g_pstXmlRoot, "param"
                        , "name", "dns2", MXML_DESCEND);
    if(NULL == param)
    {
        goto err_proc1;
    }
    value = mxmlElementGetAttr(param, "value");
    fprintf(fp, "MNGTDNS2=%s\n", (value&&*value)?value:"");

    param = mxmlFindElement(vlan, g_pstXmlRoot, "param"
                        , "name", "pppuser", MXML_DESCEND);
    if(NULL == param)
    {
        goto err_proc1;
    }
    value = mxmlElementGetAttr(param, "value");
    fprintf(fp, "MNGTPPPUSERNAME=%s\n", (value&&*value)?value:"");

    param = mxmlFindElement(vlan, g_pstXmlRoot, "param"
                        , "name", "ppppwd", MXML_DESCEND);
    if(NULL == param)
    {
        goto err_proc1;
    }
    value = mxmlElementGetAttr(param, "value");
    fprintf(fp, "MNGTPPPPASSWORD=%s\n", (value&&*value)?value:"");

    fprintf(fp, "\n");
    /*---------------------mngt vlan-------------------*/
    eth = mxmlFindElement(network, g_pstXmlRoot, "eth1"
                        , NULL, NULL, MXML_DESCEND);
    if(NULL == eth)
    {
        goto err_proc1;
    }

    param = mxmlFindElement(eth, g_pstXmlRoot, "param"
                        , "name", "ip", MXML_DESCEND);
    if(NULL == param)
    {
        goto err_proc1;
    }
    value = mxmlElementGetAttr(param, "value");
    fprintf(fp, "LANIP=%s\n", (value&&*value)?value:"");

    param = mxmlFindElement(eth, g_pstXmlRoot, "param"
                        , "name", "mask", MXML_DESCEND);
    if(NULL == param)
    {
        goto err_proc1;
    }
    value = mxmlElementGetAttr(param, "value");
    fprintf(fp, "LANMASK=%s\n", (value&&*value)?value:"");

    param = mxmlFindElement(eth, g_pstXmlRoot, "param"
                        , "name", "mtu", MXML_DESCEND);
    if(NULL == param)
    {
        goto err_proc1;
    }
    value = mxmlElementGetAttr(param, "value");
    fprintf(fp, "LANMTU=%s\n", (value&&*value)?value:"");

    param = mxmlFindElement(eth, g_pstXmlRoot, "param"
                        , "name", "mac", MXML_DESCEND);
    if(NULL == param)
    {
        goto err_proc1;
    }
    value = mxmlElementGetAttr(param, "value");
    fprintf(fp, "LANMAC=%s\n", (value&&*value)?value:"");

    fprintf(fp, "\n#DMZ : on:localip\n");
    
    
    param = mxmlFindElement(network, g_pstXmlRoot, "param"
                            , "name", "dmz", MXML_DESCEND);
    if(NULL == param)
    {
        goto err_proc1;
    }
    value = mxmlElementGetAttr(param, "value");
    fprintf(fp, "DMZ=%s\n", (value&&*value)?value:"");
    
    param = mxmlFindElement(network, g_pstXmlRoot, "param"
                        , "name", "remotemngt", MXML_DESCEND);
    if(NULL == param)
    {
        goto err_proc1;
    }
    value = mxmlElementGetAttr(param, "value");
    fprintf(fp, "REMOTEMGNT=%s\n", (value&&*value)?value:"");

    fprintf(fp, "\n#NATFWD : on:proto:localip:localportn\n");
    /*---------------------dhcp server-------------------*/
    natfwd = mxmlFindElement(network, g_pstXmlRoot, "natfwd"
                        , NULL, NULL, MXML_DESCEND);
    if(NULL == natfwd)
    {
        goto err_proc1;
    }

    for(i = 0; i < 8; i++)
    {
        sprintf(szBuf, "fwd%d", i);
        fwd = mxmlFindElement(natfwd, g_pstXmlRoot, szBuf
                            , NULL, NULL, MXML_DESCEND);
        if(NULL == fwd)
        {
            goto err_proc1;
        }
        
        fprintf(fp, "NATFWD%d=", i);
        param = mxmlFindElement(fwd, g_pstXmlRoot, "param"
                                , "name", "flag", MXML_DESCEND);
        if(NULL == param)
        {
            goto err_proc1;
        }
        value = mxmlElementGetAttr(param, "value");
        fprintf(fp, "%s:", (value&&*value)?value:"off");

        param = mxmlFindElement(fwd, g_pstXmlRoot, "param"
                                , "name", "proto", MXML_DESCEND);
        if(NULL == param)
        {
            goto err_proc1;
        }
        value = mxmlElementGetAttr(param, "value");
        fprintf(fp, "%s:", (value&&*value)?value:"");
        
        param = mxmlFindElement(fwd, g_pstXmlRoot, "param"
                                , "name", "localip", MXML_DESCEND);
        if(NULL == param)
        {
            goto err_proc1;
        }
        value = mxmlElementGetAttr(param, "value");
        fprintf(fp, "%s:", (value&&*value)?value:"");
        
        param = mxmlFindElement(fwd, g_pstXmlRoot, "param"
                                , "name", "localport", MXML_DESCEND);
        if(NULL == param)
        {
            goto err_proc1;
        }
        value = mxmlElementGetAttr(param, "value");
        fprintf(fp, "%s\n", (value&&*value)?value:"");
        
    }

    fprintf(fp, "\n");
    /*---------------------dhcp server-------------------*/
    eth = mxmlFindElement(network, g_pstXmlRoot, "dhcp_server"
                        , NULL, NULL, MXML_DESCEND);
    if(NULL == eth)
    {
        goto err_proc1;
    }

    param = mxmlFindElement(eth, g_pstXmlRoot, "param"
                        , "name", "flag", MXML_DESCEND);
    if(NULL == param)
    {
        goto err_proc1;
    }
    value = mxmlElementGetAttr(param, "value");
    fprintf(fp, "DHCPENABLED=%s\n", (value&&*value)?value:"");

    param = mxmlFindElement(eth, g_pstXmlRoot, "param"
                        , "name", "ipstart", MXML_DESCEND);
    if(NULL == param)
    {
        goto err_proc1;
    }
    value = mxmlElementGetAttr(param, "value");
    fprintf(fp, "DHCPIPSTART=%s\n", (value&&*value)?value:"");

    param = mxmlFindElement(eth, g_pstXmlRoot, "param"
                        , "name", "ipend", MXML_DESCEND);
    if(NULL == param)
    {
        goto err_proc1;
    }
    value = mxmlElementGetAttr(param, "value");
    fprintf(fp, "DHCPIPEND=%s\n", (value&&*value)?value:"");

    param = mxmlFindElement(eth, g_pstXmlRoot, "param"
                        , "name", "mask", MXML_DESCEND);
    if(NULL == param)
    {
        goto err_proc1;
    }
    value = mxmlElementGetAttr(param, "value");
    fprintf(fp, "DHCPMASK=%s\n", (value&&*value)?value:"");

    param = mxmlFindElement(eth, g_pstXmlRoot, "param"
                        , "name", "leasetime", MXML_DESCEND);
    if(NULL == param)
    {
        goto err_proc1;
    }
    value = mxmlElementGetAttr(param, "value");
    fprintf(fp, "DHCPLEASETIME=%s\n", (value&&*value)?value:"");

    param = mxmlFindElement(eth, g_pstXmlRoot, "param"
                        , "name", "gateway", MXML_DESCEND);
    if(NULL == param)
    {
        goto err_proc1;
    }
    value = mxmlElementGetAttr(param, "value");
    fprintf(fp, "DHCPGATEWAY=%s\n", (value&&*value)?value:"");

    param = mxmlFindElement(eth, g_pstXmlRoot, "param"
                        , "name", "dns1", MXML_DESCEND);
    if(NULL == param)
    {
        goto err_proc1;
    }
    value = mxmlElementGetAttr(param, "value");
    fprintf(fp, "DHCPDNS1=%s\n", (value&&*value)?value:"");

    param = mxmlFindElement(eth, g_pstXmlRoot, "param"
                        , "name", "dns2", MXML_DESCEND);
    if(NULL == param)
    {
        goto err_proc1;
    }
    value = mxmlElementGetAttr(param, "value");
    fprintf(fp, "DHCPDNS2=%s\n", (value&&*value)?value:"");

    return AOS_SUCC;
    
err_proc1:
    fclose(fp);
err_proc0:
    mxmlDelete(g_pstXmlRoot);
    return AOS_FAIL;
}

