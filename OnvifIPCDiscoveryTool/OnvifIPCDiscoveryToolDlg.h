// OnvifIPCDiscoveryToolDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"

#include "Discovery.h"

// COnvifIPCDiscoveryToolDlg 对话框
class COnvifIPCDiscoveryToolDlg : public CDialog
{
// 构造
public:
	COnvifIPCDiscoveryToolDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_ONVIFIPCDISCOVERYTOOL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

	void  ClearOnlineListView();

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonRefresh();
    afx_msg void OnDestroy();

	afx_msg void OnButtonGetStreamUrl();
	afx_msg void OnButtonUserLoginInfo();

	DECLARE_MESSAGE_MAP()
public:
		
	void  OnAddToList(DEVICE_IPC_INFO *pIPCInfo);

	COnvifDiscovery   m_OnvifDevicesSearcher;
	CListCtrl m_ctlList1;
	

};
