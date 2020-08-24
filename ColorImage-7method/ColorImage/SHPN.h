#pragma once


// SHPN 对话框

class SHPN : public CDialogEx
{
	DECLARE_DYNAMIC(SHPN)

public:
	SHPN(CWnd* pParent = NULL, int a = 10);   // 标准构造函数
	int *hpn;
	virtual ~SHPN();
	
// 对话框数据
	enum { IDD = IDD_DIALOGSET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int m_hpn;
	afx_msg void OnBnClickedOk();
};
