// deprejbg.h : main header file for the DEPREJBG application
//

#if !defined(AFX_DEPREJBG_H__EC5C1768_2335_4B87_8F5F_A3D345E5A5F9__INCLUDED_)
#define AFX_DEPREJBG_H__EC5C1768_2335_4B87_8F5F_A3D345E5A5F9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CDeprejbgApp:
// See deprejbg.cpp for the implementation of this class
//

class CDeprejbgApp : public CWinApp
{
public:
	CDeprejbgApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDeprejbgApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CDeprejbgApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEPREJBG_H__EC5C1768_2335_4B87_8F5F_A3D345E5A5F9__INCLUDED_)
