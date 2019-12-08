
#pragma once

struct  MediaProfile
{
	int  count;
	char stream_uri[3][256];
	char profile_name[3][32];
};

#ifdef __cplusplus
extern"C"{
#endif

#ifdef		ONVIFCLIENTLIB_EXPORTS	
#define		ONVIFDLL_API	__declspec(dllexport)
#else
#define		ONVIFDLL_API	__declspec(dllimport)
#endif

//函数作用： 传入参数
//返回值：  始终返回0
ONVIFDLL_API int SetDeviceHost(char * szDeviceIP, int port, char * user, char * pass);

//函数作用：  获取设备信息
//返回值：0：成功； -1：连接成功，但获取信息失败； -2：连接服务器失败，可能设备不在线
ONVIFDLL_API int GetStreamInfo(MediaProfile * pProfile);

#ifdef __cplusplus
}
#endif