// SHPN.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "deprejbg.h"
#include "SHPN.h"
#include "afxdialogex.h"


// SHPN �Ի���

IMPLEMENT_DYNAMIC(SHPN, CDialogEx)

SHPN::SHPN(CWnd* pParent /*=NULL*/, int a)
	: CDialogEx(SHPN::IDD, pParent)
{
	m_hpn = a;
}

SHPN::~SHPN()
{
}

void SHPN::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_hpn);
	DDV_MinMaxInt(pDX, m_hpn, 1, 64);
}


BEGIN_MESSAGE_MAP(SHPN, CDialogEx)
	ON_BN_CLICKED(IDOK, &SHPN::OnBnClickedOk)
END_MESSAGE_MAP()


// SHPN ��Ϣ�������


void SHPN::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	OnOK();
}
