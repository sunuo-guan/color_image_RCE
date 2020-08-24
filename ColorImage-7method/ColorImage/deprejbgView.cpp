// deprejbgView.cpp : implementation of the CDeprejbgView class
//

#include "stdafx.h"
#include "deprejbg.h"

#include "deprejbgDoc.h"
#include "deprejbgView.h"
#include "SHPN.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDeprejbgView

IMPLEMENT_DYNCREATE(CDeprejbgView, CScrollView)

BEGIN_MESSAGE_MAP(CDeprejbgView, CScrollView)
	//{{AFX_MSG_MAP(CDeprejbgView)
	ON_COMMAND(ID_DE_ORI, OnDeOri)
	ON_COMMAND(ID_DE_FIXED, OnDeFixed)
	ON_COMMAND(ID_DE_PRE, OnDePre)
	ON_COMMAND(ID_SAVEWATERMARKED, OnSavewatermarked)
	ON_COMMAND(ID_TEST, OnTest)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_COMMAND(ID_EDIT_PASTE, &CDeprejbgView::OnEditPaste)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDeprejbgView construction/destruction

CDeprejbgView::CDeprejbgView()
{
	// TODO: add construction code here
	a=10;
}

CDeprejbgView::~CDeprejbgView()
{
}

BOOL CDeprejbgView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CDeprejbgView drawing

