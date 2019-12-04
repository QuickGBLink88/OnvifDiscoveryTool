#pragma once



class DEVICE_IPC_INFO
{
public:
	char          hardware_type[128]; //Ó²¼þÐÍºÅ
	char          DeviceName[64];
	char          DeviceID[128]; //uuid
	char          IP[32];
	int           Port;

	DEVICE_IPC_INFO()
	{
		memset(DeviceName, 0, sizeof(DeviceName));
		memset(DeviceID, 0, sizeof(DeviceID));
		memset(IP, 0, sizeof(IP));
		memset(hardware_type, 0, sizeof(hardware_type));
		Port = 80;
	}

	DEVICE_IPC_INFO& operator =(const DEVICE_IPC_INFO& t) 
	{
		strcpy(hardware_type, t.hardware_type);
		strcpy(DeviceName, t.DeviceName);
		strcpy(DeviceID, t.DeviceID);
		strcpy(IP, t.IP);
		Port = t.Port;

		return *this;
	}
};

//typedef void (CALLBACK* DeviceDiscoverCB)(char * szDeviceName, char * szIP, int port, char * uuid, int nStreamNumber, char * stream_name[], LPVOID lpContext);
typedef void (CALLBACK* DeviceDiscoverCB)(DEVICE_IPC_INFO * pInfo, LPVOID lpContext);

class COnvifDiscovery
{
public:
	COnvifDiscovery(void);
	~COnvifDiscovery(void);

	void SetDiscoveryInfoCB(DeviceDiscoverCB lpCallback, LPVOID lpContext);
	int  StartFind(void);
	void StopFind();

protected:
	static DWORD WINAPI ReceivingThrd(void * pParam);

	int probeSocketIni(int *fd);
	void probeSocketEnd(int *fd);
	int sendProbe(int fd,const char *uuid,const char *types);
	int recvProbeMatches(int fd,int timeout);

	void ReceivingLoop(void);

	 int     m_fd;
	 BOOL    m_bIsReceiving;
	 HANDLE  m_hRcvThread;

	 DeviceDiscoverCB m_lpCallback;
	 LPVOID           m_lpContext;
};
