#pragma once


// SHPN �Ի���

class SHPN : public CDialogEx
{
	DECLARE_DYNAMIC(SHPN)

public:
	SHPN(CWnd* pParent = NULL, int a = 10);   // ��׼���캯��
	int *hpn;
	virtual ~SHPN();
	
// �Ի�������
	enum { IDD = IDD_DIALOGSET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	int m_hpn;
	afx_msg void OnBnClickedOk();
};
