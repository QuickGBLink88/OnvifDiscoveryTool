#include "StdAfx.h"
#include "CommonFunctions.h"


CString   UIntToString(UINT value)
{
   CString str;
   str.Format(_T("%u"), value);
   return str;
}

INT   StringToInt(CString & str)
{
     return atoi(str);
}

CString   IntToString(INT value)
{
   CString str;
   str.Format(_T("%d"), value);
   return str;
}

 INT   HexToInt(const char * szHex, int len)
 {
   int sum = 0;
   
   for(int i = 0; i<len; i++)
   {
     char c = szHex[len-1-i];
	 c = ::toupper(c);
     switch(c)
     {
     case 'A':
        sum += 10*(1<<(4*i));
        break;
     case 'B':
        sum += 11*(1<<(4*i));
        break;
     case 'C':
        sum += 12*(1<<(4*i));
        break;
     case 'D':
        sum += 13*(1<<(4*i));
        break;
     case 'E':
        sum += 14*(1<<(4*i));
        break;
     case 'F':
        sum += 15*(1<<(4*i));
        break;
     default:
        sum += ((int)(c-48))*(1<<(4*i));
     }
    }//for
   return sum;
 }

CString  GetAppDir()
{
	TCHAR szLocalPath[MAX_PATH]; 
	GetModuleFileName( 0, szLocalPath, MAX_PATH );
    CString strTemp;
	strTemp  = szLocalPath;
	strTemp = strTemp.Left(strTemp.ReverseFind('\\'));
	return strTemp;
}

CString CreateUuid()
{
   CString szGUID;

  // UUID uuidRoute;
  // RPC_STATUS st;

  // st = UuidCreate(&uuidRoute);
  // if (st != RPC_S_OK)
  // {
  //    return "";
  // }

  // unsigned char *pszGuid;     
  // UuidToString(&uuidRoute, &pszGuid);
  //// _bstr_t bstrUUID = _bstr_t((char*)pszGuid);     
  // szGUID = pszGuid;
  // RpcStringFree(&pszGuid);

    GUID       guid; 

	if(S_OK  ==  ::CoCreateGuid(&guid)) 
	{ 
		szGUID.Format( "%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X",
			guid.Data1,
			guid.Data2,
			guid.Data3,
			guid.Data4[0],  
			guid.Data4[1],
			guid.Data4[2],  
			guid.Data4[3], 
			guid.Data4[4], 
			guid.Data4[5],
			guid.Data4[6],
			guid.Data4[7]); 
	}

   return szGUID;
}

CString  ToIPStr(DWORD dwIP)
{
	struct in_addr   in;
	in.S_un.S_addr = dwIP; //htonl(dwIP);
	char * pStr = inet_ntoa(in); 
	return CString(pStr);
}

int RtspUrlGetIpPort(char *pUrl, char *pIp, USHORT *pPort, char *pName)
{
	UINT   dwIP;
	char	*pChar,*pStreamName;
	char	str[256];

	if( strlen( pUrl ) < 16 )	return -1;
	strcpy_s( str, 256, pUrl );

	if( _strnicmp( str, "rtsp://", 7 ) )
		return -2;

	pChar = strchr( str+7, '/' );
	//	if( pChar == NULL )	return -3;
	if( pChar == NULL )
	{
		pStreamName = NULL;
	}
	else
	{
		pStreamName = pChar + 1;
		*pChar = 0;
	}

	pChar = strchr( str+7, ':' );
	if( pChar )
	{
		*pChar = 0;
		pChar ++;
		*pPort = (USHORT)strtoul( pChar, NULL, 10 );
	}
	else
	{
		*pPort = 554;
	}

	dwIP = inet_addr( str+7 );
	if( dwIP == INADDR_NONE )
		return -4;

	strcpy(pIp, str+7);

	if( pStreamName )
	{
		strcpy_s( pName, 128, pStreamName );
		//sprintf_s( pUrl, 256, "rtsp://%d.%d.%d.%d/%s", (*pIp)&0xFF, ((*pIp)>>8)&0xFF, ((*pIp)>>16)&0xFF, (*pIp)>>24, pStreamName );
	}
	else
	{
		pName[0] = 0;
		//sprintf_s( pUrl, 256, "rtsp://%d.%d.%d.%d/stream", (*pIp)&0xFF, ((*pIp)>>8)&0xFF, ((*pIp)>>16)&0xFF, (*pIp)>>24 );
	}

	return 0;
}

