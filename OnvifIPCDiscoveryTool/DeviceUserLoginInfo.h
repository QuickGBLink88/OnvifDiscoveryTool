#pragma once


// CDeviceUserLoginInfo 对话框

class CDeviceUserLoginInfo : public CDialog
{
	DECLARE_DYNAMIC(CDeviceUserLoginInfo)

public:
	CDeviceUserLoginInfo(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDeviceUserLoginInfo();

// 对话框数据
	enum { IDD = IDD_DIALOG_DEVICE_AUTH_INFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();

public:

   CString   m_strUserName;
   CString   m_strPassword;
};
