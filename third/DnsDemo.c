/ @Brief : 发送DNS查询报文
// @Param: usID: 报文ID编号
//                pSocket: 需要发送的socket
//                szDnsServer: DNS服务器地址
//                szDomainName: 需要查询的域名
// @Retrun: true表示发送成功，false表示发送失败
bool SendDnsPack(IN unsigned short usID, IN SOCKET *pSocket, IN const char *szDnsServer, IN const char *szDomainName) {
    bool bRet = false;

    if (*pSocket == INVALID_SOCKET 
        || szDomainName == NULL 
        || szDnsServer == NULL 
        || strlen(szDomainName) == 0 
        || strlen(szDnsServer) == 0)
    {
        return bRet;
    }
    
    unsigned int uiDnLen = strlen(szDomainName);

    // 判断域名合法性，域名的首字母不能是点号，域名的
    // 最后不能有两个连续的点号 
    if ('.' == szDomainName[0] || ( '.' == szDomainName[uiDnLen - 1] 
          && '.' == szDomainName[uiDnLen - 2]) 
       )
    {
        return bRet;
    }
    
    /* 1. 将域名转换为符合查询报文的格式 */
    // 查询报文的格式是类似这样的：
    //      6 j o c e n t 2 m e 0
    unsigned int uiQueryNameLen = 0;
    BYTE *pbQueryDomainName = (BYTE *)malloc(uiDnLen + 1 + 1);
    if (pbQueryDomainName == NULL)
    {
        return bRet;
    }
    // 转换后的查询字段长度为域名长度 +2
    memset(pbQueryDomainName, 0, uiDnLen + 1 + 1);

    // 下面的循环作用如下：
    // 如果域名为  jocent.me ，则转换成了 6 j o c e n t  ，还有一部分没有复制
    // 如果域名为  jocent.me.，则转换成了 6 j o c e n t 2 m e
    unsigned int uiPos    = 0;
    unsigned int i        = 0;
    for ( i = 0; i < uiDnLen; ++i)
    {
      if (szDomainName[i] == '.')
      {
          pbQueryDomainName[uiPos] = i - uiPos;
          if (pbQueryDomainName[uiPos] > 0)
          {
              memcpy(pbQueryDomainName + uiPos + 1, szDomainName + uiPos, i - uiPos);
          }
          uiPos = i + 1;
      }
    }
        
    // 如果域名的最后不是点号，那么上面的循环只转换了一部分
    // 下面的代码继续转换剩余的部分， 比如 2 m e
    if (szDomainName[i-1] != '.')
    {
      pbQueryDomainName[uiPos] = i - uiPos;
      memcpy(pbQueryDomainName + uiPos + 1, szDomainName + uiPos, i - uiPos);
      uiQueryNameLen = uiDnLen + 1 + 1;
    }
    else
    {
      uiQueryNameLen = uiDnLen + 1;    
    }
    // 填充内容  头部 + name + type + class
    DNSHeader *PDNSPackage = (DNSHeader*)malloc(sizeof(DNSHeader) + uiQueryNameLen + 4);
    if (PDNSPackage == NULL)
    {
        goto exit;
    }
    memset(PDNSPackage, 0, sizeof(DNSHeader) + uiQueryNameLen + 4);

    // 填充头部内容
    PDNSPackage->usTransID = htons(usID);  // ID
    PDNSPackage->RD = 0x1;   // 表示期望递归
    PDNSPackage->Questions = htons(0x1);  // 本文第一节所示，这里用htons做了转换

    // 填充正文内容  name + type + class
    BYTE* PText = (BYTE*)PDNSPackage + sizeof(DNSHeader);
    memcpy(PText, pbQueryDomainName, uiQueryNameLen);

    unsigned short *usQueryType = (unsigned short *)(PText + uiQueryNameLen);
    *usQueryType = htons(0x1);        // TYPE: A

    ++usQueryType;
    *usQueryType = htons(0x1);        // CLASS: IN    

    // 需要发送到的DNS服务器的地址
    sockaddr_in dnsServAddr = {};
    dnsServAddr.sin_family = AF_INET;
    dnsServAddr.sin_port = ::htons(53);  // DNS服务端的端口号为53
    dnsServAddr.sin_addr.S_un.S_addr = ::inet_addr(szDnsServer);
    
    // 将查询报文发送出去
    int nRet = ::sendto(*pSocket,
        (char*)PDNSPackage,
        sizeof(DNSHeader) + uiQueryNameLen + 4,
        0,
        (sockaddr*)&dnsServAddr,
        sizeof(dnsServAddr));
    if (SOCKET_ERROR == nRet)
    {
        printf("DNSPackage Send Fail! \n");
        goto exit;
    }
    
    // printf("DNSPackage Send Success! \n");
    bRet = true;
    
// 统一的资源清理处       
exit:
    if (PDNSPackage)
    {
        free(PDNSPackage);
        PDNSPackage = NULL;
    }

    if (pbQueryDomainName)
    {
        free(pbQueryDomainName);
        pbQueryDomainName = NULL;
    }
    
    return bRet;
}