static BOOL ValidateIPString(char* pszIP)
{
	if(pszIP == NULL || strlen(pszIP) < 7)
		return FALSE;

	int a1, a2, a3, a4;
	int nParams = sscanf(pszIP, "%d.%d.%d.%d", &a1, &a2, &a3, &a4);
	if(nParams != 4)
		return FALSE;

	sprintf(pszIP, "%d.%d.%d.%d", a1, a2, a3, a4);
	return TRUE;
}

int HttpUrlGetIpPort(char *pUrl, char *pIp, USHORT *pPort, char *pName)
{
	UINT   dwIP;
	char	*pChar,*pStreamName;
	char	str[256];
	char    tempIP[64] = {0};

	if( strlen( pUrl ) < 16 )	return -1;
	strcpy_s( str, 256, pUrl );

	if( _strnicmp( str, "http://", 7 ) )
		return -2;

	pChar = strchr( str+7, '/' );
	//	if( pChar == NULL )	return -3;
	if( pChar == NULL )
	{
		pStreamName = NULL;
	}
	else
	{
		pStreamName = pChar + 1;
		*pChar = 0;
	}

	pChar = strchr( str+7, ':' );
	if( pChar )
	{
		*pChar = 0;
		pChar ++;
		*pPort = (USHORT)strtoul( pChar, NULL, 10 );
	}
	else
	{
		*pPort = 80;
	}

	strcpy(tempIP, str+7);
	ValidateIPString(tempIP); //去除冗余字符

	dwIP = inet_addr( tempIP );
	if( dwIP == INADDR_NONE )
		return -4;

	strcpy(pIp, tempIP);

	if( pStreamName )
	{
		strcpy_s( pName, 128, pStreamName );
		//sprintf_s( pUrl, 256, "rtsp://%d.%d.%d.%d/%s", (*pIp)&0xFF, ((*pIp)>>8)&0xFF, ((*pIp)>>16)&0xFF, (*pIp)>>24, pStreamName );
	}
	else
	{
		pName[0] = 0;
		//sprintf_s( pUrl, 256, "rtsp://%d.%d.%d.%d/stream", (*pIp)&0xFF, ((*pIp)>>8)&0xFF, ((*pIp)>>16)&0xFF, (*pIp)>>24 );
	}

	return 0;
}


CString  RectToString(RECT & rc)
{
	CString str;
	str.Format("%d,%d,%d,%d", rc.left, rc.top, rc.right, rc.bottom);
	return str;
}

BOOL     StringToRect(CString str, RECT &rc)
{
	int nParams = sscanf(str, "%d,%d,%d,%d", &rc.left, &rc.top, &rc.right, &rc.bottom);
	return (nParams == 4);
}


BOOL StringToCTime(const char * szTime, CTime & t)
{
	int nYear, nMonth, nDay, nHour, nMinute, nSec;
	int nParams = sscanf(szTime, "%04d-%02d-%02d %02d:%02d:%02d", &nYear, &nMonth, &nDay, &nHour, &nMinute, &nSec);
	if (nParams != 6)
	{
		return FALSE;
	}

	t = CTime(nYear, nMonth, nDay, nHour, nMinute, nSec);
	return TRUE;
}


CString CTimeToString(CTime & t)
{
	CString strTime;
	strTime.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"), t.GetYear(), t.GetMonth(), t.GetDay(), t.GetHour(), t.GetMinute(), t.GetSecond());
	return strTime;
}

 CString  SecondsToTimeStr(int nSec)
{
	int ns, hh, mm, ss;
	int tns, thh, tmm, tss;
	tns = nSec; //秒
	thh = tns/3600; //小时
	tmm = (tns%3600)/60; //分钟
	tss = (tns%60); //秒

	CString str;
	if (thh > 0)
	{
		str.Format(_T("%02d小时%02d分%02d秒"), thh, tmm, tss);
	}
	else
	{
		if (tmm > 0)
		{
			str.Format(_T("%02d分%02d秒"), tmm, tss);
		}
		else
		{
			str.Format(_T("%02d秒"), tss);
		}
	}

	return str;
}