void CDeprejbgView::OnDraw(CDC* pDC)
{
	char temp[1024];
	int left,top,i;
	CDeprejbgDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here

	//绘制打开的图像以及刷新
	if(pDoc->horigimage!=NULL)
	{
		pDoc->lporigimage=(unsigned char *)pDoc->horigimage;
		if(pDoc->bitfile->bfType==0x4d42)
		{ 
			if(pDoc->bitinfo->biBitCount==8)
			{ 
			::StretchDIBits(pDC->m_hDC,2,2,pDoc->ImageWidth,
			pDoc->ImageHeight,0,0,
			pDoc->ImageWidth,pDoc->ImageHeight,
			pDoc->lporigimage+1024+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER),
			(BITMAPINFO *)(pDoc->lporigimage+sizeof(BITMAPFILEHEADER)),
			DIB_RGB_COLORS,SRCCOPY);
			}
			else if(pDoc->bitinfo->biBitCount==24)
			{ 
			::StretchDIBits(pDC->m_hDC,2,2,pDoc->ImageWidth,
			pDoc->ImageHeight,0,0,
			pDoc->ImageWidth,pDoc->ImageHeight,
			pDoc->lporigimage+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER),
			(BITMAPINFO *)(pDoc->lporigimage+sizeof(BITMAPFILEHEADER)),
			DIB_RGB_COLORS,SRCCOPY);
			}
			else if(pDoc->bitinfo->biBitCount==1)
			{ 
			::StretchDIBits(pDC->m_hDC,2,2,pDoc->ImageWidth,
			pDoc->ImageHeight,0,0,
			pDoc->ImageWidth,pDoc->ImageHeight,
			pDoc->lporigimage+62,
			(BITMAPINFO *)(pDoc->lporigimage+sizeof(BITMAPFILEHEADER)),
			DIB_RGB_COLORS,SRCCOPY);
			} 
		}
	}

	if( !pDoc->failed && pDoc->hwmimage)
	{
		::StretchDIBits(pDC->m_hDC,pDoc->ImageWidth+4,2,pDoc->ImageWidth,
			pDoc->ImageHeight,0,0,
			pDoc->ImageWidth,pDoc->ImageHeight,
			//pDoc->lpwmimage+1024+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER),
			pDoc->lpwmimage+ sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER),
			(BITMAPINFO *)(pDoc->lpwmimage+sizeof(BITMAPFILEHEADER)),
			DIB_RGB_COLORS,SRCCOPY);

		pDC->SetTextColor(RGB(255,0,0));//Set text color as RED color
		left=pDoc->ImageWidth+128;//The left position
		top=pDoc->ImageHeight+12;//The top position
		//sprintf(temp, "Pairs: %d, PSNR=%6.4f, Pure payload: %d bits (%4.3f bpp) of %d， R上下标记数量: %d,%d， B上下标记数量: %d,%d， R最大溢出: %d，%d， B最大溢出：%d,%d", pDoc->met + 1, pDoc->psnr, pDoc->payload, (double)pDoc->payload / (double)(pDoc->ImageHeight*pDoc->ImageWidth), pDoc->embnb, pDoc->Rmore, pDoc->Rless, pDoc->Bmore, pDoc->Bless, pDoc->Rhigh, pDoc->Rlow, pDoc->Bhigh, pDoc->Blow);
		sprintf(temp, "Pairs: %d, PSNR=%6.4f, Pure payload: %d bits (%4.3f bpp) of %d", pDoc->met + 1, pDoc->psnr, pDoc->payload, (double)pDoc->payload / (double)(pDoc->ImageHeight*pDoc->ImageWidth), pDoc->embnb);
		//sprintf(temp,"Pure payload: %d", pDoc->payload);
		//Get the display information
		pDC->TextOut(left,top,temp);//Display the information
	}
	else if(pDoc->failed)
	{
		::StretchDIBits(pDC->m_hDC,pDoc->ImageWidth+4,2,pDoc->ImageWidth,
			pDoc->ImageHeight,0,0,
			pDoc->ImageWidth,pDoc->ImageHeight,
			pDoc->lpwmimage+1024+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER),
			(BITMAPINFO *)(pDoc->lporigimage+sizeof(BITMAPFILEHEADER)),
			DIB_RGB_COLORS,SRCCOPY);

		pDC->SetTextColor(RGB(255,0,0));//Set text color as RED color
		left=pDoc->ImageWidth+172;//The left position
		top=pDoc->ImageHeight+12;//The top position
		sprintf(temp,"Deficient capacity! Embedding failed!, ind=%d, ct=%d,pairs=%d", pDoc->ind, pDoc->lml*8-pDoc->pct, pDoc->met + 1);
		//sprintf(temp,"Pure payload: %d", pDoc->payload);
		//Get the display information
		pDC->TextOut(left,top,temp);//Display the information
	}

	if(pDoc->wmheight>0)
	{
		::StretchDIBits(pDC->m_hDC,260,pDoc->ImageHeight+6,pDoc->wmwidth,
			pDoc->wmheight, 0, 0, pDoc->wmwidth,pDoc->wmheight,
			pDoc->m_CurrentDirectory+62,
			(BITMAPINFO *)(pDoc->m_CurrentDirectory+sizeof(BITMAPFILEHEADER)),
			DIB_RGB_COLORS,SRCCOPY);

	}

	if(pDoc->extracted)
	{

		pDC->SetTextColor(RGB(255,0,0));//Set text color as RED color
		left=pDoc->ImageWidth+88;//The left position
		top=pDoc->ImageHeight+32;//The top position
		sprintf(temp,"Original image recovered! Number of the extracted bits: %d of %d", pDoc->expld, pDoc->extnb);
		//sprintf(temp,"Pure payload: %d", pDoc->payload);
		//Get the display information
		pDC->TextOut(left,top,temp);//Display the information

	}


	//背景分割
	if (pDoc->hbkimage)
	{
		::StretchDIBits(pDC->m_hDC, pDoc->ImageWidth + 4, pDoc->ImageHeight + 64, pDoc->ImageWidth,
			pDoc->ImageHeight, 0, 0,
			pDoc->ImageWidth, pDoc->ImageHeight,
			pDoc->lpbkimage + 1024 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER),
			(BITMAPINFO *)(pDoc->lpbkimage + sizeof(BITMAPFILEHEADER)),
			DIB_RGB_COLORS, SRCCOPY);

		//背景色信息
		pDC->SetTextColor(RGB(255, 0, 0));//Set text color as RED color
		left = pDoc->ImageWidth * 2 + 10;//The left position
		top = pDoc->ImageHeight / 2;//The top position
		sprintf(temp, "bknum = %d,percent = %f", pDoc->bknum, pDoc->percent);
		pDC->TextOut(left, top, temp);//Display the information

		temp[0] = '\0';
		for (int i = 0; i < pDoc->bknum; i++)
		{
			sprintf(temp, "%s %d", temp, pDoc->bkpixel[i]);
			pDC->SetTextColor(RGB(255, 0, 0));//Set text color as RED color
			left = pDoc->ImageWidth * 2 + 10;//The left position
			top = pDoc->ImageHeight / 2 + 20 + i * 2;//The top position
			pDC->TextOut(left, top, temp);//Display the information
		}

	}
}


