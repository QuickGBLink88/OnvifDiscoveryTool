#include "StdAfx.h"
#include "Discovery.h"
#include <list>
#include "CommonFunctions.h"

#define MAXBUF 16*1024
#define PUERTO 3702
#define GRUPO "239.255.255.250"


COnvifDiscovery::COnvifDiscovery(void)
{
	m_fd = 0;
	m_bIsReceiving = FALSE;
	m_hRcvThread = NULL;

	m_lpCallback = NULL;
	m_lpContext = NULL;
}

COnvifDiscovery::~COnvifDiscovery(void)
{
	if(m_hRcvThread)
	{
		::CloseHandle(m_hRcvThread);
		m_hRcvThread = NULL;
	}
}

void COnvifDiscovery::SetDiscoveryInfoCB(DeviceDiscoverCB lpCallback, LPVOID lpContext)
{
	  m_lpCallback = lpCallback;
	  m_lpContext = lpContext;
}

int COnvifDiscovery::probeSocketIni(int *fd)
{
    struct ip_mreq mreq;
    struct sockaddr_in  cli;
    memset( &mreq, 0, sizeof(struct ip_mreq) );
    memset( &cli, 0, sizeof(struct sockaddr_in));

	//创建UDP Socket
    if( (*fd = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP) ) < 0 ){
        OutputDebugString("socket failed \n");
		::closesocket(*fd);
        return -1;
    }

    //if (inet_aton(GRUPO, &mreq.imr_multiaddr) < 0) {
    //    perror("inet_aton");
    //    close(*fd);
    //    return -1;
    //}

	//当多网卡或多IP存在时，绑定第一个IP
	hostent* thisHost = gethostbyname("");
	char* szLocalIP = inet_ntoa(*(struct in_addr*)*thisHost->h_addr_list);

	//绑定端口
    cli.sin_family = AF_INET;
    cli.sin_port = htons(/*PUERTO*/0); //让系统随机分配端口
    cli.sin_addr.s_addr = /*htonl(INADDR_ANY)*/inet_addr(szLocalIP);
    if( bind(*fd, (struct sockaddr *)&cli, sizeof(cli)) < 0 ){
        OutputDebugString("bind socket failed \n");
        return -1;
    }

	//加入到组播组
    mreq.imr_multiaddr.s_addr = inet_addr(GRUPO);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if( setsockopt(*fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char*)&mreq, sizeof(mreq)) < 0 ){
        OutputDebugString("setsockopt IP_ADD_MEMBERSHIP failed \n");
        closesocket(*fd);
        return -1;
    }

	//设置TTL
	int nTTL = 100;
	if(setsockopt(*fd, IPPROTO_IP, IP_MULTICAST_TTL, (const char*)&nTTL, sizeof(int)) < 0)
	{
		OutputDebugString("setsockopt IP_MULTICAST_TTL failed \n");
	}
	//设置不接收本机发出去的组播回声包
	int nLoopBack = 0;
	if(setsockopt(*fd, IPPROTO_IP, IP_MULTICAST_LOOP, (const char*)&nLoopBack, sizeof(int)) < 0)
	{
		OutputDebugString("setsockopt IP_MULTICAST_LOOP failed \n");
	}

    //设置接收缓冲区大小
	int nRecvBuf = 512*1024; 
    setsockopt( *fd, SOL_SOCKET,SO_RCVBUF, (const char*)&nRecvBuf, sizeof(int));
    return 0;
}

void COnvifDiscovery::probeSocketEnd(int *fd)
{
	if(*fd != 0)
	{
		closesocket(*fd);
		*fd = 0;
	}
}

