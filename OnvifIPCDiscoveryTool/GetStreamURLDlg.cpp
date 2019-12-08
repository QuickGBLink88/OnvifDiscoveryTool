// GetStreamURLDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "OnvifIPCDiscoveryTool.h"
#include "GetStreamURLDlg.h"

#define WM_STREAM_INFO_NOTIFY_MSG   WM_USER + 82   //流URL信息请求完成通知


// CGetStreamURLDlg 对话框

IMPLEMENT_DYNAMIC(CGetStreamURLDlg, CDialog)

CGetStreamURLDlg::CGetStreamURLDlg(CWnd* pParent /*=NULL*/, DEVICE_IPC_INFO * pIpcInfo)
	: CDialog(CGetStreamURLDlg::IDD, pParent)
{
   m_hOnvifRequestThread = NULL;
   memset(&m_mediaProfile, 0, sizeof(m_mediaProfile));
   m_pIPCInfo = pIpcInfo;
}

CGetStreamURLDlg::~CGetStreamURLDlg()
{
}

void CGetStreamURLDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_RESPONSE_INFO, m_responseStatus);
}


BEGIN_MESSAGE_MAP(CGetStreamURLDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CGetStreamURLDlg::OnBnClickedOk)
	ON_MESSAGE(WM_STREAM_INFO_NOTIFY_MSG, OnStreamInfoNotifyMSg)
	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(IDC_COMBO_STREAM_INDEX, &CGetStreamURLDlg::OnCbnSelchangeComboStreamIndex)
END_MESSAGE_MAP()


// CGetStreamURLDlg 消息处理程序
BOOL CGetStreamURLDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_responseStatus.ShowWindow(1);
	m_responseStatus.SetColors(RGB(255, 0, 0),  GetSysColor( COLOR_BTNFACE ));
	m_responseStatus.SetWindowText(_T("正在获取信息..."));

	//创建线程向设备发送Onvif请求
	 DWORD threadID = 0;
	m_hOnvifRequestThread = CreateThread(NULL, 0, OnvifThrd, this, 0, &threadID);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


	
DWORD WINAPI CGetStreamURLDlg::OnvifThrd(void * pParam)
{
	CGetStreamURLDlg * pThisDlg = (CGetStreamURLDlg*)pParam;

	pThisDlg->OnvifThreadProc();

	return 0;
}

	
int CGetStreamURLDlg::OnvifThreadProc()
{
	RequestStreamInfo();
	return 0;
}

void CGetStreamURLDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	OnOK();
}


void CGetStreamURLDlg::OnDestroy()
{
	if (m_hOnvifRequestThread != NULL) 
	{
        WaitForSingleObject(m_hOnvifRequestThread, INFINITE);
		::CloseHandle(m_hOnvifRequestThread);
		m_hOnvifRequestThread = NULL;
	}

	CDialog::OnDestroy();
}



void CGetStreamURLDlg::RequestStreamInfo()
{
	AfxGetApp()->BeginWaitCursor();

	SetDeviceHost(m_pIPCInfo->IP, m_pIPCInfo->Port, m_pIPCInfo->username, m_pIPCInfo->password);

	memset(&m_mediaProfile, 0, sizeof(m_mediaProfile));

	int nRet = GetStreamInfo(&m_mediaProfile);
	if(nRet == 0) //获取信息成功
	{
		//m_responseStatus.ShowWindow(0);
		m_responseStatus.SetWindowText("获取流信息成功");
	}
	else if(nRet == -1)
	{
		m_responseStatus.ShowWindow(1);
		m_responseStatus.SetWindowText(_T("获取信息失败，可能用户登录认证不通过或该用户无权限访问Onvif设备服务"));
	}
	else if(nRet == -2)
	{
		m_responseStatus.ShowWindow(1);
		m_responseStatus.SetWindowText(_T("连接服务器失败，可能设备不在线"));
	}

	AfxGetApp()->EndWaitCursor();


	PostMessage(WM_STREAM_INFO_NOTIFY_MSG, 0, 0);
}

//流URL信息请求完成通知
LRESULT CGetStreamURLDlg::OnStreamInfoNotifyMSg(WPARAM wParam, LPARAM lParam)
{
	if(m_mediaProfile.count > 0)
	{
		CString str;
		for(int i = 0; i< m_mediaProfile.count; i++)
		{
			str.Format(_T("%d"), i+1);
			((CComboBox*)GetDlgItem(IDC_COMBO_STREAM_INDEX))->AddString(str);
		}
		((CComboBox*)GetDlgItem(IDC_COMBO_STREAM_INDEX))->SetCurSel(0);

		str.Format(_T("%s"), m_mediaProfile.profile_name[0]);
		GetDlgItem(IDC_EDIT_STREAM_NAME)->SetWindowText(str);

		str.Format(_T("%s"), m_mediaProfile.stream_uri[0]);
		GetDlgItem(IDC_EDIT_STREAM_URI)->SetWindowText(str);
	}

	if(m_hOnvifRequestThread != NULL)
	{
		::CloseHandle(m_hOnvifRequestThread);
		m_hOnvifRequestThread = NULL;
	}

	return 0;
}

void CGetStreamURLDlg::OnCbnSelchangeComboStreamIndex()
{
	int nStreamIndex = ((CComboBox*)GetDlgItem(IDC_COMBO_STREAM_INDEX))->GetCurSel();
	if(nStreamIndex < m_mediaProfile.count)
	{
        CString str;
		str.Format(_T("%s"), m_mediaProfile.profile_name[nStreamIndex]);
		GetDlgItem(IDC_EDIT_STREAM_NAME)->SetWindowText(str);

		str.Format(_T("%s"), m_mediaProfile.stream_uri[nStreamIndex]);
		GetDlgItem(IDC_EDIT_STREAM_URI)->SetWindowText(str);
	}
}


