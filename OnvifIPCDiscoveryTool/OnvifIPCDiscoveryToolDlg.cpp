// OnvifIPCDiscoveryToolDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "OnvifIPCDiscoveryTool.h"
#include "OnvifIPCDiscoveryToolDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// COnvifIPCDiscoveryToolDlg 对话框

COnvifIPCDiscoveryToolDlg::COnvifIPCDiscoveryToolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COnvifIPCDiscoveryToolDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void COnvifIPCDiscoveryToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ctlList1);
}

BEGIN_MESSAGE_MAP(COnvifIPCDiscoveryToolDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, &COnvifIPCDiscoveryToolDlg::OnButtonRefresh)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// COnvifIPCDiscoveryToolDlg 消息处理程序

BOOL COnvifIPCDiscoveryToolDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	// Init WinSock
	WSADATA   data;
	int ret = WSAStartup(0x0202, &data);
	if (ret != 0)
	{
		WSACleanup();
		return FALSE;
	}

    m_ctlList1.SetExtendedStyle(/*LVS_EX_GRIDLINES|*/LVS_EX_FULLROWSELECT);

	m_ctlList1.InsertColumn(0,"IP地址", LVCFMT_LEFT, 150);
	m_ctlList1.InsertColumn(1,"序列号", LVCFMT_LEFT, 150);
    m_ctlList1.InsertColumn(2,"设备类型", LVCFMT_LEFT, 100);
	m_ctlList1.InsertColumn(3,"硬件型号", LVCFMT_LEFT, 100);
	m_ctlList1.InsertColumn(4,"版本信息", LVCFMT_LEFT, 100);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void COnvifIPCDiscoveryToolDlg::OnDestroy()
{
	ClearOnlineListView();

	WSACleanup();

	CDialog::OnDestroy();

}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void COnvifIPCDiscoveryToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR COnvifIPCDiscoveryToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CALLBACK OnvifDeviceDiscoverCB(DEVICE_IPC_INFO *pIpcInfo, LPVOID lpContext)
{
   if(pIpcInfo && lpContext)
   {
	  COnvifIPCDiscoveryToolDlg * pView = (COnvifIPCDiscoveryToolDlg*)lpContext;

	  pView->OnAddToList(pIpcInfo);
   }
}

void  COnvifIPCDiscoveryToolDlg::ClearOnlineListView()
{
	int nItemCount = m_ctlList1.GetItemCount();
	for(int i=0; i<nItemCount; i++)
	{
	   DEVICE_IPC_INFO * pIPCInfo = (DEVICE_IPC_INFO*)m_ctlList1.GetItemData(i);
	   if(pIPCInfo != NULL)
	   {
           delete pIPCInfo;
	   } 
	}
	m_ctlList1.DeleteAllItems();
}

void COnvifIPCDiscoveryToolDlg::OnButtonRefresh()
{
	 ClearOnlineListView();

	 m_OnvifDevicesSearcher.StopFind();

	 m_OnvifDevicesSearcher.SetDiscoveryInfoCB(OnvifDeviceDiscoverCB, this);
	 m_OnvifDevicesSearcher.StartFind();
}

void  COnvifIPCDiscoveryToolDlg::OnAddToList(DEVICE_IPC_INFO *pIPCInfo)
{
   	int nListItem = m_ctlList1.GetItemCount();
	m_ctlList1.InsertItem(nListItem, pIPCInfo->IP, -1);
	m_ctlList1.SetItemText(nListItem, 0, pIPCInfo->IP);
	m_ctlList1.SetItemText(nListItem, 1, pIPCInfo->DeviceID);
	m_ctlList1.SetItemText(nListItem, 2, "IPC");
	m_ctlList1.SetItemText(nListItem, 3, pIPCInfo->hardware_type);

	//DEVICE_IPC_INFO * pNewIpc = new DEVICE_IPC_INFO();

	m_ctlList1.SetItemData(nListItem, (DWORD_PTR)pIPCInfo);
}