int COnvifDiscovery::sendProbe(int fd,const char *uuid,const char *types)
{
    int r;
    struct sockaddr_in srv;
    memset( &srv, 0, sizeof(struct sockaddr_in) );
    srv.sin_family = AF_INET;
    srv.sin_port = htons(PUERTO);
	srv.sin_addr.s_addr = inet_addr(GRUPO);
    //if( inet_aton(GRUPO, &srv.sin_addr ) < 0 ) {
    //    perror("inet_aton");
    //    close(fd);
    //    return -1;
    //}
    char Probe[4096];
    sprintf(Probe,"<?xml version=\"1.0\" encoding=\"utf-8\"?>"\
            "<Envelope xmlns:dn=\"http://www.onvif.org/ver10/network/wsdl\" xmlns=\"http://www.w3.org/2003/05/soap-envelope\">"\
            "<Header>"\
            "<wsa:MessageID xmlns:wsa=\"http://schemas.xmlsoap.org/ws/2004/08/addressing\">%s</wsa:MessageID>"\
            "<wsa:To xmlns:wsa=\"http://schemas.xmlsoap.org/ws/2004/08/addressing\">urn:schemas-xmlsoap-org:ws:2005:04:discovery</wsa:To>"\
            "<wsa:Action xmlns:wsa=\"http://schemas.xmlsoap.org/ws/2004/08/addressing\">http://schemas.xmlsoap.org/ws/2005/04/discovery/Probe</wsa:Action>"\
            "</Header>"\
            "<Body>"\
            "<Probe xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns=\"http://schemas.xmlsoap.org/ws/2005/04/discovery\">"\
            "<Types>%s</Types>"\
            "<Scopes />"\
            "</Probe>"\
            "</Body>"\
            "</Envelope>\r\n",uuid,types);

	//sprintf(Probe, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"\
 //     "<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://www.w3.org/2003/05/soap-envelope\" xmlns:SOAP-ENC=\"http://www.w3.org/2003/05/soap-encoding\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xop=\"http://www.w3.org/2004/08/xop/include\" xmlns:wsa=\"http://schemas.xmlsoap.org/ws/2004/08/addressing\" xmlns:tns=\"http://schemas.xmlsoap.org/ws/2005/04/discovery\" xmlns:dn=\"http://www.onvif.org/ver10/network/wsdl\" xmlns:wsa5=\"http://www.w3.org/2005/08/addressing\">"\
	//  "<SOAP-ENV:Header><wsa:MessageID>urn:uuid:131D84FA-15DC-4508-AC59-3CEF31D42474</wsa:MessageID>"\
	//  "<wsa:To SOAP-ENV:mustUnderstand=\"true\">urn:schemas-xmlsoap-org:ws:2005:04:discovery</wsa:To>"\
	//  "</SOAP-ENV:Header><SOAP-ENV:Body>"\
	//  "<tns:Probe>"\
	//  "<tns:Types>dn:NetworkVideoTransmitter</tns:Types>"\
	//  "</tns:Probe>"\
	//  "</SOAP-ENV:Body>"
	//  "</SOAP-ENV:Envelope>");

    if( (r = sendto(fd, Probe, strlen(Probe), 0, (struct sockaddr *)&srv, sizeof(srv))) < 0 )
    {
		OutputDebugString("sendto failed \n");
        closesocket(fd);
        return -1;
    }

    return 0;
}



DWORD WINAPI COnvifDiscovery::ReceivingThrd(void * pParam)
{
	TRACE("ReceivingThrd started.\n");

	COnvifDiscovery * pSock = (COnvifDiscovery *) pParam;
	if (pSock)
	{
		pSock->m_bIsReceiving = TRUE;
	     pSock->ReceivingLoop();
	 }	

	pSock->m_bIsReceiving = FALSE;

	TRACE("ReceivingThrd exited\n");

	return 0;
}


//UDP接收
void COnvifDiscovery::ReceivingLoop(void)
{
    recvProbeMatches(m_fd, 800);
    probeSocketEnd(&m_fd);
}

int COnvifDiscovery::StartFind(void)
{
	if (m_hRcvThread != NULL && m_bIsReceiving) 
	{
        return -2;
	}

	int nRet = 0;
    nRet = probeSocketIni(&m_fd);
	if(nRet < 0)
	{
		return nRet;
	}

	nRet = sendProbe(m_fd,"uuid:e6153fce-af7a-4b48-94f7-05562e09b4f0","dn:NetworkVideoTransmitter");
	if(nRet < 0)
	{
		ASSERT(0);
		m_fd = 0;
		return -1;
	}

    DWORD threadID = 0;
	m_hRcvThread = CreateThread(NULL, 0, ReceivingThrd, this, 0, &threadID);
    return (m_hRcvThread != NULL);
}

void  COnvifDiscovery::StopFind()
{
	m_bIsReceiving = FALSE;
	probeSocketEnd(&m_fd);

	if (m_hRcvThread != NULL) 
	{
        WaitForSingleObject(m_hRcvThread, INFINITE);
		::CloseHandle(m_hRcvThread);
		m_hRcvThread = NULL;
	}
}

