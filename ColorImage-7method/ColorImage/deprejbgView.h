// deprejbgView.h : interface of the CDeprejbgView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEPREJBGVIEW_H__8D672E6B_B96C_4CEE_A901_86951D5FAEC0__INCLUDED_)
#define AFX_DEPREJBGVIEW_H__8D672E6B_B96C_4CEE_A901_86951D5FAEC0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CDeprejbgView : public CScrollView
{
protected: // create from serialization only
	CDeprejbgView();
	DECLARE_DYNCREATE(CDeprejbgView)

// Attributes
public:
	CDeprejbgDoc* GetDocument();
	int a;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDeprejbgView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDeprejbgView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CDeprejbgView)
	afx_msg void OnDeOri();
	afx_msg void OnDeFixed();
	afx_msg void OnDePre();
	afx_msg void OnSavewatermarked();
	afx_msg void OnTest();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEditPaste();
};

#ifndef _DEBUG  // debug version in deprejbgView.cpp
inline CDeprejbgDoc* CDeprejbgView::GetDocument()
   { return (CDeprejbgDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEPREJBGVIEW_H__8D672E6B_B96C_4CEE_A901_86951D5FAEC0__INCLUDED_)
