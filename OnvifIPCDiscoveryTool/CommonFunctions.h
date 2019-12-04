#pragma once



extern CString   UIntToString(UINT value);
extern INT       StringToInt(CString & str);
extern CString   IntToString(INT value);
extern  INT      HexToInt(const char * szHex, int len);
extern  CString  RectToString(RECT & rc);
extern  BOOL     StringToRect(CString str, RECT &rc);
extern CString   GetAppDir();

//获取一个随机的UUID字符串
extern CString  CreateUuid();

//获取字符串格式的IP，输入的dwIP参数为网络字节序
extern CString  ToIPStr(DWORD dwIP);

//根据输入的HTTP URL分离出IP、端口、资源路径
extern int HttpUrlGetIpPort(char *pUrl, char *pIp, USHORT *pPort, char *pName);

//根据输入的RTSP URL分离出IP、端口、流名称
extern int RtspUrlGetIpPort(char *pUrl, char *pIp, USHORT *pPort, char *pName);

extern BOOL StringToCTime(const char * szTime, CTime & t);
extern CString CTimeToString(CTime & t);


extern CString  SecondsToTimeStr(int nSec);