int COnvifDiscovery::recvProbeMatches(int fd,int timeout)
{
    int r;
    char buf[MAXBUF];
    unsigned long before= GetTickCount();

	std::list<DEVICE_IPC_INFO*> lstIpcs;
	
    char address[1024]; //内存空间要分配得大一点，否则可能会发生缓冲区溢出
	char hardwareID[128] = {0};
	char serial[128] = {0};

    char ProbeMatches[1024];
    char    *p_str_begin    = NULL;
    char    *p_str_end      = NULL;
    int i = 0;

	 //设置接收超时时间
	//int timeout = timeout; 
	if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) == SOCKET_ERROR)
	{
		TRACE("Error: Set UDP Recv timeout \n");
	}

	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

    while(m_bIsReceiving && GetTickCount() - before < 1200)
    {
        memset(buf,0,sizeof(buf));
		int nAddrLen = sizeof(addr);
        if( (r = recvfrom(fd, buf, MAXBUF, 0, (SOCKADDR*)&addr , &nAddrLen)) <= 0 )
        {
			//TRACE("error: %d \n", ::WSAGetLastError());

			if(WSAETIMEDOUT == ::WSAGetLastError()) //超时，这个错误可以忽略 
			{
				continue;
			}
            OutputDebugString("recvfrom failed \n");
            closesocket(fd);
            return -1;
        }
        else
        {
            buf[r] = 0;
			TRACE("IP: [%s]\n", inet_ntoa(addr.sin_addr));
			TRACE("\r\n");

			DEVICE_IPC_INFO * pIPC = new DEVICE_IPC_INFO();
			//memset(pIPC, 0, sizeof(DEVICE_IPC_INFO));

            memset(ProbeMatches,0,sizeof(ProbeMatches));

			int nPrefixType = -1;

            p_str_begin = strstr(buf,"<tns:ProbeMatches");
            p_str_end   = strstr(buf,"</tns:ProbeMatches>");
            if(p_str_begin != NULL && p_str_end != NULL)
            {
				nPrefixType = 1;
            }
			else
			{
				p_str_begin = strstr(buf,"<d:ProbeMatches");
				p_str_end   = strstr(buf,"</d:ProbeMatches>");
				if(p_str_begin != NULL && p_str_end != NULL)
				{
					nPrefixType = 2;
				}
			}
	
           if(nPrefixType == -1)
		   {
			   delete pIPC;
			   continue;
		   }

			memcpy(ProbeMatches, p_str_begin, p_str_end-p_str_begin);
			// printf("%s \n",ProbeMatches);
			memset(address,0,sizeof(address));

			int prefixLen = 0;
			if(nPrefixType == 1)
			{
			   prefixLen = strlen("<tns:XAddrs>");
			   p_str_begin = strstr(ProbeMatches, "<tns:XAddrs>");
			   p_str_end   = strstr(ProbeMatches, "</tns:XAddrs>"); 
			}
			else if(nPrefixType == 2)
			{
			   prefixLen = strlen("<d:XAddrs>");
			   p_str_begin = strstr(ProbeMatches, "<d:XAddrs>");
			   p_str_end   = strstr(ProbeMatches, "</d:XAddrs>"); 
			}

			if(p_str_begin != NULL && p_str_end != NULL) 
			{
				ASSERT( p_str_end-p_str_begin-prefixLen < sizeof(address));

				if(p_str_end-p_str_begin-prefixLen >= sizeof(address))
				{
					delete pIPC;
					continue;
				}

				memcpy(address, p_str_begin+prefixLen, p_str_end-p_str_begin-prefixLen); //获取设备查询URL
				TRACE("Addr: %s \n",address);

				USHORT uPort = 0;
				char onvifAddrName[128] = {0};
				int result = HttpUrlGetIpPort(address, pIPC->IP, &uPort, onvifAddrName);
				ASSERT(result == 0);

				pIPC->Port = uPort;
			}

		    p_str_begin = strstr(buf,"onvif://www.onvif.org/hardware/");
           
            if(p_str_begin != NULL )
            {
				p_str_end   = strstr(p_str_begin, " ");

				if(p_str_end != NULL)
				{
					int prefixLen = strlen("onvif://www.onvif.org/hardware/");
				    memcpy(hardwareID, p_str_begin+prefixLen, p_str_end-p_str_begin-prefixLen); //获取硬件型号

					strcpy(pIPC->hardware_type, hardwareID);
				}
            }

		    p_str_begin = strstr(buf, "onvif://www.onvif.org/serial/");
           
            if(p_str_begin != NULL )
            {
				p_str_end   = strstr(p_str_begin, " ");

				if(p_str_end != NULL)
				{
					int prefixLen = strlen("onvif://www.onvif.org/serial/");
				    memcpy(serial, p_str_begin+prefixLen, p_str_end-p_str_begin-prefixLen); //获取序列号

					//strcpy(pIPC->uuid, serial);
					strcpy(pIPC->DeviceID, serial);
				}
            }

			//if(strlen(pIPC->DeviceID) == 0)
			//{
			//	strcpy(pIPC->DeviceID, CreateUuid()); //随机分配一个ID
			//}
			
		   if(m_lpCallback)
		      m_lpCallback(pIPC, m_lpContext); //调用回调函数传递数据给界面

			lstIpcs.push_back(pIPC);
        }
    }

	TRACE("Onvif found ipc number: %d \n", (int)lstIpcs.size());
	//std::list<DEVICE_IPC_INFO*>::iterator it;
	//for(it = lstIpcs.begin(); it != lstIpcs.end(); it++)
	//{
	//	 DEVICE_IPC_INFO * pDev = *it;
	//}
	lstIpcs.clear();

	//退出组播组
	struct ip_mreq mreq;
    memset( &mreq, 0, sizeof(struct ip_mreq) );
    mreq.imr_multiaddr.s_addr = inet_addr(GRUPO);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if( setsockopt(fd, IPPROTO_IP, IP_DROP_MEMBERSHIP, (const char*)&mreq, sizeof(mreq)) < 0 ){
        OutputDebugString("IP_DROP_MEMBERSHIP failed \n");
        closesocket(fd);
    }

    return 0;
}