/////////////////////////////////////////////////////////////////////////////
// CDeprejbgView printing

BOOL CDeprejbgView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CDeprejbgView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CDeprejbgView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CDeprejbgView diagnostics

#ifdef _DEBUG
void CDeprejbgView::AssertValid() const
{
	CView::AssertValid();
}

void CDeprejbgView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CDeprejbgDoc* CDeprejbgView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDeprejbgDoc)));
	return (CDeprejbgDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDeprejbgView message handlers

void CDeprejbgView::OnDeOri() 
{
	// TODO: Add your command handler code here
	CDeprejbgDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	//pDoc->deoriRGB();
	//pDoc->deoriRGBSamePlus();
	//pDoc->Genhanced();
	//pDoc->ColorToGray();
	//pDoc->Yenhanced();
	//pDoc->GrayReference();
	//pDoc->enhancedMax();
	//pDoc->threePreProcess();
	//pDoc->reservedOnePairs();               //失败
	//pDoc->samePlus();
	//pDoc->maxEnhancedWithSamePlus();
	//pDoc->colorToGrayOnlyPreprocess(48);
	//pDoc->Test2();
	pDoc->twoSortAndPreprocess();
	Invalidate();
}

void CDeprejbgView::OnDeFixed() 
{
	// TODO: Add your command handler code here
	CDeprejbgDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->defix();
	Invalidate();
	
}

void CDeprejbgView::OnDePre() 
{
	// TODO: Add your command handler code here
	CDeprejbgDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	//pDoc->depreRGB();
	//pDoc->GenhancedRecovery();
	//pDoc->ColorToGrayRecovery();
	//pDoc->Test();
	//pDoc->enhancedMaxRe();
	//pDoc->threePreProcessRecover();
	//pDoc->maxEnhancedWithSamePlusRecover();
	pDoc->twoSortAndPreprocessRecover();
	Invalidate();
	
}

void CDeprejbgView::OnTest()
{
	CDeprejbgDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->Test();
	Invalidate();
}

void CDeprejbgView::OnSavewatermarked() 
{
	// TODO: Add your command handler code here
	CDeprejbgDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->desaveas();
	Invalidate();
	
}

void CDeprejbgView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	
	// TODO: Add your specialized code here and/or call the base class
	
	SetScrollSizes(MM_TEXT,CSize(4096, 2048));
	CScrollView::OnInitialUpdate();
}


void CDeprejbgView::OnEditPaste()
{
	// TODO: 在此添加命令处理程序代码
	
	CDeprejbgDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	unsigned char fthreshold;	//临时值
	fthreshold=pDoc->threshold;	//保存原值
	/*
    SHPN cgf(this,a);//打开窗函数设置对话框
    if(cgf.DoModal()==IDCANCEL)//若取消，恢复原先窗类型和窗参数
	{
		pDoc->threshold=fthreshold;
	}
    else
	{
		a=cgf.m_hpn;
		pDoc->threshold=a;
	}*/
	Invalidate();	
}
