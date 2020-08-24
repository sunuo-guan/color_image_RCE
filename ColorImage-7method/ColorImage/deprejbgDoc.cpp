// deprejbgDoc.cpp : implementation of the CDeprejbgDoc class
//

#include "stdio.h"

#include<time.h>
#include<fstream>
#include<conio.h>
#include "stdafx.h"
#include "deprejbg.h"
#include <string>
#include <windows.h>
#include <gdiplus.h>
#include<cmath>
#pragma comment(lib, "gdiplus.lib")

#include "deprejbgDoc.h"

using namespace std;
using namespace Gdiplus;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TESTBUF_SIZE 400000L

unsigned char *testbuf;
long testbuf_len;
struct pixel
{
	unsigned char val;
	int t;
};


static void *checkedmalloc(size_t n)
{
  void *p;
  
  if ((p = calloc(1, n)) == NULL) {
    fprintf(stderr, "Sorry, not enough memory available!\n");
    exit(1);
  }
  
  return p;
}

static void testbuf_write(int v, void *dummy)
{
  if (testbuf_len < TESTBUF_SIZE)
    testbuf[testbuf_len++] = v;
  (void) dummy;
  return;
}


static void testbuf_writel(unsigned char *start, size_t len, void *dummy)
{
  if (testbuf_len < TESTBUF_SIZE) {
    if (testbuf_len + len < TESTBUF_SIZE)
      memcpy(testbuf + testbuf_len, start, len);
    else
      memcpy(testbuf + testbuf_len, start, TESTBUF_SIZE - testbuf_len);
  }
  testbuf_len += len;

#ifdef DEBUG
  {
    unsigned char *p;
    unsigned sum = 0;
    
    for (p = start; p - start < (ptrdiff_t) len; sum = (sum ^ *p++) << 1);
    printf("  testbuf_writel: %4d bytes, checksum %04x\n",
	   len, sum & 0xffff);
  }
#endif

  (void) dummy;
  return;
}

/////////////////////////////////////////////////////////////////////////////
// CDeprejbgDoc

IMPLEMENT_DYNCREATE(CDeprejbgDoc, CDocument)

BEGIN_MESSAGE_MAP(CDeprejbgDoc, CDocument)
	//{{AFX_MSG_MAP(CDeprejbgDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDeprejbgDoc construction/destruction

CDeprejbgDoc::CDeprejbgDoc()
{
	// TODO: add one-time construction code here
	ltime=1197085784;
	horigimage=NULL;
	hwmimage=NULL;
	hpreimage=NULL;
	hdifimage=NULL;
	hkey=NULL;
	hlocmap=NULL;
	hhismap=NULL;
	hblkhis=NULL;
	hblkval=NULL;
	hembmap=NULL;
	hembovh=NULL;
	hextmap=NULL;
	hextlen=NULL;
	hprehist=NULL;
	hlbldbn=NULL;
	wmheight=0;
	extracted=0;
	failed=0;
	mext=0;
	met=0;
	hblkflg = NULL;
	hbkimage = NULL;
	hphist = NULL;
	threshold = 25;
	newPre = true;     //�Ƿ�ʹ���µ�Ԥ����

	//��ʼ����������
	for (int i = 0; i < 2140; i++)
	{
		for (int j = 0; j < 2140; j++)
		{
			isBackground[i][j] = false;
		}
	}

	for (int i = 0; i < 256; i++)
	{
		bkpixel[i] = 0;
	}

	doBksega = false;

	percent = 0;
	bknum = 0;

	//��ɫͼ�����ӵĴ���
	Init();
	 f = 0;
}
   
CDeprejbgDoc::~CDeprejbgDoc()
{
}

BOOL CDeprejbgDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



static void output_bie(unsigned char *start, size_t len, void *file)
{
	fwrite(start, 1, len, (FILE *) file);
	testbuf_len = testbuf_len + len;
	return;
}

static int line_out(const struct jbg85_dec_state *s,
		    unsigned char *start, size_t len,
		    unsigned long y, void *bitmap)
{
  memcpy((unsigned char *) bitmap + len * y, start, len);
  return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CDeprejbgDoc serialization

void CDeprejbgDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{

//	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"ͼ���ļ�|*.bmp;*.raw;*.vq||");
//	dlg.m_ofn.lpstrTitle="��ԭʼͼ��";

		// TODO: add loading code here
		CFile *fp;
	    //Get the file pointer
		fp=ar.GetFile();													//If the user specifies a filename and clicks the OK button, the return value is nonzero. The buffer pointed to by the lpstrFile member of the OPENFILENAME structure contains the full path and filename specified by the user.
		if (!fp)
		{
			return;
		}
		m_ImageSize=fp->GetLength();		//������BITMAPHEANDER,BITMAPINFOHEADER,����ֵ��ȫ����Ϣ
		if(horigimage)	
		{
			GlobalFree(horigimage); 
			horigimage=NULL;
		}
		//Allocates the specified number of bytes from the heap. 
		horigimage=GlobalAlloc(GMEM_FIXED,m_ImageSize);		//GlobalAlloc	The GlobalAlloc function allocates the specified number of bytes from the heap.If the function succeeds, the return value is a handle to the newly allocated memory object.
		//Change the handle of bmp image from HANDLE pattern to LPSTR pattern 
		lporigimage=(unsigned char *)horigimage;
		
		//Open the original image  file
		ar.Read(lporigimage,m_ImageSize);

		strcpy_s(FileTitle, fp->GetFileName());
		

		bitfile=(LPBITMAPFILEHEADER)lporigimage;								//BITMAPFILEHEADER	The BITMAPFILEHEADER structure contains information about the type, size, and layout of a file that contains a device-independent bitmap (DIB)
		bitinfo=(LPBITMAPINFOHEADER)(lporigimage+sizeof(BITMAPFILEHEADER));	//BITMAPINFOHEADER	The BITMAPINFOHEADER structure contains information about the dimensions and color format of a device-independent bitmap (DIB). 

		if(bitinfo->biCompression!=0)
		{ 
			AfxMessageBox("�ļ���ѹ��ͼ��");
			return;
		}
		ImageWidth=(short int)bitinfo->biWidth;
		ImageHeight=(short int)bitinfo->biHeight;
				
		//��ɫͼ�����ӵĴ���
		m_ColorImageSize = m_ImageSize;
		//Ϊ��Ƕ��ԭ���ĻҶȴ�������ж�д
		m_ImageSize = 1024 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + ImageWidth * ImageHeight * sizeof(unsigned char);
		
		onlyPreProcess = false;
		leftPreProcess = false;
		leftPreProcessRecover = false;
		colorToGrayPreprocessRecover = false;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDeprejbgDoc diagnostics

#ifdef _DEBUG
void CDeprejbgDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDeprejbgDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDeprejbgDoc commands

void CDeprejbgDoc::deori(int mm)
{
	
	//FILE *fored=fopen("embedall-test.txt","wb");	//���ˮӡǶ���һЩ�������
	
	//��Щ��������������Ƕ��Ч���Ĳ���
	int row, column, i, j, g;
	double h, di;
	long pld, tpl, mc, mt;
	time_t stime , etime ;


	mc = 0;
	mt = 3;
	embnb =0;
	//bkgdseg();
	time( &stime );
	_getch(); 
	
	for(threshold=mm; threshold<mm+1; threshold=threshold+1)
	//for(threshold=10; threshold<51; threshold++)
	{
	time( &stime );
	_getch(); 
		payload=0;

		g=1024+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);	//ͼ�񴢴�����ֵ��Ϣ��λ��ƫ����,1024��ʾbiBitCount==8
		
		//Ϊwmimage��preimage�����ڴ沢��origimage�����ݸ��Ƹ�����
		if(hwmimage)
		{ 
			GlobalFree(hwmimage);  
			hwmimage=NULL;
		}
 		hwmimage=GlobalAlloc(GMEM_FIXED,m_ImageSize);		//GlobalAlloc	The GlobalAlloc function allocates the specified number of bytes from the heap.If the function succeeds, the return value is a handle to the newly allocated memory object.
		lpwmimage=(unsigned char *)hwmimage;
		memcpy(lpwmimage,lporigimage,m_ImageSize);

		if(hpreimage)
		{ 
			GlobalFree(hpreimage);  
			hpreimage=NULL;
		}
		hpreimage=GlobalAlloc(GMEM_FIXED,m_ImageSize);		//GlobalAlloc	The GlobalAlloc function allocates the specified number of bytes from the heap.If the function succeeds, the return value is a handle to the newly allocated memory object.
		lppreimage=(unsigned char *)hpreimage;
		memcpy(lppreimage,lporigimage,m_ImageSize);

		//tpl=-1;
		payload=0;

		/*
		for(i=0; payload>tpl && failed==0 && i<64; i++) //&& i<34
		{
			tpl=payload;
			payload=0;

			pld=deremb(lppreimage, lpwmimage, i);
			payload=payload+pld;

			memcpy(lpwmimage,lporigimage,m_ImageSize);
			memcpy(lppreimage,lporigimage,m_ImageSize);
		}
		*/

//		i=43; 

		met=threshold-1;                                     //met��ֵΪ�����ڶ���Ƕ���Sֵ
		failed=0;
		payload=0;

		if(met>-1 && threshold>0)
		{
			pld=deremb(lppreimage, lpwmimage, met);           //���������ͼ������ͼ����ǿ����
			payload=pld;
			memcpy(lpwmimage,lppreimage,m_ImageSize);

		}
		else	failed=1;

		if(payload>mc)
		{
			mc=payload;
			mt=threshold;
		}

//	}
		time( &etime );
//	threshold--;

		//����Ĺ����Ǽ���Ƕ��Ч������
		psnr=0.0;

		//����psnr = sum((origiamge[i][j]-preimage[i][j])^2)
		for(row=0;row<ImageHeight;row++)
		{
			for(column=0;column<ImageWidth;column++)
			{
				i=*(lporigimage+g+row*ImageWidth+column);		//origimage��[row,column]���ص�ֵ
				j=*(lppreimage+g+row*ImageWidth+column);		//preimage��[row,column]���ص�ֵ
				h=i-j;
				psnr=psnr+h*h;
			}
		}

		di=psnr/(ImageHeight*ImageWidth);

		psnr=10*log10((255*255)/di);

		  
		//��Ƕ����pure hiding rateд���ļ�
		/*
		FILE *payloadtxt;
		payloadtxt = fopen("E:\\experiment\\task\\result\\pure hiding rate.txt", "a");
		if (payloadtxt != NULL)
			fprintf(payloadtxt, "\n %d \t %6.4f", threshold, (double)payload / (double)(ImageHeight*ImageWidth));
		fclose(payloadtxt);
        
		//��Ƕ��д���ļ�
		FILE *embnbtxt;
		embnbtxt = fopen("E:\\experiment\\task\\result\\embnb hiding rate.txt", "a");
		if (embnbtxt != NULL)
			fprintf(embnbtxt, "\n%6.4f", (double)embnb / (double)(ImageHeight*ImageWidth));
		fclose(embnbtxt);
		*/

		//fprintf(fored, "%6.4f ", psnr); 
		//
		//fprintf(fored, "%6.4f ", (double)payload/(double)(ImageHeight*ImageWidth)); 
		//
		//fprintf(fored, "%d \r\n", met+1);
		//
		//fprintf(fored, "%ld\n" , etime - stime );

//		fprintf(fored, "\n");
//		fprintf(fored, "endl");

	}

	//fclose(fored);

}

long CDeprejbgDoc::deremb(unsigned char *lpwmimg, unsigned char *lporigimg, int t)
{
	unsigned short int row,le,re;
	unsigned char tm;                     //��¼ѡ���bin����Ҫ���ϲ�������bin������ֵ
	unsigned char lm;                     //��¼�bin
	unsigned char tmp, ln;
	int i,j,k,m,et,n,a,ml,mr,rn,tn,pn,hist[256],order[256],ordlf[256],ordrg[128],ft;
	bool lo=true, ro=true, flg, phis[256],flag[256];
	long column,g, el;
	struct jbg85_enc_state s;



	g=1024+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
	ln=0;
	pct=0;
	pn=0;

	wp=lpwmimg+g;
	lp=lporigimg+g;              
	ml=mr=ImageHeight*ImageWidth;
	la=ra=0;
	testbuf_len=0;
	
	for(i=0;i<256;i++)
	{
		hist[i]=0;
	}
	
	g=ImageHeight*ImageWidth;

	if(hhismap)
	{ 
		GlobalFree(hhismap);  
		hhismap=NULL;
	}
	hhismap = GlobalAlloc(GMEM_FIXED,g*2*sizeof(unsigned char));
	lphismap=(unsigned char *)hhismap;
	

	if(hlbldbn)
	{ 
		GlobalFree(hlbldbn);  
		hlbldbn=NULL;
	}
	hlbldbn = GlobalAlloc(GMEM_FIXED,256*sizeof(unsigned char));
	lplbldbn=(unsigned char *)hlbldbn;

	//����ֵ-1
	for(i=0; i<256; i++)
		*(lplbldbn+i)=-1;

	if(hblkhis)
	{ 
		GlobalFree(hblkhis);  
		hblkhis=NULL;
	}
	hblkhis = GlobalAlloc(GMEM_FIXED, ImageHeight*ImageWidth*sizeof(unsigned int));
	lpblkhis=(unsigned int *)hblkhis;
	
	if(hprehist)
	{ 
		GlobalFree(hprehist);  
		hprehist=NULL;
	}
	hprehist = GlobalAlloc(GMEM_FIXED, 2 * (t + 2) *sizeof(unsigned char));
	lpprehist=(unsigned char *)hprehist;

	if(hblkval)
	{ 
		GlobalFree(hblkval);  
		hblkval=NULL;
	}
	hblkval = GlobalAlloc(GMEM_FIXED, ImageHeight*ImageWidth*sizeof(int));
	lpblkval=(int *)hblkval;

	//����ֵ
	for(i=0;i<ImageHeight;i++)
		for(j=0;j<ImageWidth;j++)
		{
			*(lpblkhis+i*ImageWidth+j)=0;		//lpblkhis[i][j] = 0
			*(lpblkval+i*ImageWidth+j)=-1;		//lpblkval[i][j] = -1
		}

	/*ͳ��ͼ��ֱ��ͼ*/
	for(i=0;i<ImageHeight;i++)
	{
		for(j=0;j<ImageWidth;j++)
		{
			lm=*(lp+i*ImageWidth+j);		//lm = lp[i][j]
			flg=0;

			for(k=0; k<pn+1; k++)
			{
				if(lm==*(lpblkval+k))		//��lm == lpblkval[k]����ʾֱ��ͼ����һ��lm��ֵ����ֵ��bin����+1
				{
					tn=*(lpblkhis+k);
					*(lpblkhis+k)=tn+1;
					flg=1;
				}
			}

			if(flg==0)						//��lpblkval��û��lmֵ����ֱ��ͼ���½�lm����Ӧ������1
			{
				pn++;
				*(lpblkhis+pn)=1;
				*(lpblkval+pn)=lm;
			}

		}
	}
	
	/*ֱ��ͼ����hist��ֵ*/
	for(i=0; i<256; i++)
		for(k=1; k<pn+1; k++)
		{
			lm=*(lpblkval+k);
			if(lm==i)
			{
				rn=*(lpblkhis+k);
				hist[i]=rn;               
			}
		}
		
	/*lpblkval��lpblkhis��lpblkval��ֵ��С��������*/
	for(i=0; i<pn; i++)
		for(k=1; k<pn-i; k++)          //k=1��
		{
			lm=*(lpblkval+k);
			tm=*(lpblkval+k+1);
			if(lm>tm)
			{
				*(lpblkval+k)=tm;
				*(lpblkval+k+1)=lm;
				rn=*(lpblkhis+k);
				tn=*(lpblkhis+k+1);
				*(lpblkhis+k)=tn;
				*(lpblkhis+k+1)=rn;

			}
		}

	/* ��ʼ��order[]��flag[] */
	for(i=0; i<256; i++)
	{
		order[i]=-1;
		flag[i]=0;
	}

	/* order��0-255�ֱ��lpblkhis������С����������±�� */
	for(i=0; i<pn; i++)
	{
		a=ImageHeight*ImageWidth;
		for(k=pn; k>0; k--)
		{
			rn=*(lpblkhis+k);                         //����
			if(rn<a && flag[k-1]==0)                  //����Сֵ��flag�������ҵ���k
			{
				order[i]=k;
				a=rn+1;
			}
		}
		flag[order[i]-1]=1;
	}

	for(i=0; i<256; i++)
	{
		phis[i]=0;
	}

	ml=0;


	int zi, fi, rj, mi;
	unsigned char rm;
	
	if (leftPreProcess == false)
	{
		for (i = 0; i < 128; i++)
		{
			ordlf[i] = -1;                                       //��ʼ��ordlf����ʼֵΪ-1
			if (hist[i] == 0 && ml < t + 1)                           //�п�bin���ҿ�bin��������������Ƕ��ʱbin������S��Ҫ��ʱ����
			{
				lm = (unsigned char)i;

				for (row = 0; row < ImageHeight; row++)
					for (column = 0; column < ImageWidth; column++)
					{
						tmp = *(wp + row * ImageWidth + column);
						if (tmp < lm)
							*(wp + row * ImageWidth + column) = tmp + 1;      //С�ڿ�bin��bin������
					}

				*(lpprehist + ml) = i;                                  //��¼�˿�bin������ֵ
				ml++;

				for (j = 1; j < pn + 1; j++)
				{
					tm = *(lpblkval + j);
					if (tm < lm)
						*(lpblkval + j) = tm + 1;                         //С��i��ֵ��+1
				}

			}
		}

		le = ml;
		ln = 0;

		/* ֱ��ͼ��ߵ�Ԥ����bin merging�����bin */
		int zi, fi, rj, mi;
		unsigned char rm;

		if (ml < t + 1)
		{
			if (newPre == false)
			{
				j = 0;
				for (i = 0; i < 128; )
				{
					while (i < 128 && j < pn  && order[j] < 128)           //����pn�������bin�ĸ߶ȴӰ����߸���ordlf
					{
						ordlf[i] = order[j];
						i = i + 1;
						j = j + 1;
					}
					j = j + 1;
					if (j > pn - 1 || i > 127)
						break;
				}

				for (i = ln; i < 128 && ml < t + 1; i++)
				{
					if (*(lpblkhis + ordlf[i]) > 0)							//���bin�߶ȴ���0
					{
						lm = *(lpblkval + ordlf[i]);						//lm=�bin������ֵ
						tm = lm;
						if (lm == 0)										//���bin����ֵΪ0��tmָ���ұ�bin
						{
							tm = *(lpblkval + ordlf[i] + 1);
						}
						else if (*(lpblkhis + ordlf[i] - 1) > 0 && lm < 128)     //���bin�����bin��Ϊ��bin������ߵ�binû�б���Ϲ�����ѡ����ߵ�bin������ѡ�ұߵ�bin
						{
							tm = *(lpblkval + ordlf[i] - 1);					//�bin�����bin����¼

							m = 1;

							for (j = i + 1; j < 127; j++)
							{
								if (*(lpblkval + ordlf[j]) == tm)            //�ж���ߵ�bin��û�б���Ϲ������Ƿ���ԭ��bin������ԭ��bin��tmֵ����val�б��ҵ���֤��û�б���ϣ�������
								{
									m = 0;
									break;
								}
							}
							if (m == 1 && lm < 127)                          //�����bin����ϣ���ѡ�ұ�bin
								tm = *(lpblkval + ordlf[i] + 1);
						}
						else if (lm < 127)
							tm = *(lpblkval + ordlf[i] + 1);					//���bin�����binΪ��bin�����¼�bin�ұߵ�bin

						m = 1;

						for (j = i + 1; j < 127 && tm != lm; j++)
						{
							if (*(lpblkval + ordlf[j]) == tm)              //��ʱtm�ѱ�ɴ���ϵ�bin����Ϊ���Ϊ�ң������ߵ�binû�б���Ϲ������޸�ordlf���߶�������ǰŲ���ΰ����
							{
								for (m = j; m < 127; m++)
								{
									ordlf[m] = ordlf[m + 1];
								}
								m = 0;
								break;
							}
						}
						if (m == 0)                                      //����ߵ�binû�б���Ϲ�����ѡ���bin���
						{
							if (lm > tm)                                 //��tm����lm����ߣ��򽻻���ֵ��tm��lm�ұߣ��򲻱�
							{
								tmp = tm;
								tm = lm;
								lm = tmp;
								m = ordlf[i] - 1;
							}
							else
							{
								m = ordlf[i];
							}

							if (lm > -1 && lm < 127 && tm < 128)
							{

								for (row = 0; row < ImageHeight; row++)
									for (column = 0; column < ImageWidth; column++)
									{
										tmp = *(wp + row * ImageWidth + column);
										if (tmp == lm)                               //������Ϊ���bin�����1��������Ϊ�ұ�bin�����0
										{
											*(lphismap + testbuf_len) = 1;
											testbuf_len++;
										}
										else if (tmp == lm + 1)
										{
											*(lphismap + testbuf_len) = 0;
											testbuf_len++;
										}
										if (tmp < lm + 1)
											*(wp + row * ImageWidth + column) = tmp + 1;    //С���ұ�bin�����ض�+1��������1λ
									}

								*(lpprehist + ml) = lm;                               //��¼���bin��ֵ
								*(lplbldbn + ml - le) = lm + 1;                           //��¼�ұ�bin��ֵ��le�ǿ�bin������,ml�ǿ�bin�����bin������֮��

								rn = *(lpblkhis + m);
								tn = *(lpblkhis + m + 1);
								*(lpblkhis + m + 1) = rn + tn;                           //����bin��ϣ��߶����
								*(lpblkhis + m) = 0;
								pct = pct + rn + tn;

								for (j = m; j > ln + 1; j--)                              //val��his��ordlf�ֱ��ƶ�����Ӧλ��
								{
									tmp = *(lpblkval + j - 1);
									*(lpblkval + j) = tmp + 1;
									tn = *(lpblkhis + j - 1);
									*(lpblkhis + j) = tn;

									for (k = ln + 1; k < 127; k++)
									{
										if (*(lpblkval + ordlf[k]) == tmp)
										{
											ordlf[k] = ordlf[k] + 1;
										}
									}
								}

								for (j = 1 + ln; j > 0; j--)
								{
									*(lpblkhis + j) = 0;
								}

								for (j = ml - 1; j > le - 1; j--)
								{
									tmp = *(lplbldbn + j - le);
									*(lplbldbn + j - le) = tmp + 1;
								}

								ml++;
								ln++;

							}
							else if (lm == 127 || m == 0 || i > 126)
							{
								break;
							}

						}
						else											//���������ߵ�bin����Ϲ������������bin
						{
							//i=i+1;
							for (m = i; m < 127; m++)
							{
								ordlf[m] = ordlf[m + 1];
							}
							if (ordlf[i] == ordlf[127])
							{
								break;
							}
							else
							{
								i = i - 1;
							}
						}
					}
				}

			}
			else if (newPre == true)
			{
				j = 0;
				for (i = 0; i < 128; )
				{
					while (i < 128 && j < pn  && order[j] < 128 && *(lpblkval + order[j]) < 128)           //����pn�������bin�ĸ߶ȴӰ����߸���ordlf
					{
						ordlf[i] = order[j];
						i = i + 1;
						j = j + 1;
					}
					j = j + 1;
					if (j > pn - 1 || i > 127)
						break;
				}

				zi = 0;

				for (; zi < 128 && ml < t + 1; zi++)
				{
					i = ln;
					fi = ordlf[i];
					if (*(lpblkhis + fi) > 0 && *(lpblkval + fi) < 128)                    //������ҷǿ�bin
					{
						lm = *(lpblkval + fi);
						tm = lm;
						m = 1;
						if (lm == 0)
						{
							rm = *(lpblkval + fi + 1);
							m = 0;                                        //ֻ�ܺ��ұ߽��

							for (j = i + 1; j < 127; j++)
							{
								if (*(lpblkval + ordlf[j]) == tm)
								{
									rj = j;
									break;
								}
							}
						}
						else if (lm == 127)
						{
							tm = *(lpblkval + fi - 1);
							m = 2;                                          //ֻ�ܺ���߽��

							for (j = i + 1; j < 127; j++)
							{
								if (*(lpblkval + ordlf[j]) == rm)
								{
									rj = j;
									break;
								}
							}
						}
						else
						{
							tm = *(lpblkval + fi - 1);
							rm = *(lpblkval + fi + 1);

							for (j = i + 1; j < 127; j++)                               //���ұȽ��ĸ���С�����С��m=2���ұ�С��m=0
							{
								if (*(lpblkval + ordlf[j]) == tm)
								{
									m = 2;
									break;
								}
								else if (*(lpblkval + ordlf[j]) == rm)
								{
									m = 0;
									rj = j;
									break;
								}
							}
						}

						if (m == 2)                                                  //������߽�С
						{
							for (j = i + 1; j < 127 && tm != lm; j++)
							{
								if (*(lpblkval + ordlf[j]) == tm)
								{
									for (m = j; m < 127; m++)
									{
										ordlf[m] = ordlf[m + 1];
									}
									break;
								}
							}

							tmp = tm;
							tm = lm;
							lm = tmp;
							mi = fi - 1;

							if (lm > -1 && lm < 127 && tm < 128)
							{

								for (row = 0; row < ImageHeight; row++)
									for (column = 0; column < ImageWidth; column++)
									{
										tmp = *(wp + row * ImageWidth + column);
										if (tmp == lm)
										{
											*(lphismap + testbuf_len) = 1;
											testbuf_len++;
										}
										else if (tmp == lm + 1)
										{
											*(lphismap + testbuf_len) = 0;                    //lphismap����
											testbuf_len++;
										}
										if (tmp < lm + 1)
											*(wp + row * ImageWidth + column) = tmp + 1;
									}

								*(lpprehist + ml) = lm;                                       //lpprehist����
								*(lplbldbn + ml - le) = lm + 1;

								rn = *(lpblkhis + mi);
								tn = *(lpblkhis + mi + 1);
								*(lpblkhis + mi + 1) = rn + tn;
								*(lpblkhis + mi) = 0;
								pct = pct + rn + tn;

								for (j = mi; j > ln + 1; j--)
								{
									tmp = *(lpblkval + j - 1);
									*(lpblkval + j) = tmp + 1;
									tn = *(lpblkhis + j - 1);
									*(lpblkhis + j) = tn;

									for (k = ln + 1; k < 127; k++)
									{
										if (*(lpblkval + ordlf[k]) == tmp)
										{
											ordlf[k] = ordlf[k] + 1;
										}
									}

								}

								for (j = ln + 1; j > 0; j--)
								{
									*(lpblkhis + j) = 0;
								}

								for (j = ml - 1; j > le - 1; j--)
								{
									tmp = *(lplbldbn + j - le);
									*(lplbldbn + j - le) = tmp + 1;
								}

								ml++;

								for (k = ln + 1; k < 127; k++)
								{
									if (*(lpblkhis + ordlf[k]) > *(lpblkhis + mi + 1))
									{
										break;
									}
								}

								for (j = ln; j < k - 1; j++)
								{
									ordlf[j] = ordlf[j + 1];
								}
								ordlf[k - 1] = mi + 1;

							}
						}
						else if (m == 0)                                             //�����ұ߽�С
						{
							mi = fi;

							if (lm > -1 && lm < 127 && rm < 128)
							{

								for (row = 0; row < ImageHeight; row++)
									for (column = 0; column < ImageWidth; column++)
									{
										tmp = *(wp + row * ImageWidth + column);
										if (tmp == lm)
										{
											*(lphismap + testbuf_len) = 1;
											testbuf_len++;
										}
										else if (tmp == lm + 1)
										{
											*(lphismap + testbuf_len) = 0;                   //lphismap����
											testbuf_len++;
										}
										if (tmp < lm + 1)
											*(wp + row * ImageWidth + column) = tmp + 1;           //λ�ơ�bin���
									}

								*(lpprehist + ml) = lm;                                      //lpprehist����
								*(lplbldbn + ml - le) = lm + 1;

								rn = *(lpblkhis + mi);
								tn = *(lpblkhis + mi + 1);
								*(lpblkhis + mi + 1) = rn + tn;
								*(lpblkhis + mi) = 0;
								pct = pct + rn + tn;

								/*lpblkval��lpblkhis����*/
								for (j = mi; j > ln + 1; j--)
								{
									tmp = *(lpblkval + j - 1);
									*(lpblkval + j) = tmp + 1;
									tn = *(lpblkhis + j - 1);
									*(lpblkhis + j) = tn;

									for (k = ln + 1; k < 127; k++)
									{
										if (*(lpblkval + ordlf[k]) == tmp)
										{
											ordlf[k] = ordlf[k] + 1;
										}
									}
								}

								for (j = 1 + ln; j > 0; j--)
								{
									*(lpblkhis + j) = 0;
								}

								for (j = ml - 1; j > le - 1; j--)
								{
									tmp = *(lplbldbn + j - le);
									*(lplbldbn + j - le) = tmp + 1;
								}

								ml++;

								for (k = ln + 1; k < 127; k++)
								{
									if (*(lpblkhis + ordlf[k]) > *(lpblkhis + mi + 1))
									{
										break;
									}
								}

								if (ordlf[i + 1] == mi + 1)
								{
									for (j = ln + 1; j < k - 1; j++)
									{
										ordlf[j] = ordlf[j + 1];
									}
									ordlf[k - 1] = mi + 1;
								}
								else
								{
									for (j = rj; j < k - 1; j++)
									{
										ordlf[j] = ordlf[j + 1];
									}
									ordlf[k - 1] = mi + 1;
								}

								ln++;

							}
							else if (lm == 127 || mi == 0 || zi > 126)
							{
								break;
							}

						}
						else
						{
							//i=i+1;
							for (m = i; m < 127; m++)
							{
								ordlf[m] = ordlf[m + 1];
							}
							if (ordlf[i] == ordlf[127])
							{
								break;
							}
							else
							{
								i = i - 1;
							}
						}
					}
				}
			}
		}
	}
	else
	{
		/*��߿�bin��Ԥ����*/
		for (i = 0; i < 256; i++)
		{
			ordlf[i] = -1;                                       //��ʼ��ordlf����ʼֵΪ-1
			if (hist[i] == 0 && ml < t + 1)                           //�п�bin���ҿ�bin��������������Ƕ��ʱbin������S��Ҫ��ʱ����
			{
				lm = (unsigned char)i;

				for (row = 0; row < ImageHeight; row++)
					for (column = 0; column < ImageWidth; column++)
					{
						tmp = *(wp + row * ImageWidth + column);
						if (tmp < lm)
							*(wp + row * ImageWidth + column) = tmp + 1;      //С�ڿ�bin��bin������
					}

				*(lpprehist + ml) = i;                                  //��¼�˿�bin������ֵ
				ml++;

				for (j = 1; j < pn + 1; j++)
				{
					tm = *(lpblkval + j);
					if (tm < lm)
						*(lpblkval + j) = tm + 1;                         //С��i��ֵ��+1
				}

			}
		}

		le = ml;
		ln = 0;

		/* ֱ��ͼ��ߵ�Ԥ����bin merging�����bin */
		int zi, fi, rj, mi;
		unsigned char rm;

		if (ml < t + 1)
		{
			if (newPre == false)
			{
				j = 0;
				for (i = 0; i < 128; )
				{
					while (i < 128 && j < pn  && order[j] < 128)           //����pn�������bin�ĸ߶ȴӰ����߸���ordlf
					{
						ordlf[i] = order[j];
						i = i + 1;
						j = j + 1;
					}
					j = j + 1;
					if (j > pn - 1 || i > 127)
						break;
				}

				for (i = ln; i < 128 && ml < t + 1; i++)
				{
					if (*(lpblkhis + ordlf[i]) > 0)							//���bin�߶ȴ���0
					{
						lm = *(lpblkval + ordlf[i]);						//lm=�bin������ֵ
						tm = lm;
						if (lm == 0)										//���bin����ֵΪ0��tmָ���ұ�bin
						{
							tm = *(lpblkval + ordlf[i] + 1);
						}
						else if (*(lpblkhis + ordlf[i] - 1) > 0 && lm < 128)     //���bin�����bin��Ϊ��bin������ߵ�binû�б���Ϲ�����ѡ����ߵ�bin������ѡ�ұߵ�bin
						{
							tm = *(lpblkval + ordlf[i] - 1);					//�bin�����bin����¼

							m = 1;

							for (j = i + 1; j < 127; j++)
							{
								if (*(lpblkval + ordlf[j]) == tm)            //�ж���ߵ�bin��û�б���Ϲ������Ƿ���ԭ��bin������ԭ��bin��tmֵ����val�б��ҵ���֤��û�б���ϣ�������
								{
									m = 0;
									break;
								}
							}
							if (m == 1 && lm < 127)                          //�����bin����ϣ���ѡ�ұ�bin
								tm = *(lpblkval + ordlf[i] + 1);
						}
						else if (lm < 127)
							tm = *(lpblkval + ordlf[i] + 1);					//���bin�����binΪ��bin�����¼�bin�ұߵ�bin

						m = 1;

						for (j = i + 1; j < 127 && tm != lm; j++)
						{
							if (*(lpblkval + ordlf[j]) == tm)              //��ʱtm�ѱ�ɴ���ϵ�bin����Ϊ���Ϊ�ң������ߵ�binû�б���Ϲ������޸�ordlf���߶�������ǰŲ���ΰ����
							{
								for (m = j; m < 127; m++)
								{
									ordlf[m] = ordlf[m + 1];
								}
								m = 0;
								break;
							}
						}
						if (m == 0)                                      //����ߵ�binû�б���Ϲ�����ѡ���bin���
						{
							if (lm > tm)                                 //��tm����lm����ߣ��򽻻���ֵ��tm��lm�ұߣ��򲻱�
							{
								tmp = tm;
								tm = lm;
								lm = tmp;
								m = ordlf[i] - 1;
							}
							else
							{
								m = ordlf[i];
							}

							if (lm > -1 && lm < 127 && tm < 128)
							{

								for (row = 0; row < ImageHeight; row++)
									for (column = 0; column < ImageWidth; column++)
									{
										tmp = *(wp + row * ImageWidth + column);
										if (tmp == lm)                               //������Ϊ���bin�����1��������Ϊ�ұ�bin�����0
										{
											*(lphismap + testbuf_len) = 1;
											testbuf_len++;
										}
										else if (tmp == lm + 1)
										{
											*(lphismap + testbuf_len) = 0;
											testbuf_len++;
										}
										if (tmp < lm + 1)
											*(wp + row * ImageWidth + column) = tmp + 1;    //С���ұ�bin�����ض�+1��������1λ
									}

								*(lpprehist + ml) = lm;                               //��¼���bin��ֵ
								*(lplbldbn + ml - le) = lm + 1;                           //��¼�ұ�bin��ֵ��le�ǿ�bin������,ml�ǿ�bin�����bin������֮��

								rn = *(lpblkhis + m);
								tn = *(lpblkhis + m + 1);
								*(lpblkhis + m + 1) = rn + tn;                           //����bin��ϣ��߶����
								*(lpblkhis + m) = 0;
								pct = pct + rn + tn;

								for (j = m; j > ln + 1; j--)                              //val��his��ordlf�ֱ��ƶ�����Ӧλ��
								{
									tmp = *(lpblkval + j - 1);
									*(lpblkval + j) = tmp + 1;
									tn = *(lpblkhis + j - 1);
									*(lpblkhis + j) = tn;

									for (k = ln + 1; k < 127; k++)
									{
										if (*(lpblkval + ordlf[k]) == tmp)
										{
											ordlf[k] = ordlf[k] + 1;
										}
									}
								}

								for (j = 1 + ln; j > 0; j--)
								{
									*(lpblkhis + j) = 0;
								}

								for (j = ml - 1; j > le - 1; j--)
								{
									tmp = *(lplbldbn + j - le);
									*(lplbldbn + j - le) = tmp + 1;
								}

								ml++;
								ln++;

							}
							else if (lm == 127 || m == 0 || i > 126)
							{
								break;
							}

						}
						else											//���������ߵ�bin����Ϲ������������bin
						{
							//i=i+1;
							for (m = i; m < 127; m++)
							{
								ordlf[m] = ordlf[m + 1];
							}
							if (ordlf[i] == ordlf[127])
							{
								break;
							}
							else
							{
								i = i - 1;
							}
						}
					}
				}

			}
			else if (newPre == true)
			{
				j = 0;
				for (i = 0; i < 256; )
				{
					while (i < 256 && j < pn  && order[j] < 256 && *(lpblkval + order[j]) < 256)           //����pn�������bin�ĸ߶ȴӰ����߸���ordlf
					{
						ordlf[i] = order[j];
						i = i + 1;
						j = j + 1;
					}
					j = j + 1;
					if (j > pn - 1 || i > 255)
						break;
				}

				zi = 0;

				for (; zi < 256 && ml < t + 1; zi++)
				{
					i = ln;
					fi = ordlf[i];
					if (*(lpblkhis + fi) > 0 && *(lpblkval + fi) < 256)                    //������ҷǿ�bin
					{
						lm = *(lpblkval + fi);
						tm = lm;
						m = 1;
						if (lm == 0)
						{
							rm = *(lpblkval + fi + 1);
							m = 0;                                        //ֻ�ܺ��ұ߽��

							for (j = i + 1; j < 255; j++)
							{
								if (*(lpblkval + ordlf[j]) == tm)
								{
									rj = j;
									break;
								}
							}
						}
						else if (lm == 255)
						{
							tm = *(lpblkval + fi - 1);
							m = 2;                                          //ֻ�ܺ���߽��

							for (j = i + 1; j < 255; j++)
							{
								if (*(lpblkval + ordlf[j]) == rm)
								{
									rj = j;
									break;
								}
							}
						}
						else
						{
							tm = *(lpblkval + fi - 1);
							rm = *(lpblkval + fi + 1);

							for (j = i + 1; j < 255; j++)                               //���ұȽ��ĸ���С�����С��m=2���ұ�С��m=0
							{
								if (*(lpblkval + ordlf[j]) == tm)
								{
									m = 2;
									break;
								}
								else if (*(lpblkval + ordlf[j]) == rm)
								{
									m = 0;
									rj = j;
									break;
								}
							}
						}

						if (m == 2)                                                  //������߽�С
						{
							for (j = i + 1; j < 255 && tm != lm; j++)
							{
								if (*(lpblkval + ordlf[j]) == tm)
								{
									for (m = j; m < 255; m++)
									{
										ordlf[m] = ordlf[m + 1];
									}
									break;
								}
							}

							tmp = tm;
							tm = lm;
							lm = tmp;
							mi = fi - 1;

							if (lm > -1 && lm < 255 && tm < 256)
							{

								for (row = 0; row < ImageHeight; row++)
									for (column = 0; column < ImageWidth; column++)
									{
										tmp = *(wp + row * ImageWidth + column);
										if (tmp == lm)
										{
											*(lphismap + testbuf_len) = 1;
											testbuf_len++;
										}
										else if (tmp == lm + 1)
										{
											*(lphismap + testbuf_len) = 0;                    //lphismap����
											testbuf_len++;
										}
										if (tmp < lm + 1)
											*(wp + row * ImageWidth + column) = tmp + 1;
									}

								*(lpprehist + ml) = lm;                                       //lpprehist����
								*(lplbldbn + ml - le) = lm + 1;

								rn = *(lpblkhis + mi);
								tn = *(lpblkhis + mi + 1);
								*(lpblkhis + mi + 1) = rn + tn;
								*(lpblkhis + mi) = 0;
								pct = pct + rn + tn;

								for (j = mi; j > ln + 1; j--)
								{
									tmp = *(lpblkval + j - 1);
									*(lpblkval + j) = tmp + 1;
									tn = *(lpblkhis + j - 1);
									*(lpblkhis + j) = tn;

									for (k = ln + 1; k < 255; k++)
									{
										if (*(lpblkval + ordlf[k]) == tmp)
										{
											ordlf[k] = ordlf[k] + 1;
										}
									}

								}

								for (j = ln + 1; j > 0; j--)
								{
									*(lpblkhis + j) = 0;
								}

								for (j = ml - 1; j > le - 1; j--)
								{
									tmp = *(lplbldbn + j - le);
									*(lplbldbn + j - le) = tmp + 1;
								}

								ml++;

								for (k = ln + 1; k < 255; k++)
								{
									if (*(lpblkhis + ordlf[k]) > *(lpblkhis + mi + 1))
									{
										break;
									}
								}

								for (j = ln; j < k - 1; j++)
								{
									ordlf[j] = ordlf[j + 1];
								}
								ordlf[k - 1] = mi + 1;

							}
						}
						else if (m == 0)                                             //�����ұ߽�С
						{
							mi = fi;

							if (lm > -1 && lm < 255 && rm < 256)
							{

								for (row = 0; row < ImageHeight; row++)
									for (column = 0; column < ImageWidth; column++)
									{
										tmp = *(wp + row * ImageWidth + column);
										if (tmp == lm)
										{
											*(lphismap + testbuf_len) = 1;
											testbuf_len++;
										}
										else if (tmp == lm + 1)
										{
											*(lphismap + testbuf_len) = 0;                   //lphismap����
											testbuf_len++;
										}
										if (tmp < lm + 1)
											*(wp + row * ImageWidth + column) = tmp + 1;           //λ�ơ�bin���
									}

								*(lpprehist + ml) = lm;                                      //lpprehist����
								*(lplbldbn + ml - le) = lm + 1;

								rn = *(lpblkhis + mi);
								tn = *(lpblkhis + mi + 1);
								*(lpblkhis + mi + 1) = rn + tn;
								*(lpblkhis + mi) = 0;
								pct = pct + rn + tn;

								/*lpblkval��lpblkhis����*/
								for (j = mi; j > ln + 1; j--)
								{
									tmp = *(lpblkval + j - 1);
									*(lpblkval + j) = tmp + 1;
									tn = *(lpblkhis + j - 1);
									*(lpblkhis + j) = tn;

									for (k = ln + 1; k < 255; k++)
									{
										if (*(lpblkval + ordlf[k]) == tmp)
										{
											ordlf[k] = ordlf[k] + 1;
										}
									}
								}

								for (j = 1 + ln; j > 0; j--)
								{
									*(lpblkhis + j) = 0;
								}

								for (j = ml - 1; j > le - 1; j--)
								{
									tmp = *(lplbldbn + j - le);
									*(lplbldbn + j - le) = tmp + 1;
								}

								ml++;

								for (k = ln + 1; k < 255; k++)
								{
									if (*(lpblkhis + ordlf[k]) > *(lpblkhis + mi + 1))
									{
										break;
									}
								}

								if (ordlf[i + 1] == mi + 1)
								{
									for (j = ln + 1; j < k - 1; j++)
									{
										ordlf[j] = ordlf[j + 1];
									}
									ordlf[k - 1] = mi + 1;
								}
								else
								{
									for (j = rj; j < k - 1; j++)
									{
										ordlf[j] = ordlf[j + 1];
									}
									ordlf[k - 1] = mi + 1;
								}

								ln++;

							}
							else if (lm == 255 || mi == 0 || zi > 254)
							{
								break;
							}

						}
						else
						{
							//i=i+1;
							for (m = i; m < 255; m++)
							{
								ordlf[m] = ordlf[m + 1];
							}
							if (ordlf[i] == ordlf[255])
							{
								break;
							}
							else
							{
								i = i - 1;
							}
						}
					}
				}
			}
		}
	}
	
	
	/*
	int kongbin[330];
	for (int i = 0; i <= 330; i++)
	{
		kongbin[i] = 0;
	}
	for (int i = 0; i <= (2 * (t + 1) + 1); i++)
	{
		kongbin[i] = *(lpprehist + i);
	}
	*/

	mr=0;
	
	if (leftPreProcess == false)
	{
		/*�ұ߿�bin��Ԥ����*/
		for(i=255; i>127; i--)
		{
			ordrg[255-i]=-1;
			if(hist[i]==0 && mr<t+1)
			{
				lm=(unsigned char)i;
				
				for(row=0;row<ImageHeight;row++)
					for(column=0;column<ImageWidth;column++)
					{
						tmp=*(wp+row*ImageWidth+column);
						if(tmp>lm)
							*(wp+row*ImageWidth+column)=tmp-1;
					}

				*(lpprehist+t+1+mr)=i;                             //���ҿ�bin��¼֮�����t+1
				mr++;

				for(j=pn;j>0;j--)
				{
					tm=*(lpblkval+j);
					if(tm>lm)
						*(lpblkval+j)=tm-1;
				}


			}
		}

		re=mr;
		ln=0;
	
		/* ֱ��ͼ�ұߵ�Ԥ����bin merging�����bin ���������̴�ͬС��*/
		if(mr<t+1)
		{	
			if (newPre == false)
			{
				j = 0;
				for (i = 0; i < 128; )
				{
					while (i < 128 && order[j]>127 && j < pn)
					{
						ordrg[i] = order[j];
						i = i + 1;
						j = j + 1;
					}
					j = j + 1;
					if (j > pn - 1 || i > 127)
					{
						break;
					}
				}


				for (i = ln; i < 128 && mr < t + 1; i++)
				{
					if (*(lpblkhis + ordrg[i]) > 0)
					{
						lm = *(lpblkval + ordrg[i]);
						tm = lm;
						if (lm == 255)
						{
							tm = *(lpblkval + ordrg[i] - 1);
						}
						else if (*(lpblkhis + ordrg[i] + 1) > 0 && lm > 127)
						{
							tm = *(lpblkval + ordrg[i] + 1);

							m = 1;

							for (j = i + 1; j < 127; j++)
							{
								if (*(lpblkval + ordrg[j]) == tm)
								{
									m = 0;
									break;
								}
							}
							if (m == 1 && lm > 128)
								tm = *(lpblkval + ordrg[i] - 1);
						}
						else if (lm > 128)
							tm = *(lpblkval + ordrg[i] - 1);

						m = 1;

						for (j = i + 1; j < 127 && lm != tm; j++)
						{
							if (*(lpblkval + ordrg[j]) == tm)
							{
								for (m = j; m < (t + 1 - re) * 3 - 1 && m < 127; m++)
								{
									ordrg[m] = ordrg[m + 1];
								}
								m = 0;
								break;
							}
						}
						if (m == 0)
						{
							if (lm < tm)
							{
								tmp = tm;
								tm = lm;
								lm = tmp;
								m = ordrg[i] + 1;
							}
							else
							{
								m = ordrg[i];
							}

							if (lm > 128 && tm > 127)
							{

								for (row = 0; row < ImageHeight; row++)
									for (column = 0; column < ImageWidth; column++)
									{
										tmp = *(wp + row * ImageWidth + column);
										if (testbuf_len < ImageHeight*ImageWidth * 2)
										{
											if (tmp == lm)
											{
												*(lphismap + testbuf_len) = 1;
												testbuf_len++;
											}
											else if (tmp == lm - 1)
											{
												*(lphismap + testbuf_len) = 0;
												testbuf_len++;
											}
											if (tmp > lm - 1)
												*(wp + row * ImageWidth + column) = tmp - 1;
										}
										else
											break;
									}

								*(lpprehist + t + 1 + mr) = lm;
								*(lplbldbn + ml - le + mr - re) = lm - 1;

								rn = *(lpblkhis + m);
								tn = *(lpblkhis + m - 1);
								*(lpblkhis + m - 1) = rn + tn;
								*(lpblkhis + m) = 0;
								pct = pct + rn + tn;

								for (j = m; j < pn - ln; j++)
								{
									tmp = *(lpblkval + j + 1);
									*(lpblkval + j) = tmp - 1;
									tn = *(lpblkhis + j + 1);
									*(lpblkhis + j) = tn;

									for (k = ln + 1; k < 127; k++)
									{
										if (*(lpblkval + ordrg[k]) == tmp)
										{
											ordrg[k] = ordrg[k] - 1;
										}
									}
								}

								for (j = pn - ln; j < pn + 1; j++)
								{
									*(lpblkhis + j) = 0;
								}

								for (j = mr - 1; j > re - 1; j--)
								{
									tmp = *(lplbldbn + ml - le + j - re);
									*(lplbldbn + ml - le + j - re) = tmp - 1;
								}

								mr++;
								ln++;

							}
							else if (lm == 128 || m == 0 || i > 126)
							{
								break;
							}
						}
						else
						{
							for (m = i; m < 127; m++)
							{
								ordrg[m] = ordrg[m + 1];
							}
							if (ordrg[i] == ordrg[127])
							{
								break;
							}
							else
							{
								i = i - 1;
							}
						}
					}
				}
			}
			else if (newPre == true)
			{
				j = 0;
				for (i = 0; i < 128; )
				{
					while (i < 128 && order[j]>127 && j < pn)
					{
						ordrg[i] = order[j];
						i = i + 1;
						j = j + 1;
					}
					j = j + 1;
					if (j > pn - 1 || i > 127)
					{
						break;
					}
				}


				zi = 0;

				for (; zi < 128 && mr < t + 1; zi++)
				{
					i = ln;
					fi = ordrg[i];

					if (*(lpblkhis + fi) > 0)
					{
						lm = *(lpblkval + fi);
						tm = lm;
						if (lm == 255)
						{
							tm = *(lpblkval + fi - 1);
							m = 0;

							for (j = i + 1; j < 127; j++)
							{
								if (*(lpblkval + ordrg[j]) == tm)
								{
									rj = j;
									break;
								}
							}
						}
						else if (lm == 128)
						{
							rm = *(lpblkval + fi + 1);
							m = 2;

							for (j = i + 1; j < 127; j++)
							{
								if (*(lpblkval + ordrg[j]) == rm)
								{
									rj = j;
									break;
								}
							}
						}
						else
						{
							tm = *(lpblkval + fi - 1);
							rm = *(lpblkval + fi + 1);

							for (j = i + 1; j < 127; j++)
							{
								if (*(lpblkval + ordrg[j]) == tm)
								{
									m = 0;
									rj = j;
									break;
								}
								else if (*(lpblkval + ordrg[j]) == rm)
								{
									m = 2;
									rj = j;
									break;
								}
							}
						}

						if (m == 2)
						{
							for (j = i + 1; j < 127 && tm != rm; j++)
							{
								if (*(lpblkval + ordrg[j]) == rm)
								{
									for (m = j; m < 127; m++)
									{
										ordrg[m] = ordrg[m + 1];
									}
									break;
								}
							}

							tm = lm;
							lm = rm;
							mi = fi;

							if (lm < 256 && lm>128 && tm > 127)
							{
								for (row = 0; row < ImageHeight; row++)
									for (column = 0; column < ImageWidth; column++)
									{
										tmp = *(wp + row * ImageWidth + column);
										if (testbuf_len < ImageHeight*ImageWidth * 2)
										{
											if (tmp == lm)
											{
												*(lphismap + testbuf_len) = 1;
												testbuf_len++;
											}
											else if (tmp == lm - 1)
											{
												*(lphismap + testbuf_len) = 0;
												testbuf_len++;
											}
											if (tmp > lm - 1)
												*(wp + row * ImageWidth + column) = tmp - 1;
										}
										else
											break;
									}

								*(lpprehist + t + 1 + mr) = lm;
								*(lplbldbn + ml - le + mr - re) = lm - 1;

								rn = *(lpblkhis + mi);
								tn = *(lpblkhis + mi + 1);
								*(lpblkhis + mi) = rn + tn;
								*(lpblkhis + mi + 1) = 0;
								pct = pct + rn + tn;

								for (j = mi + 1; j < pn - ln; j++)
								{
									tmp = *(lpblkval + j + 1);
									*(lpblkval + j) = tmp - 1;
									tn = *(lpblkhis + j + 1);
									*(lpblkhis + j) = tn;

									for (k = ln + 1; k < 127; k++)
									{
										if (*(lpblkval + ordrg[k]) == tmp)
										{
											ordrg[k] = ordrg[k] - 1;
										}
									}
								}

								for (j = pn - ln; j < pn + 1; j++)
								{
									*(lpblkhis + j) = 0;
								}

								for (j = mr - 1; j > re - 1; j--)
								{
									tmp = *(lplbldbn + ml - le + j - re);
									*(lplbldbn + ml - le + j - re) = tmp - 1;
								}

								mr++;

								for (k = ln + 1; k < 127; k++)
								{
									if (*(lpblkhis + ordrg[k]) > *(lpblkhis + mi))
									{
										break;
									}
								}

								for (j = ln; j < k - 1; j++)
								{
									ordrg[j] = ordrg[j + 1];
								}
								ordrg[k - 1] = mi;

							}
						}
						else if (m == 0)
						{
							mi = fi - 1;

							if (lm < 256 && lm>128 && tm > 127)
							{
								for (row = 0; row < ImageHeight; row++)
									for (column = 0; column < ImageWidth; column++)
									{
										tmp = *(wp + row * ImageWidth + column);
										if (testbuf_len < ImageHeight*ImageWidth * 2)
										{
											if (tmp == lm)
											{
												*(lphismap + testbuf_len) = 1;
												testbuf_len++;
											}
											else if (tmp == lm - 1)
											{
												*(lphismap + testbuf_len) = 0;
												testbuf_len++;
											}
											if (tmp > lm - 1)
												*(wp + row * ImageWidth + column) = tmp - 1;
										}
										else
											break;
									}

								*(lpprehist + t + 1 + mr) = lm;
								*(lplbldbn + ml - le + mr - re) = lm - 1;

								rn = *(lpblkhis + mi);
								tn = *(lpblkhis + mi + 1);
								*(lpblkhis + mi) = rn + tn;
								*(lpblkhis + mi + 1) = 0;
								pct = pct + rn + tn;

								for (j = mi + 1; j < pn - ln; j++)
								{
									tmp = *(lpblkval + j + 1);
									*(lpblkval + j) = tmp - 1;
									tn = *(lpblkhis + j + 1);
									*(lpblkhis + j) = tn;

									for (k = ln + 1; k < 127; k++)
									{
										if (*(lpblkval + ordrg[k]) == tmp)
										{
											ordrg[k] = ordrg[k] - 1;
										}
									}
								}

								for (j = pn - ln; j < pn + 1; j++)
								{
									*(lpblkhis + j) = 0;
								}

								for (j = mr - 1; j > re - 1; j--)
								{
									tmp = *(lplbldbn + ml - le + j - re);
									*(lplbldbn + ml - le + j - re) = tmp - 1;
								}

								mr++;

								for (k = ln + 1; k < 127; k++)
								{
									if (*(lpblkhis + ordrg[k]) > *(lpblkhis + mi))
									{
										break;
									}
								}

								if (ordrg[i + 1] == mi)
								{
									for (j = ln + 1; j < k - 1; j++)
									{
										ordrg[j] = ordrg[j + 1];
									}
									ordrg[k - 1] = mi;
								}
								else
								{
									for (j = rj; j < k - 1; j++)
									{
										ordrg[j] = ordrg[j + 1];
									}
									ordrg[k - 1] = mi;
								}

								ln++;

							}
							else if (lm == 128 || m == 0 || i > 126)
							{
								break;
							}

						}
						else
						{
							for (m = i; m < 127; m++)
							{
								ordrg[m] = ordrg[m + 1];
							}
							if (ordrg[i] == ordrg[127])
							{
								break;
							}
							else
							{
								i = i - 1;
							}
						}
					}
				}
			}

		}

		/*����Ԥ����pairsȷ�ϣ�met������ȷ��ֵ����t����met���������ܴ����Ԥ�����bin�����������Ԥ����ȷ���������S��*/
		if (ml < t + 1 || mr < t + 1)
		//if (ml < t + 2 || mr < t + 2)
		{
			ft = t;
			t = ml - 1;
			if (mr - 1 < t)
				t = mr - 1;

			memcpy(lpwmimg, lporigimg, m_ImageSize);
			g = 1024 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
			ln = 0;
			pct = 0;
			pn = 0;

			wp = lpwmimg + g;
			lp = lporigimg + g;
			ml = mr = ImageHeight * ImageWidth;
			la = ra = 0;
			testbuf_len = 0;

			for (i = 0; i < 256; i++)
			{
				hist[i] = 0;
			}

			g = ImageHeight * ImageWidth;

			if (hhismap)
			{
				GlobalFree(hhismap);
				hhismap = NULL;
			}
			hhismap = GlobalAlloc(GMEM_FIXED, g * 2);
			lphismap = (unsigned char *)hhismap;


			if (hlbldbn)
			{
				GlobalFree(hlbldbn);
				hlbldbn = NULL;
			}
			hlbldbn = GlobalAlloc(GMEM_FIXED, 256);
			lplbldbn = (unsigned char *)hlbldbn;


			for (i = 0; i < 256; i++)
				*(lplbldbn + i) = -1;

			if (hblkhis)
			{
				GlobalFree(hblkhis);
				hblkhis = NULL;
			}
			hblkhis = GlobalAlloc(GMEM_FIXED, ImageHeight*ImageWidth * sizeof(unsigned int));
			lpblkhis = (unsigned int *)hblkhis;

			if (hprehist)
			{
				GlobalFree(hprehist);
				hprehist = NULL;
			}
			hprehist = GlobalAlloc(GMEM_FIXED, 65 * 2 * sizeof(unsigned char));
			lpprehist = (unsigned char *)hprehist;

			if (hblkval)
			{
				GlobalFree(hblkval);
				hblkval = NULL;
			}
			hblkval = GlobalAlloc(GMEM_FIXED, ImageHeight*ImageWidth * 4);
			lpblkval = (int *)hblkval;

			for (i = 0; i < ImageHeight; i++)
				for (j = 0; j < ImageWidth; j++)
				{
					*(lpblkhis + i * ImageWidth + j) = 0;
					*(lpblkval + i * ImageWidth + j) = -1;
				}

			for (i = 0; i < ImageHeight; i++)
			{
				for (j = 0; j < ImageWidth; j++)
				{
					lm = *(lp + i * ImageWidth + j);
					flg = 0;

					for (k = 0; k < pn + 1; k++)
					{
						if (lm == *(lpblkval + k))
						{
							tn = *(lpblkhis + k);
							*(lpblkhis + k) = tn + 1;
							flg = 1;
						}
					}

					if (flg == 0)
					{
						pn++;
						*(lpblkhis + pn) = 1;
						*(lpblkval + pn) = lm;
					}

				}
			}

			for (i = 0; i < 256; i++)
				for (k = 1; k < pn + 1; k++)
				{
					lm = *(lpblkval + k);
					if (lm == i)
					{
						rn = *(lpblkhis + k);
						hist[i] = rn;
					}
				}


			for (i = 0; i < pn; i++)
				for (k = 1; k < pn - i; k++)
				{
					lm = *(lpblkval + k);
					tm = *(lpblkval + k + 1);
					if (lm > tm)
					{
						*(lpblkval + k) = tm;
						*(lpblkval + k + 1) = lm;
						rn = *(lpblkhis + k);
						tn = *(lpblkhis + k + 1);
						*(lpblkhis + k) = tn;
						*(lpblkhis + k + 1) = rn;

					}
				}


			for (i = 0; i < 256; i++)
			{
				order[i] = i;
				flag[i] = 0;
			}


			for (i = 0; i < pn; i++)
			{
				a = ImageHeight * ImageWidth;
				for (k = pn; k > 0; k--)
				{
					rn = *(lpblkhis + k);
					if (rn < a && flag[k - 1] == 0)
					{
						order[i] = k;
						a = rn + 1;
					}
				}
				flag[order[i] - 1] = 1;
			}

			for (i = 0; i < 256; i++)
			{
				phis[i] = 0;
			}

			ml = 0;

			for (i = 0; i < 128; i++)
			{
				ordlf[i] = -1;
				if (hist[i] == 0 && ml < t + 1)
				{
					lm = (unsigned char)i;

					for (row = 0; row < ImageHeight; row++)
						for (column = 0; column < ImageWidth; column++)
						{
							tmp = *(wp + row * ImageWidth + column);
							if (tmp < lm)
								*(wp + row * ImageWidth + column) = tmp + 1;
						}

					*(lpprehist + ml) = i;
					ml++;

					for (j = 1; j < pn + 1; j++)
					{
						tm = *(lpblkval + j);
						if (tm < lm)
							*(lpblkval + j) = tm + 1;
					}

				}
			}

			le = ml;
			ln = 0;

			if (ml < t + 1)
			{
				j = 0;
				for (i = 0; i < 128; )
				{
					while (order[j] < 128 && i < 128 && j < pn)
					{
						ordlf[i] = order[j];
						i = i + 1;
						j = j + 1;
					}
					j = j + 1;
					if (j > pn - 1 || i > 127)
						break;
				}

				for (i = ln; i < 128 && ml < t + 1; i++)
				{
					if (*(lpblkhis + ordlf[i]) > 0)
					{
						lm = *(lpblkval + ordlf[i]);
						tm = lm;
						if (lm == 0)
						{
							tm = *(lpblkval + ordlf[i] + 1);
						}
						else if (*(lpblkhis + ordlf[i] - 1) > 0 && lm < 128)
						{
							tm = *(lpblkval + ordlf[i] - 1);

							m = 1;

							for (j = i + 1; j < (ft + 1 - le) * 3 && j < 127; j++)
							{
								if (*(lpblkval + ordlf[j]) == tm)
								{
									m = 0;
									break;
								}
							}
							if (m == 1 && lm < 127)
								tm = *(lpblkval + ordlf[i] + 1);
						}
						else if (lm < 127)
							tm = *(lpblkval + ordlf[i] + 1);

						m = 1;

						for (j = i + 1; j < 127 && tm != lm; j++)
						{
							if (*(lpblkval + ordlf[j]) == tm)
							{
								for (m = j; m < 127; m++)
								{
									ordlf[m] = ordlf[m + 1];
								}
								m = 0;
								break;
							}
						}
						if (m == 0)
						{
							if (lm > tm)
							{
								tmp = tm;
								tm = lm;
								lm = tmp;
								m = ordlf[i] - 1;
							}
							else
							{
								m = ordlf[i];
							}

							if (lm > -1 && lm < 127 && tm < 128)
							{

								for (row = 0; row < ImageHeight; row++)
									for (column = 0; column < ImageWidth; column++)
									{
										tmp = *(wp + row * ImageWidth + column);
										if (tmp == lm)
										{
											*(lphismap + testbuf_len) = 1;
											testbuf_len++;
										}
										else if (tmp == lm + 1)
										{
											*(lphismap + testbuf_len) = 0;
											testbuf_len++;
										}
										if (tmp < lm + 1)
											*(wp + row * ImageWidth + column) = tmp + 1;
									}

								*(lpprehist + ml) = lm;
								*(lplbldbn + ml - le) = lm + 1;

								rn = *(lpblkhis + m);
								tn = *(lpblkhis + m + 1);
								*(lpblkhis + m + 1) = rn + tn;
								*(lpblkhis + m) = 0;
								pct = pct + rn + tn;

								for (j = m; j > ln + 1; j--)
								{
									tmp = *(lpblkval + j - 1);
									*(lpblkval + j) = tmp + 1;
									tn = *(lpblkhis + j - 1);
									*(lpblkhis + j) = tn;

									for (k = ln + 1; k < 127; k++)
									{
										if (*(lpblkval + ordlf[k]) == tmp)
										{
											ordlf[k] = ordlf[k] + 1;
										}
									}
								}

								for (j = 1 + ln; j > 0; j--)
								{
									*(lpblkhis + j) = 0;
								}

								for (j = ml - 1; j > le - 1; j--)
								{
									tmp = *(lplbldbn + j - le);
									*(lplbldbn + j - le) = tmp + 1;
								}

								ml++;
								ln++;

							}
							else if (lm == 127 || m == 0 || i > 126)
							{
								break;
							}

						}
						else
						{
							//i=i+1;
							for (m = i; m < 127; m++)
							{
								ordlf[m] = ordlf[m + 1];
							}
							if (ordlf[i] == ordlf[127])
							{
								break;
							}
							else
							{
								i = i - 1;
							}
						}
					}
				}

			}

			mr = 0;

			for (i = 255; i > 127; i--)
			{
				ordrg[255 - i] = -1;
				if (hist[i] == 0 && mr < t + 1)
				{
					lm = (unsigned char)i;

					for (row = 0; row < ImageHeight; row++)
						for (column = 0; column < ImageWidth; column++)
						{
							tmp = *(wp + row * ImageWidth + column);
							if (tmp > lm)
								*(wp + row * ImageWidth + column) = tmp - 1;
						}

					*(lpprehist + t + 1 + mr) = i;
					mr++;

					for (j = pn; j > 0; j--)
					{
						tm = *(lpblkval + j);
						if (tm > lm)
							*(lpblkval + j) = tm - 1;
					}


				}
			}

			re = mr;
			ln = 0;

			if (mr < t + 1)
			{
				j = 0;
				for (i = 0; i < 128; )
				{
					while (i < 128 && order[j]>127 && j < pn)
					{
						ordrg[i] = order[j];
						i = i + 1;
						j = j + 1;
					}
					j = j + 1;
					if (j > pn - 1 || i > 127)
					{
						break;
					}
				}


				for (i = ln; i < 128 && mr < t + 1; i++)
				{
					if (*(lpblkhis + ordrg[i]) > 0)
					{
						lm = *(lpblkval + ordrg[i]);
						tm = lm;
						if (lm == 255)
						{
							tm = *(lpblkval + ordrg[i] - 1);
						}
						else if (*(lpblkhis + ordrg[i] + 1) > 0 && lm > 127)
						{
							tm = *(lpblkval + ordrg[i] + 1);

							m = 1;

							for (j = i + 1; j < 127; j++)
							{
								if (*(lpblkval + ordrg[j]) == tm)
								{
									m = 0;
									break;
								}
							}
							if (m == 1 && lm > 128)
								tm = *(lpblkval + ordrg[i] - 1);
						}
						else if (lm > 128)
							tm = *(lpblkval + ordrg[i] - 1);

						m = 1;

						for (j = i + 1; j < 127 && lm != tm; j++)
						{
							if (*(lpblkval + ordrg[j]) == tm)
							{
								for (m = j; m < 127; m++)
								{
									ordrg[m] = ordrg[m + 1];
								}
								m = 0;
								break;
							}
						}
						if (m == 0)
						{
							if (lm < tm)
							{
								tmp = tm;
								tm = lm;
								lm = tmp;
								m = ordrg[i] + 1;
							}
							else
							{
								m = ordrg[i];
							}

							if (lm > 128 && tm > 127)
							{

								for (row = 0; row < ImageHeight; row++)
									for (column = 0; column < ImageWidth; column++)
									{
										tmp = *(wp + row * ImageWidth + column);
										if (testbuf_len < ImageHeight*ImageWidth * 2)
										{
											if (tmp == lm)
											{
												*(lphismap + testbuf_len) = 1;
												testbuf_len++;
											}
											else if (tmp == lm - 1)
											{
												*(lphismap + testbuf_len) = 0;
												testbuf_len++;
											}
											if (tmp > lm - 1)
												*(wp + row * ImageWidth + column) = tmp - 1;
										}
										else
											break;
									}

								*(lpprehist + t + 1 + mr) = lm;
								*(lplbldbn + ml - le + mr - re) = lm - 1;

								rn = *(lpblkhis + m);
								tn = *(lpblkhis + m - 1);
								*(lpblkhis + m - 1) = rn + tn;
								*(lpblkhis + m) = 0;
								pct = pct + rn + tn;

								for (j = m; j < pn - ln; j++)
								{
									tmp = *(lpblkval + j + 1);
									*(lpblkval + j) = tmp - 1;
									tn = *(lpblkhis + j + 1);
									*(lpblkhis + j) = tn;

									for (k = ln + 1; k < 127; k++)
									{
										if (*(lpblkval + ordrg[k]) == tmp)
										{
											ordrg[k] = ordrg[k] - 1;
										}
									}
								}

								for (j = pn - ln; j < pn + 1; j++)
								{
									*(lpblkhis + j) = 0;
								}

								for (j = mr - 1; j > re - 1; j--)
								{
									tmp = *(lplbldbn + ml - le + j - re);
									*(lplbldbn + ml - le + j - re) = tmp - 1;
								}

								mr++;
								ln++;

							}
							else if (lm == 128 || m == 0 || i > 126)
							{
								break;
							}
						}
						else
						{
							for (m = i; m < 127; m++)
							{
								ordrg[m] = ordrg[m + 1];
							}
							if (ordrg[i] == ordrg[127])
							{
								break;
							}
							else
							{
								i = i - 1;
							}
						}
					}
				}

			}

			met = t;

		}
	}
	else
	{
		re = mr;
		for (int i = t + 1; i < 2 * (t + 1); i++)
		{
			*(lpprehist + i) = 0;
		}
	}


	lbn=ml+mr-le-re;                       //ͨ����ϴ���Ŀ�bin����

	*(lpprehist+2*(t+1))=le;               //lpprehist�ֱ����ߺ��ұߵ�ԭʼ��bin����
	*(lpprehist+2*(t+1)+1)=re;

	zn=0;

	tstbfloc=0;
	lpoh=0;

	if(pct>0)                       //����bin��Ϲ���
	{
		if(pct%8==0)
			g=pct/8;
		else
		{
			g=pct/8+1;
			
			for(column=pct;column<g*8;column++)
			{
				*(lphismap+column)=0;
			}
		}


		if(hlocmap)
		{ 
			GlobalFree(hlocmap);  
			hlocmap=NULL;
		}
		hlocmap = GlobalAlloc(GMEM_FIXED,g);
		locmap=(unsigned char *)hlocmap;
	
		/*����¼bin���λ�õ�map�ŵ�locmap*/
		for(column=0;column<g*8;column++)
		{
			if(column%8==0)	lm=0;
			
			tmp=*(lphismap+column);
			if(tmp==1)
				lm=(lm<<1)+1;
			else
				lm=(lm<<1)+0;

			if(column%8==7)	*(locmap+column/8)=lm;
		}

		zn=testbuf_len;                              //zn��lohismap�ĳ���
		testbuf_len=g;                               //testbuf_len��locmap�ĳ���

		lml=testbuf_len;							 //lml��locap�ĳ���
		g=0;

		if (onlyPreProcess == false)
		{
			for(m=0; m<t+1; m++)
			{
				el = derem(lpwmimg, lporigimg, m, met, testbuf_len, zn, locmap, lpprehist);                  //����Ƕ��
				g=g+el;
				memcpy(lporigimg,lpwmimg,m_ImageSize);
			}
		}
		else
		{
			isMerge = true;
		}
		

	}
	else
	{
		g=0;

		if(hhismap)
		{ 
			GlobalFree(hhismap);
			hhismap=NULL;  
		}
		
		/*
		if(hlbldbn)
		{ 
			GlobalFree(hlbldbn);  
			hlbldbn=NULL;
		}*/

		if (onlyPreProcess == false)
		{
			for(m=0; m<t+1; m++)
			{
				el = derem(lpwmimg, lporigimg, m, met, testbuf_len, zn, locmap, lpprehist);
				g=g+el;
				memcpy(lporigimg,lpwmimg,m_ImageSize);

			}
		}
		else
		{
			isMerge = false;
		}

	}
	
	return g;

}

long CDeprejbgDoc::derem(unsigned char *lpwmimg, unsigned char *lporigimg, int t, int met, long testbuf_len, long zn, unsigned char *locmap, unsigned char *lpprehist)
{
	//FILE *forg=fopen("D:\\orgimap.txt","wb");
	unsigned short int column,row;
	unsigned char tmp, lmp;
	int i,j,k,m,x,y,et;
	long *lplen, bflen, ct, g, el;
	double di,h;
	struct jbg85_enc_state s;

	g=1024+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);

	wp=lpwmimg+g;
	lp=lporigimg+g;

	ct=0;

	//threshold=12;

	
	if(t==met)                                 //����ǵ����ڶ���Ƕ�룬��ʱtΪ������һ��Ƕ��ѭ��
	{

		if(hembovh)
		{ 
			GlobalFree(hembovh);  
			hembovh=NULL;
		}

		if(met==0)
		{
			ohpn=1;
			if(zn==0) 
			{
				testbuf_len=0;
				tstbfloc=testbuf_len+16;                                     //�������16�������14��Ҫ�ں�����ȡʱ����14�ĳ���
			}
			else
				tstbfloc=testbuf_len+32;                                     //�������32�������28��Ҫ�ں�����ȡʱ����28�ĳ���

			hembovh = GlobalAlloc(GMEM_FIXED, (tstbfloc+4+sizeof(long)*2)*8);
		}			
		else if(ohpn<2 && met>0)
		{
			hembovh = GlobalAlloc(GMEM_FIXED, (tstbfloc+6+sizeof(long)*2)*8);
		}
		else
			hembovh = GlobalAlloc(GMEM_FIXED, (tstbfloc+6+sizeof(long)*3)*8);

		lpembovh=(unsigned char *)hembovh;

		unsigned char shiftvalue, *flen, fl;		//Define the shifted value 
		int shiftnumber=0;				//Define the number of shift 


		fl=met;

		//��¼S��ֵ
		for(k=0; k<8; k++)
		{
			*(lpembovh+k)=0;
			shiftnumber=k;
			shiftvalue=(fl>>shiftnumber)&1;
			*(lpembovh+k)=shiftvalue;
		}
		
		//��16�����ص�LSB
		row=ImageHeight-1;
		for(column=0;column<16;column++)
		{
			*(lpembovh+8+column)=0;
			tmp=*(wp+row*ImageWidth+column);
			*(lpembovh+8+column)=tmp-(tmp/2)*2;
		}
		
		fl=ohpn;

		//��¼ohpn
		for(k=0; k<8; k++)
		{
			*(lpembovh+24+k)=0;
			shiftnumber=k;
			shiftvalue=(fl>>shiftnumber)&1;
			*(lpembovh+24+k)=shiftvalue;
		}
		/*����met��lsb��ohpn��¼��4B*/

		if(met>0)                        //������Ҫ��¼����Ϣ
		{
			//��¼ǰһ��Ƕ���õ�����binֵ
			fl=lb;

			for(k=0; k<8; k++)
			{
				*(lpembovh+32+k)=0;
				shiftnumber=k;
				shiftvalue=(fl>>shiftnumber)&1;
				*(lpembovh+32+k)=shiftvalue;
			}

			fl=rb;

			for(k=0; k<8; k++)
			{
				*(lpembovh+40+k)=0;
				shiftnumber=k;
				shiftvalue=(fl>>shiftnumber)&1;
				*(lpembovh+40+k)=shiftvalue;
			}

			//��¼Ԥ����bookkeeping�ĳ���
			bflen=(long)testbuf_len;
			lplen=&bflen;

			
			for(i=0; i<sizeof(long); i++)
			{
				flen=(unsigned char *)lplen+i;
				fl=*flen;
				for(k=0; k<8; k++)
				{
					*(lpembovh+48+8*i+k)=0;
					shiftnumber=k;
					shiftvalue=(fl>>shiftnumber)&1;
					*(lpembovh+48+8*i+k)=shiftvalue;
				}
			}

			lplen=&zn;
			for(i=0; i<sizeof(long); i++)
			{
				flen=(unsigned char *)lplen+i;
				fl=*flen;
				for(k=0; k<8; k++)
				{
					*(lpembovh+(6+sizeof(long))*8+8*i+k)=0;
					shiftnumber=k;
					shiftvalue=(fl>>shiftnumber)&1;
					*(lpembovh+(6+sizeof(long))*8+8*i+k)=shiftvalue;
				}
			}

			if(ohpn>1)       
			{
				if(met>ohpn-1)				//��met>ohpn-1�����ʾ���һ��Ƕ�벻�ü�¼bookkeeping����ʱ��¼lpoh�������ʾ���һ��Ƕ����ȻҪ��¼bookkeeping����ʱֱ�Ӽ�¼tstbfloc
				{

					bflen=(long)lpoh;
					lplen=&bflen;

					for(i=0; i<sizeof(long); i++)
					{
						flen=(unsigned char *)lplen+i;
						fl=*flen;
						for(k=0; k<8; k++)
						{
							*(lpembovh+(6+sizeof(long)*2)*8+8*i+k)=0;
							shiftnumber=k;
							shiftvalue=(fl>>shiftnumber)&1;
							*(lpembovh+(6+sizeof(long)*2)*8+8*i+k)=shiftvalue;
						}
					}
				}
				else
				{

					bflen=(long)tstbfloc;
					lplen=&bflen;

					for(i=0; i<sizeof(long); i++)
					{
						flen=(unsigned char *)lplen+i;
						fl=*flen;
						for(k=0; k<8; k++)
						{
							*(lpembovh+(6+sizeof(long)*2)*8+8*i+k)=0;
							shiftnumber=k;
							shiftvalue=(fl>>shiftnumber)&1;
							*(lpembovh+(6+sizeof(long)*2)*8+8*i+k)=shiftvalue;
						}
					}
					memcpy(lpembovh+(6+sizeof(long)*3)*8,lpembmap+testbuf_len*8+16*(met+2)-tstbfloc*8,tstbfloc*8);
				}
			}

		}
		else
		{

			bflen=(long)testbuf_len;
			lplen=&bflen;

			for(i=0; i<sizeof(long); i++)
			{
				flen=(unsigned char *)lplen+i;
				fl=*flen;
				for(k=0; k<8; k++)
				{
					*(lpembovh+32+8*i+k)=0;
					shiftnumber=k;
					shiftvalue=(fl>>shiftnumber)&1;
					*(lpembovh+32+8*i+k)=shiftvalue;
				}
			}

			lplen=&zn;
			for(i=0; i<sizeof(long); i++)
			{
				flen=(unsigned char *)lplen+i;
				fl=*flen;
				for(k=0; k<8; k++)
				{
					*(lpembovh+(4+sizeof(long))*8+8*i+k)=0;
					shiftnumber=k;
					shiftvalue=(fl>>shiftnumber)&1;
					*(lpembovh+(4+sizeof(long))*8+8*i+k)=shiftvalue;
				}
			}

			unsigned char shiftvalue, *flen, fl;		//Define the shifted value 
			int shiftnumber=0;				//Define the number of shift 


			for(i=0; i<testbuf_len; i++)
			{
				fl=*(locmap+i);
				for(k=0; k<8; k++)
				{
					*(lpembovh+(4+sizeof(long)*2+i)*8+k)=0;
					shiftnumber=k;
					shiftvalue=(fl>>shiftnumber)&1;
					*(lpembovh+(4+sizeof(long)*2+i)*8+k)=shiftvalue;
				}
			}

			if(zn==0)
			{
				for(i=0; i<2; i++)
				{
					fl=*(lpprehist+i);
					for(k=0; k<8; k++)
					{
						*(lpembovh+(4+sizeof(long)*2+testbuf_len+i)*8+k)=0;
						shiftnumber=k;
						shiftvalue=(fl>>shiftnumber)&1;
						*(lpembovh+(4+sizeof(long)*2+testbuf_len+i)*8+k)=shiftvalue;
					}
				}
			}
			else
			{
				for(i=0; i<4; i++)
				{
					fl=*(lpprehist+i);
					for(k=0; k<8; k++)
					{
						*(lpembovh+(4+sizeof(long)*2+testbuf_len+i)*8+k)=0;
						shiftnumber=k;
						shiftvalue=(fl>>shiftnumber)&1;
						*(lpembovh+(4+sizeof(long)*2+testbuf_len+i)*8+k)=shiftvalue;
					}
				}
			}


		}


		el=0;
		g=0;
		mpv=0;
		lpv=0;
		rpv=0;
		plv=0;

		if (addDataEmbedJudge == true)                   //��addDataEmbed��ûȫ��Ƕ�룬���Ƶ�addDataRemain�м���Ƕ��
		{
			/*��S=1ʱ����Ҫ�ȸ�ֵ�ͳ�ʼ��*/
			if (met == 0)
				addDataLenRemain = addDataLen;

			/*��ʼ����addDataEmbedʣ������ݸ��Ƶ�addDataRemain��*/
			if (haddDataRemain)
			{
				GlobalFree(haddDataRemain);
				haddDataRemain = NULL;
			}
			haddDataRemain = GlobalAlloc(GMEM_FIXED, addDataLenRemain * 8);
			addDataRemain = (unsigned char *)haddDataRemain;
			
			if (addDataLenRemain > 0)
			{
				memcpy(addDataRemain, addDataEmbed + addDataLen * 8 - addDataLenRemain * 8, addDataLenRemain * 8);
				/*
				for (int i = 0; i < addDataLenRemain * 8; i++)
				{
					FILE *payloadtxt;
					payloadtxt = fopen("E:\\experiment\\task\\ColorImage-7method\\addDataRemain.txt", "a");
					if (payloadtxt != NULL)
						fprintf(payloadtxt, "%d \t", *(addDataRemain + i));
					fclose(payloadtxt);
				}
				*/
			}
		}

		g=blkemb(lpembovh, wp, 0, 0, met, tstbfloc, el, t);
		embnb=g+embnb;

		ct=0;
		i=0;

		if(g<(6+tstbfloc+sizeof(long)*3)*8)
		{
			ind=(6+tstbfloc+sizeof(long)*3)*8-g;
			failed=1;
		}
		else if(met==0)
			ct=g-(tstbfloc+4+sizeof(long)*2)*8;
		else if(ohpn<2)
			ct=g-(6+tstbfloc+sizeof(long)*2)*8;
		else
			ct=g-(6+tstbfloc+sizeof(long)*3)*8;

		if (addDataEmbedJudge == true)
		{
			if (ct < addDataLenRemain * 8)
			{
				failed = 1;
				ind = (6 + tstbfloc + sizeof(long) * 3) * 8 - addDataLenRemain;
			}
				
		}

		if(hembovh)
		{ 
			GlobalFree(hembovh);  
			hembovh=NULL;
		}

		hembovh = GlobalAlloc(GMEM_FIXED, 16);

		lpembovh=(unsigned char *)hembovh;


		fl=lb;

		for(k=0; k<8; k++)
		{
			*(lpembovh+k)=0;
			shiftnumber=k;
			shiftvalue=(fl>>shiftnumber)&1;
			*(lpembovh+k)=shiftvalue;
		}

		fl=rb;

		for(k=0; k<8; k++)
		{
			*(lpembovh+8+k)=0;
			shiftnumber=k;
			shiftvalue=(fl>>shiftnumber)&1;
			*(lpembovh+8+k)=shiftvalue;
		}

		row=ImageHeight-1;
		for(column=0;column<16;column++)
		{
			tmp=*(wp+row*ImageWidth+column);
			*(wp+row*ImageWidth+column)=(tmp/2)*2+*(lpembovh+column);
			lmp=*(wp+row*ImageWidth+column);
		}

	}
	else if(zn>0 && t==0)                        //��һ��Ƕ�룬����bin merge
	{
		ct=zn;

		if(hembmap)
		{ 
			GlobalFree(hembmap);  
			hembmap=NULL;
		}
		hembmap = GlobalAlloc(GMEM_FIXED, (testbuf_len+2*(met+2))*8);
		lpembmap=(unsigned char *)hembmap;

		unsigned char shiftvalue, *flen, fl;		//Define the shifted value 
		int shiftnumber=0;				//Define the number of shift 

		/*lpembmap�ĸ�ֵ����locmap����lpembmap*/
		for(i=0; i<testbuf_len; i++)
		{
			fl=*(locmap+i);
			for(k=0; k<8; k++)
			{
				*(lpembmap+i*8+k)=0;
				shiftnumber=k;
				shiftvalue=(fl>>shiftnumber)&1;
				*(lpembmap+i*8+k)=shiftvalue;
			}
		}

		/*lpembmap�ĸ�ֵ����lpprehist����lpembmap*/
		for(i=0; i<2*(met+2); i++)
		{
			fl=*(lpprehist+i);
			for(k=0; k<8; k++)
			{
				*(lpembmap+(testbuf_len+i)*8+k)=0;
				shiftnumber=k;
				shiftvalue=(fl>>shiftnumber)&1;
				*(lpembmap+(testbuf_len+i)*8+k)=shiftvalue;
			}
		}

		el=0;
		g=0;
		mpv=0;
		lpv=0;
		rpv=0;
		plv=0;		
		
		if (addDataEmbedJudge == true)
		{
			/*��ʼ����addDataEmbed���Ƶ�addDataRemain��*/
			if (haddDataRemain)
			{
				GlobalFree(haddDataRemain);
				haddDataRemain = NULL;
			}
			haddDataRemain = GlobalAlloc(GMEM_FIXED, addDataLen * 8);
			addDataRemain = (unsigned char *)haddDataRemain;
			memcpy(addDataRemain, addDataEmbed, addDataLen * 8);
			addDataLenRemain = addDataLen;
		}

		g=blkems(lpembmap, wp, 0, 0, met, testbuf_len+2*(met+2), el, t);            //��һ��Ƕ�룬��������lpembmap�ĳ���
		embnb=g+embnb;

		if(g<testbuf_len*8+16*(met+2))                                              //��һ�ֲ���ȫ��Ƕ��ȥ����ohpn=2������ȫ��Ƕ��ȥ��ohpn=1��tstbfloc��¼���ж��ٳ�����ҪǶ
		{
			ohpn=2;                                                                  
			tstbfloc=testbuf_len-g/8+2*(met+2);
			ct=g-8*(g/8);
		}
		else
		{
			ct=g-testbuf_len*8-16*(met+2);
			ohpn=1;
			tstbfloc=0;
		}

		if (addDataEmbedJudge == true && tstbfloc == 0)
		{
			if(ct < addDataLen*8)
				addDataLenRemain = addDataLen - ct/8;
			else
				addDataLenRemain = 0;                                                //���о�Ƕ��������
		}

	}
	else if(zn==0 && t==0)                               //��һ��Ƕ�룬��û��bin merge����ʱohpn=0
	{
		if(hembmap)
		{ 
			GlobalFree(hembmap);  
			hembmap=NULL;
		}

		hembmap = GlobalAlloc(GMEM_FIXED, 16*(met+2));

		lpembmap=(unsigned char *)hembmap;

		unsigned char shiftvalue, *flen, fl;		//Define the shifted value 
		int shiftnumber=0;				//Define the number of shift 

		/*lpembmap�ĸ�ֵ����lpprehist����lpembmap*/
		for(i=0; i<2*(met+2); i++)
		{
			fl=*(lpprehist+i);
			for(k=0; k<8; k++)
			{
				*(lpembmap+i*8+k)=0;
				shiftnumber=k;
				shiftvalue=(fl>>shiftnumber)&1;
				*(lpembmap+i*8+k)=shiftvalue;
			}
		}

		el=0;
		g=0;
		mpv=0;
		lpv=0;
		rpv=0;
		plv=0;

		if (addDataEmbedJudge == true)
		{
			/*��ʼ����addDataEmbed���Ƶ�addDataRemain��*/
			if (haddDataRemain)
			{
				GlobalFree(haddDataRemain);
				haddDataRemain = NULL;
			}
			haddDataRemain = GlobalAlloc(GMEM_FIXED, addDataLen * 8);
			addDataRemain = (unsigned char *)haddDataRemain;
			memcpy(addDataRemain, addDataEmbed, addDataLen * 8);
			addDataLenRemain = addDataLen;
		}

		g=blkems(lpembmap, wp, 0, 0, met, 2*(met+2), el, t);                       //��һ��Ƕ�룬��������lpembmap�ĳ���
		embnb=g+embnb;

		if(g>16*(met+2))
			ct=g-16*(met+2);
		else
			failed=true;
		
		ohpn=0; 

		if (addDataEmbedJudge == true && failed ==false)
		{
			if(ct < addDataLen*8)
				addDataLenRemain = addDataLen - ct/8;
			else
				addDataLenRemain = 0;
		}	                                                                  
	}
	else if(t<met)                      //���ձ����
	{

		if(hembovh)
		{ 
			GlobalFree(hembovh);  
			hembovh=NULL;
		}
		hembovh = GlobalAlloc(GMEM_FIXED, (tstbfloc+2)*8);
		lpembovh=(unsigned char *)hembovh;

		unsigned char shiftvalue, *flen, fl;		//Define the shifted value 
		int buflen, shiftnumber=0;				//Define the number of shift 
		buflen=testbuf_len;

		fl=lb;                                  //lb��rb��ȷ����������Ƕ�벽��

		for(k=0; k<8; k++)
		{
			*(lpembovh+k)=0;
			shiftnumber=k;
			shiftvalue=(fl>>shiftnumber)&1;
			*(lpembovh+k)=shiftvalue;
		}

		fl=rb;

		for(k=0; k<8; k++)
		{
			*(lpembovh+8+k)=0;
			shiftnumber=k;
			shiftvalue=(fl>>shiftnumber)&1;
			*(lpembovh+8+k)=shiftvalue;
		}

		if(tstbfloc>0)                                             //��bookkeeping��ûȫ��Ƕ�룬���Ƶ�lpembvh��
			memcpy(lpembovh+16,lpembmap+testbuf_len*8+16*(met+2)-tstbfloc*8,tstbfloc*8);

		if (addDataEmbedJudge == true)                   //��addDataEmbed��ûȫ��Ƕ�룬���Ƶ�addDataRemain�м���Ƕ��
		{
			
			/*��ʼ����addDataEmbedʣ������ݸ��Ƶ�addDataRemain��*/
			if (haddDataRemain)
			{
				GlobalFree(haddDataRemain);
				haddDataRemain = NULL;
			}
			haddDataRemain = GlobalAlloc(GMEM_FIXED, addDataLenRemain * 8);
			addDataRemain = (unsigned char *)haddDataRemain;
			
			if (addDataLenRemain > 0)
			{
				memcpy(addDataRemain, addDataEmbed + addDataLen * 8 - addDataLenRemain * 8, addDataLenRemain * 8);
			}
		}

		el=0;
		g=0;
		mpv=0;
		lpv=0;
		rpv=0;
		plv=0;

		g=blkems(lpembovh, wp, 0, 0, met, tstbfloc, el, t);                    //Ƕ��ĺ�������Ƕ��ĳ���lpembovh������һ��Ƕ����������bin��ֵ
		embnb=g+embnb;

		//ct=0;
		i=0;

		if(g<tstbfloc*8+16)											//��bookkeeping��ûȫ��Ƕ�룬��ohpn+1��tstbfloc��ʾ��ʣ������Ҫ����
		{
			ohpn++;
			tstbfloc=tstbfloc+2-g/8;                                //���ᶪʧ��Ϣ����Ϊʣ�µĲ�����λ���������map���棬��һ��������Ƕ�����Զ��һ��ohpn������Ҫ����
			ct=g-8*(g/8);
		}
		else if(tstbfloc>0)
		{
			lpoh=tstbfloc;                                         //��¼���һ��bookkeepingǶ��ʱ�ĳ���
			ct=g-(tstbfloc+2)*8;
			tstbfloc=0;
			buflen=0;
		}
		else
			ct=g-2*8;

		if (addDataEmbedJudge == true && tstbfloc == 0)
		{
			if (ct < addDataLenRemain*8)
				addDataLenRemain = addDataLenRemain - ct/8;
			else
				addDataLenRemain = 0;
		}

	}
	else
	{
		ct=0;
	}

		
	//fclose(forg);
	return ct;

}

//�򿪴�Ƕ���ˮӡ�ļ�
void CDeprejbgDoc::defix()
{

	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"ͼ���ļ�|*.bmp;*.raw;*.vq||");
	dlg.m_ofn.lpstrTitle="��ˮӡͼ��";
	if(dlg.DoModal()==IDOK)
	{	CFile cf(dlg.GetPathName(),CFile::modeRead);
		filesize=cf.GetLength(); 
		if(dlg.GetFileExt()=="bmp")
		{
		    cf.Read(m_CurrentDirectory,filesize);
			wmfile=(LPBITMAPFILEHEADER)m_CurrentDirectory;
			wminfo=(LPBITMAPINFOHEADER)(m_CurrentDirectory+sizeof(BITMAPFILEHEADER));
			if(wmfile->bfType!=0x4d42)
			{ 
				AfxMessageBox("�ļ�����bmp�ļ�");
				return;
			} 
			if(wminfo->biBitCount!=1)
			{ 
				AfxMessageBox("�ļ�����2ֵͼ��!");
				return;
			} 
			if(wminfo->biCompression!=0)
			{ 
				AfxMessageBox("�ļ���ѹ��ͼ��");
				return;
			}
			unsigned char shiftvalue;		//Define the shifted value 
			int i,shiftnumber=0;				//Define the number of shift 
			for(i=0; i<wminfo->biHeight*wminfo->biWidth; i++)
			{
				m_OriginalWatermark[i]=0;
				shiftnumber=i%8;			//Get the remainder of i
				shiftvalue=(m_CurrentDirectory[62+i/8]>>shiftnumber)&1;
				m_OriginalWatermark[i]=shiftvalue;		
			}

			wmheight=wminfo->biHeight;
			wmwidth=wminfo->biWidth;

		}
		CurrentDirectory=dlg.GetPathName();
		this->SetTitle(CurrentDirectory);
	}
	else
	{
		CurrentDirectory="�ļ���ʧ��";
	}
	this->SetTitle(CurrentDirectory);
	
}

//�ų������16�����ص�Ƕ�뺯��
long CDeprejbgDoc::blkemb(unsigned char *tp, unsigned char *mp, int row, int column, int bs, long buf_len, long el, int t)
{
	//FILE *fori = fopen("D:\\orimg.txt", "wb");
	int i, j, k, kk, pn, tn, rn, tmb, pk, emr, eml, emn, x, y, a, b, c, suml[5], sumr[5], bkorderpixel[256];
	float eff[24], hef;
	bool flg, flag[256];
	unsigned char tm, pm, order[256], candl[5], candr[5];
	int first = -1, second = -1;
	
	eml = emr = 0;
	pn = 0;
	a = b = 0;
	c = 3;

	if(bs==0) tmb=4+sizeof(long)*2;
	else if(t==0)	tmb=0;
	else if(t==bs && ohpn<2) tmb=6+sizeof(long)*2;
	else if(t==bs && ohpn>1) tmb=6+sizeof(long)*3;
	else if(t<bs) tmb=2;


	if(hblkhis)
	{ 
		GlobalFree(hblkhis);  
		hblkhis=NULL;
	}
	hblkhis = GlobalAlloc(GMEM_FIXED, ImageHeight*ImageWidth*sizeof(unsigned int));
	lpblkhis=(unsigned int *)hblkhis;

	if(hblkval)
	{ 
		GlobalFree(hblkval);  
		hblkval=NULL;
	}
	hblkval = GlobalAlloc(GMEM_FIXED, ImageHeight*ImageWidth*4);
	lpblkval=(int *)hblkval;

	/*ͳ�Ƴ����16�����ص�LSB��Ŀǰ��lpblkval��lpblkhis*/
	for(i=0;i<ImageHeight-1;i++)
		for(j=0;j<ImageWidth;j++)
		{
			*(lpblkhis+i*ImageWidth+j)=0;
			*(lpblkval+i*ImageWidth+j)=-1;
		}

	i=ImageHeight-1;
	for(j=16;j<ImageWidth;j++)
	{
		*(lpblkhis+i*ImageWidth+j)=0;
		*(lpblkval+i*ImageWidth+j)=-1;
	}

	for(i=0;i<ImageHeight-1;i++)
	{
		for(j=0;j<ImageWidth;j++)
		{
			lm=*(mp+i*ImageWidth+j);
			//fprintf(fori, "%d ", lm); 
			flg=0;

			for(k=0; k<pn+1; k++)
			{
				if(lm==*(lpblkval+k))
				{
					tn=*(lpblkhis+k);
					*(lpblkhis+k)=tn+1;
					flg=1;
				}
			}

			if(flg==0)
			{
				pn++;
				*(lpblkhis+pn)=1;
				*(lpblkval+pn)=lm;
			}

		}
		//fprintf(fori, " \n");
	}

	
	i=ImageHeight-1;
	for(j=16;j<ImageWidth;j++)
	{
		lm=*(mp+i*ImageWidth+j);
		//fprintf(fori, "%d ", lm); 
		flg=0;

		for(k=0; k<pn+1; k++)
		{
			if(lm==*(lpblkval+k))
			{
				tn=*(lpblkhis+k);
				*(lpblkhis+k)=tn+1;
				flg=1;
			}
		}

		if(flg==0)
		{
			pn++;
			*(lpblkhis+pn)=1;
			*(lpblkval+pn)=lm;
		}

	}
	//fprintf(fori, " \n");

	//fclose(fori);

	for(i=0; i<pn; i++)
		for(k=1; k<pn-i; k++)
		{
			lm=*(lpblkval+k);
			tm=*(lpblkval+k+1);
			if(lm>tm)
			{
				*(lpblkval+k)=tm;
				*(lpblkval+k+1)=lm;
				rn=*(lpblkhis+k);
				tn=*(lpblkhis+k+1);
				*(lpblkhis+k)=tn;
				*(lpblkhis+k+1)=rn;

			}
		}

	for(i=0; i<256; i++)
	{
		order[i]=i;
		flag[i]=0;
	}


	for(i=1; i<pn; i++)
	{
		a=0;
		for (k = pn; k > 0; k--)
		//for(k=pn-1; k>0; k--)
		{
			rn=*(lpblkhis+k);
			if(rn>a && flag[k]==0)
			{
				order[i]=k;
				a=rn;
			}
		}
		flag[order[i]]=1;
	}


	/*�����ָ���㷨����Ҫ*/
	if (doBksega)
	{
		//������ֱ��ͼͳ��
		int bkHist[256] = { 0 }, bkgn = 0;

		for (int i = 0; i < ImageHeight; i++)
		{
			for (int j = 0; j < ImageWidth; j++)
			{
				if (isBackground[i][j] == 1)
				{
					bkHist[*(mp + i*ImageWidth + j)]++;   //��lpΪmp
					bkgn++;
				}
			}
		}
		//���¶���ֱ��ͼ
		kk = 0;
		int nbkHist[256] = { 0 };
		for (int i = 0; i < 256; i++)
		{
			if (bkHist[i] != 0)
			{
				nbkHist[kk] = bkHist[i];
				bkorderpixel[kk] = i;
				kk++;
			}
		}

		int bkorder[256];
		for (int i = 0; i < 256; i++)
		{
			bkorder[i] = i;
		}

		int tempbk, tempbkorder;
		for (int i = 0; i < 255; i++)
		{
			for (int j = 0; j < 255 - i; j++)
			{
				if (nbkHist[j]<nbkHist[j + 1])
				{
					tempbk = nbkHist[j];
					nbkHist[j] = nbkHist[j + 1];
					nbkHist[j + 1] = tempbk;

					tempbkorder = bkorder[j];
					bkorder[j] = bkorder[j + 1];
					bkorder[j + 1] = tempbkorder;

					tempbkorder = bkorderpixel[j];
					bkorderpixel[j] = bkorderpixel[j + 1];
					bkorderpixel[j + 1] = tempbkorder;

				}

			}
		}

		double size = ImageHeight * ImageWidth;

		percent = 0;
		bknum = 0;
		for (int i = 0; i < 256; i++)
		{
			percent = (double)nbkHist[i] / bkgn;
			if (percent < 0.002 || bknum > 268)
			{
				break;
			}
			else
			{
				bknum++;
				bkpixel[bknum - 1] = bkorderpixel[i];
			}
		}

		//******************************

		for (int i = 1; i < pn; i++)
		{
			first == -1;
			second == -1;
			tm = *(lpblkval + order[i]);
			kk = 0;
			for (int k = 0; k < bknum; k++)
			{
				pm = bkorderpixel[k];
				if (pm == tm)
				{
					kk++;
					k= bknum;
				}
			}
			if (kk == 0 && first == -1)
			{
				first = i;
				if (second > -1)
				{
					i = pn;
					break;
				}
			}
			else if (kk == 0 && second == -1)
			{
				second = i;
				if (first > -1)
				{
					i = pn;
					break;
				}
			}
		}
	}
	
	/*
	for (int i = 1; i < pn; i++)
	{
		first == -1;
		second == -1;
		tm = *(lpblkval + order[i]);
		kk = 0;
		for (int k = 0; k < lbn; k++)
		{
			pm = *(lplbldbn+k);
			if (pm == tm)
			{
				kk++;
				k= lbn;
			}
		}
		if (kk == 0 && first == -1)
		{
			first = i;
			if (second > -1)
			{
				i = pn;
				break;
			}
		}
		else if (kk == 0 && second == -1)
		{
			second = i;
			if (first > -1)
			{
				i = pn;
				break;
			}
		}
	}*/

	if (first == -1)
	{
		first = 1;
		second = 2;
	}
	//******************************** 

	tm=*(lpblkval+order[first]);
	pm=*(lpblkval+order[second]);

	if(tm>pm)
	{
		pm=*(lpblkval+order[first]);
		tm=*(lpblkval+order[second]);
	}

	lb=(unsigned char)tm;
	rb=(unsigned char)pm;

	
	for (int k = 0; k < lbn; k++)
	{
		lm = *(lplbldbn+k);
		if (lm <lb)
			*(lplbldbn+k) = lm-1;
		else if (lm >rb)
			*(lplbldbn+k) = lm+1;
	}


	/*ֱ��ͼ�޸ģ�����Ƕ�벿��*/
	for(i=0;i<ImageHeight-1;i++)
		for(j=0;j<ImageWidth;j++)
		{
			lm=*(mp+i*ImageWidth+j);

			if(lm<tm)
				*(mp+i*ImageWidth+j)=lm-1;
			else if(lm>pm)
				*(mp+i*ImageWidth+j)=lm+1;
			else if(lm==tm)
			{
				if(el<(buf_len+tmb)*8)
				{
					if(*(tp+el)==1)
					{
						mpv++;
						*(mp+i*ImageWidth+j)=lm-1;
					}
					else
						lpv++;

					el++;
				}
				else if (addDataEmbedJudge == true && el < (addDataLenRemain * 8) + (buf_len + tmb) * 8)
				{
					int addDataNum = el - (buf_len + tmb) * 8;
					if (*(addDataRemain + addDataNum) == 1)
					{
						mpv++;
						*(mp + i * ImageWidth + j) = lm - 1;
					}
					else
						lpv++;

					el++;
				}
				else                                       
				{
					if (addDataEmbedJudge == true)
						emn = el - (buf_len + tmb + addDataLenRemain) * 8;
					else
						emn=el-(buf_len+tmb)*8;
					if(m_OriginalWatermark[emn%(wminfo->biHeight*wminfo->biWidth)]==1)
					{
						mpv++;
						*(mp+i*ImageWidth+j)=lm-1;
					}
					else
						lpv++;

					el++;
				}

			}
			else if(lm==pm)
			{
				if(el<(buf_len+tmb)*8)
				{
					if(*(tp+el)==1)
					{
						plv++;
						*(mp+i*ImageWidth+j)=lm+1;
					}
					else
						rpv++;

					el++;
				}
				else if (addDataEmbedJudge == true && el < (addDataLenRemain * 8) + (buf_len + tmb) * 8)
				{
					int addDataNum = el - (buf_len + tmb) * 8;
					if (*(addDataRemain + addDataNum) == 1)
					{
						plv++;
						*(mp + i * ImageWidth + j) = lm + 1;
					}
					else
						rpv++;

					el++;
				}
				else                                     
				{
					if (addDataEmbedJudge == true)
						emn = el - (buf_len + tmb + addDataLenRemain) * 8;
					else
						emn = el - (buf_len + tmb) * 8;
					if(m_OriginalWatermark[emn%(wminfo->biHeight*wminfo->biWidth)]==1)
					{
						plv++;
						*(mp+i*ImageWidth+j)=lm+1;
					}
					else
						rpv++;

					el++;
				}
			}
		}
					
	i=ImageHeight-1;
	for(j=16;j<ImageWidth;j++)
	{
		lm=*(mp+i*ImageWidth+j);

		if(lm<tm)
			*(mp+i*ImageWidth+j)=lm-1;
		else if(lm>pm)
			*(mp+i*ImageWidth+j)=lm+1;
		else if(lm==tm)
		{
			if(el<(buf_len+tmb)*8)
			{
				if(*(tp+el)==1)
				{
					mpv++;
					*(mp+i*ImageWidth+j)=lm-1;
				}
				else
					lpv++;

				el++;
			}
			else if (addDataEmbedJudge == true && el < (addDataLenRemain * 8) + (buf_len + tmb) * 8)
			{
				int addDataNum = el - (buf_len + tmb) * 8;
				if (*(addDataRemain + addDataNum) == 1)
				{
					mpv++;
					*(mp + i * ImageWidth + j) = lm - 1;
				}
				else
					lpv++;

				el++;
			}
			else                                             
			{
				if (addDataEmbedJudge == true)
					emn = el - (buf_len + tmb + addDataLenRemain) * 8;
				else
					emn = el - (buf_len + tmb) * 8;
				if(m_OriginalWatermark[emn%(wminfo->biHeight*wminfo->biWidth)]==1)
				{
					mpv++;
					*(mp+i*ImageWidth+j)=lm-1;
				}
				else
					lpv++;

				el++;
			}

		}
		else if(lm==pm)                                         
		{
			if(el<(buf_len+tmb)*8)
			{
				if(*(tp+el)==1)
				{
					plv++;
					*(mp+i*ImageWidth+j)=lm+1;
				}
				else
					rpv++;

				el++;
			}
			else if (addDataEmbedJudge == true && el < (addDataLenRemain * 8) + (buf_len + tmb) * 8)
			{
				int addDataNum = el - (buf_len + tmb) * 8;
				if (*(addDataRemain + addDataNum) == 1)
				{
					plv++;
					*(mp + i * ImageWidth + j) = lm + 1;
				}
				else
					rpv++;

				el++;
			}
			else
			{
				if (addDataEmbedJudge == true)
					emn = el - (buf_len + tmb + addDataLenRemain) * 8;
				else
					emn = el - (buf_len + tmb) * 8;
				if(m_OriginalWatermark[emn%(wminfo->biHeight*wminfo->biWidth)]==1)
				{
					plv++;
					*(mp+i*ImageWidth+j)=lm+1;
				}
				else
					rpv++;

				el++;
			}
		}
	}

	return el;

}

//Ƕ�뺯��
long CDeprejbgDoc::blkems(unsigned char *tp, unsigned char *mp, int row, int column, int bs, long buf_len, long el, int t)
{
	//FILE *fori = fopen("orimgs.txt", "wb");
	int i, j, k, kk, tn, rn, tmb, pk, emr, eml, emn, x, y, a, b, c, suml[5], sumr[5], bkorderpixel[256];
	int pn;                           //��¼��ʱֱ��ͼ���ж��ٸ�bin
	float eff[24], hef;
	bool flg, flag[256];
	unsigned char tm, pm, order[256], candl[5], candr[5];
	int first = -1, second = -1;

	eml = emr = 0;
	pn = 0;
	a = b = 0;
	c = 3;

	//tmb�Ǹ���Ƕ��ʱͷ����Ϣ��Ҫ�ĳ���
	if(bs==0) tmb=4+sizeof(long)*2;
	else if(t==0)	tmb=0;
	else if(t==bs && ohpn<2) tmb=6+sizeof(long)*2;
	else if(t==bs && ohpn>1) tmb=6+sizeof(long)*3;
	else if(t<bs) tmb=2;


	if(hblkhis)
	{ 
		GlobalFree(hblkhis);  
		hblkhis=NULL;
	}
	hblkhis = GlobalAlloc(GMEM_FIXED, ImageHeight*ImageWidth*sizeof(unsigned int));
	lpblkhis=(unsigned int *)hblkhis;

	if(hblkval)
	{ 
		GlobalFree(hblkval);  
		hblkval=NULL;
	}
	hblkval = GlobalAlloc(GMEM_FIXED, ImageHeight*ImageWidth*4);
	lpblkval=(int *)hblkval;

	for(i=0;i<ImageHeight;i++)
		for(j=0;j<ImageWidth;j++)
		{
			*(lpblkhis+i*ImageWidth+j)=0;
			*(lpblkval+i*ImageWidth+j)=-1;
		}

	/*ͳ��Ŀǰ��lpblkval��lpblkhis*/
	for(i=0;i<ImageHeight;i++)
	{
		for(j=0;j<ImageWidth;j++)
		{
			lm=*(mp+i*ImageWidth+j);
			//fprintf(fori, "%d ", lm); 
			flg=0;

			for(k=0; k<pn+1; k++)
			{
				if(lm==*(lpblkval+k))
				{
					tn=*(lpblkhis+k);
					*(lpblkhis+k)=tn+1;
					flg=1;
				}
			}

			if(flg==0)
			{
				pn++;                                   //�˲�����pnֵΪ��ǰֱ��ͼ��bin������
				*(lpblkhis+pn)=1;
				*(lpblkval+pn)=lm;
			}

		}
		//fprintf(fori, " \n");
	}

	//fprintf(fori, " \n");

	//fclose(fori);


	/*ֱ��ͼ���죬��value�͵�������*/
	for(i=0; i<pn; i++)
		for(k=1; k<pn-i; k++)
		{
			lm=*(lpblkval+k);
			tm=*(lpblkval+k+1);
			if(lm>tm)
			{
				*(lpblkval+k)=tm;
				*(lpblkval+k+1)=lm;
				rn=*(lpblkhis+k);
				tn=*(lpblkhis+k+1);
				*(lpblkhis+k)=tn;
				*(lpblkhis+k+1)=rn;

			}
		}


	for(i=0; i<256; i++)
	{
		order[i]=i;
		flag[i]=0;
	}

	int num[256];
	int value[256];
	int aaaa;
	for (int ii = 0; ii < 256; ii++)
	{
		num[ii] = *(lpblkhis + ii);
		value[ii] = *(lpblkval + ii);
		if (order[ii] == 240)
		{
			aaaa = ii;
		}
	}

	/* order��0-255�ֱ��lpblkhis������С����������±�� */
	for(i=1; i<pn; i++)
	{
		a=0;
		//for (k = pn-1; k > 0; k--)
		for(k=pn; k>0; k--)
		{
			rn=*(lpblkhis+k);
			if(rn>a && flag[k]==0)
			{
				order[i]=k;
				a=rn;
			}
		}
		flag[order[i]]=1;
	}

	//û���õ������ָ�
	if (doBksega)
	{
		//������ֱ��ͼͳ��
		int bkHist[256] = { 0 }, bkgn = 0;

		for (int i = 0; i < ImageHeight; i++)
		{
			for (int j = 0; j < ImageWidth; j++)
			{
				if (isBackground[i][j] == 1)
				{
					bkHist[*(mp + i*ImageWidth + j)]++;   //��lpΪmp
					bkgn++;
				}
			}
		}
		//���¶���ֱ��ͼ
		kk = 0;
		int nbkHist[256] = { 0 };
		for (int i = 0; i < 256; i++)
		{
			if (bkHist[i] != 0)
			{
				nbkHist[kk] = bkHist[i];
				bkorderpixel[kk] = i;
				kk++;
			}
		}

		int bkorder[256];
		for (int i = 0; i < 256; i++)
		{
			bkorder[i] = i;
		}

		int tempbk, tempbkorder;
		for (int i = 0; i < 255; i++)
		{
			for (int j = 0; j < 255 - i; j++)
			{
				if (nbkHist[j]<nbkHist[j + 1])
				{
					tempbk = nbkHist[j];
					nbkHist[j] = nbkHist[j + 1];
					nbkHist[j + 1] = tempbk;

					tempbkorder = bkorder[j];
					bkorder[j] = bkorder[j + 1];
					bkorder[j + 1] = tempbkorder;

					tempbkorder = bkorderpixel[j];
					bkorderpixel[j] = bkorderpixel[j + 1];
					bkorderpixel[j + 1] = tempbkorder;

				}

			}
		}

		double size = ImageHeight * ImageWidth;

		percent = 0;
		bknum = 0;
		for (int i = 0; i < 256; i++)
		{
			percent = (double)nbkHist[i] / bkgn;
			if (percent < 0.002 || bknum > 268)
			{
				break;
			}
			else
			{
				bknum++;
				bkpixel[bknum - 1] = bkorderpixel[i];
			}
		}

		//******************************

		for (int i = 1; i < pn; i++)
		{
			first == -1;
			second == -1;
			tm = *(lpblkval + order[i]);
			kk = 0;
			for (int k = 0; k < bknum; k++)
			{
				pm = bkorderpixel[k];
				if (pm == tm)
				{
					kk++;
					k= bknum;
				}
			}
			if (kk == 0 && first == -1)
			{
				first = i;
				if (second > -1)
				{
					i = pn;
					break;
				}
			}
			else if (kk == 0 && second == -1)
			{
				second = i;
				if (first > -1)
				{
					i = pn;
					break;
				}
			}
		}
	}	

	/*
	for (int i = 1; i < pn; i++)
	{
		first == -1;
		second == -1;
		tm = *(lpblkval + order[i]);
		kk = 0;
		for (int k = 0; k < lbn; k++)
		{
			pm = *(lplbldbn+k);
			if (pm == tm)
			{
				kk++;
				k= lbn;
			}
		}
		if (kk == 0 && first == -1)
		{
			first = i;
			if (second > -1)
			{
				i = pn;
				break;
			}
		}
		else if (kk == 0 && second == -1)
		{
			second = i;
			if (first > -1)
			{
				i = pn;
				break;
			}
		}
	}*/

	if (first == -1)
	{
		first = 1;
		second = 2;
	}
	//******************************** 

	tm=*(lpblkval+order[first]);
	pm=*(lpblkval+order[second]);

	//bin��ֵ˳������
	if(tm>pm)
	{
		pm=*(lpblkval+order[first]);
		tm=*(lpblkval+order[second]);
	}

	lb=(unsigned char)tm;
	rb=(unsigned char)pm;
	
	
	for (int k = 0; k < lbn; k++)
	{
		lm = *(lplbldbn+k);
		if (lm <lb)
			*(lplbldbn+k) = lm-1;
		else if (lm >rb)
			*(lplbldbn+k) = lm+1;
	}



	/*ֱ��ͼ�޸ģ�����Ƕ�벿��*/
	for(i=0;i<ImageHeight;i++)
		for(j=0;j<ImageWidth;j++)
		{
			lm=*(mp+i*ImageWidth+j);

			if(lm<tm)
				*(mp+i*ImageWidth+j)=lm-1;
			else if(lm>pm)
				*(mp+i*ImageWidth+j)=lm+1;
			else if(lm==tm)
			{
				if(el<(buf_len+tmb)*8)
				{
					if(*(tp+el)==1)
					{
						mpv++;
						*(mp+i*ImageWidth+j)=lm-1;
					}
					else
						lpv++;

					el++;
				}
				else if (addDataEmbedJudge == true && el < (addDataLenRemain * 8) + (buf_len + tmb) * 8)
				{
					int addDataNum = el - (buf_len + tmb) * 8;
					if (*(addDataRemain + addDataNum) == 1)
					{
						mpv++;
						*(mp + i * ImageWidth + j) = lm - 1;
					}
					else
						lpv++;

					el++;
				}
				else                                      
				{
					if (addDataEmbedJudge == true)
						emn = el - (buf_len + tmb + addDataLenRemain) * 8;
					else
						emn = el - (buf_len + tmb) * 8;
					if(m_OriginalWatermark[emn%(wminfo->biHeight*wminfo->biWidth)]==1)
					{
						mpv++;
						*(mp+i*ImageWidth+j)=lm-1;
					}
					else
						lpv++;

					el++;
				}

			}
			else if(lm==pm)
			{
				if(el<(buf_len+tmb)*8)
				{
					if(*(tp+el)==1)
					{
						plv++;
						*(mp+i*ImageWidth+j)=lm+1;
					}
					else
						rpv++;

					el++;
				}
				else if (addDataEmbedJudge == true && el < (addDataLenRemain * 8) + (buf_len + tmb) * 8)
				{
					int addDataNum = el - (buf_len + tmb) * 8;
					if (*(addDataRemain + addDataNum) == 1)
					{
						plv++;
						*(mp + i * ImageWidth + j) = lm + 1;
					}
					else
						rpv++;

					el++;
				}
				else                                   
				{
					if (addDataEmbedJudge == true)
						emn = el - (buf_len + tmb + addDataLenRemain) * 8;
					else
						emn = el - (buf_len + tmb) * 8;
					if(m_OriginalWatermark[emn%(wminfo->biHeight*wminfo->biWidth)]==1)
					{
						plv++;
						*(mp+i*ImageWidth+j)=lm+1;
					}
					else
						rpv++;

					el++;
				}
			}
		}
					
	return el;

}

//��������lpwmimage(lppreimage)
//��ȡ
void CDeprejbgDoc::depre()
{
	extracted = 0;

	int row, column, i, j, g;
	double h, di;
	long pld, ct;
	unsigned char tmp;
	expld=0;
	extnb=0;
	if(hdifimage)
	{ 
		GlobalFree(hdifimage);  
		hdifimage=NULL;
	}
	hdifimage=GlobalAlloc(GMEM_FIXED, ImageHeight*ImageWidth*2);		//GlobalAlloc	The GlobalAlloc function allocates the specified number of bytes from the heap.If the function succeeds, the return value is a handle to the newly allocated memory object.
	lpdifimage=(int *)hdifimage;

	/*
	if (hpreimage)
	{
		GlobalFree(hpreimage);
		hpreimage = NULL;
	}
	hpreimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);		//GlobalAlloc	The GlobalAlloc function allocates the specified number of bytes from the heap.If the function succeeds, the return value is a handle to the newly allocated memory object.
	lppreimage = (unsigned char *)hpreimage;
	*/

	expld = extrec(lppreimage, lpwmimage);                            //������������lppreimage������lppreimage�ǻҶ�ͼ��ʽ
	/*
	if (hpreimage)
	{
		GlobalFree(hpreimage);
		hpreimage = NULL;
	}
	hpreimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);		//GlobalAlloc	The GlobalAlloc function allocates the specified number of bytes from the heap.If the function succeeds, the return value is a handle to the newly allocated memory object.
	lppreimage = (unsigned char *)hpreimage;
	f++;
	*/
	//all-first
	/*
	FILE *fpB = fopen("C:\\Users\\May_2\\Desktop\\wmlp.txt", "ab+");

	for (int i = g1; i < m_ImageSize; i += 1)
	{
		fprintf(fpB, "%d ", *(lpwmimage + i));
	}
	fprintf(fpB, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
	fclose(fpB);
	expld=extrec(lppreimage, lpwmimage);
	f++;
	*/

	/*

	FILE *fpB = fopen("C:\\Users\\May_2\\Desktop\\wmlp.txt","ab+" );

	for (int i = g1; i < m_ImageSize; i += 1)
	{
		fprintf(fpB, "%d ", *(lppreimage + i));
	}
	fprintf(fpB, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
	fclose(fpB);
	*/
	memcpy(lpwmimage,lppreimage,m_ImageSize);

	//�������
	g=1024+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);

	ct=0;
	for(row=0;row<ImageHeight;row++)
		for(column=0;column<ImageWidth;column++)
		{
			tmp=*(lppreimage+g+row*ImageWidth+column);
			lm=*(lporigimage+g+row*ImageWidth+column);
			if(lm!=tmp)
			{
				ct++;	
			}
		}

	if (ct==0) 	extracted=1;
	

}

long  CDeprejbgDoc::extrec(unsigned char *lpwmimg, unsigned char *lporigimg)   //ori-enhanced need to return to wm,�������wm������Ҫ�����ԭ��wm
{
	
//	FILE *fextr=fopen("E:\\median preserving\\rwbimhm\\exteremap.txt","w");
	unsigned short int column,row;
	unsigned char tmp, lmp, lsb[16], lm, tml, tmr, pmx, pmn;
	int i,j,k,m,x,y, t, lr;
	long g, el, *lplen, extn, pld;
	double di,h;
	struct jbg85_enc_state s;
	HANDLE hmaplen=NULL;
	mext = 0;

	struct jbg85_dec_state d;
	size_t plane_size, buffer_len;
	unsigned char *image, *buffer;
	size_t bpl, cnt;
	int result;


	unsigned char shiftvalue, fl;

	t=0;
	lr=1;
	extn=0;
	
	g=1024+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);

	wp=lpwmimg+g;
	lp=lporigimg+g;
	/*
	FILE *fpB = fopen("C:\\Users\\May_2\\Desktop\\wmlp.txt", "ab+");

	for (int i = g; i < m_ImageSize; i += 1)
	{
		fprintf(fpB, "%d ", *(lporigimg + i));
	}
	fprintf(fpB, "<");
	fclose(fpB);
	*/

	/*��ȡ���Ƕ���õ�����bin*/
	tmp=0;
	for(k=0; k<8; k++)
	{
		fl=*(wp+(ImageHeight-1)*ImageWidth+7-k);
		shiftvalue=fl%2;
		tmp=(tmp<<1)+shiftvalue;
	}
	lb=tmp;


	for(k=0; k<8; k++)
	{
		fl=*(wp+(ImageHeight-1)*ImageWidth+15-k);
		shiftvalue=fl%2;
		tmp=(tmp<<1)+shiftvalue;
	}
	rb=tmp;


	if(hextlen)
	{ 
		GlobalFree(hextlen);  
		hextlen=NULL;
	}
	hextlen = GlobalAlloc(GMEM_FIXED, ImageHeight*ImageWidth*2);
	extlen=(unsigned char *)hextlen;

	el=0;
	g=0;

	g=blkext(wp, 0, 0, ImageWidth, el, t);                         //�״���ȡ��tΪ0 

	extnb=g;

	unsigned char *flsb;
	if(hmaplen)	
	{
		GlobalFree(hmaplen);  
		hmaplen=NULL;
	}
	hmaplen=GlobalAlloc(GMEM_FIXED,sizeof(long));
	lplen=(long *)hmaplen;

	/*S����ȡ*/
	tmp=0;
	for(k=0; k<8; k++)
	{
		shiftvalue=*(extlen+7-k);
		tmp=(tmp<<1)+shiftvalue;
	}
	mext=tmp;

	/*16LSBs����ȡ*/
	for(k=0; k<16; k++)
	{
		shiftvalue=*(extlen+k+8);
		lsb[k]=shiftvalue;
	}

	/*ohpn����ȡ*/
	tmp=0;
	for(k=0; k<8; k++)
	{
		shiftvalue=*(extlen+7-k+24);
		tmp=(tmp<<1)+shiftvalue;
	}
	ohpn=tmp;
	
	/*���16�����صĻָ�*/
	row=ImageHeight-1;
	for(column=0;column<16;column++)
	{
		lm=*(wp+row*ImageWidth+column);
		*(wp+row*ImageWidth+column)=2*(lm/2)+lsb[column];	
	}

	if(hphist)	
	{
		GlobalFree(hphist);  
		hphist=NULL;
	}
	hphist=GlobalAlloc(GMEM_FIXED,2*(mext+2)*sizeof(unsigned char));
	lphist=(unsigned char *)hphist;
	





	

	if(mext>0)
	{
		/*��ȡ��һ�ָֻ�������bin*/
		tmp=0;
		for(k=0; k<8; k++)
		{
			shiftvalue=*(extlen+7-k+32);
			tmp=(tmp<<1)+shiftvalue;
		}
		lb=tmp;


		tmp=0;	
		for(k=0; k<8; k++)
		{
			shiftvalue=*(extlen+15-k+32);
			tmp=(tmp<<1)+shiftvalue;
		}
		rb=tmp;

		/*��ȡbookkeeping���ȣ���testbuf_len*/
		flsb=(unsigned char *)lplen;
		for(i=0; i<sizeof(long); i++)
		{
			*(flsb+i)=0;	
			for(k=0; k<8; k++)
			{
				shiftvalue=*(extlen+48+7-k+8*i);
				*(flsb+i)=(*(flsb+i)<<1)+shiftvalue;
			}
		}

		exbuflen=*lplen;

		if(hextmap)
		{ 
			GlobalFree(hextmap);  
			hextmap=NULL;
		}
		hextmap = GlobalAlloc(GMEM_FIXED, exbuflen);
		lpextmap=(unsigned char *)hextmap;
		
		/*��ȡexct����zn*/
		flsb=(unsigned char *)lplen;
		for(i=0; i<sizeof(long); i++)
		{
			*(flsb+i)=0;	
			for(k=0; k<8; k++)
			{
				shiftvalue=*(extlen+7-k+8*(i+sizeof(long)+6));
				*(flsb+i)=(*(flsb+i)<<1)+shiftvalue;
			}
		}

		exct=*lplen;		

		if(exct==0)	exbuflen=0;

		if(ohpn>1)
		{
			/*��ȡlpoh*/
			flsb=(unsigned char *)lplen;
			for(i=0; i<sizeof(long); i++)
			{
				*(flsb+i)=0;	
				for(k=0; k<8; k++)
				{
					shiftvalue=*(extlen+7-k+8*(6+sizeof(long)*2)+8*i);
					*(flsb+i)=(*(flsb+i)<<1)+shiftvalue;
				}
			}

			lpoh=*lplen;

			extn=g-8*(6+sizeof(long)*3);

			/*�����һ��Ƕ�루����һ����ȡ����Ƕ��bookkeeping��Ϣʱ����ȡlpextmap��lphist*/
			if(t==mext-ohpn+1)
			{
				if(lpoh>2*(mext+2)-1)                      //��ʣ��Ƕ��������lpoh>lpprehist����lphismap��lpprehist��Ҫ��ȡ������ֻҪ��ȡlpprehist
				{
					for(i=0; i<lpoh-2*(mext+2); i++)
					{
						*(lpextmap+exbuflen-lpoh+2*(mext+2)+i)=0;
						for(k=0; k<8; k++)
						{
							shiftvalue=*(extlen+7-k+8*(6+sizeof(long)*3)+8*i);
							*(lpextmap+exbuflen-lpoh+2*(mext+2)+i)=(*(lpextmap+exbuflen-lpoh+2*(mext+2)+i)<<1)+shiftvalue;
						}
					}
			
					for(i=0; i<2*(mext+2); i++)
						for(k=0; k<8; k++)
						{
							shiftvalue=*(extlen+7-k+8*(lpoh-2*mext+2+sizeof(long)*3+i));
							*(lphist+i)=(*(lphist+i)<<1)+shiftvalue;
							//phist[8*i+k]=*(extlen+7-k+8*(lpoh-2*mext+i));
						}

					extn=extn-lpoh*8;
					if (addDataEmbedJudge == true)
					{
						dataLen[0] = 8 * (extn / 8);
						memcpy(ExtractData, extlen + 8 * (6 + sizeof(long) * 3 + lpoh), dataLen[0]);
						haveStored = haveStored + dataLen[0];
					}

					lpoh=lpoh-2*(mext+2);
					lphf=false;
				}
				else
				{
					for(i=0; i<lpoh; i++)
						for(k=0; k<8; k++)
						{
							shiftvalue=*(extlen+7-k+8*(i+6+sizeof(long)*3));
							*(lphist+2*(mext+2)-lpoh+i)=(*(lphist+2*(mext+2)-lpoh+i)<<1)+shiftvalue;

						}

					extn=extn-lpoh*8;
					if (addDataEmbedJudge == true)
					{
						dataLen[0] = 8 * (extn / 8);
						memcpy(ExtractData, extlen + 8 * (6 + sizeof(long) * 3 + lpoh), dataLen[0]);
						haveStored = haveStored + dataLen[0];
					}

					lpoh=lpoh-2*(mext+2);
					lphf=true;
				}
			}
			
			if (addDataEmbedJudge == true)
			{
				dataLen[0] = 8 * (extn / 8);
				memcpy(ExtractData, extlen + 8 * (6 + sizeof(long) * 3), dataLen[0]);
				haveStored = haveStored + dataLen[0];
			}
			
		}
		else 
		{
			extn=g-8*(6+sizeof(long)*2);

			if (addDataEmbedJudge == true)
			{
				dataLen[0] = 8 * (extn / 8);
				memcpy(ExtractData, extlen + 8 * (6 + sizeof(long) * 2), dataLen[0]);
				haveStored = haveStored + dataLen[0];
			}
		}
		
		for(i=0; i<256; i++)
			phist[i]=0;

        for(j=1; j<mext+1; j++)
        {
			pld=dextr(lppreimage, lporigimage, j);
			extn=extn+pld;
        }
	}
	else                  //��mext=0����s=1���˲������˻ָ�ֱ��ͼ
	{
		/*exbuflen����ȡ��bookkeeping�ĳ���*/
		flsb=(unsigned char *)lplen;
		for(i=0; i<sizeof(long); i++)
		{
			*(flsb+i)=0;	
			for(k=0; k<8; k++)
			{
				shiftvalue=*(extlen+32+7-k+8*i);
				*(flsb+i)=(*(flsb+i)<<1)+shiftvalue;
			}
		}

		exbuflen=*lplen;

		if(hextmap)
		{ 
			GlobalFree(hextmap); 
			hextmap=NULL;
		}
		hextmap = GlobalAlloc(GMEM_FIXED, exbuflen);
		lpextmap=(unsigned char *)hextmap;

		/*exct����ȡ*/
		flsb=(unsigned char *)lplen;
		for(i=0; i<sizeof(long); i++)
		{
			*(flsb+i)=0;	
			for(k=0; k<8; k++)
			{
				shiftvalue=*(extlen+7-k+8*(i+sizeof(long)+4));
				*(flsb+i)=(*(flsb+i)<<1)+shiftvalue;
			}
		}
		exct=*lplen;

		if(exct==0)	exbuflen=0;

		if(exct>0) 
		{
			
			/*lpextmap����ȡ*/
			for(i=0; i<exbuflen; i++)
			{
				*(lpextmap+i)=0;
				for(k=0; k<8; k++)
				{
					shiftvalue=*(extlen+7-k+8*(i+sizeof(long)*2+4));
					*(lpextmap+i)=(*(lpextmap+i)<<1)+shiftvalue;
				}
			}

			/*lphist����ȡ*/
			for(i=0; i<4; i++)
				for(k=0; k<8; k++)
				{
					shiftvalue=*(extlen+7-k+8*(i+exbuflen+sizeof(long)*2+4));
					*(lphist+i)=(*(lphist+i)<<1)+shiftvalue;
				}

			//el=g-exbuflen*8-256;			

//			extn=0;
			if (colorToGrayPreprocessRecover = false)
			{
				preProcessRecover(lphist, lpextmap, mext, exct, wp);
			}
			/*
			tml=*(lphist+2*mext+2);
			tmr=*(lphist+2*mext+3);

			for(k=0; k<mext+1-tmr; k=k+1)
			{
				pmx=*(lphist+2*mext+1-k)-1;
				y=0;

				for(row=0;row<ImageHeight;row++)
					for(column=0;column<ImageWidth;column++)
					{
						lm=*(wp+row*ImageWidth+column);
						if(lm==pmx)	
							y++;	
					}

				exct=exct-y;

				for(row=0;row<ImageHeight;row++)
					for(column=0;column<ImageWidth;column++)
					{
						lm=*(wp+row*ImageWidth+column);
						if(lm>pmx)	*(wp+row*ImageWidth+column)=lm+1;	
						else if(lm==pmx)
						{
							i=exct/8; 
							j=7+i*8-exct;
							tmp=*(lpextmap+i);
							m=(tmp>>j)&1;
							if(m==1)	
								*(wp+row*ImageWidth+column)=lm+1;	
							exct++;
						}
					}
				
				exct=exct-y;

			}
						

			for(k=tmr-1; k>-1; k--)
			{
				pmx=*(lphist+mext+1+k)-1;
				for(row=0;row<ImageHeight;row++)
					for(column=0;column<ImageWidth;column++)
					{
						lm=*(wp+row*ImageWidth+column);
						if(lm>pmx)	*(wp+row*ImageWidth+column)=lm+1;	
					}

			}


		
			for(k=0; k<mext+1-tml; k=k+1)
			{
				pmn=*(lphist+mext-k)+1;
				y=0;

				for(row=0;row<ImageHeight;row++)
					for(column=0;column<ImageWidth;column++)
					{
						lm=*(wp+row*ImageWidth+column);
						if(lm==pmn)	
							y++;	
					}

				exct=exct-y;

				for(row=0;row<ImageHeight;row++)
					for(column=0;column<ImageWidth;column++)
					{
						lm=*(wp+row*ImageWidth+column);
						if(lm<pmn)	*(wp+row*ImageWidth+column)=lm-1;	
						else if(lm==pmn)
						{
							i=exct/8; 
							j=7+i*8-exct;
							tmp=*(lpextmap+i);
							m=(tmp>>j)&1;
							if(m==1)	
								*(wp+row*ImageWidth+column)=lm-1;	
							exct++;
						}
					}

				exct=exct-y;

			}
							
	
			for(k=tml-1; k>-1; k--)
			{
				pmn=*(lphist+k)+1;

				for(row=0;row<ImageHeight;row++)
					for(column=0;column<ImageWidth;column++)
					{
						lm=*(wp+row*ImageWidth+column);
						if(lm<pmn)	*(wp+row*ImageWidth+column)=lm-1;	
					}

			}
		*/
			extn=g-8*(4+sizeof(long)*2+exbuflen+4);

			if (addDataEmbedJudge == true)
			{
				dataLen[0] = 8 * (extn / 8);
				memcpy(ExtractData, extlen + 8 * (4 + sizeof(long) * 2 + exbuflen + 4 + 28), dataLen[0]);         //��28����ǰ��ĳ���
				haveStored = haveStored + dataLen[0];
			}

		}
		else
		{
			for(i=0; i<2; i++)
				for(k=0; k<8; k++)
				{
					shiftvalue=*(extlen+7-k+8*(i+sizeof(long)*2+4));
					*(lphist+i)=(*(lphist+i)<<1)+shiftvalue;
				}

			if (colorToGrayPreprocessRecover = false)
			{
				preProcessRecover(lphist, lpextmap, mext, 0, wp);
			}
			
			/*for(k=0; k>-1; k--)
			{
				pmn=*(lphist+k)+1;

				for(row=0;row<ImageHeight;row++)
					for(column=0;column<ImageWidth;column++)
					{
						lm=*(wp+row*ImageWidth+column);
						if(lm<pmn)	*(wp+row*ImageWidth+column)=lm-1;	
					}

			}

			for(k=0; k>-1; k--)
			{
				pmx=*(lphist+mext+1+k)-1;
				for(row=0;row<ImageHeight;row++)
					for(column=0;column<ImageWidth;column++)
					{
						lm=*(wp+row*ImageWidth+column);
						if(lm>pmx)	*(wp+row*ImageWidth+column)=lm+1;	
					}

			}
			*/
			extn=g-8*(4+sizeof(long)*2+2);

			if (addDataEmbedJudge == true)
			{
				dataLen[0] = 8 * (extn / 8);
				memcpy(ExtractData, extlen + 8 * (4 + sizeof(long) * 2 + 2 + 14), dataLen[0]);           //��14����ǰ��ĳ���
				haveStored = haveStored + dataLen[0];
			}
		}

	}

	
	
//	fclose(fextr);


	return extn;
}

long CDeprejbgDoc::dextr(unsigned char *lpwmimg, unsigned char *lporigimg, int t)
{
//	FILE *fex=fopen("D:\\eximap.txt","w");
	unsigned short int column,row;
	unsigned char tmp, lmp, lm, tml, tmr, pmx, pmn;
	int i,j,k,m,x,y;
	long g, el, *lplen, extn, 	lmwidth=8*((ImageWidth+7)/8);
	double di,h;
	struct jbg85_enc_state s;
	HANDLE hmaplen=NULL;

	struct jbg85_dec_state d;
	size_t plane_size, buffer_len;
	unsigned char *image, *buffer;
	size_t bpl, cnt;
	int result;

	unsigned char shiftvalue, fl;
	
	g=1024+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);

	if(hextlen)
	{ 
		GlobalFree(hextlen);  
		hextlen=NULL;
	}
	hextlen = GlobalAlloc(GMEM_FIXED, 2*ImageHeight*ImageWidth);
	extlen=(unsigned char *)hextlen;

	wp=lpwmimg+g;
	lp=lporigimg+g;
	


	el=0;
	g=0;

	g=blkexs(wp, 0, 0, ImageWidth, el, t);

	extnb=extnb+g;

	if(t<mext-ohpn+1)                  //��ʱ��ȡ�����ݶ��ǷǼ�¼����
	{
		if(t<mext)                    //�����һ�֣�����ȡ��һ�ָֻ���Ҫ������bin
		{
			tmp=0;
			for(k=0; k<8; k++)
			{
				shiftvalue=*(extlen+7-k);
				tmp=(tmp<<1)+shiftvalue;
			}
			lb=tmp;


			tmp=0;	
			for(k=0; k<8; k++)
			{
				shiftvalue=*(extlen+15-k);
				tmp=(tmp<<1)+shiftvalue;
			}
			rb=tmp;

			el=g-16;

			if (addDataEmbedJudge == true)
			{
				dataLen[t] = 8 * (el / 8);
				memcpy(ExtractData + haveStored, extlen + 16, dataLen[t]);
				haveStored = haveStored + dataLen[t];
			}

		}
		else                         //�������һ�֣���ָ���Ԥ����ǰ��ֱ��ͼ�����������zn=0,��ֻ��¼��lpprehist��
		{		
			for(i=0; i<2*(mext+2); i++)
				for(k=0; k<8; k++)
				{
					shiftvalue=*(extlen+7-k+8*i);
					*(lphist+i)=(*(lphist+i)<<1)+shiftvalue;
					//phist[8*i+k]=*(extlen+7-k+8*i);
				}

			el=g-16*(mext+2);		
			if (colorToGrayPreprocessRecover = false)
			{
				preProcessRecover(lphist, lpextmap, mext, 0, wp);
			}
			/*
			tml=*(lphist+2*mext+2);
			tmr=*(lphist+2*mext+3);
			
			for(k=tml-1; k>-1; k--)
			{
				pmn=*(lphist+k)+1;

				for(row=0;row<ImageHeight;row++)
					for(column=0;column<ImageWidth;column++)
					{
						lm=*(wp+row*ImageWidth+column);
						if(lm<pmn)	*(wp+row*ImageWidth+column)=lm-1;	
					}

			}

			for(k=tmr-1; k>-1; k--)
			{
				pmx=*(lphist+mext+1+k)-1;
				for(row=0;row<ImageHeight;row++)
					for(column=0;column<ImageWidth;column++)
					{
						lm=*(wp+row*ImageWidth+column);
						if(lm>pmx)	*(wp+row*ImageWidth+column)=lm+1;	
					}

			}
			*/

			if (addDataEmbedJudge == true)
			{
				dataLen[t] = 8 * (el / 8);
				memcpy(ExtractData + haveStored, extlen + 16 * (mext + 2), dataLen[t]);
				haveStored = haveStored + dataLen[t];
			}
			
		}
	}
	else if(t<mext)                                //��ʱ��ȡ��������bookkeeping��¼����
	{
		tmp=0;
		for(k=0; k<8; k++)
		{
			shiftvalue=*(extlen+7-k);
			tmp=(tmp<<1)+shiftvalue;
		}
		lb=tmp;


		tmp=0;	
		for(k=0; k<8; k++)
		{
			shiftvalue=*(extlen+15-k);
			tmp=(tmp<<1)+shiftvalue;
		}
		rb=tmp;


		if(t==mext-ohpn+1)                         //��ǡ���Ǽ�¼bookkeeping������һ��S����Ҫ��ȡlpprehist��lohismap
		{
			if(lpoh>2*(mext+2)-1)                  //����¼����Ϣ��lpprehist������Ҫ��ȡlpprehist��lohismap
			{
				for(i=0; i<lpoh-2*(mext+2); i++)
				{
					*(lpextmap+exbuflen-lpoh+2*(mext+2)+i)=0;
					for(k=0; k<8; k++)
					{
						shiftvalue=*(extlen+7-k+8*(2+i));
						*(lpextmap+exbuflen-lpoh+2*(mext+2)+i)=(*(lpextmap+exbuflen-lpoh+2*(mext+2)+i)<<1)+shiftvalue;
					}
				}
		
				for(i=0; i<2*(mext+2); i++)
					for(k=0; k<8; k++)
					{
						shiftvalue=*(extlen+7-k+8*(lpoh-2*mext-2+i));
						*(lphist+i)=(*(lphist+i)<<1)+shiftvalue;
						//phist[8*i+k]=*(extlen+7-k+8*(lpoh-2*mext+i));
					}

				el=g-16-lpoh*8;

				if (addDataEmbedJudge == true)
				{
					dataLen[t] = 8 * (el / 8);
					memcpy(ExtractData + haveStored, extlen + 16 + lpoh * 8, dataLen[t]);
					haveStored = haveStored + dataLen[t];
				}

				lpoh=lpoh-2*(mext+2);
				lphf=false;
			}
			else                                    //����¼����Ϣ��lpprehist�̣���ֻҪ��ȡlpprehist
			{
				for(i=0; i<lpoh; i++)
					for(k=0; k<8; k++)
					{
						shiftvalue=*(extlen+7-k+8*(2+i));
						*(lphist+2*(mext+2)-lpoh+i)=(*(lphist+2*(mext+2)-lpoh+i)<<1)+shiftvalue;
						//phist[8*(2*(mext+1)-lpoh+i)+k]=*(extlen+7-k+8*(2+i));
					}

				el=g-16-lpoh*8;

				if (addDataEmbedJudge == true)
				{
					dataLen[t] = 8 * (el / 8);
					memcpy(ExtractData + haveStored, extlen + 16 + lpoh * 8, dataLen[t]);
					haveStored = haveStored + dataLen[t];
				}

				lpoh=lpoh-2*(mext+2);
				lphf=true;
			}

		}
		else
		{
			if(lphf)
			{
				extn=-lpoh;

				lpoh=lpoh+(g-16)/8;

				for(i=0; i<(g-16)/8-extn; i++)
				{
					*(lpextmap+exbuflen-lpoh+i)=0;
					for(k=0; k<8; k++)
					{
						shiftvalue=*(extlen+7-k+8*(2+i));
						*(lpextmap+exbuflen-lpoh+i)=(*(lpextmap+exbuflen-lpoh+i)<<1)+shiftvalue;
					}
				}

				for(i=0; i<extn; i++)
					for(k=0; k<8; k++)
					{
						shiftvalue=*(extlen+7-k+8*((g-16)/8-extn+2+i));
						*(lphist+i)=(*(lphist+i)<<1)+shiftvalue;
						//phist[8*i+k]=*(extlen+7-k+8*((g-16)/8-extn+2+i));
					}

				el=g-(g/8)*8;
				lphf=false;

				if (addDataEmbedJudge == true)
				{
					dataLen[t] = 8 * (el / 8);                                           //���ƴ�ʱΪ0
					memcpy(ExtractData + haveStored, extlen + (g / 8) * 8, dataLen[t]);
					haveStored = haveStored + dataLen[t];
				}

			}
			else
			{
				lpoh=lpoh+(g-16)/8;

				for(i=0; i<(g-16)/8; i++)
				{
					*(lpextmap+exbuflen-lpoh+i)=0;
					for(k=0; k<8; k++)
					{
						shiftvalue=*(extlen+7-k+8*(2+i));
						*(lpextmap+exbuflen-lpoh+i)=(*(lpextmap+exbuflen-lpoh+i)<<1)+shiftvalue;
					}
				}

				el=g-(g/8)*8;

				if (addDataEmbedJudge == true)
				{
					dataLen[t] = 8 * (el / 8);                                           //���ƴ�ʱΪ0
					memcpy(ExtractData + haveStored, extlen + (g / 8) * 8, dataLen[t]);
					haveStored = haveStored + dataLen[t];
				}
			}

		}
		

	}
	else                            //�������һ����ȡ
	{

		if(ohpn<2)                  //ohpn=1����S=1,��ͬ�����ȡlpextmap��lphistҲ��ͬ
		{

			for(i=0; i<exbuflen; i++)
			{
				*(lpextmap+i)=0;
				for(k=0; k<8; k++)
				{
					shiftvalue=*(extlen+7-k+8*i);
					*(lpextmap+i)=(*(lpextmap+i)<<1)+shiftvalue;
				}
			}
		
			for(i=0; i<2*(mext+2); i++)
				for(k=0; k<8; k++)
				{
					shiftvalue=*(extlen+7-k+8*(i+exbuflen));
					*(lphist+i)=(*(lphist+i)<<1)+shiftvalue;
					//phist[8*i+k]=*(extlen+7-k+8*(i+exbuflen));
				}

			el=g-exbuflen*8-16*(mext+2);	

			if (addDataEmbedJudge == true)
			{
				dataLen[t] = 8 * (el / 8);                                           
				memcpy(ExtractData + haveStored, extlen + exbuflen * 8 + 16 * (mext + 2), dataLen[t]);
				haveStored = haveStored + dataLen[t];
			}

		}
		else
		{
			lpoh=lpoh+g/8;

			for(i=0; i<g/8; i++)
			{
				*(lpextmap+i)=0;
				for(k=0; k<8; k++)
				{
					shiftvalue=*(extlen+7-k+8*i);
					*(lpextmap+i)=(*(lpextmap+i)<<1)+shiftvalue;
				}
			}

			el=g-(g/8)*8;

			if (addDataEmbedJudge == true)
			{
				dataLen[t] = 8 * (el / 8);                                           //���ƴ�ʱΪ0
				memcpy(ExtractData + haveStored, extlen + (g / 8) * 8, dataLen[t]);
				haveStored = haveStored + dataLen[t];
			}
		}

		/*�ָ�ΪԤ����ǰ��ֱ��ͼ*/
		if(exct>0) 
		{
			/*
			tml=*(lphist+2*mext+2);
			tmr=*(lphist+2*mext+3);
		

			for(k=0; k<mext+1-tmr; k=k+1)
			{
				pmx=*(lphist+2*mext+1-k)-1;
				y=0;

				for(row=0;row<ImageHeight;row++)
					for(column=0;column<ImageWidth;column++)
					{
						lm=*(wp+row*ImageWidth+column);
						if(lm==pmx)	
							y++;	
					}

				exct=exct-y;

				for(row=0;row<ImageHeight;row++)
					for(column=0;column<ImageWidth;column++)
					{
						lm=*(wp+row*ImageWidth+column);
						if(lm>pmx)	*(wp+row*ImageWidth+column)=lm+1;	
						else if(lm==pmx)
						{
							i=exct/8; 
							j=7+i*8-exct;
							tmp=*(lpextmap+i);
							m=(tmp>>j)&1;
							if(m==1)	
								*(wp+row*ImageWidth+column)=lm+1;	
							exct++;
						}
					}
				
				exct=exct-y;

			}
						

			for(k=tmr-1; k>-1; k--)
			{
				pmx=*(lphist+mext+1+k)-1;
				for(row=0;row<ImageHeight;row++)
					for(column=0;column<ImageWidth;column++)
					{
						lm=*(wp+row*ImageWidth+column);
						if(lm>pmx)	*(wp+row*ImageWidth+column)=lm+1;	
					}
			}
						

			for(k=0; k<mext+1-tml; k=k+1)
			{
				pmn=*(lphist+mext-k)+1;
				y=0;

				for(row=0;row<ImageHeight;row++)
					for(column=0;column<ImageWidth;column++)
					{
						lm=*(wp+row*ImageWidth+column);
						if(lm==pmn)	
							y++;	
					}

				exct=exct-y;

				for(row=0;row<ImageHeight;row++)
					for(column=0;column<ImageWidth;column++)
					{
						lm=*(wp+row*ImageWidth+column);
						if(lm<pmn)	*(wp+row*ImageWidth+column)=lm-1;	
						else if(lm==pmn)
						{
							i=exct/8; 
							j=7+i*8-exct;
							tmp=*(lpextmap+i);
							m=(tmp>>j)&1;
							if(m==1)	
								*(wp+row*ImageWidth+column)=lm-1;	
							exct++;
						}
					}

				exct=exct-y;

			}
			
		
			for(k=tml-1; k>-1; k--)
			{
				pmn=*(lphist+k)+1;

				for(row=0;row<ImageHeight;row++)
					for(column=0;column<ImageWidth;column++)
					{
						lm=*(wp+row*ImageWidth+column);
						if(lm<pmn)	*(wp+row*ImageWidth+column)=lm-1;	
					}

			}

			*/
			if (colorToGrayPreprocessRecover = false)
			{
				preProcessRecover(lphist, lpextmap, mext, exct, wp);
			}
		}
		else                         //zn=0��ȫ��empty bin
		{
			if (colorToGrayPreprocessRecover = false)
			{
				preProcessRecover(lphist, lpextmap, mext, 0, wp);  //chosen=3
			}
			/*
			for(k=0; k<mext+1; k=k+1)
			{
				pmn=*(lphist+mext-k)+1;

				for(row=0;row<ImageHeight;row++)
					for(column=0;column<ImageWidth;column++)
					{
						lm=*(wp+row*ImageWidth+column);
						if(lm<pmn)	*(wp+row*ImageWidth+column)=lm-1;	

					}

			}
			for(k=0; k<mext+1; k=k+1)
			{
				pmx=*(lphist+2*mext+1-k)-1;
				for(row=0;row<ImageHeight;row++)
					for(column=0;column<ImageWidth;column++)
					{
						lm=*(wp+row*ImageWidth+column);
						if(lm>pmx)	*(wp+row*ImageWidth+column)=lm+1;	
					}

			}
			*/
		}


	}


	



//	fclose(fex);

	return el;
}

//�״���ȡ
long CDeprejbgDoc::blkext(unsigned char *mp, int row, int column, int bs, long el, int t)
{
	//FILE *fwm=fopen("D:\\wmimg.txt","wb");
	int i, j, k, pn, tn, rn, tmb, pk, emr, eml, emn, emlm, emrp, error, x, y, a, b;
	bool flg;
	unsigned char tm, pm;
	unsigned int hr, hm, hl, hp;

	eml=emr=emlm=emrp=error=0;
	pn=0;
	a=b=0;

	if(t==0) tmb=-2*sizeof(long)+5;
	else if(t==0) tmb=-2*sizeof(long)+2;
	else tmb=0;


	tm=lb;
	pm=rb;

	//������ȡ�������һ��֮��
	for(i=0;i<ImageHeight-1;i++)
		for(j=0;j<ImageWidth;j++)
		{
			lm=*(mp+i*ImageWidth+j);

			if(lm<tm-1)
				*(mp+i*ImageWidth+j)=lm+1;
			else if(lm>pm+1)
				*(mp+i*ImageWidth+j)=lm-1;
			else if(lm==tm-1)
			{

				*(extlen+el)=1;                                    //��ȡ�����ݶ�������
				*(mp+i*ImageWidth+j)=lm+1;
				if(el<(testbuf_len+2*sizeof(long)+tmb)*8)
				{

				}
				else                                             
				{
					emn=el-(testbuf_len+2*sizeof(long)+tmb)*8;
					if(m_OriginalWatermark[emn%(wminfo->biHeight*wminfo->biWidth)]!=1)
					{
						error++;
					}
				}
					
				el++;

			}
			else if(lm==tm)
			{
				*(extlen+el)=0;                                         //��ȡ�����ݶ�������
				if(el<(testbuf_len+2*sizeof(long)+tmb)*8)
				{

				}
				else                                                           
				{
					emn=el-(testbuf_len+2*sizeof(long)+tmb)*8;
					if(m_OriginalWatermark[emn%(wminfo->biHeight*wminfo->biWidth)]!=0)
					{
						error++;
					}
				}
					
				el++;
			}
			else if(lm==pm)
			{

				//*(mp+(row*bs+i)*ImageWidth+column*bs+j)=lm-1;
				*(extlen+el)=0;
				if(el<(testbuf_len+2*sizeof(long)+tmb)*8)
				{

				}
				else
				{
					emn=el-(testbuf_len+2*sizeof(long)+tmb)*8;
					if(m_OriginalWatermark[emn%(wminfo->biHeight*wminfo->biWidth)]!=0)
					{
						error++;
					}
				}
					
				el++;

			}
			else if(lm==pm+1)
			{

				//*(mp+(row*bs+i)*ImageWidth+column*bs+j)=lm-1;
				*(extlen+el)=1;					
				*(mp+i*ImageWidth+j)=lm-1;
				if(el<(testbuf_len+2*sizeof(long)+tmb)*8)
				{

				}
				else
				{
					emn=el-(testbuf_len+2*sizeof(long)+tmb)*8;
					if(m_OriginalWatermark[emn%(wminfo->biHeight*wminfo->biWidth)]!=1)
					{
						error++;
					}
				}
					
				el++;

			}
		}	

	//���һ��������ȡ
	i=ImageHeight-1;
	for(j=16;j<ImageWidth;j++)
	{
		lm=*(mp+i*ImageWidth+j);

		if(lm<tm-1)
			*(mp+i*ImageWidth+j)=lm+1;
		else if(lm>pm+1)
			*(mp+i*ImageWidth+j)=lm-1;
		else if(lm==tm-1)
		{

			*(extlen+el)=1;
			*(mp+i*ImageWidth+j)=lm+1;
			if(el<(testbuf_len+2*sizeof(long)+tmb)*8)
			{
				if(*(lpembmap+el)!=1)
				{
					error++;
				}
			}
			else
			{
				emn=el-(testbuf_len+2*sizeof(long)+tmb)*8;
				if(m_OriginalWatermark[emn%(wminfo->biHeight*wminfo->biWidth)]!=1)
				{
					error++;
				}
			}
				
			el++;

		}
		else if(lm==tm)
		{
			*(extlen+el)=0;
			if(el<(testbuf_len+2*sizeof(long)+tmb)*8)
			{
				if(*(lpembmap+el)!=0)
				{
					error++;
				}
			}
			else
			{
				emn=el-(testbuf_len+2*sizeof(long)+tmb)*8;
				if(m_OriginalWatermark[emn%(wminfo->biHeight*wminfo->biWidth)]!=0)
				{
					error++;
				}
			}
				
			el++;
		}
		else if(lm==pm)
		{

			//*(mp+(row*bs+i)*ImageWidth+column*bs+j)=lm-1;
			*(extlen+el)=0;
			if(el<(testbuf_len+2*sizeof(long)+tmb)*8)
			{
				if(*(lpembmap+el)!=0)
				{
					error++;
				}
			}
			else
			{
				emn=el-(testbuf_len+2*sizeof(long)+tmb)*8;
				if(m_OriginalWatermark[emn%(wminfo->biHeight*wminfo->biWidth)]!=0)
				{
					error++;
				}
			}
				
			el++;

		}
		else if(lm==pm+1)
		{

			//*(mp+(row*bs+i)*ImageWidth+column*bs+j)=lm-1;
			*(extlen+el)=1;					
			*(mp+i*ImageWidth+j)=lm-1;
			if(el<(testbuf_len+2*sizeof(long)+tmb)*8)
			{
				if(*(lpembmap+el)!=1)
				{
					error++;
				}
			}
			else
			{
				emn=el-(testbuf_len+2*sizeof(long)+tmb)*8;
				if(m_OriginalWatermark[emn%(wminfo->biHeight*wminfo->biWidth)]!=1)
				{
					error++;
				}
			}
				
			el++;

		}
	}	

	return el;
	
}

//���״���ȡ֮���������ȡ
long CDeprejbgDoc::blkexs(unsigned char *mp, int row, int column, int bs, long el, int t)
{
	//FILE *fwm=fopen("D:\\wmimgs.txt","wb");
	int i, j, k, pn, tn, rn, tmb, pk, emr, eml, emn, emlm, emrp, error, x, y, a, b;
	bool flg;
	unsigned char tm, pm;
	unsigned int hr, hm, hl, hp;

	eml=emr=emlm=emrp=error=0;
	pn=0;
	a=b=0;

	if(t==0) tmb=-2*sizeof(long)+5;
	else if(t==0) tmb=-2*sizeof(long)+2;
	else tmb=0;


	tm=lb;
	pm=rb;

	/*������ȡ������extlen��*/
	for(i=0;i<ImageHeight;i++)
		for(j=0;j<ImageWidth;j++)
		{
			lm=*(mp+i*ImageWidth+j);

			if(lm<tm-1)
				*(mp+i*ImageWidth+j)=lm+1;
			else if(lm>pm+1)
				*(mp+i*ImageWidth+j)=lm-1;
			else if(lm==tm-1)
			{

				*(extlen+el)=1;
				*(mp+i*ImageWidth+j)=lm+1;
				if(el<(testbuf_len+2*sizeof(long)+tmb)*8)
				{

				}
				else
				{
					emn=el-(testbuf_len+2*sizeof(long)+tmb)*8;
					if(m_OriginalWatermark[emn%(wminfo->biHeight*wminfo->biWidth)]!=1)
					{
						error++;
					}
				}
					
				el++;

			}
			else if(lm==tm)
			{
				*(extlen+el)=0;
				if(el<(testbuf_len+2*sizeof(long)+tmb)*8)
				{

				}
				else
				{
					emn=el-(testbuf_len+2*sizeof(long)+tmb)*8;
					if(m_OriginalWatermark[emn%(wminfo->biHeight*wminfo->biWidth)]!=0)
					{
						error++;
					}
				}
					
				el++;
			}
			else if(lm==pm)
			{

				//*(mp+(row*bs+i)*ImageWidth+column*bs+j)=lm-1;
				*(extlen+el)=0;
				if(el<(testbuf_len+2*sizeof(long)+tmb)*8)
				{

				}
				else
				{
					emn=el-(testbuf_len+2*sizeof(long)+tmb)*8;
					if(m_OriginalWatermark[emn%(wminfo->biHeight*wminfo->biWidth)]!=0)
					{
						error++;
					}
				}
					
				el++;

			}
			else if(lm==pm+1)
			{

				//*(mp+(row*bs+i)*ImageWidth+column*bs+j)=lm-1;
				*(extlen+el)=1;					
				*(mp+i*ImageWidth+j)=lm-1;
				if(el<(testbuf_len+2*sizeof(long)+tmb)*8)
				{

				}
				else
				{
					emn=el-(testbuf_len+2*sizeof(long)+tmb)*8;
					if(m_OriginalWatermark[emn%(wminfo->biHeight*wminfo->biWidth)]!=1)
					{
						error++;
					}
				}
					
				el++;

			}
		}	


	return el;
	
}

void CDeprejbgDoc::desaveas()
{
	OPENFILENAME osembedfn;			//osembedfn--opensaveembededwatermarkfilename
	{
		osembedfn.lStructSize=sizeof(OPENFILENAME);
		osembedfn.hwndOwner=NULL;
		osembedfn.hInstance=NULL;
		osembedfn.lpstrFilter=TEXT("BMP image files *.bmp\0*.bmp\0All File *.*\0*.*\0\0");
		osembedfn.lpstrCustomFilter=NULL;
		osembedfn.nMaxCustFilter=0;
		osembedfn.nFilterIndex=1;
		osembedfn.lpstrFile=watermarkedfilename;
		osembedfn.nMaxFile=500;
		osembedfn.lpstrFileTitle=watermarkedfiletitle;
		osembedfn.nMaxFileTitle=500;
		osembedfn.lpstrInitialDir=NULL;
		osembedfn.lpstrTitle="SAVE bmp file";
		osembedfn.Flags=OFN_FILEMUSTEXIST;
		osembedfn.nFileOffset;
		osembedfn.nFileExtension;
		osembedfn.lpstrDefExt="BMP";
		osembedfn.lCustData=NULL;
		osembedfn.lpfnHook=NULL;
		osembedfn.lpTemplateName=NULL;
	}
	watermarkedfilename[0]='\0';
    if(	GetSaveFileName(&osembedfn)!=NULL)
	{
		FILE *fp;
		fp=fopen(watermarkedfilename,"wb");//"wb"? why not "w"?
		if(fp==NULL)
		{
			AfxMessageBox("Can Not Open File To Write");
			return ;
		}
		fwrite(lpwmimage,sizeof(unsigned char),m_ColorImageSize,fp);
		fclose(fp);
	}
}

//Ԥ����ֱ��ͼ�ָ�
void CDeprejbgDoc::preProcessRecover(unsigned char *lphist, unsigned char *lpextmap, unsigned char mext,long exct,unsigned char *wp)  
{
	unsigned short int column, row;
	int i, j, k, m, x, y, t, lr;
	unsigned char tml, tmr, pmx, pmn, tmp;

	if (exct>0)
	{
		tml = *(lphist + 2 * mext + 2);                      //���ԭʼ��bin����
		tmr = *(lphist + 2 * mext + 3);						 //�ұ�ԭʼ��bin����

		if (leftPreProcessRecover == false)
		{
			for (int k = 0; k < mext + 1 - tmr; k = k + 1)			//�Ӻ���ǰ�����Ȼָ��ұ�merge bin
			{
				pmx = *(lphist + 2 * mext + 1 - k) - 1;         
				y = 0;

				for (row = 0; row < ImageHeight; row++)
					for (column = 0; column < ImageWidth; column++)
					{
						lm = *(wp + row * ImageWidth + column);
						if (lm == pmx)
							y++;
					}

				exct = exct - y;

				for (row = 0; row < ImageHeight; row++)
					for (column = 0; column < ImageWidth; column++)
					{
						lm = *(wp + row * ImageWidth + column);
						if (lm > pmx)	*(wp + row * ImageWidth + column) = lm + 1;
						else if (lm == pmx)
						{
							i = exct / 8;
							j = 7 + i * 8 - exct;
							tmp = *(lpextmap + i);
							m = (tmp >> j) & 1;
							if (m == 1)
								*(wp + row * ImageWidth + column) = lm + 1;
							exct++;
						}
					}

				exct = exct - y;

			}

			for (k = tmr - 1; k > -1; k--)						//�Ӻ���ǰ���ָ��ұ�empty bin
			{
				pmx = *(lphist + mext + 1 + k) - 1;
				for (row = 0; row < ImageHeight; row++)
					for (column = 0; column < ImageWidth; column++)
					{
						lm = *(wp + row * ImageWidth + column);
						if (lm > pmx)	*(wp + row * ImageWidth + column) = lm + 1;
					}

			}
		}


		for (k = 0; k < mext + 1 - tml; k = k + 1)				//�Ӻ���ǰ���ָ����merge bin
		{
			pmn = *(lphist + mext - k) + 1;
			y = 0;

			for (row = 0; row < ImageHeight; row++)
				for (column = 0; column < ImageWidth; column++)
				{
					lm = *(wp + row * ImageWidth + column);
					if (lm == pmn)
						y++;
				}

			exct = exct - y;

			for (row = 0; row < ImageHeight; row++)
				for (column = 0; column < ImageWidth; column++)
				{
					lm = *(wp + row * ImageWidth + column);
					if (lm < pmn)	*(wp + row * ImageWidth + column) = lm - 1;
					else if (lm == pmn)
					{
						i = exct / 8;
						j = 7 + i * 8 - exct;
						tmp = *(lpextmap + i);
						m = (tmp >> j) & 1;
						if (m == 1)
							*(wp + row * ImageWidth + column) = lm - 1;
						exct++;
					}
				}

			exct = exct - y;

		}

		for (k = tml - 1; k > -1; k--)                      //�Ӻ���ǰ���ָ����empty bin
		{
			pmn = *(lphist + k) + 1;
			
			for (row = 0; row < ImageHeight; row++)
				for (column = 0; column < ImageWidth; column++)
				{
					lm = *(wp + row * ImageWidth + column);
					if (lm < pmn)	*(wp + row * ImageWidth + column) = lm - 1;
				}

		}
	}
	/*
	else if (exct == 0)
	{
		tml = *(lphist + 2 * mext + 2);
		tmr = *(lphist + 2 * mext + 3);

		for (k = tml - 1; k > -1; k--)
		{
			pmn = *(lphist + k) + 1;

			for (row = 0; row < ImageHeight; row++)
				for (column = 0; column < ImageWidth; column++)
				{
					lm = *(wp + row * ImageWidth + column);
					if (lm < pmn)	*(wp + row * ImageWidth + column) = lm - 1;
				}

		}

		for (k = tmr - 1; k > -1; k--)
		{
			pmx = *(lphist + mext + 1 + k) - 1;
			for (row = 0; row < ImageHeight; row++)
				for (column = 0; column < ImageWidth; column++)
				{
					lm = *(wp + row * ImageWidth + column);
					if (lm > pmx)	*(wp + row * ImageWidth + column) = lm + 1;
				}

		}

	}
	*/
	else if (exct==0)
	{
		for (k = 0; k < mext + 1; k = k + 1)                  //���
		{
			pmn = *(lphist + mext - k) + 1;

			for (row = 0; row < ImageHeight; row++)
				for (column = 0; column < ImageWidth; column++)
				{
					lm = *(wp + row * ImageWidth + column);
					if (lm < pmn)	*(wp + row * ImageWidth + column) = lm - 1;
				}
		}

		if (leftPreProcessRecover == false)
		{
			for (k = 0; k < mext + 1; k = k + 1)                //�ұ�
			{
				pmx = *(lphist + 2 * mext + 1 - k) - 1;
				for (row = 0; row < ImageHeight; row++)
					for (column = 0; column < ImageWidth; column++)
					{
						lm = *(wp + row * ImageWidth + column);
						if (lm > pmx)	*(wp + row * ImageWidth + column) = lm + 1;
					}
			}
		}
		
	}
	
}

//��ɫpsnr����
double CDeprejbgDoc::psnrColorCalculate(unsigned char *originalImage, unsigned char *enhancedImage)
{
	double psnr;
	int i, j;
	double h,di;

	int ColorHead = m_ColorImageSize - ImageWidth * ImageHeight * sizeof(unsigned char) * 3;   //��ɫͼ��λͼ���

	psnr = 0.0;

	//����psnr = sum((origiamge[i][j]-preimage[i][j])^2)
	for (int pixel = ColorHead; pixel < m_ColorImageSize; pixel++)
	{
		i = *(originalImage + pixel);		
		j = *(enhancedImage + pixel);		
		h = i - j;
		psnr = psnr + h * h;

	}

	di = psnr / (ImageHeight*ImageWidth*3);

	psnr = 10 * log10((255 * 255) / di);

	return psnr;
}

//��ɫͼ�����ӵĺ�������
void CDeprejbgDoc::Init()
{
	hRimage = NULL;
	hGimage = NULL;
	hBimage = NULL;

	lpRimage = NULL;
	lpGimage = NULL;
	lpBimage = NULL;
	lpoimage = NULL;


	//MAX
	hMaximage = NULL;
	lpMaximage = NULL;
	hMediaimage = NULL;
	lpMediaimage = NULL;
	hMinimage = NULL;
	lpMinimage = NULL;
	hCimage = NULL;
	lpCimage = NULL;
	hC2image = NULL;
	lpC2image = NULL;
}

void CDeprejbgDoc::deoriRGBPre(unsigned char *originalImage)
{
	int g = 1024 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	int g2 = m_ColorImageSize - sizeof(unsigned char)*ImageWidth*ImageHeight * 3;

	//-------------------------------------MAX---------------------------------------------------
	//������¼�����MAX����
	if (hMaximage)
	{
		GlobalFree(hMaximage);
		hMaximage = NULL;
	}
	hMaximage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	lpMaximage = (unsigned char*)hMaximage;
	
	if (hMreimage)
	{
		GlobalFree(hMreimage);
		hMreimage = NULL;
	}
	hMreimage = GlobalAlloc(GMEM_FIXED, ImageWidth*ImageHeight*4);
	lpMreimage = (int*)hMreimage;
	
	if (hMediaimage)
	{
		GlobalFree(hMediaimage);
		hMediaimage = NULL;
	}
	hMediaimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	lpMediaimage = (unsigned char*)hMediaimage;
	
	if (hMereimage)
	{
		GlobalFree(hMereimage);
		hMereimage = NULL;
	}
	hMereimage = GlobalAlloc(GMEM_FIXED, ImageWidth*ImageHeight*4);
	lpMereimage = (int*)hMereimage;
	
	if (hMinimage)
	{
		GlobalFree(hMinimage);
		hMinimage = NULL;
	}
	hMinimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	lpMinimage = (unsigned char*)hMinimage;
	
	if (hMireimage)
	{
		GlobalFree(hMireimage);
		hMireimage = NULL;
	}
	hMireimage = GlobalAlloc(GMEM_FIXED, ImageWidth*ImageHeight*4);
	
	lpMireimage = (int*)hMireimage;
	
	if (hCimage)
	{
		GlobalFree(hCimage);
		hCimage = NULL;
	}
	hCimage = GlobalAlloc(GMEM_FIXED, ImageWidth*ImageHeight * 8);
	lpCimage = (double*)hCimage;
	
	if (hC2image)
	{
		GlobalFree(hC2image);
		hC2image = NULL;
	}
	hC2image = GlobalAlloc(GMEM_FIXED, ImageWidth*ImageHeight * 8);
	lpC2image = (double*)hC2image;

	//����RGB������
	
	pixel sort[3];
	int k = 0;
	for (int i = g2; i < m_ColorImageSize; i += 3)
	{
		sort[0].val = *(originalImage + i);          //B
		sort[0].t = 0;
		sort[1].val = *(originalImage + i+1);          //G
		sort[1].t = 1;
		sort[2].val = *(originalImage + i+2);          //R
		sort[2].t = 2;
		
		//��С��������
		pixel d;
		if (sort[0].val>sort[1].val) { d = sort[0]; sort[0] = sort[1]; sort[1] = d; }
		if (sort[0].val>sort[2].val) { d = sort[0]; sort[0] = sort[2]; sort[2] = d; }
		if (sort[1].val>sort[2].val) { d = sort[1]; sort[1] = sort[2]; sort[2] = d; }

		*(lpMinimage + g + k) = sort[0].val;
		*(lpMireimage + k) = sort[0].t;
		*(lpMediaimage + g + k) = sort[1].val;
		*(lpMereimage + k) = sort[1].t;
		*(lpMaximage + g + k) = sort[2].val;
		*(lpMreimage + k) = sort[2].t;
		*(lpCimage +k) = (double)(sort[1].val - sort[0].val) / (double)(sort[2].val - sort[0].val);   //media-min/max-min
		*(lpC2image + k) = (double)(sort[0].val) / (double)(sort[2].val);               //min/max
		
		k++;
		
	}
	
	
}

void CDeprejbgDoc::deoriRGB()
{
	int mm = 30;
	int g = 1024 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	int g2 = m_ColorImageSize - ImageWidth * ImageHeight * sizeof(unsigned char) * 3;
	deoriRGBPre(lporigimage);

	//��¼lporigimage��ָ��
	lpoimage = lporigimage;

	//����MAXͨ��
	lporigimage = lpMaximage;
	deori(mm);
	if (hMaximage)
	{
		GlobalFree(hMaximage);
		hMaximage = NULL;
	}
	hMaximage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	lpMaximage = (unsigned char*)hMaximage;
	memcpy(lpMaximage, lpwmimage, m_ImageSize);

	//����Minͨ��
	//lporigimage = lpMinimage;
	//deori();
	if (hMinimage)
	{
		GlobalFree(hMinimage);
		hMinimage = NULL;
	}
	hMinimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	lpMinimage = (unsigned char*)hMinimage;
	int z = 0;
	for (int i = g; i < m_ImageSize; i++)
	{
		double c2 = *(lpC2image + z);
		double c = *(lpCimage + z);
		//double min = *(lpMinimage + i);
		double max = *(lpMaximage + i);
		//double median = *(lpMediaimage + i);
		if (max != 0)
			*(lpMinimage + i) = max * c2 + 0.5;      //��������
		else
			*(lpMinimage + i) = 0;
		z++;
	}



	//memcpy(lpMinimage, lpwmimage, m_ImageSize);

	//����Mediaͨ��

	if (hMediaimage)
	{
		GlobalFree(hMediaimage);
		hMediaimage = NULL;
	}
	hMediaimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	lpMediaimage = (unsigned char*)hMediaimage;
	int y = 0;
	for (int i = g; i < m_ImageSize; i++)
	{
		double c = *(lpCimage + y);
		double c2 = *(lpC2image + y);
		double max = *(lpMaximage + i);
		double min = *(lpMinimage + i);
		//double median = *(lpMediaimage + i);
		if (max != min)
			*(lpMediaimage + i) = max * (c - c2 * c + c2) + 0.5;   //��������
		else
			*(lpMediaimage + i) = max;
		y++;
	}

	/*
	//�������B G R������
	FILE *fpB = fopen("C:\\Users\\May_2\\Desktop\\wmlpMaximage.txt", "wb");
	FILE *fpG = fopen("C:\\Users\\May_2\\Desktop\\wmlpMinimage3.txt", "wb");
	FILE *fpR = fopen("C:\\Users\\May_2\\Desktop\\wmlpMediaimage3.txt", "wb");
	for (int i = g; i < m_ImageSize; i++)
	{
		fprintf(fpB, "%d ", *(lpMaximage + i));
		fprintf(fpG, "%d ", *(lpMinimage + i));
		fprintf(fpR, "%d ", *(lpMediaimage + i));
	}
	fclose(fpB);
	fclose(fpG);
	fclose(fpR);
	*/

	/*
	//����Rͨ��
	lporigimage = lpRimage;
	deori();
	if (hRimage)
	{
		GlobalFree(hRimage);
		hRimage = NULL;
	}
	hRimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	lpRimage = (unsigned char*)hRimage;
	memcpy(lpRimage, lpwmimage, m_ImageSize);

	//����Gͨ��
	lporigimage = lpGimage;
	deori();
	if (hGimage)
	{
		GlobalFree(hGimage);
		hGimage = NULL;
	}
	hGimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	lpGimage = (unsigned char*)hGimage;
	memcpy(lpGimage, lpwmimage, m_ImageSize);

	//����Bͨ��
	lporigimage = lpBimage;
	deori();
	if (hBimage)
	{
		GlobalFree(hBimage);
		hBimage = NULL;
	}
	hBimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	lpBimage = (unsigned char*)hBimage;
	memcpy(lpBimage, lpwmimage, m_ImageSize);

	*/

	//��RGB��ֵ������ڴ�
	if (hwmimage)
	{
		GlobalFree(hwmimage);
		hwmimage = NULL;
	}
	hwmimage = GlobalAlloc(GMEM_FIXED, m_ColorImageSize);
	lpwmimage = (unsigned char*)hwmimage;



	//��ָ
	lporigimage = lpoimage;

	//���ļ�ͷд�����ָ��
	for (int i = 0; i < g2; i++)
	{
		*(lpwmimage + i) = *(lporigimage + i);
	}

	int max;
	int t;
	int k = 0;
	int j = g;
	for (int i = g2; i < m_ColorImageSize; i += 3)
	{

		*(lpwmimage + i + *(lpMreimage + k)) = *(lpMaximage + j);
		*(lpwmimage + i + *(lpMereimage + k)) = *(lpMediaimage + j);
		*(lpwmimage + i + *(lpMireimage + k)) = *(lpMinimage + j);

		j++;
		k++;
	}


	/*
	int k = 0;
	for (int i = g2; i < m_ColorImageSize; i += 3)
	{
		*(lpwmimage + i) = *(lpBimage + g + k);
		*(lpwmimage + i + 1) = *(lpGimage + g + k);
		*(lpwmimage + i + 2) = *(lpRimage + g + k);
		k++;
	}

	//��ָ
	lporigimage = lpoimage;

	//���ļ�ͷд�����ָ��
	for (int i = 0; i < m_ColorImageSize; i++)
	{
		*(lpwmimage + i) = *(lporigimage + i);
	}
	*/

	//FILE *fp = fopen("C:\\Users\\dell\\Desktop\\deori.txt", "wb");
	//for (int i = g2; i < m_ColorImageSize; i += 3)
	//{
	//	fprintf(fp, "(%d,%d,%d)", *(lpwmimage + i), *(lpwmimage + i + 1), *(lpwmimage + i + 2));
	//}
	//fclose(fp);
	/*
	//�������B G R������
	int u = 0;
	FILE *fpB = fopen("C:\\Users\\May_2\\Desktop\\Mediaimage.txt", "wb");
	FILE *fpA = fopen("C:\\Users\\May_2\\Desktop\\Aimage.txt", "wb");
	FILE *fpR = fopen("C:\\Users\\May_2\\Desktop\\Maximage.txt", "wb");
	FILE *fpH = fopen("C:\\Users\\May_2\\Desktop\\Minimage.txt", "wb");
	for (int i = g2; i < m_ColorImageSize; i=i+3)
	{
		fprintf(fpB, "%d ", *(lpCimage +u));
		fprintf(fpA, "(");
		fprintf(fpA, "%d ", *(lpwmimage + i));
		fprintf(fpA, ",");
		fprintf(fpA, "%d ", *(lpwmimage + i+1));
		fprintf(fpA, ",");
		fprintf(fpA, "%d ", *(lpwmimage + i+2));
		fprintf(fpA, ")");
		fprintf(fpR, "%d ", *(lpMaximage + g+u));
		fprintf(fpH, "%d ", *(lpMinimage + g + u));
		u++;
	}
	fclose(fpB);
	fclose(fpA);
	fclose(fpR);
	fclose(fpH);
	*/
}

void CDeprejbgDoc::deoriRGBSamePlus() 
{
	int s =50;
	for (int pairs =s; pairs < s+1; pairs++)
	{
		int GrayHead = 1024 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
		int ColorHead = m_ColorImageSize - ImageWidth * ImageHeight * sizeof(unsigned char) * 3;
		deoriRGBPre(lporigimage);
		//��¼lporigimage��ָ��
		lpoimage = lporigimage;

		int metInMax = pairs - 1;
		long exctMaxChannel, testbuf_lenMaxChannel;

		//����MAXͨ��
		onlyPreProcess = true;

		lporigimage = lpMaximage;
		deori(pairs);

		if (hMaxEnimage)
		{
			GlobalFree(hMaxEnimage);
			hMaxEnimage = NULL;
		}
		hMaxEnimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
		lpMaxEnimage = (unsigned char*)hMaxEnimage;

		memcpy(lpMaxEnimage,lpwmimage,m_ImageSize);

		exctMaxChannel = zn;
		testbuf_lenMaxChannel = testbuf_len;
		if (hprehistMaxChannel)
		{
			GlobalFree(hprehistMaxChannel);
			hprehistMaxChannel = NULL;
		}
		hprehistMaxChannel = GlobalAlloc(GMEM_FIXED, 2 * (metInMax + 2) * sizeof(unsigned char));
		lpprehistMaxChannel = (unsigned char *)hprehistMaxChannel;
		memcpy(lpprehistMaxChannel, lpprehist, 2 * (metInMax + 2) * sizeof(unsigned char));
		if (isMerge == true)
		{
			if (hlocmapMaxChannel)
			{
				GlobalFree(hlocmapMaxChannel);
				hlocmapMaxChannel = NULL;
			}
			hlocmapMaxChannel = GlobalAlloc(GMEM_FIXED, lml);
			locmapMaxChannel = (unsigned char *)hlocmapMaxChannel;
			memcpy(locmapMaxChannel, locmap, lml);
		}
		
		onlyPreProcess = false;

		if (hMaximageTemporary)
		{
			GlobalFree(hMaximageTemporary);
			hMaximageTemporary = NULL;
		}
		hMaximageTemporary = GlobalAlloc(GMEM_FIXED, m_ImageSize);
		lpMaximageTemporary = (unsigned char*)hMaximageTemporary;

		memcpy(lpMaximageTemporary, lpMaxEnimage, m_ImageSize);

		int reservedBin = 0;
		int metInMaxEmbed = metInMax - reservedBin;
		long g = 0, el;
		for (int m = 0; m < metInMaxEmbed + 1; m++)
		{
			el = derem(lpMaximageTemporary, lpMaxEnimage, m, metInMaxEmbed, testbuf_lenMaxChannel, exctMaxChannel, locmapMaxChannel, lpprehistMaxChannel);                  //����Ƕ��
			g = g + el;
			memcpy(lpMaxEnimage, lpMaximageTemporary, m_ImageSize);
			/*
			if (failed == 1)
			{
				maxPairs = m;
				break;
			}
			*/
		}


		/*
		if (hMaximage)
		{
			GlobalFree(hMaximage);
			hMaximage = NULL;
		}
		hMaximage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
		lpMaximage = (unsigned char*)hMaximage;
		memcpy(lpMaximage, lpwmimage, m_ImageSize); 
		*/

		for (int i = GrayHead; i < m_ImageSize; i++)
		{
			unsigned char maxEnShift, maxOriShift;
			maxEnShift = *(lpMaxEnimage + i) + reservedBin;
			*(lpMaxEnimage + i) = maxEnShift;
			if (maxEnShift > 255)
				break;
		}

		//����Minͨ��
		/*
		if (hMinimage)
		{
			GlobalFree(hMinimage);
			hMinimage = NULL;
		}
		hMinimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
		lpMinimage = (unsigned char*)hMinimage;
		*/

		if (hMinEnimage)
		{
			GlobalFree(hMinEnimage);
			hMinEnimage = NULL;
		}
		hMinEnimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
		lpMinEnimage = (unsigned char*)hMinEnimage;

		/*
		int z = 0;
		for (int i = g; i < m_ImageSize; i++)
		{
			double c2 = *(lpC2image + z);
			double c = *(lpCimage + z);
			//double min = *(lpMinimage + i);
			double max = *(lpMaxEnimage + i);
			//double median = *(lpMediaimage + i);
			if (max != 0)
				*(lpMinEnimage + i) = max * c2 + 0.5;
			else
				*(lpMinEnimage + i) = 0;
			z++;
		}
		*/

		//����Mediaͨ��
		/*
		if (hMediaimage)
		{
			GlobalFree(hMediaimage);
			hMediaimage = NULL;
		}
		hMediaimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
		lpMediaimage = (unsigned char*)hMediaimage;
		*/

		if (hMedianEnimage)
		{
			GlobalFree(hMedianEnimage);
			hMedianEnimage = NULL;
		}
		hMedianEnimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
		lpMedianEnimage = (unsigned char*)hMedianEnimage;

		/*
		int y = 0;
		for (int i = g; i < m_ImageSize; i++)
		{
			double c = *(lpCimage + y);
			double c2 = *(lpC2image + y);
			double max = *(lpMaxEnimage + i);
			double min = *(lpMinEnimage + i);
			//double median = *(lpMediaimage + i);
			if (max != min)
				*(lpMedianEnimage + i) = max * (c - c2 * c + c2) + 0.5;
			else
				*(lpMedianEnimage + i) = max;
			y++;
		}
		*/

		int minError=0,medianError=0,minMax=0;
		int negativeNumMin=0, negativeNumMedian = 0;
		for (int i = GrayHead; i < m_ImageSize; i++)
		{
			int maxOri, medianOri, minOri, maxEn, medianEn, minEn;
			
			maxOri = *(lpMaximage + i);
			maxEn = *(lpMaxEnimage + i);
			medianOri = *(lpMediaimage + i);
			minOri = *(lpMinimage + i);

			//*(lpMedianEnimageInt + i) = medianOri + (maxEn - maxOri);
			medianEn= medianOri + (maxEn - maxOri);
			if (medianEn < 0)
			{
				negativeNumMedian++;
				//*(lpMedianEnimage + i) = 0;
			}
			else
			{
				//*(lpMedianEnimage + i) = medianEn;
			}
			if (medianEn < medianError)	medianError = medianEn;
			//*(lpMinEnimageInt + i) = minOri + (maxEn - maxOri);
			minEn= minOri + (maxEn - maxOri);
			if (minEn < minError)	minError = minEn;
			if (minEn < 0) 
			{
				negativeNumMin++;
				//*(lpMinEnimage + i) = 0;
			}
			else
			{
				//*(lpMinEnimage + i) = minEn;
			}

		}

		/*ȷ��reservedBin������minError/2*/
		double absminError = abs(minError);
		reservedBin = absminError / 2 + 0.5;
		//reservedBin = 25;
		/*����һ��Ƕ��*/
		//����MAXͨ��
		onlyPreProcess = true;

		lporigimage = lpMaximage;
		deori(pairs);

		if (hMaxEnimage)
		{
			GlobalFree(hMaxEnimage);
			hMaxEnimage = NULL;
		}
		hMaxEnimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
		lpMaxEnimage = (unsigned char*)hMaxEnimage;

		memcpy(lpMaxEnimage, lpwmimage, m_ImageSize);

		exctMaxChannel = zn;
		testbuf_lenMaxChannel = testbuf_len;
		if (hprehistMaxChannel)
		{
			GlobalFree(hprehistMaxChannel);
			hprehistMaxChannel = NULL;
		}
		hprehistMaxChannel = GlobalAlloc(GMEM_FIXED, 2 * (metInMax + 2) * sizeof(unsigned char));
		lpprehistMaxChannel = (unsigned char *)hprehistMaxChannel;
		memcpy(lpprehistMaxChannel, lpprehist, 2 * (metInMax + 2) * sizeof(unsigned char));
		if (isMerge == true)
		{
			if (hlocmapMaxChannel)
			{
				GlobalFree(hlocmapMaxChannel);
				hlocmapMaxChannel = NULL;
			}
			hlocmapMaxChannel = GlobalAlloc(GMEM_FIXED, lml);
			locmapMaxChannel = (unsigned char *)hlocmapMaxChannel;
			memcpy(locmapMaxChannel, locmap, lml);
		}

		onlyPreProcess = false;

		if (hMaximageTemporary)
		{
			GlobalFree(hMaximageTemporary);
			hMaximageTemporary = NULL;
		}
		hMaximageTemporary = GlobalAlloc(GMEM_FIXED, m_ImageSize);
		lpMaximageTemporary = (unsigned char*)hMaximageTemporary;

		memcpy(lpMaximageTemporary, lpMaxEnimage, m_ImageSize);

		metInMaxEmbed = metInMax - reservedBin;
		g = 0;
		el = 0;
		for (int m = 0; m < metInMaxEmbed + 1; m++)
		{
			el = derem(lpMaximageTemporary, lpMaxEnimage, m, metInMaxEmbed, testbuf_lenMaxChannel, exctMaxChannel, locmapMaxChannel, lpprehistMaxChannel);                  //����Ƕ��
			g = g + el;
			memcpy(lpMaxEnimage, lpMaximageTemporary, m_ImageSize);
			/*
			if (failed == 1)
			{
				maxPairs = m;
				break;
			}
			*/
		}
		payload = g;

		for (int i = GrayHead; i < m_ImageSize; i++)
		{
			unsigned char maxEnShift, maxOriShift,maxEn;
			maxEn = *(lpMaxEnimage + i);
			maxEnShift = *(lpMaxEnimage + i) + reservedBin;
			*(lpMaxEnimage + i) = maxEnShift;
			if (maxEnShift > 255)
				break;
		}

		if (hMinEnimage)
		{
			GlobalFree(hMinEnimage);
			hMinEnimage = NULL;
		}
		hMinEnimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
		lpMinEnimage = (unsigned char*)hMinEnimage;

		if (hMedianEnimage)
		{
			GlobalFree(hMedianEnimage);
			hMedianEnimage = NULL;
		}
		hMedianEnimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
		lpMedianEnimage = (unsigned char*)hMedianEnimage;

		minError = 0, medianError = 0, minMax = 0;
		negativeNumMin = 0, negativeNumMedian = 0;
		for (int i = GrayHead; i < m_ImageSize; i++)
		{
			int maxOri, medianOri, minOri, maxEn, medianEn, minEn;

			maxOri = *(lpMaximage + i);
			maxEn = *(lpMaxEnimage + i);
			medianOri = *(lpMediaimage + i);
			minOri = *(lpMinimage + i);

			//*(lpMedianEnimageInt + i) = medianOri + (maxEn - maxOri);
			medianEn = medianOri + (maxEn - maxOri);
			if (medianEn < 0)
			{
				negativeNumMedian++;
				*(lpMedianEnimage + i) = 0;
			}
			else
			{
				*(lpMedianEnimage + i) = medianEn;
			}
			if (medianEn < medianError)	medianError = medianEn;
			//*(lpMinEnimageInt + i) = minOri + (maxEn - maxOri);
			minEn = minOri + (maxEn - maxOri);
			if (minEn < minError)	minError = minEn;
			if (minEn < 0)
			{
				negativeNumMin++;
				*(lpMinEnimage + i) = 0;
			}
			else
			{
				*(lpMinEnimage + i) = minEn;
			}

		}

		//��RGB��ֵ������ڴ�
		if (hwmimage)
		{
			GlobalFree(hwmimage);
			hwmimage = NULL;
		}
		hwmimage = GlobalAlloc(GMEM_FIXED, m_ColorImageSize);
		lpwmimage = (unsigned char*)hwmimage;

		//��ָ
		lporigimage = lpoimage;

		//���ļ�ͷд�����ָ��
		for (int i = 0; i < ColorHead; i++)
		{
			*(lpwmimage + i) = *(lporigimage + i);
		}

		int k = 0;
		int j = GrayHead;
		for (int i = ColorHead; i < m_ColorImageSize; i += 3)
		{

			*(lpwmimage + i + *(lpMreimage + k)) = *(lpMaxEnimage + j);
			*(lpwmimage + i + *(lpMereimage + k)) = *(lpMedianEnimage + j);
			*(lpwmimage + i + *(lpMireimage + k)) = *(lpMinEnimage + j);

			j++;
			k++;
		}

		//��¼�����ͼ��
		lphsvimage = lpwmimage;

		met = metInMaxEmbed;

		psnr = psnrColorCalculate(lporigimage, lpwmimage);

		/*
		//������ͼ
		char filename[500];

		filename[0] = '\0';

		sprintf(filename, "E:\\experiment\\task\\result\\color-max-pi\\kodim24_%d.bmp",mm);

		FILE *fb;

		fb = fopen(filename, "wb");
		if (fb == NULL)
		{
			AfxMessageBox("Can Not Open File To Write");
			return;
		}
		fwrite(lpwmimage, sizeof(unsigned char), m_ColorImageSize, fb);
		fclose(fb);
		*/
	}
}

void CDeprejbgDoc::depreRGBPre(unsigned char *originalImage)
{
	int g = 1024 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	int g2 = m_ColorImageSize - sizeof(unsigned char)*ImageWidth*ImageHeight * 3;

	if (hRimage)
	{
		GlobalFree(hRimage);
		hRimage = NULL;
	}
	hRimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	lpRimage = (unsigned char*)hRimage;

	if (hGimage)
	{
		GlobalFree(hGimage);
		hGimage = NULL;
	}
	hGimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	lpGimage = (unsigned char*)hGimage;

	if (hBimage)
	{
		GlobalFree(hBimage);
		hBimage = NULL;
	}
	hBimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	lpBimage = (unsigned char*)hBimage;

	if (hMreimage)
	{
		GlobalFree(hMreimage);
		hMreimage = NULL;
	}
	hMreimage = GlobalAlloc(GMEM_FIXED, ImageWidth*ImageHeight * 4);
	lpMreimage = (int*)hMreimage;

	if (hMereimage)
	{
		GlobalFree(hMereimage);
		hMereimage = NULL;
	}
	hMereimage = GlobalAlloc(GMEM_FIXED, ImageWidth*ImageHeight * 4);
	lpMereimage = (int*)hMereimage;

	if (hMireimage)
	{
		GlobalFree(hMireimage);
		hMireimage = NULL;
	}
	hMireimage = GlobalAlloc(GMEM_FIXED, ImageWidth*ImageHeight * 4);

	lpMireimage = (int*)hMireimage;



	if (hCimage)
	{
		GlobalFree(hCimage);
		hCimage = NULL;
	}
	hCimage = GlobalAlloc(GMEM_FIXED, ImageWidth*ImageHeight * 8);
	lpCimage = (double*)hCimage;

	if (hC2image)
	{
		GlobalFree(hC2image);
		hC2image = NULL;
	}
	hC2image = GlobalAlloc(GMEM_FIXED, ImageWidth*ImageHeight * 8);
	lpC2image = (double*)hC2image;



	pixel sort[3];
	int k = 0;
	for (int i = g2; i < m_ColorImageSize; i += 3)
	{
		sort[0].val = *(originalImage + i);          //B
		sort[0].t = 0;
		sort[1].val = *(originalImage + i + 1);          //G
		sort[1].t = 1;
		sort[2].val = *(originalImage + i + 2);          //R
		sort[2].t = 2;

		//��С��������
		pixel d;
		if (sort[0].val > sort[1].val) { d = sort[0]; sort[0] = sort[1]; sort[1] = d; }
		if (sort[0].val > sort[2].val) { d = sort[0]; sort[0] = sort[2]; sort[2] = d; }
		if (sort[1].val > sort[2].val) { d = sort[1]; sort[1] = sort[2]; sort[2] = d; }

		*(lpMinimage + g + k) = sort[0].val;
		*(lpMireimage + k) = sort[0].t;
		*(lpMediaimage + g + k) = sort[1].val;
		*(lpMereimage + k) = sort[1].t;
		*(lpMaximage + g + k) = sort[2].val;
		*(lpMreimage + k) = sort[2].t;
		*(lpCimage + k) = (double)(sort[1].val - sort[0].val) / (double)(sort[2].val - sort[0].val);
		*(lpC2image + k) = (double)(sort[0].val) / (double)(sort[2].val);    //MIN/MAX;
		k++;
	}
	
}

void CDeprejbgDoc::depreRGB()
{
	int g = 1024 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	int g2 = m_ColorImageSize - ImageWidth*ImageHeight * sizeof(unsigned char) * 3;
	depreRGBPre(lpwmimage);

	if (hwmimage)
	{
		GlobalFree(hwmimage);
		hwmimage = NULL;
	}
	hwmimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	lpwmimage = (unsigned char*)hwmimage;

	//����Max����

	memcpy(lppreimage, lpMaximage, m_ImageSize);
	depre();
	if (hMaximage)
	{
		GlobalFree(hMaximage);
		hMaximage = NULL;
	}
	hMaximage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	lpMaximage = (unsigned char*)hMaximage;

	memcpy(lpMaximage, lpwmimage, m_ImageSize);
	//����Min����
	if (hMinimage)
	{
		GlobalFree(hMinimage);
		hMinimage = NULL;
	}
	hMinimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	lpMinimage = (unsigned char*)hMinimage;
	int z = 0;
	for (int i = g; i < m_ImageSize; i++)
	{
		double c2 = *(lpC2image + z);
		double max = *(lpMaximage + i);
		if (max != 0)
			*(lpMinimage + i) = c2 * max + 0.5;
		else
			*(lpMinimage + i) = 0;
		z++;
	}
//����medium����	

	if (hMediaimage)
	{
		GlobalFree(hMediaimage);
		hMediaimage = NULL;
	}
	hMediaimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	lpMediaimage = (unsigned char*)hMediaimage;
	int y = 0;
	for (int i = g; i < m_ImageSize; i++)
	{
		double c = *(lpCimage + y);
		double c2 = *(lpC2image + y);
		double max = *(lpMaximage + i);
		double min = *(lpMinimage + i);
		if (max != min)
			*(lpMediaimage + i) = max * (c - c2 * c + c2) + 0.5;      //��������
		else
			*(lpMediaimage + i) = max;
		y++;
	}

	//��RGB��ֵ������ڴ�
	if (hwmimage)
	{
		GlobalFree(hwmimage);
		hwmimage = NULL;
	}
	hwmimage = GlobalAlloc(GMEM_FIXED, m_ColorImageSize);
	lpwmimage = (unsigned char*)hwmimage;

	

	int max;
	int t;
	int k = 0;
	int j = g;
	for (int i = g2; i < m_ColorImageSize; i += 3)
	{

		*(lpwmimage + i + *(lpMreimage + k)) = *(lpMaximage + j);
		*(lpwmimage + i + *(lpMereimage + k)) = *(lpMediaimage + j);
		*(lpwmimage + i + *(lpMireimage + k)) = *(lpMinimage + j);

		j++;
		k++;
	}
	//���ļ�ͷд�����ָ��
	for (int i = 0; i < g2; i++)
	{
		*(lpwmimage + i) = *(lporigimage + i);
	}
	
}

void CDeprejbgDoc::Genhanced()
{
	//��ǿSֵ
	int pairs = 30;

	//�Ȳ���Ƕ���ɫlocationmap
	addDataEmbedJudge = false;

	//��¼lporigimage��ָ��
	lpoimage = lporigimage;

	int GrayHead = 1024 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);                 //�Ҷ�ͼ��λͼ�������
	int ColorHead = m_ColorImageSize - ImageWidth * ImageHeight * sizeof(unsigned char) * 3;   //��ɫͼ��λͼ���

	/*��¼λ��ͼ��������*/
	struct jbg85_enc_state s;
	int locationMapWidth;              //locationmap�Ŀ����뱣֤�ܱ�8����
	long locationMapSize;              //locationmap��С
	unsigned char location8;           //һ���ֽڣ���¼8��λ��

	locationMapWidth = 8 * ((ImageWidth + 7) / 8);           //����ȡ����8������
	locationMapSize = ImageHeight * locationMapWidth / 8;    //һ���ֽڴ�8��λ�����Ҫ�߳˿�����8

	/*��ʼ��*/
	if (hRchannel)
	{
		GlobalFree(hRchannel);
		hRchannel = NULL;
	}
	hRchannel = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	Rchannel = (unsigned char*)hRchannel;

	if (hGchannel)
	{
		GlobalFree(hGchannel);
		hGchannel = NULL;
	}
	hGchannel = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	Gchannel = (unsigned char*)hGchannel;

	
	if (hBchannel)
	{
		GlobalFree(hBchannel);
		hBchannel = NULL;
	}
	hBchannel = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	Bchannel = (unsigned char*)hBchannel;

	//λ��ͼ��ʼ��
	if (hGchannelMap)
	{
		GlobalFree(hGchannelMap);
		hGchannelMap = NULL;
	}
	hGchannelMap = GlobalAlloc(GMEM_FIXED, locationMapSize);
	GchannelMap = (unsigned char*)hGchannelMap;
	for (int i = 0; i < locationMapSize; i++)
		*(GchannelMap + i) = 0;

	/*RGBͨ���ֱ�ֵ*/
	int z = 0;
	for (int j = ColorHead; j < m_ColorImageSize; j = j + 3)
	{
		*(Rchannel + GrayHead + z) = *(lporigimage + j + 2);
		*(Gchannel + GrayHead + z) = *(lporigimage + j + 1);
		*(Bchannel + GrayHead + z) = *(lporigimage + j);
		z++;
	}

	//��ǿGͨ��
	lporigimage = Gchannel;
	//memcpy(lporigimage+GrayHead, Gchannel+GrayHead, m_ImageSize);
	deori(pairs);

	if (hGchannelEnhanced)
	{
		GlobalFree(hGchannelEnhanced);
		hGchannelEnhanced = NULL;
	}
	hGchannelEnhanced = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	GchannelEnhanced = (unsigned char*)hGchannelEnhanced;
	memcpy(GchannelEnhanced, lpwmimage, m_ImageSize);

	if (hRchannelEnhanced)
	{
		GlobalFree(hRchannelEnhanced);
		hRchannelEnhanced = NULL;
	}
	hRchannelEnhanced = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	RchannelEnhanced = (unsigned char*)hRchannelEnhanced;

	if (hBchannelEnhanced)
	{
		GlobalFree(hBchannelEnhanced);
		hBchannelEnhanced = NULL;
	}
	hBchannelEnhanced = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	BchannelEnhanced = (unsigned char*)hBchannelEnhanced;

	Rless = 0, Rmore = 0, Bless = 0, Bmore = 0;
	Rlow = 0, Rhigh = 255, Blow = 0, Bhigh = 255;

	/*Gͨ����ǿ��R��Bͨ����ֵ���Ÿı�*/
	for (int row = 0; row < ImageHeight; row++)
		for (int column = 0; column < locationMapWidth; column++)
		{
			int traverse = GrayHead + row * ImageWidth + column;                  //����ȫͼ������ֵ
			int a;
			int p;
			int tmp;
			a = *(traverse + GchannelEnhanced) + *(traverse + Rchannel) - *(traverse + Gchannel);              //a=G'+(R-G)
			p = *(traverse + GchannelEnhanced) + *(traverse + Bchannel) - *(traverse + Gchannel);              //p=G'+(B-G)

			if (column % 8 == 0)      location8 = 0;          //��8Ϊ���ȣ���ʼʱ����0

			if (column < ImageWidth)                          //������ԭͼ���ʱ�����д���
			{
				
				//����һ��ͨ�������������������
				if (a >= 0 && a <= 255 && p >= 0 && p <= 255)
				{
					*(RchannelEnhanced + traverse) = a;
					*(BchannelEnhanced + traverse) = p;
					location8 = (location8 << 1) + 0;
				}
				else
				{
					*(RchannelEnhanced + traverse) = *(Rchannel + traverse);
					*(BchannelEnhanced + traverse) = *(Bchannel + traverse);
					location8 = (location8 << 1) + 1;
				}

				/*
				//�ĸ�ͨ��������ĸ�ͨ������,�����������locationMap��¼
				if (a < 0)
				{
					//*(RchannelEnhanced + traverse) = 0;
					//if (a < Rlow) { Rlow = a; }                 //ͳ����С�����ֵ
					//Rless++;
					*(RchannelEnhanced + traverse) = *(Rchannel + traverse);
					tmp = 1;
				}
				else if (a > 255)
				{
					//*(RchannelEnhanced + traverse) = 255;
					//if (a >Rhigh) { Rhigh = a; }               //ͳ���������ֵ
					//Rmore++;
					*(RchannelEnhanced + traverse) = *(Rchannel + traverse);
					tmp = 1;
				}
				else
				{
					*(RchannelEnhanced + traverse) = a;
					tmp = 0;
				}

				if (p < 0)
				{
					//*(BchannelEnhanced + traverse) = 0;
					//if (p < Blow) { Blow = p; }               //ͳ����С�����ֵ
					//Bless++;
					*(BchannelEnhanced + traverse) = *(Bchannel + traverse);
					tmp = 1;
				}
				else if (p > 255)
				{
					//*(BchannelEnhanced + traverse) = 255;
					//if (p > Bhigh) { Bhigh = p; }             //ͳ���������ֵ
					//Bmore++;
					*(BchannelEnhanced + traverse) = *(Bchannel + traverse);
					tmp = 1;
				}
				else
				{
					*(BchannelEnhanced + traverse) = p;
					tmp = 0;
				}
				*/
			}
			else
				location8 = (location8 << 1) + 0;																		//������ԭͼ���ʱ��ʹ��0���

			if (column % 8 == 7)        *(GchannelMap + (row*locationMapWidth + column) / 8) = location8;            //����8λʱ��location8��ֵ��map��


			//GBestS = min;
			//BBestS = max;
		}

	if (hwmimage)
	{
		GlobalFree(hwmimage);
		hwmimage = NULL;
	}
	hwmimage = GlobalAlloc(GMEM_FIXED, m_ColorImageSize);
	lpwmimage = (unsigned char*)hwmimage;

	//���ļ�ͷд�����ָ��
	for (int m = 0; m < ColorHead; m++)
	{
		*(lpwmimage + m) = *(lpoimage + m);
	}

	z = GrayHead;
	for (int o = ColorHead; o < m_ColorImageSize; o = o + 3)
	{
		*(lpwmimage + o + 2) = *(RchannelEnhanced + z);
		*(lpwmimage + o + 1) = *(GchannelEnhanced + z);
		*(lpwmimage + o) = *(BchannelEnhanced + z);
		z++;
	}

	//��ָ
	lporigimage = lpoimage;

	/*locationMapѹ��*/
	if (hlocationCompression)
	{
		GlobalFree(hlocationCompression);
	}
	hlocationCompression = GlobalAlloc(GMEM_FIXED, locationMapSize);
	locationCompression = (unsigned char *)hlocationCompression;
	for (int i = 0; i < locationMapSize; i++)
		*(locationCompression + i) = 0;

	memcpy(locationCompression, GchannelMap, locationMapSize);

	testbuf_len = 0;

	testbuf = (unsigned char *)checkedmalloc(TESTBUF_SIZE);                        //ѹ�������

	jbg85_enc_init(&s, locationMapWidth, ImageHeight, testbuf_writel, NULL);                //��ʼ��
	jbg85_enc_options(&s, JBG_TPBON, 0, -1);                                       //��������

	for (int row=0;row<ImageHeight;row++)
		jbg85_enc_lineout(&s, locationCompression + row * locationMapWidth / 8, locationCompression + (row - 1)*locationMapWidth / 8, locationCompression + (row - 2)*locationMapWidth / 8);    //������һ�У�ǰһ�У�ǰǰһ��

	buflen = testbuf_len;

	memcpy(locationCompression, testbuf, buflen);

	ColorLocationMap(locationCompression, buflen, -1);

	addDataLen = buflen + sizeof(long);

	ColorToThreeTimesGray();
}

void CDeprejbgDoc::GenhancedRecovery()
{
	bool haveChannel = false;
	ColorToThreeTimesGrayRecovery(haveChannel);

	/*��¼λ��ͼ��������*/
	struct jbg85_dec_state d;
	int locationMapWidth;              //locationmap�Ŀ����뱣֤�ܱ�8����
	long locationMapSize;              //locationmap��С
	unsigned char location8;           //һ���ֽڣ���¼8��λ��
	unsigned char *image, *buffer;
	size_t plane_size, buffer_len;
	size_t cnt;

	locationMapWidth = 8 * ((ImageWidth + 7) / 8);           //����ȡ����8������
	locationMapSize = ImageHeight * locationMapWidth / 8;    //һ���ֽڴ�8��λ�����Ҫ�߳˿�����8

	int GrayHead = 1024 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);                 //�Ҷ�ͼ��λͼ�������
	int ColorHead = m_ColorImageSize - ImageWidth * ImageHeight * sizeof(unsigned char) * 3;   //��ɫͼ��λͼ���

	/*��ʼ��*/
	if (hGchannelEnhanced)
	{
		GlobalFree(hGchannelEnhanced);
		hGchannelEnhanced = NULL;
	}
	hGchannelEnhanced = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	GchannelEnhanced = (unsigned char*)hGchannelEnhanced;

	if (hRchannelEnhanced)
	{
		GlobalFree(hRchannelEnhanced);
		hRchannelEnhanced = NULL;
	}
	hRchannelEnhanced = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	RchannelEnhanced = (unsigned char*)hRchannelEnhanced;

	if (hBchannelEnhanced)
	{
		GlobalFree(hBchannelEnhanced);
		hBchannelEnhanced = NULL;
	}
	hBchannelEnhanced = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	BchannelEnhanced = (unsigned char*)hBchannelEnhanced;

	if (hRchannel)
	{
		GlobalFree(hRchannel);
		hRchannel = NULL;
	}
	hRchannel = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	Rchannel = (unsigned char*)hRchannel;

	if (hGchannel)
	{
		GlobalFree(hGchannel);
		hGchannel = NULL;
	}
	hGchannel = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	Gchannel = (unsigned char*)hGchannel;

	if (hBchannel)
	{
		GlobalFree(hBchannel);
		hBchannel = NULL;
	}
	hBchannel = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	Bchannel = (unsigned char*)hBchannel;

	/*RGBͨ���ֱ�ֵ*/
	int z = 0;
	for (int j = ColorHead; j < m_ColorImageSize; j = j + 3)
	{
		*(RchannelEnhanced + GrayHead + z) = *(lpwmimage + j + 2);
		*(GchannelEnhanced + GrayHead + z) = *(lpwmimage + j + 1);
		*(BchannelEnhanced + GrayHead + z) = *(lpwmimage + j);
		z++;
	}

	if (hwmimage)
	{
		GlobalFree(hwmimage);
		hwmimage = NULL;
	}
	hwmimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	lpwmimage = (unsigned char*)hwmimage;

	if (hpreimage)
	{
		GlobalFree(hpreimage);
		hpreimage = NULL;
	}
	hpreimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);		
	lppreimage = (unsigned char *)hpreimage;

	/*�ָ�Gͨ�����ݣ�����Gchannel��*/
	memcpy(lppreimage, GchannelEnhanced, m_ImageSize);   
	depre();
	memcpy(Gchannel, lpwmimage, m_ImageSize);

	/*��ѹ*/

	plane_size = ImageHeight * locationMapWidth;
	int result;

	//buffer_len = ((lmwidth >> 3) + !!(lmwidth & 7)) * 3;            //����������
	buffer_len = plane_size;
	buffer = (unsigned char *)checkedmalloc(buffer_len);           //�����������ڴ�
	image = (unsigned char *)checkedmalloc(plane_size);            //���ͼ������ڴ�
	jbg85_dec_init(&d, buffer, buffer_len, line_out, image);
	//result = jbg85_dec_in(&d, locationCompression, buflen, &cnt);             //�����ֱ��ǣ�ѹ��������testbuf��ѹ�������ݳ���buflen
	result = jbg85_dec_in(&d, colorLocationMap, ExtractMapLen, &cnt);             //�����ֱ��ǣ�ѹ��������testbuf��ѹ�������ݳ���buflen

	for (int row = 0; row < ImageHeight; row++)
		for (int column = 0; column < ImageWidth; column++)
		{
			int i = column / 8;                                                 //��location map�Ŀ�
			int j = 7 + i * 8 - column;                                             //�������ƶ�Ӧ��λ����
			unsigned char tmp = *(image + row * locationMapWidth / 8 + i);                               //��ȡ��Ӧ��Ԥ����location map���ݣ�һ��
			int m = (tmp >> j) & 1;                                               //mΪ��ȡ����location map��λ���ݣ�Ϊ0��1

			int traverse = GrayHead + row * ImageWidth + column;                  //����ȫͼ������ֵ
			int a;
			int p;
			a = *(traverse + Gchannel) + *(traverse + RchannelEnhanced) - *(traverse + GchannelEnhanced);              //a=G'+(R-G)
			p = *(traverse + Gchannel) + *(traverse + BchannelEnhanced) - *(traverse + GchannelEnhanced);              //p=G'+(B-G)

			if (m == 1)
			{
				*(Rchannel + traverse) = *(RchannelEnhanced + traverse);
				*(Bchannel + traverse) = *(BchannelEnhanced + traverse);
			}
			else if (m == 0)
			{
				*(Rchannel + traverse) = a;
				*(Bchannel + traverse) = p;
			}
		}

	if (hwmimage)
	{
		GlobalFree(hwmimage);
		hwmimage = NULL;
	}
	hwmimage = GlobalAlloc(GMEM_FIXED, m_ColorImageSize);
	lpwmimage = (unsigned char*)hwmimage;

	//���ļ�ͷд�����ָ��
	for (int m = 0; m < ColorHead; m++)
	{
		*(lpwmimage + m) = *(lpoimage + m);
	}

	z = GrayHead;
	for (int o = ColorHead; o < m_ColorImageSize; o = o + 3)
	{
		*(lpwmimage + o + 2) = *(Rchannel + z);
		*(lpwmimage + o + 1) = *(Gchannel + z);
		*(lpwmimage + o) = *(Bchannel + z);
		z++;
	}

	//��ָ
	lporigimage = lpoimage;

}
 
void CDeprejbgDoc::ColorLocationMap(unsigned char *ColorMap, long MapLength, int channel)
{
	unsigned char intermediary;
	unsigned char* lenmediary;
	unsigned char* channelmediary;
	int shiftnumber, shiftvalue;
	long *lengthEmbed;
	int *channelEmbed;
	int parameterLen;
	addDataEmbedJudge = true;
	
	if (channel > 2 || channel < 0)
	{
		if (haddDataEmbed)
		{
			GlobalFree(haddDataEmbed);
			haddDataEmbed = NULL;
		}
		haddDataEmbed = GlobalAlloc(GMEM_FIXED, (MapLength + sizeof(long)) * 8);
		addDataEmbed = (unsigned char *)haddDataEmbed;

		/*Maplength��ֵ��addDataEmbed��*/
		lengthEmbed = &MapLength;
		for (int i = 0; i < sizeof(long); i++)
		{
			lenmediary = (unsigned char *)lengthEmbed + i;
			intermediary = *lenmediary;
			for (int k = 0; k < 8; k++)
			{
				*(addDataEmbed + 8 * i + k) = 0;
				shiftnumber = k;
				shiftvalue = (intermediary >> shiftnumber) & 1;
				*(addDataEmbed + 8 * i + k) = shiftvalue;
			}
		}
		parameterLen = sizeof(long);
	}
	else if (channel == 0 || channel == 1 || channel == 2)
	{
		if (haddDataEmbed)
		{
			GlobalFree(haddDataEmbed);
			haddDataEmbed = NULL;
		}
		haddDataEmbed = GlobalAlloc(GMEM_FIXED, (MapLength + sizeof(long) + sizeof(int)) * 8);
		addDataEmbed = (unsigned char *)haddDataEmbed;

		//��Ƕ��map������Ƕ����ǿ��channel���ĸ�
		/*Maplength��ֵ��addDataEmbed��*/
		lengthEmbed = &MapLength;
		for (int i = 0; i < sizeof(long); i++)
		{
			lenmediary = (unsigned char *)lengthEmbed + i;
			intermediary = *lenmediary;
			for (int k = 0; k < 8; k++)
			{
				*(addDataEmbed + 8 * i + k) = 0;
				shiftnumber = k;
				shiftvalue = (intermediary >> shiftnumber) & 1;
				*(addDataEmbed + 8 * i + k) = shiftvalue;
			}
		}

		/*channel��ֵ��addDataEmbed��*/
		channelEmbed = &channel;
		for (int i = 0; i < sizeof(int); i++)
		{
			channelmediary = (unsigned char *)channelEmbed + i;
			intermediary = *channelmediary;
			for (int k = 0; k < 8; k++)
			{
				*(addDataEmbed + 8 * (sizeof(long) + i) + k) = 0;
				shiftnumber = k;
				shiftvalue = (intermediary >> shiftnumber) & 1;
				*(addDataEmbed + 8 * (sizeof(long) + i) + k) = shiftvalue;
			}
		}

		parameterLen = sizeof(long) + sizeof(int);
	}
	else
	{
		failed = true;
	}


	/*ColorMap��ֵ��addDataEmbed��*/
	for (int i = 0; i < MapLength; i++)
	{
		intermediary = *(ColorMap + i);
		for (int k = 0; k < 8; k++)
		{
			*(addDataEmbed + 8 * (i + parameterLen) + k) = 0;
			shiftnumber = k;
			shiftvalue = (intermediary >> shiftnumber) & 1;
			*(addDataEmbed + 8 * (i + parameterLen) + k) = shiftvalue;
		}
	}
	
	/*
	for (int i = 0; i < (MapLength + parameterLen)*8; i++)
	{
		FILE *payloadtxt;
		payloadtxt = fopen("E:\\experiment\\task\\ColorImage-7method\\addDataEmbed.txt", "a");
		if (payloadtxt != NULL)
			fprintf(payloadtxt, "%d \t", *(addDataEmbed+i));
		fclose(payloadtxt);
	}
	*/

}

void CDeprejbgDoc::ColorMapRecovery(bool haveChannel)
{
	int last;
	long *maplen;
	HANDLE hmaplen = NULL;
	int *channelChosen;
	HANDLE hchannelChosen = NULL;
	long start = 0;	
	unsigned char shiftvalue;
	unsigned char *intermediate;
	int parameterLen;

	/*�ҳ����һ����ȡ�������ݵ�����last*/
	for (int i = 63; i >= 0; i--)
	{
		if (dataLen[i] == 0)
			last = i - 1;
	}

	if (hmaplen)
	{
		GlobalFree(hmaplen);
		hmaplen = NULL;
	}
	hmaplen = GlobalAlloc(GMEM_FIXED, sizeof(long));
	maplen = (long *)hmaplen;

	/*�ȶ�ȡextractdata���һ����ȡ�������ݣ��˿������к���map����*/
	intermediate = (unsigned char *)maplen;
	for (int i = 0; i < sizeof(long); i++)
	{
		*(intermediate + i) = 0;
		for (int k = 0; k < 8; k++)
		{
			shiftvalue = *(ExtractData + haveStored - dataLen[last] + 8 * i + 7 - k);
			*(intermediate + i) = (*(intermediate + i) << 1) + shiftvalue;
		}
	}
	ExtractMapLen = *maplen;

	if (haveChannel == true)
	{
		/*��ȡ����ǿ���ĸ�channel*/
		parameterLen = sizeof(long) + sizeof(int);

		if (hchannelChosen)
		{
			GlobalFree(hchannelChosen);
			hchannelChosen = NULL;
		}
		hchannelChosen = GlobalAlloc(GMEM_FIXED, sizeof(int));
		channelChosen = (int *)hchannelChosen;

		intermediate = (unsigned char *)channelChosen;
		for (int i = 0; i < sizeof(int); i++)
		{
			*(intermediate + i) = 0;
			for (int k = 0; k < 8; k++)
			{
				shiftvalue = *(ExtractData + haveStored - dataLen[last] + 8 * (sizeof(long) + i) + 7 - k);
				*(intermediate + i) = (*(intermediate + i) << 1) + shiftvalue;
			}
		}
		channelChosenExtract = *channelChosen;
	}
	else
	{
		parameterLen = sizeof(long);
	}

	if (hExtractDataOrder)
	{
		GlobalFree(hExtractDataOrder);
		hExtractDataOrder = NULL;
	}
	hExtractDataOrder = GlobalAlloc(GMEM_FIXED, m_ImageSize * 24);
	ExtractDataOrder = (unsigned char*)hExtractDataOrder;

	/*��ExtractData��˳�������*/
	for (int i = last; i >= 0; i--)
	{
		haveStored = haveStored - dataLen[i];
		memcpy(ExtractDataOrder + start, ExtractData + haveStored, dataLen[i]);
		start = dataLen[i] + start;
	}
	
	for (int i = 0; i < addDataLen *8; i++)
	{
		/*
		FILE *payloadtxt;
		payloadtxt = fopen("E:\\experiment\\task\\ColorImage-7method\\ExtractDataOrder.txt", "a");
		if (payloadtxt != NULL)
			fprintf(payloadtxt, "%d \t", *(ExtractDataOrder + i));
		fclose(payloadtxt);
		*/

		if (*(ExtractDataOrder + i) != *(addDataEmbed + i))
		{
			FILE *payloadtxt;
			payloadtxt = fopen("E:\\experiment\\task\\ColorImage-7method\\different.txt", "a");
			if (payloadtxt != NULL)
				fprintf(payloadtxt, "%d \t", i);
			fclose(payloadtxt);
		}
	}
	
	/*�ӵ�����˳���ExtractDataOrderȡ��map*/
	if (hExtractMap)
	{
		GlobalFree(hExtractMap);
		hExtractMap = NULL;
	}
	hExtractMap = GlobalAlloc(GMEM_FIXED, ExtractMapLen * 8);
	ExtractMap = (unsigned char*)hExtractMap;
	memcpy(ExtractMap, ExtractDataOrder + parameterLen * 8, ExtractMapLen * 8);

	/*��λ�������õ�ѹ�����locationmap*/
	if (hcolorLocationMap)
	{
		GlobalFree(hcolorLocationMap);
		hcolorLocationMap = NULL;
	}
	hcolorLocationMap = GlobalAlloc(GMEM_FIXED, ExtractMapLen);
	colorLocationMap = (unsigned char*)hcolorLocationMap;

	for (int i = 0; i < ExtractMapLen; i++)
	{
		*(colorLocationMap + i) = 0;
		for (int k = 0; k < 8; k++)
		{
			shiftvalue = *(ExtractMap + 7 - k + 8 * i);
			*(colorLocationMap + i) = (*(colorLocationMap + i) << 1) + shiftvalue;
		}
	}

	addDataEmbedJudge = false;

}

//��ɫת��Ҷ�
void CDeprejbgDoc::ColorToThreeTimesGray()
{
	//��ǿ����
	int pairs = 1;

	//��¼lporigimage��ָ��
	lpoimage = lporigimage;

	//����������ǿ��Ĳ�ɫͼ����Ϊ����
	lporigimage = lpwmimage;

	//ת�Ҷ�ͼ
	unsigned long m_ImageSize_ori = 1024 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + ImageWidth * ImageHeight * sizeof(unsigned char);
	int ColorImageWidth = ImageWidth;
	ImageWidth = 3 * ImageWidth;   //RGB�ֱ𵥶������Ҷ�ͼ��һ������ֵ��ͼ���Ϊԭ��3��
	m_ImageSize = 1024 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + ImageWidth * ImageHeight * sizeof(unsigned char);
	
	//����ɫͼ��RGBͨ����ֵ�ֱ�ֵ���Ҷ�ͼ��ʽ
	int GrayHead = 1024 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);                 //�Ҷ�ͼ��λͼ�������
	int ColorHead = m_ColorImageSize - ColorImageWidth * ImageHeight * sizeof(unsigned char) * 3;   //��ɫͼ��λͼ���

	//��ʼ��
	if (hImage2D)
	{
		GlobalFree(hImage2D);
		hImage2D = NULL;
	}
	hImage2D = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	Image2D = (unsigned char*)hImage2D;

	//��RGB����Ҷ�
	int z = 0;
	for (int j = ColorHead; j < m_ColorImageSize; j = j + 3)
	{
		*(Image2D + GrayHead + z) = *(lporigimage + j);
		*(Image2D + GrayHead + z + 1) = *(lporigimage + j + 1);
		*(Image2D + GrayHead + z + 2) = *(lporigimage + j + 2);
		z = z + 3;
	}

	//��ʼ��
	if (hwmimage)
	{
		GlobalFree(hwmimage);
		hwmimage = NULL;
	}
	hwmimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	lpwmimage = (unsigned char*)hwmimage;

	//��ǿת����ĻҶ�ͼ
	lporigimage = Image2D;
	deori(pairs);

	if (hImage2DEnhanced)
	{
		GlobalFree(hImage2DEnhanced);
		hImage2DEnhanced = NULL;
	}
	hImage2DEnhanced = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	Image2DEnhanced = (unsigned char*)hImage2DEnhanced;
	memcpy(Image2DEnhanced, lpwmimage, m_ImageSize);

	//��ʼ��
	if (hwmimage)
	{
		GlobalFree(hwmimage);
		hwmimage = NULL;
	}
	hwmimage = GlobalAlloc(GMEM_FIXED, m_ColorImageSize);
	lpwmimage = (unsigned char*)hwmimage;

	//���ļ�ͷд�����ָ��
	for (int m = 0; m < ColorHead; m++)
	{
		*(lpwmimage + m) = *(lpoimage + m);
	}

	z = GrayHead;
	for (int o = ColorHead; o < m_ColorImageSize; o = o + 3)
	{
		*(lpwmimage + o) = *(Image2DEnhanced + z);
		*(lpwmimage + o + 1) = *(Image2DEnhanced + z + 1);
		*(lpwmimage + o + 2) = *(Image2DEnhanced + z + 2);
		z = z + 3;
	}

	//��ָ
	lporigimage = lpoimage;

	//��д
	m_ImageSize = m_ImageSize_ori;
	ImageWidth = ColorImageWidth;
}

void CDeprejbgDoc::ColorToThreeTimesGrayRecovery(bool haveChannel)
{
	/*��ȡ�������ݴ洢�ĳ�ʼ��*/
	if (hExtractData)
	{
		GlobalFree(hExtractData);
		hExtractData = NULL;
	}
	hExtractData = GlobalAlloc(GMEM_FIXED, m_ImageSize * 24);
	ExtractData = (unsigned char*)hExtractData;

	/*�������ݳ��ȵļ�¼����*/
	for (int i = 0; i < 64; i++)
	{
		dataLen[i] = 0;
	}
	
	//��ʼ������
	haveStored = 0;

	//��¼lporigimage��ָ��
	lpoimage = lporigimage;

	//ת�Ҷ�ͼ
	unsigned long int m_ImageSize_ori = m_ImageSize;
	int ColorImageWidth = ImageWidth;
	ImageWidth = 3 * ImageWidth;
	m_ImageSize = 1024 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + ImageWidth * ImageHeight * sizeof(unsigned char);

	//����ɫͼ��RGBͨ����ֵ�ֱ�ֵ���Ҷ�ͼ��ʽ
	int GrayHead = 1024 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);                 //�Ҷ�ͼ��λͼ�������
	int ColorHead = m_ColorImageSize - ColorImageWidth * ImageHeight * sizeof(unsigned char) * 3;   //��ɫͼ��λͼ���

	//��ɫͼ��תΪ�Ҷ�ͼ��
	//��ʼ��
	if (hImage2DEnhanced)
	{
		GlobalFree(hImage2DEnhanced);
		hImage2DEnhanced = NULL;
	}
	hImage2DEnhanced = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	Image2DEnhanced = (unsigned char*)hImage2DEnhanced;

	//��RGB��I����Ҷ�
	int z = 0;
	for (int j = ColorHead; j < m_ColorImageSize; j = j + 3)
	{
		// *(Image2D + GrayHead + z) = *(lporigimage + j) * 0.3 + *(lporigimage + j + 1) * 0.59 + *(lporigimage + j + 2) * 0.11;
		*(Image2DEnhanced + GrayHead + z) = *(lpwmimage + j);
		*(Image2DEnhanced + GrayHead + z + 1) = *(lpwmimage + j + 1);
		*(Image2DEnhanced + GrayHead + z + 2) = *(lpwmimage + j + 2);
		z = z + 3;
	}

	//��ǿ���������ʼ��
	if (hwmimage)
	{
		GlobalFree(hwmimage);
		hwmimage = NULL;
	}
	hwmimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	lpwmimage = (unsigned char*)hwmimage;

	if (hpreimage)
	{
		GlobalFree(hpreimage);
		hpreimage = NULL;
	}
	hpreimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);	
	lppreimage = (unsigned char *)hpreimage;

	//�ָ�
	memcpy(lppreimage, Image2DEnhanced, m_ImageSize);
	depre();

	//��ʼ��
	if (hImage2D)
	{
		GlobalFree(hImage2D);
		hImage2D = NULL;
	}
	hImage2D = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	Image2D = (unsigned char*)hImage2D;

	//�ָ��������
	memcpy(Image2D, lpwmimage, m_ImageSize);

	//���ļ�ͷд�����ָ��
	for (int m = 0; m < ColorHead; m++)
	{
		*(lpwmimage + m) = *(lpoimage + m);
	}

	z = GrayHead;
	for (int o = ColorHead; o < m_ColorImageSize; o = o + 3)
	{
		*(lpwmimage + o) = *(Image2D + z);
		*(lpwmimage + o + 1) = *(Image2D + z + 1);
		*(lpwmimage + o + 2) = *(Image2D + z + 2);
		z = z + 3;
	}

	//��ָ
	lporigimage = lpoimage;

	//��д
	m_ImageSize = m_ImageSize_ori;
	ImageWidth = ColorImageWidth;

	//���ã���ȡѹ��������
	ColorMapRecovery(haveChannel);
}

//��ǿmax���ģ�����ֱ����Gͨ����
void CDeprejbgDoc::enhancedMax() 
{
	//��ǿ����
	int pairs = 50;

	//��¼lporigimage��ָ��
	lpoimage = lporigimage;

	//����ɫͼ��RGBͨ����ֵ�ֱ�ֵ���Ҷ�ͼ��ʽ
	int GrayHead = 1024 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);                 //�Ҷ�ͼ��λͼ�������
	int ColorHead = m_ColorImageSize - ImageWidth * ImageHeight * sizeof(unsigned char) * 3;   //��ɫͼ��λͼ���

	/*��¼λ��ͼ��������*/
	struct jbg85_enc_state s;
	int locationMapWidth;              //locationmap�Ŀ����뱣֤�ܱ�8����
	long locationMapSize;              //locationmap��С
	unsigned char location8;           //һ���ֽڣ���¼8��λ��

	locationMapWidth = 8 * ((ImageWidth + 7) / 8);           //����ȡ����8������
	locationMapSize = ImageHeight * locationMapWidth / 8;    //һ���ֽڴ�8��λ�����Ҫ�߳˿�����8

	//��ʼ��
	if (hRchannel)
	{
		GlobalFree(hRchannel);
		hRchannel = NULL;
	}
	hRchannel = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	Rchannel = (unsigned char*)hRchannel;

	if (hGchannel)
	{
		GlobalFree(hGchannel);
		hGchannel = NULL;
	}
	hGchannel = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	Gchannel = (unsigned char*)hGchannel;

	if (hBchannel)
	{
		GlobalFree(hBchannel);
		hBchannel = NULL;
	}
	hBchannel = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	Bchannel = (unsigned char*)hBchannel;

	if (hlocationMapColor)
	{
		GlobalFree(hlocationMapColor);
		hlocationMapColor = NULL;
	}
	hlocationMapColor = GlobalAlloc(GMEM_FIXED, ImageWidth*ImageHeight);
	locationMapColor = (unsigned char*)hlocationMapColor;

	if (hoverflowMap)
	{
		GlobalFree(hoverflowMap);
		hoverflowMap = NULL;
	}
	hoverflowMap = GlobalAlloc(GMEM_FIXED, ImageWidth*ImageHeight);
	overflowMap = (unsigned char*)hoverflowMap;

	//��ͨ����ֵ
	int z = 0;
	for (int j = ColorHead; j < m_ColorImageSize; j = j + 3)
	{
		*(Rchannel + GrayHead + z) = *(lporigimage + j + 2);
		*(Gchannel + GrayHead + z) = *(lporigimage + j + 1);
		*(Bchannel + GrayHead + z) = *(lporigimage + j);
		z++;
	}

	//���ã�����c1��c2
	deoriRGBPre(lporigimage);

	//�ҵ�Maxֵ����ͨ��
	int Rnum = 0, Gnum = 0, Bnum = 0;
	for (int i = 0; i < m_ImageSize-GrayHead; i++)
	{
		if (*(lpMreimage + i) == 2)
			Rnum++;
		else if (*(lpMreimage + i) == 1)
			Gnum++;
		else if (*(lpMreimage + i) == 0)
			Bnum++;
	}

	//������ǿ��ĳ�ʼ��
	if (hchannelEnhanced)
	{
		GlobalFree(hchannelEnhanced);
		hchannelEnhanced = NULL;
	}
	hchannelEnhanced = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	channelEnhanced = (unsigned char*)hchannelEnhanced;

	//int channel;  //�����ǿ�����ĸ�ͨ��,ȡֵ��RGBPre����һ��
	//�ĸ������ǿ�ĸ�ͨ��
	if (Gnum >= Rnum & Gnum >= Bnum)
	{
		lporigimage = Gchannel;
		deori(pairs);
		channel = 1;
		memcpy(channelEnhanced, lpwmimage, m_ImageSize);
	}
	else if (Rnum >= Gnum & Rnum >= Bnum)
	{
		lporigimage = Rchannel;
		deori(pairs);
		channel = 2;
		memcpy(channelEnhanced, lpwmimage, m_ImageSize);
	}
	else if(Bnum>=Gnum & Bnum>=Rnum)
	{
		lporigimage = Bchannel;
		deori(pairs);
		channel = 0;
		memcpy(channelEnhanced, lpwmimage, m_ImageSize);
	}
	Blow = channel;
	//Bhigh = Bnum;

	//������¼max��median��min��ǿ��ָ��ĳ�ʼ��
	if (hMaxEnhanced)
	{
		GlobalFree(hMaxEnhanced);
		hMaxEnhanced = NULL;
	}
	hMaxEnhanced = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	MaxEnhanced = (unsigned char*)hMaxEnhanced;

	if (hMedianEnhanced)
	{
		GlobalFree(hMedianEnhanced);
		hMedianEnhanced = NULL;
	}
	hMedianEnhanced = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	MedianEnhanced = (unsigned char*)hMedianEnhanced;
		
	if (hMinEnhanced)
	{
		GlobalFree(hMinEnhanced);
		hMinEnhanced = NULL;
	}
	hMinEnhanced = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	MinEnhanced = (unsigned char*)hMinEnhanced;

	//��ǿ��max��median��min����ֵ
	int k = 0;
	Rless = 0, Rmore = 0, Bless = 0, Bmore = 0, Rhigh = 0, Rlow = 0;
	int maxErrorMax=0, maxErrorMed=0, maxErrorMin=0;
	for (int i = GrayHead; i < m_ImageSize; i++)
	{
		double c2 = *(lpC2image + k);
		double c = *(lpCimage + k);
		double c2En, cEn;
		double maxEn, medianEn, minEn;
		double maxRe, medianRe, minRe;
		unsigned char maxEnChar, medianEnChar, minEnChar;
		bool overflow = false;
		unsigned char maxReChar, medianReChar, minReChar;
		unsigned char oriMax = *(lpMaximage + i);
		unsigned char oriMedian = *(lpMediaimage + i);
		unsigned char oriMin = *(lpMinimage + i);
		int errorMax = 0, errorMed = 0, errorMin = 0;
		
		int row = 211, column = 579;
		int height, width;
		if (i== row*ImageWidth+column+GrayHead)
		{
			height = (i - GrayHead) / ImageWidth;
			width = height * ImageWidth;
		}
			
		//����ǿ��Ϊmax
		if (channel == *(lpMreimage + k))
		{
			maxEn = *(channelEnhanced + i);
			maxEnChar = (unsigned char)maxEn;
			if (maxEnChar != 0)
				minEn = maxEn * c2 + 0.5;
			else
				minEn = 0;
			minEnChar = (unsigned char)minEn;
			if (maxEnChar != minEnChar)
				medianEn = maxEn * (c - c2 * c + c2) + 0.5;
			else
				medianEn = maxEn;
			medianEnChar = (unsigned char)medianEn;
			
			cEn = ((double)(medianEnChar - minEnChar)) / ((double)(maxEnChar - minEnChar));
			c2En = ((double)minEnChar) / ((double)maxEnChar);

			maxRe = oriMax;
			maxReChar = (unsigned char)maxRe;
			if (maxReChar != 0)
				minRe = maxRe * c2En + 0.5;
			else
				minRe = 0;
			minReChar = (unsigned char)minRe;
			if (maxReChar != minReChar)
				medianRe = maxRe * (cEn - c2En * cEn + c2En) + 0.5;
			else
				medianRe = maxRe;
			medianReChar = (unsigned char)medianRe;

			//�жϻ�ԭ���ֵ��ԭͼֵ�Ƿ���ȣ��������ǿֵ��������ԭֵ
			*(MaxEnhanced + i) = *(channelEnhanced + i);
			if ((unsigned char)minReChar != (unsigned char)oriMin || (unsigned char)medianReChar != (unsigned char)oriMedian)
			{
				/*
				//�����ܻ�Ը������ԭֵ+��ǿͨ����ǰ��֮�Ч��������
				minEn = oriMin + maxEnChar - maxRe;
				medianEn = oriMedian + maxEnChar - maxRe;
				*(MinEnhanced + i) = minEn;
				*(MedianEnhanced + i) = medianEn;
				if (minEn < 0 || minEn > 255)
					*(MinEnhanced + i) = oriMin;
				if (medianEn < 0 || medianEn > 255)
					*(MedianEnhanced + i) = oriMedian;
				*/

				/*��������ֵ��ûʲô��
				if (minReChar - oriMin > errorMax)
					errorMax = minReChar - oriMin;
				if (medianReChar - oriMedian > errorMax)
					errorMax = medianReChar - oriMedian;
				*/

				*(MinEnhanced + i) = oriMin;
				*(MedianEnhanced + i) = oriMedian;

				Rless++;          
				*(locationMapColor + k) = 1;
				*(overflowMap + k) = 0;
			}
			else
			{
				*(MinEnhanced + i) = minEnChar;
				*(MedianEnhanced + i) = medianEnChar;
				*(locationMapColor + k) = 0;
				*(overflowMap + k) = 0;
				Rmore++;
			}

		}

		//����ǿ��Ϊmin
		else if (channel == *(lpMireimage + k))
		{
			minEn = *(channelEnhanced + i);
			minEnChar = (unsigned char)minEn;
			if (oriMin != 0)
			{
				maxEn = minEn / c2 + 0.5;
				if (maxEn >= 256)
				{
					errorMax = maxEn - 255;
					overflow = true;		//���Ϊ256����ʾ�޷���ԭ�����ֵ����max��median�����ı䣿
				}
			}
			else
			{
				overflow = true;			//���������Ҫ������ǣ�
				maxEn = 256;
			}
				
			maxEnChar = (unsigned char)maxEn;
			if (oriMin != 0)
			{
				medianEn = minEn * (c / c2 + 1 - c) + 0.5;
				if (medianEn >= 256)
				{
					errorMed = medianEn - 255;
					overflow = true;
				}
			}
			else {
				overflow = true;
				medianEn = 256;
			}
				
			medianEnChar = (unsigned char)medianEn;

			/*
			maxEnChar = (unsigned char)maxEn;
			medianEnChar = (unsigned char)medianEn;
			minEnChar = (unsigned char)minEn;
			*/

			cEn = ((double)(medianEnChar - minEnChar)) / ((double)(maxEnChar - minEnChar));
			c2En = ((double)minEnChar) / ((double)maxEnChar);

			minRe = oriMin;
			minReChar = (unsigned char)minRe;
			if (minEnChar != 0)
				maxRe = minRe / c2En + 0.5;
			else
				maxRe = maxEn;
			maxReChar = (unsigned char)maxRe;
			if (minEnChar != 0)
				medianRe = minRe * (cEn / c2En + 1 - cEn) + 0.5;
			else
				medianRe = medianEn;
			medianReChar = (unsigned char)medianRe;
			

			*(MinEnhanced + i) = *(channelEnhanced + i);
			if ((unsigned char)maxReChar != (unsigned char)oriMax || (unsigned char)medianReChar != (unsigned char)oriMedian)
			{
				/*
				//�����ܻ�Ը������ԭֵ+��ǿͨ����ǰ��֮�Ч��������
				maxEn = oriMax + minEnChar - minRe;
				medianEn = oriMedian + minEnChar - minRe;
				*(MaxEnhanced + i) = maxEn;
				*(MedianEnhanced + i) = medianEn;
				if (maxEn > 255 || maxEn < 0)
					*(MaxEnhanced + i) = *(lpMaximage + i);
				if (medianEn > 255 || medianEn < 0)
					*(MedianEnhanced + i) = *(lpMediaimage + i);
				*/
				if (overflow==true)
				{
					*(overflowMap + k) = 1;
					*(locationMapColor + k) = 0;
					Bless++;
				}
				else
				{
					*(locationMapColor + k) = 1;
					*(overflowMap + k) = 0;
				}

				*(MaxEnhanced + i) = *(lpMaximage + i);
				*(MedianEnhanced + i) = *(lpMediaimage + i);

				if (maxReChar - oriMax > errorMax)
					errorMax = maxReChar - oriMax;
				if (medianReChar - oriMedian > errorMax)
					errorMax = medianReChar - oriMedian;

			}
			else
			{
				*(MaxEnhanced + i) = maxEnChar;
				*(MedianEnhanced + i) = medianEnChar;
				Bmore++;
				*(locationMapColor + k) = 0;
				*(overflowMap + k) = 0;
			}
		}
		 
		//����ǿ��Ϊmedian
		else if (channel == *(lpMereimage + k))
		{
			medianEn = *(channelEnhanced + i);
			medianEnChar = (unsigned char)medianEn;
			if (oriMax != 0 && (c2 + c - c * c2) != 0)
			{
				maxEn = medianEn / (c2 + c - c * c2) + 0.5;
				if (maxEn >= 256)
				{
					errorMax = maxEn - 255;
					overflow = true;		//���Ϊ256����ʾ�޷���ԭ�����ֵ����max��median�����ı䣿
				}
			}
			else {
				overflow = true;			//���������Ҫ������ǣ�
				maxEn = 256;
			}				
			maxEnChar = (unsigned char)maxEn;
			if (oriMax != 0 && (c2 + c - c * c2) != 0)
			{
				minEn = medianEn * c2 / (c2 + c - c * c2) + 0.5;
				if (minEn >= 256)
				{
					errorMin = minEn - 255;
					overflow = true;
				}
			}
			else {
				overflow = true;
				minEn = 256;
			}
			minEnChar = (unsigned char)minEn;


			/*
			maxEnChar = (unsigned char)maxEn;
			medianEnChar = (unsigned char)medianEn;
			minEnChar = (unsigned char)minEn;
			*/

			cEn = ((double)(medianEnChar - minEnChar)) / ((double)(maxEnChar - minEnChar));
			c2En = ((double)minEnChar) / ((double)maxEnChar);

			medianRe = oriMedian;
			medianReChar = (unsigned char)medianRe;
			if (maxEnChar != 0 && (c2En + cEn - cEn * c2En) != 0)
				maxRe = medianRe / (c2En + cEn - cEn * c2En) + 0.5;
			else
				maxRe = maxEn;
			maxReChar = (unsigned char)maxRe;
			if (maxEnChar != 0 && (c2En + cEn - cEn * c2En) != 0)
				minRe = medianRe * c2En / (c2En + cEn - cEn * c2En) + 0.5;
			else
				minRe = minEn;
			minReChar = (unsigned char)minRe;

			/*
			maxReChar = (unsigned char)maxRe;
			medianReChar = (unsigned char)medianRe;
			minReChar = (unsigned char)minRe;
			*/

			*(MedianEnhanced + i) = *(channelEnhanced + i);
			if ((unsigned char)maxReChar != (unsigned char)oriMax || (unsigned char)minReChar != (unsigned char)oriMin)
			{
				/*
				//�����ܻ�Ը������ԭֵ+��ǿͨ����ǰ��֮�Ч��������
				maxEn = oriMax + medianEnChar - medianRe;
				minEn = oriMin + medianEnChar - medianRe;
				*(MaxEnhanced + i) = *(lpMaximage + i);
				*(MinEnhanced + i) = *(lpMinimage + i);
				if (maxEn > 255 || maxEn < 0)
					*(MaxEnhanced + i) = *(lpMaximage + i);
				if (minEn > 255 || minEn < 0)
					*(MinEnhanced + i) = *(lpMinimage + i);
				*/
				
				if (overflow==true)
				{
					*(locationMapColor + k) = 0;
					*(overflowMap + k) = 1;
					Bless++;
				}
				else 
				{
					*(locationMapColor + k) = 1;
					*(overflowMap + k) = 0;
				}
				*(MaxEnhanced + i) = *(lpMaximage + i);
				*(MinEnhanced + i) = *(lpMinimage + i);
			}
			else
			{
				*(MaxEnhanced + i) = maxEnChar;
				*(MinEnhanced + i) = minEnChar;
				*(locationMapColor + k) = 0;
				*(overflowMap + k) = 0;
				Rhigh++;
			}
		}
		k++;
		if (maxErrorMax < errorMax)	maxErrorMax = errorMax;
		if (maxErrorMed < errorMed)	maxErrorMed = errorMed;
		if (maxErrorMin < errorMin)	maxErrorMin = errorMin;
		//Bhigh = errorMax;
	}

	//��RGB��ֵ������ڴ�
	if (hwmimage)
	{
		GlobalFree(hwmimage);
		hwmimage = NULL;
	}
	hwmimage = GlobalAlloc(GMEM_FIXED, m_ColorImageSize);
	lpwmimage = (unsigned char*)hwmimage;

	//��ָ
	lporigimage = lpoimage;

	//���ļ�ͷд�����ָ��
	for (int i = 0; i < ColorHead; i++)
	{
		*(lpwmimage + i) = *(lporigimage + i);
	}

	//int max;
	//int t;
	k = 0;
	int j = GrayHead;
	for (int i = ColorHead; i < m_ColorImageSize; i += 3)
	{
		*(lpwmimage + i + *(lpMreimage + k)) = *(MaxEnhanced + j);
		*(lpwmimage + i + *(lpMereimage + k)) = *(MedianEnhanced + j);
		*(lpwmimage + i + *(lpMireimage + k)) = *(MinEnhanced + j);
		j++;
		k++;
	}
	
	////////////////////////////////////////////////
	/*Ԥ�ⷽ�����ó���*/
	if (hRchannelEnhanced)
	{
		GlobalFree(hRchannelEnhanced);
		hRchannelEnhanced = NULL;
	}
	hRchannelEnhanced = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	RchannelEnhanced = (unsigned char*)hRchannelEnhanced;

	if (hGchannelEnhanced)
	{
		GlobalFree(hGchannelEnhanced);
		hGchannelEnhanced = NULL;
	}
	hGchannelEnhanced = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	GchannelEnhanced = (unsigned char*)hGchannelEnhanced;

	if (hBchannelEnhanced)
	{
		GlobalFree(hBchannelEnhanced);
		hBchannelEnhanced = NULL;
	}
	hBchannelEnhanced = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	BchannelEnhanced = (unsigned char*)hBchannelEnhanced;

	z = GrayHead;
	for (int j = ColorHead; j < m_ColorImageSize; j = j + 3)
	{
		*(RchannelEnhanced + z) = *(lpwmimage + j + 2);
		*(GchannelEnhanced + z) = *(lpwmimage + j + 1);
		*(BchannelEnhanced + z) = *(lpwmimage + j);
		z++;
	}

	if (hreferOri1)
	{
		GlobalFree(hreferOri1);
		hreferOri1 = NULL;
	}
	hreferOri1 = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	referOri1 = (unsigned char*)hreferOri1;

	if (hreferOri2)
	{
		GlobalFree(hreferOri2);
		hreferOri2 = NULL;
	}
	hreferOri2 = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	referOri2 = (unsigned char*)hreferOri2;

	if (hreferEn1)
	{
		GlobalFree(hreferEn1);
		hreferEn1 = NULL;
	}
	hreferEn1 = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	referEn1 = (unsigned char*)hreferEn1;

	if (hreferEn2)
	{
		GlobalFree(hreferEn2);
		hreferEn2 = NULL;
	}
	hreferEn2 = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	referEn2 = (unsigned char*)hreferEn2;

	if (hreferPredict1Ori)
	{
		GlobalFree(hreferPredict1Ori);
		hreferPredict1Ori = NULL;
	}
	hreferPredict1Ori = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	referPredict1Ori = (unsigned char*)hreferPredict1Ori;

	if (hreferPredict2Ori)
	{
		GlobalFree(hreferPredict2Ori);
		hreferPredict2Ori = NULL;
	}
	hreferPredict2Ori = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	referPredict2Ori = (unsigned char*)hreferPredict2Ori;

	if (hreferPredict1En)
	{
		GlobalFree(hreferPredict1En);
		hreferPredict1En = NULL;
	}
	hreferPredict1En = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	referPredict1En = (unsigned char*)hreferPredict1En;

	if (hreferPredict2En)
	{
		GlobalFree(hreferPredict2En);
		hreferPredict2En = NULL;
	}
	hreferPredict2En = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	referPredict2En = (unsigned char*)hreferPredict2En;

	if (hreferHoldOri)
	{
		GlobalFree(hreferHoldOri);
		hreferHoldOri = NULL;
	}
	hreferHoldOri = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	referHoldOri = (unsigned char*)hreferHoldOri;

	if (hreferHoldEn)
	{
		GlobalFree(hreferHoldEn);
		hreferHoldEn = NULL;
	}
	hreferHoldEn = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	referHoldEn = (unsigned char*)hreferHoldEn;

	if (hmapFlow)
	{
		GlobalFree(hmapFlow);
		hmapFlow = NULL;
	}
	hmapFlow = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	mapFlow = (unsigned char*)hmapFlow;

	if (hmapFlow2)
	{
		GlobalFree(hmapFlow2);
		hmapFlow2 = NULL;
	}
	hmapFlow2 = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	mapFlow2 = (unsigned char*)hmapFlow2;

	if (channel == 0)
	{
		for (int i = GrayHead; i < m_ImageSize; i++)
		{
			*(referOri1 + i) = *(Rchannel + i);
			*(referOri2 + i) = *(Gchannel + i);
			*(referHoldOri + i) = *(Bchannel + i);
			
			*(referEn1 + i) = *(RchannelEnhanced + i);
			*(referEn2 + i) = *(GchannelEnhanced + i);
			*(referHoldEn + i) = *(BchannelEnhanced + i);
		}
	}
	else if (channel == 1)
	{
		for (int i = GrayHead; i < m_ImageSize; i++)
		{
			*(referOri1 + i) = *(Rchannel + i);
			*(referHoldOri + i) = *(Gchannel + i);
			*(referOri2 + i) = *(Bchannel + i);

			*(referEn1 + i) = *(RchannelEnhanced + i);
			*(referHoldEn + i) = *(GchannelEnhanced + i);
			*(referEn2 + i) = *(BchannelEnhanced + i);
		}
	}
	else if (channel == 2)
	{
		for (int i = GrayHead; i < m_ImageSize; i++)
		{
			*(referHoldOri + i) = *(Rchannel + i);
			*(referOri1 + i) = *(Gchannel + i);
			*(referOri2 + i) = *(Bchannel + i);

			*(referHoldEn + i) = *(RchannelEnhanced + i);
			*(referEn1 + i) = *(GchannelEnhanced + i);
			*(referEn2 + i) = *(BchannelEnhanced + i);
		}
	}

	/*
	k = 0;
	for (int row = 0; row < ImageHeight; row++)
		for (int column = 0; column < ImageWidth; column++)
		{
			double ori1Sum, en1Sum, ori2Sum, en2Sum;
			ori1Sum = 0, en1Sum = 0, ori2Sum = 0, en2Sum = 0;
			if (*(locationMapColor + row * ImageWidth + column) == 1)
			{
				int num = 0;
				for (int i = row - 1; i <= row + 1; i++)
					for (int j = column - 1; j <= column + 1; j++)
					{
						if (j<0 || j>=ImageWidth || i<0 || i>=ImageHeight)
							continue;
						if (*(locationMapColor + i * ImageWidth + j) == 0)
						{
							ori1Sum = *(referOri1 + i * ImageWidth + j + GrayHead) + ori1Sum;
							ori2Sum = *(referOri2 + i * ImageWidth + j + GrayHead) + ori2Sum;
							en1Sum = *(referEn1 + i * ImageWidth + j + GrayHead) + en1Sum;
							en2Sum = *(referEn2 + i * ImageWidth + j + GrayHead) + en2Sum;
							num++;
						}
					}
				
				if (num == 0)
				{
					*(mapFlow + k) = 1;
				}
				else
				{
					int ori1Ave = ori1Sum / num + 0.5;
					int ori2Ave = ori2Sum / num + 0.5;
					int en1Ave = en1Sum / num + 0.5;
					int en2Ave = en2Sum / num + 0.5;

					int referOri1Val = *(referOri1 + row * ImageWidth + column + GrayHead);
					int referOri2Val = *(referOri2 + row * ImageWidth + column + GrayHead);

					int correlation1 = en1Ave + (ori1Ave - referOri1Val);
					int correlation2 = en2Ave + (ori2Ave - referOri2Val);

					if (correlation1 > 255 || correlation2 > 255 || correlation1 < 0 || correlation2 < 0)
					{
						*(mapFlow + k) = 1;
					}
					else
					{
						*(referEn1 + row * ImageWidth + column + GrayHead) = correlation1;
						*(referEn2 + row * ImageWidth + column + GrayHead) = correlation2;
						*(mapFlow + k) = 0;
					}
					
				}
				k++;
			}
		}
	*/

	/*
	k = 0;
	for (int row = 0; row < ImageHeight; row++)
		for (int column = 0; column < ImageWidth; column++)
		{
			int ori1Diff, ori2Diff, ori1MinDiff, ori2MinDiff, correlation1, correlation2;
			ori1MinDiff = 256, ori2MinDiff = 256;
			if (*(locationMapColor + row * ImageWidth + column) == 1)
			{
				int num = 0;
				int neighbor1[4], neighbor2[4];

				int referOri1Val = *(referOri1 + row * ImageWidth + column + GrayHead);
				int referOri2Val = *(referOri2 + row * ImageWidth + column + GrayHead);

				for (int i = row - 1; i <= row; i++)
					for (int j = column - 1; j <= column; j++)
					{
						if (num != 3)
						{
							if (j < 0 || j >= ImageWidth || i < 0 || i >= ImageHeight)
								continue;
						//if (*(locationMapColor + i * ImageWidth + j) == 0)
						//{
							ori1Diff = *(referOri1 + i * ImageWidth + j + GrayHead) - referOri1Val;
							ori2Diff = *(referOri2 + i * ImageWidth + j + GrayHead) - referOri2Val;
							if (abs(ori1Diff) < abs(ori1MinDiff))
							{
								ori1MinDiff = ori1Diff;
								correlation1 = *(referEn1 + i * ImageWidth + j + GrayHead) + ori1MinDiff;
							}
							if (abs(ori2Diff) < abs(ori2MinDiff))
							{
								ori2MinDiff = ori2Diff;
								correlation2 = *(referEn2 + i * ImageWidth + j + GrayHead) + ori2MinDiff;
							}
							num++;
						}
						
						//}
					}

				if (num == 0)
				{
					*(mapFlow + k) = 1;
				}
				else
				{
					if (correlation1 > 255 || correlation2 > 255 || correlation1 < 0 || correlation2 < 0)
					{
						*(mapFlow + k) = 1;
					}
					else
					{
						*(referEn1 + row * ImageWidth + column + GrayHead) = correlation1;
						*(referEn2 + row * ImageWidth + column + GrayHead) = correlation2;
						*(mapFlow + k) = 0;
					}

				}
				k++;
			}
		}
	*/
	
	/*
	k = 0;
	for(int row=0;row<ImageHeight;row++)
		for (int column = 0; column < ImageWidth; column++)
		{
			int medOri1, medOri2, medEn1, medEn2;
			int oriNeighbor1[3], enNeighbor1[3], oriNeighbor2[3], enNeighbor2[3];
			int correlation1, correlation2;
			if (*(locationMapColor + row * ImageWidth + column) == 0)     //��map���0�������ԭֵ       
			{
				*(referPredict1Ori + row * ImageWidth + column + GrayHead) = *(referOri1 + row * ImageWidth + column + GrayHead);
				*(referPredict1En + row * ImageWidth + column + GrayHead) = *(referEn1 + row * ImageWidth + column + GrayHead);
				*(referPredict2Ori + row * ImageWidth + column + GrayHead) = *(referOri2 + row * ImageWidth + column + GrayHead);
				*(referPredict2En + row * ImageWidth + column + GrayHead) = *(referEn2 + row * ImageWidth + column + GrayHead);
			}
			else if (*(locationMapColor + row * ImageWidth + column) == 1)
			{
				int num = 0;
				for(int i =row-1;i<=row;i++)
					for (int j = column - 1; j <= column; j++)
					{
						if (num < 3)
						{
							if (j < 0 || j >= ImageWidth || i < 0 || i >= ImageHeight)
							{
								num++;
								continue;
							}
							oriNeighbor1[num] = *(referPredict1Ori + i * ImageWidth + j + GrayHead);
							enNeighbor1[num] = *(referPredict1En + i * ImageWidth + j + GrayHead);
							oriNeighbor2[num] = *(referPredict2Ori + i * ImageWidth + j + GrayHead);
							enNeighbor2[num] = *(referPredict2En + i * ImageWidth + j + GrayHead);
							num++;
						}
						
					}

				medOri1 = MED(oriNeighbor1[0], oriNeighbor1[1], oriNeighbor1[2]);
				medOri2 = MED(oriNeighbor2[0], oriNeighbor2[1], oriNeighbor2[2]);
				medEn1 = MED(enNeighbor1[0], enNeighbor1[1], enNeighbor1[2]);
				medEn2 = MED(enNeighbor2[0], enNeighbor2[1], enNeighbor2[2]);

				int referOri1Val = *(referOri1 + row * ImageWidth + column + GrayHead);
				int referOri2Val = *(referOri2 + row * ImageWidth + column + GrayHead);

				if ((medOri1 - referOri1Val) < 10 && (medOri2 - referOri2Val) < 10)
				{
					correlation1 = medEn1 + (medOri1 - referOri1Val);
					correlation2 = medEn2 + (medOri2 - referOri2Val);

					if (correlation1 > 255 || correlation1 < 0 || correlation2 > 255 || correlation2 < 0)
					{
						*(mapFlow + k) = 1;
					}
					else
					{
						*(referEn1 + row * ImageWidth + column + GrayHead) = correlation1;
						*(referEn2 + row * ImageWidth + column + GrayHead) = correlation2;
						*(mapFlow + k) = 0;
					}
				}
				else
				{
					*(mapFlow + k) = 1;
				}

				k++;

			}
		}
	*/
	int kkk=0;
	int max=0,min=256;
	k = 0;
	for (int row = 0; row < ImageHeight; row++)
		for (int column = 0; column < ImageWidth; column++)
		{
			int predictOri1, predictOri2, predictEn1, predictEn2, referOri1Val, referOri2Val;
			int oriNeighbor1[3], enNeighbor1[3], oriNeighbor2[3], enNeighbor2[3];
			int correlation1, correlation2;
			if (*(locationMapColor + row * ImageWidth + column) == 0)     //��map���0�������ԭֵ       
			{
				*(referPredict1Ori + row * ImageWidth + column + GrayHead) = *(referOri1 + row * ImageWidth + column + GrayHead);
				*(referPredict1En + row * ImageWidth + column + GrayHead) = *(referEn1 + row * ImageWidth + column + GrayHead);
				*(referPredict2Ori + row * ImageWidth + column + GrayHead) = *(referOri2 + row * ImageWidth + column + GrayHead);
				*(referPredict2En + row * ImageWidth + column + GrayHead) = *(referEn2 + row * ImageWidth + column + GrayHead);
			}
			else if (*(locationMapColor + row * ImageWidth + column) == 1)
			{
				int error = *(referHoldEn + row * ImageWidth + column + GrayHead) - *(referHoldOri + row * ImageWidth + column + GrayHead);
				unsigned char en = *(referHoldEn + row * ImageWidth + column + GrayHead);
				unsigned char ori = *(referHoldOri + row * ImageWidth + column + GrayHead);
				error = abs(error);
				if (error > max )	max = error;
				if (error < min)	min = error;
				if (max == 255)
					max = error;

				predictOri1 = predict(referHoldOri, referPredict1Ori, row, column);
				*(referPredict1Ori + row * ImageWidth + column + GrayHead) = predictOri1;

				predictEn1 = predict(referHoldEn, referPredict1En, row, column);
				*(referPredict1En + row * ImageWidth + column + GrayHead) = predictEn1;

				predictOri2 = predict(referHoldOri, referPredict2Ori, row, column);
				*(referPredict2Ori + row * ImageWidth + column + GrayHead) = predictOri2;

				predictEn2 = predict(referHoldEn, referPredict2En, row, column);
				*(referPredict2En + row * ImageWidth + column + GrayHead) = predictEn2;

				referOri1Val = *(referOri1 + row * ImageWidth + column + GrayHead);
				referOri2Val = *(referOri2 + row * ImageWidth + column + GrayHead);
				
				if ((predictOri1 - referOri1Val) < 10 && (predictOri2 - referOri2Val) < 10)
				{
					correlation1 = predictEn1 + (predictOri1 - referOri1Val);                        //˳��Ҫע��
					correlation2 = predictEn2 + (predictOri2 - referOri2Val);
				//if (predictOri1 != 0 && predictOri2 != 0)
				//{
					//correlation1 = predictEn1 * (referOri1Val / predictOri1);                        //˳��Ҫע��
					//correlation2 = predictEn2 * (referOri2Val / predictOri2);
										
					if (correlation1 > 255 || correlation1 < 0 || correlation2 > 255 || correlation2 < 0)
					{
						*(mapFlow + k) = 1;
					}
					else
					{
						*(referEn1 + row * ImageWidth + column + GrayHead) = correlation1; 
						*(referEn2 + row * ImageWidth + column + GrayHead) = correlation2;
						*(mapFlow + k) = 0;
						kkk++;
					}
					
				}
				else
				{
					*(mapFlow + k) = 1;
				}
				

				k++;
			}
		}
		
	mapFlowLen = k;

	if (channel == 0)
	{
		for (int i = GrayHead; i < m_ImageSize; i++)
		{
			*(RchannelEnhanced + i) = *(referEn1 + i);
			*(GchannelEnhanced + i) = *(referEn2 + i);
		}
	}
	else if (channel == 1)
	{
		for (int i = GrayHead; i < m_ImageSize; i++)
		{
			*(RchannelEnhanced + i) = *(referEn1 + i);
			*(BchannelEnhanced + i) = *(referEn2 + i);
		}
	}
	else if (channel == 2)
	{
		for (int i = GrayHead; i < m_ImageSize; i++)
		{
			*(GchannelEnhanced + i) = *(referEn1 + i);
			*(BchannelEnhanced + i) = *(referEn2 + i);
		}
	}

	j = GrayHead;
	for (int i = ColorHead; i < m_ColorImageSize; i += 3)
	{
		*(lpwmimage + i + 2) = *(RchannelEnhanced + j);
		*(lpwmimage + i + 1) = *(GchannelEnhanced + j);
		*(lpwmimage + i) = *(BchannelEnhanced + j);
		j++;
	}

	///////////////////////////////

	/*locationMapѹ��*/
	if (hlocationCompression)
	{
		GlobalFree(hlocationCompression);
	}
	hlocationCompression = GlobalAlloc(GMEM_FIXED, locationMapSize);
	locationCompression = (unsigned char *)hlocationCompression;
	for (int i = 0; i < locationMapSize; i++)
		*(locationCompression + i) = 0;

	if (hmapBeforeCompression)
	{
		GlobalFree(hmapBeforeCompression);
	}
	hmapBeforeCompression = GlobalAlloc(GMEM_FIXED, locationMapSize);
	mapBeforeCompression = (unsigned char *)hmapBeforeCompression;
	for (int i = 0; i < locationMapSize; i++)
		*(mapBeforeCompression + i) = 0;
	
	for(int row=0;row<ImageHeight;row++)
		for (int column = 0; column < locationMapWidth; column++)
		{
			int sign = row * ImageWidth + column;

			if (column % 8 == 0) location8 = 0;

			if (column < ImageWidth)
			{
				if (*(locationMapColor + sign) == 1)
					location8 = (location8 << 1) + 1;
				else
					location8 = (location8 << 1) + 0;
			}
			else
				location8 = (location8 << 1) + 0;

			if (column % 8 == 7)            *(mapBeforeCompression + (row*locationMapWidth + column) / 8) = location8;
		}
	

	memcpy(locationCompression, mapBeforeCompression, locationMapSize);

	testbuf_len = 0;

	testbuf = (unsigned char *)checkedmalloc(TESTBUF_SIZE);                        //ѹ�������

	jbg85_enc_init(&s, locationMapWidth, ImageHeight, testbuf_writel, NULL);                //��ʼ��
	jbg85_enc_options(&s, JBG_TPBON, 0, -1);                                       //��������

	for (int row = 0; row < ImageHeight; row++)
		jbg85_enc_lineout(&s, locationCompression + row * locationMapWidth / 8, locationCompression + (row - 1)*locationMapWidth / 8, locationCompression + (row - 2)*locationMapWidth / 8);    //������һ�У�ǰһ�У�ǰǰһ��

	buflen = testbuf_len;

	memcpy(locationCompression, testbuf, buflen);

	ColorLocationMap(locationCompression, buflen, channel);

	addDataLen = buflen + sizeof(long) + sizeof(int);

	int num_1 = 0, num_0 = 0;
	for (int i = 0; i < ImageWidth*ImageHeight; i++)
	{
		if (*(overflowMap + i) == 1)	num_1++;
		num_0 = ImageWidth * ImageHeight - num_1;
	}
	int num_3 = 0, num_4 = 0;
	for (int i = 0; i < ImageWidth*ImageHeight; i++)
	{
		if (*(locationMapColor + i) == 1)	num_3++;
		num_4 = ImageWidth * ImageHeight - num_3;
	}
	int maxUnre = -1, minUnre = 256;
	for (int i = 0; i < ImageWidth*ImageHeight; i++)
	{
		int pixelEn,pixelOri;
		if (*(locationMapColor + i) == 1)
		{
			pixelEn = *(referHoldEn + i + GrayHead);
			pixelOri = *(referHoldOri + i + GrayHead);
			if (maxUnre < pixelOri)       maxUnre = pixelOri;
			if (minUnre > pixelOri)		minUnre = pixelOri;
		}
	}

	int numUn[256], numTotal[256], value[256], label = 0;
	for (int i = 0; i < 256; i++) {
		numUn[i] = 0;
		value[i] = -1;
		numTotal[i] = 0;
	}
	for (int i = 0; i < ImageWidth*ImageHeight; i++)
	{
		if (*(locationMapColor + i) == 1) {
			for (int k = 0; k < 256; k++){
				if (*(referHoldOri + i + GrayHead) == value[k])
					break;
				if (k == 255)
				{
					value[label] = *(referHoldOri + i + GrayHead);
					label++;
				}
			}
		}
	}

	for (int i = 0; i < ImageWidth*ImageHeight; i++)
	{
		if (*(locationMapColor + i) == 1)
		{
			for (int k = 0; k < 256; k++)
			{
				if (*(referHoldOri + i + GrayHead) == value[k])
				{
					numUn[k]++;
					numTotal[k]++;
					break;
				}
			}
		}
		else
		{
			for (int k = 0; k < 256; k++)
			{
				if (*(referHoldOri + i + GrayHead) == value[k])
				{
					numTotal[k]++;
					break;
				}
			}
		}
	}

	int markNumTotal = 0;
	for (int i = 0; i < 256; i++) {
		markNumTotal = markNumTotal + numTotal[i];
	}
	//ColorToThreeTimesGray();
}

int CDeprejbgDoc::MED(int c, int b, int a)
{
	int max, min;
	if (a > b)
		max = a, min = b;
	else
		max = b, min = a;

	int x;
	x = a + b - c;
	if (c > max)
		return min;
	else if (c < min)
		return max;
	else
		return x;
	
}

int CDeprejbgDoc::predict(unsigned char *refer, unsigned char *bePredicted, int row, int column)
{
	int GrayHead = 1024 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);                 //�Ҷ�ͼ��λͼ�������

	int predict, threshold, diff;
	int Dm, Dh, Dv, Dd, Dad, De;
	int Ir;
	int pixel[8];
	int num = 0;

	Ir = *(refer + GrayHead + row * ImageWidth + column);
	threshold = 10;        //threshold = {1,2,...,10} ��ȷ��

	for(int i=row-1;i<=row+1;i++)
		for (int j = column - 1; j <= column + 1; j++)
		{
			if (i == row && j == column)
				continue;

			pixel[num] = *(refer + GrayHead + i * ImageWidth + j);
			num++;
		}

	Dm = 0;
	for (int i = 0; i < num; i++)
		Dm = pixel[i] + Dm;
	Dm = abs(Dm / 8 - Ir);

	Dh = (pixel[3] + pixel[4]) / 2 - Ir;
	Dh = abs(Dh);

	Dv = (pixel[1] + pixel[6]) / 2 - Ir;
	Dv = abs(Dv);

	Dd = (pixel[0] + pixel[7]) / 2 - Ir;
	Dd = abs(Dd);

	Dad = (pixel[2] + pixel[5]) / 2 - Ir;
	Dad = abs(Dad);

	//De = min{Dh, Dv, Dd, Dad}
	De = Dh;
	if (De > Dv)		De = Dv;
	if (De > Dd)		De = Dd;
	if (De > Dad)		De = Dad;

	diff = abs(Dm - De);
	if (diff <= threshold)
	{
		predict = (*(bePredicted + GrayHead + (row - 1)*ImageWidth + column) + *(bePredicted + GrayHead + row * ImageWidth + column - 1)) / 2;
	}
	else
	{
		if (De == Dh)
			predict = *(bePredicted + GrayHead + row * ImageWidth + column - 1);
		else if (De == Dv)
			predict = *(bePredicted + GrayHead + (row - 1) * ImageWidth + column);
		else if (De == Dd)
			predict = *(bePredicted + GrayHead + (row - 1) * ImageWidth + column - 1);
		else
			predict = *(bePredicted + GrayHead + (row - 1) * ImageWidth + column + 1);
	}

	return predict;
}

void CDeprejbgDoc::enhancedMaxRe()
{
	//bool haveChannel = true;
	//ColorToThreeTimesGrayRecovery(haveChannel);

	/*��¼λ��ͼ��������*/
	struct jbg85_dec_state d;
	int locationMapWidth;              //locationmap�Ŀ����뱣֤�ܱ�8����
	long locationMapSize;              //locationmap��С
	unsigned char location8;           //һ���ֽڣ���¼8��λ��
	unsigned char *image, *buffer;
	size_t plane_size, buffer_len;
	size_t cnt;

	locationMapWidth = 8 * ((ImageWidth + 7) / 8);           //����ȡ����8������
	locationMapSize = ImageHeight * locationMapWidth / 8;    //һ���ֽڴ�8��λ�����Ҫ�߳˿�����8

	//��¼lporigimage��ָ��
	lpoimage = lporigimage;

	//����ɫͼ��RGBͨ����ֵ�ֱ�ֵ���Ҷ�ͼ��ʽ
	int GrayHead = 1024 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);                 //�Ҷ�ͼ��λͼ�������
	int ColorHead = m_ColorImageSize - ImageWidth * ImageHeight * sizeof(unsigned char) * 3;   //��ɫͼ��λͼ���

	//������ǿ���ĸ�ͨ��
	int channelChosen = channelChosenExtract;

	//����RGBpre�����򣬼���c��c2
	depreRGBPre(lpwmimage);

	//��ʼ��
	if (hMaxRecover)
	{
		GlobalFree(hMaxRecover);
		hMaxRecover = NULL;
	}
	hMaxRecover = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	MaxRecover = (unsigned char*)hMaxRecover;

	if (hMedianRecover)
	{
		GlobalFree(hMedianRecover);
		hMedianRecover = NULL;
	}
	hMedianRecover = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	MedianRecover = (unsigned char*)hMedianRecover;

	if (hMinRecover)
	{
		GlobalFree(hMinRecover);
		hMinRecover = NULL;
	}
	hMinRecover = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	MinRecover = (unsigned char*)hMinRecover;

	if (hchannelRecovery)
	{
		GlobalFree(hchannelRecovery);
		hchannelRecovery = NULL;
	}
	hchannelRecovery = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	channelRecovery = (unsigned char*)hchannelRecovery;

	if (hRchannel)
	{
		GlobalFree(hRchannel);
		hRchannel = NULL;
	}
	hRchannel = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	Rchannel = (unsigned char*)hRchannel;

	if (hGchannel)
	{
		GlobalFree(hGchannel);
		hGchannel = NULL;
	}
	hGchannel = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	Gchannel = (unsigned char*)hGchannel;

	if (hBchannel)
	{
		GlobalFree(hBchannel);
		hBchannel = NULL;
	}
	hBchannel = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	Bchannel = (unsigned char*)hBchannel;

	//��ǿ���ͨ����ֵ
	int z = 0;
	for (int j = ColorHead; j < m_ColorImageSize; j = j + 3)
	{
		*(Rchannel + GrayHead + z) = *(lpwmimage + j + 2);
		*(Gchannel + GrayHead + z) = *(lpwmimage + j + 1);
		*(Bchannel + GrayHead + z) = *(lpwmimage + j);
		z++;
	}

	//��ʼ��
	if (hwmimage)
	{
		GlobalFree(hwmimage);
		hwmimage = NULL;
	}
	hwmimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	lpwmimage = (unsigned char*)hwmimage;

	//�жϱ���ǿ��Ϊ��ͨ���������л�ԭ
	if (channelChosen == 2)
	{
		memcpy(lppreimage, Rchannel, m_ImageSize);
		depre();
	}
	else if (channelChosen == 1)
	{
		memcpy(lppreimage, Gchannel, m_ImageSize);
		depre();
	}
	else if (channelChosen == 0)
	{
		memcpy(lppreimage, Bchannel, m_ImageSize);
		depre();
	}
	memcpy(channelRecovery, lpwmimage, m_ImageSize);

	/*map��ѹ*/

	plane_size = ImageHeight * locationMapWidth;
	int result;

	//buffer_len = ((lmwidth >> 3) + !!(lmwidth & 7)) * 3;            //����������
	buffer_len = plane_size;
	buffer = (unsigned char *)checkedmalloc(buffer_len);           //�����������ڴ�
	image = (unsigned char *)checkedmalloc(plane_size);            //���ͼ������ڴ�
	jbg85_dec_init(&d, buffer, buffer_len, line_out, image);
	result = jbg85_dec_in(&d, locationCompression, buflen, &cnt);             //�����ֱ��ǣ�ѹ��������testbuf��ѹ�������ݳ���buflen
	//result = jbg85_dec_in(&d, colorLocationMap, ExtractMapLen, &cnt);             //�����ֱ��ǣ�ѹ��������testbuf��ѹ�������ݳ���buflen

	/*��ѹ��map��λ��ֵ*/
	if (hcolorMapRecover)
	{
		GlobalFree(hcolorMapRecover);
		hcolorMapRecover = NULL;
	}
	hcolorMapRecover = GlobalAlloc(GMEM_FIXED, ImageWidth*ImageHeight);
	colorMapRecover = (unsigned char*)hcolorMapRecover;

	for (int row = 0; row < ImageHeight; row++)
		for (int column = 0; column < ImageWidth; column++)
		{
			int i = column / 8;                                                 //��location map�Ŀ�
			int sign = 7 + i * 8 - column;                                             //�������ƶ�Ӧ��λ����
			unsigned char tmp = *(image + row * locationMapWidth / 8 + i);                               //��ȡ��Ӧ��Ԥ����location map���ݣ�һ��
			int m = (tmp >> sign) & 1;
			*(colorMapRecover + row * ImageWidth + column) = m;
		}


	/*�ָ�*/
	int k = 0;
	//int num = 0;
	for (int i = GrayHead; i < m_ImageSize; i++)
	{
		double c2En = *(lpC2image + k);
		double cEn = *(lpCimage + k);
		double maxRe, medianRe, minRe;
		unsigned char maxEnChar, medianEnChar, minEnChar;
		unsigned char maxReChar, medianReChar, minReChar;

		//����ǿͨ����Ŀǰ��������������max
		if (channelChosen == *(lpMreimage + k))
		{
			maxRe = *(channelRecovery + i);
			maxReChar = (unsigned char)maxRe;
			if (*(colorMapRecover + k) == 0)
			{
				if (maxReChar != 0)
					minRe = maxRe * c2En + 0.5;
				else
					minRe = 0;
				minReChar = (unsigned char)minRe;
				if (maxReChar != minReChar)
					medianRe = maxRe * (cEn - c2En * cEn + c2En) + 0.5;
				else
					medianRe = maxRe;
				medianReChar = (unsigned char)medianRe;
			}
			else
			{
				minReChar = *(lpMinimage+ i);
				medianReChar = *(lpMediaimage + i);
			}
		}

		//����ǿͨ����Ŀǰ��������������min
		else if (channelChosen == *(lpMireimage + k))
		{
			minRe = *(channelRecovery + i);
			minReChar = (unsigned char)minRe;
			if (*(colorMapRecover + k) == 0)
			{
				maxRe = minRe / c2En + 0.5;
				medianRe = minRe * (cEn / c2En + 1 - cEn) + 0.5;
				maxReChar = (unsigned char)maxRe;
				medianReChar = (unsigned char)medianRe;
			}
			else
			{
				maxReChar = *(lpMaximage + i);
				medianReChar = *(lpMediaimage + i);
			}
		}

		//����ǿͨ����Ŀǰ��������������median
		else if (channelChosen == *(lpMereimage + k))
		{
			medianRe = *(channelRecovery + i);
			medianReChar = (unsigned char)medianRe;
			if (*(colorMapRecover + k) == 0)
			{
				maxRe = medianRe / (c2En + cEn - cEn * c2En) + 0.5;
				minRe = medianRe * c2En / (c2En + cEn - cEn * c2En) + 0.5;
				maxReChar = (unsigned char)maxRe;
				minReChar = (unsigned char)minRe;
			}
			else
			{
				maxReChar = *(lpMaximage + i);
				minReChar = *(lpMinimage + i);
			}

		}

		*(MaxRecover + i) = maxReChar;
		*(MedianRecover + i) = medianReChar;
		*(MinRecover + i) = minReChar;

		k++;
	}

	//��RGB��ֵ������ڴ�
	if (hwmimage)
	{
		GlobalFree(hwmimage);
		hwmimage = NULL;
	}
	hwmimage = GlobalAlloc(GMEM_FIXED, m_ColorImageSize);
	lpwmimage = (unsigned char*)hwmimage;

	//��ָ
	lporigimage = lpoimage;

	//���ļ�ͷд�����ָ��
	for (int i = 0; i < ColorHead; i++)
	{
		*(lpwmimage + i) = *(lporigimage + i);
	}

	//int max;
	//int t;
	k = 0;
	int j = GrayHead;
	for (int i = ColorHead; i < m_ColorImageSize; i += 3)
	{
		*(lpwmimage + i + *(lpMreimage + k)) = *(MaxRecover + j);
		*(lpwmimage + i + *(lpMereimage + k)) = *(MedianRecover + j);
		*(lpwmimage + i + *(lpMireimage + k)) = *(MinRecover + j);
		j++;
		k++;
	}

}

void CDeprejbgDoc::Test(){
	struct jbg85_enc_state s;
	int lmwidth;
	unsigned char tmp;
	long g;

	int GrayHead = 1024 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	lp = lporigimage + GrayHead;
	
	lmwidth = 8 * ((ImageWidth + 7) / 8);       //��ͼƬ��ȸĳ�����ȡ�����Ա�8����

	g = ImageHeight * lmwidth / 8;         //locationmap��С

	if (hhismap)
	{
		GlobalFree(hhismap);
	}
	hhismap = GlobalAlloc(GMEM_FIXED, g);
	lphismap = (unsigned char *)hhismap;
	for (int i = 0; i < g; i++)
		*(lphismap + i) = 0;              //location map��ʼ�� ����ֵ0

	

	for (int row = 0; row < ImageHeight; row++)
		for (int column = 0; column < lmwidth; column++)
		{
			if (column % 8 == 0)	lm = 0;                         //��8λΪ���ȣ���ʼʱ����0

			if (column < ImageWidth)
			{
				tmp = *(lp + row * ImageWidth + column);          //lp���Ǵ���ǰͼ���ָ�룬�˴�Ϊ��ȡԭͼ�������ֵ
				if (tmp < 129)									
					lm = (lm << 1) + 1;
				else
					lm = (lm << 1) + 0;                        //������������Ϊ����������ֵ�� ���¼����lm����һλ����1������Ϊ����������ֵ�����¼����lm����һλ����0
			}
			else
				lm = (lm << 1) + 0;                           //������ԭͼ���ʱ��ʹ��0���

			if (column % 8 == 7)	*(lphismap + (row*lmwidth + column) / 8) = lm;      //����8λʱ��lm��ֵ��map��
		}

	if (hlocmap)
	{
		GlobalFree(hlocmap);
	}
	hlocmap = GlobalAlloc(GMEM_FIXED, g);
	locmap = (unsigned char *)hlocmap;
	for (int i = 0; i < g; i++)
		*(locmap + i) = 0;

	memcpy(locmap, lphismap, g);

	testbuf_len = 0;

	testbuf = (unsigned char *)checkedmalloc(TESTBUF_SIZE);                        //ѹ�������

	jbg85_enc_init(&s, lmwidth, ImageHeight, testbuf_writel, NULL);                //��ʼ��
	jbg85_enc_options(&s, JBG_TPBON, 0, -1);                                       //��������

	for (int row = 0; row < ImageHeight; row++)
		jbg85_enc_lineout(&s, locmap + row * lmwidth/8, locmap + (row - 1)*lmwidth/8, locmap + (row - 2)*lmwidth/8);    //������һ�У�ǰһ�У�ǰǰһ��

	buflen = testbuf_len;
	Blow = buflen;
	/*��ѹ*/
	struct jbg85_dec_state d;
	unsigned char *image, *buffer;
	size_t plane_size, buffer_len;
	size_t cnt;

	plane_size = ImageHeight * lmwidth;
	int result;

	//buffer_len = ((lmwidth >> 3) + !!(lmwidth & 7)) * 3;            //����������
	buffer_len = plane_size;
	buffer = (unsigned char *)checkedmalloc(buffer_len);           //�����������ڴ�
	image = (unsigned char *)checkedmalloc(plane_size);            //���ͼ������ڴ�
	jbg85_dec_init(&d, buffer, buffer_len, line_out, image);
	result = jbg85_dec_in(&d, testbuf, buflen, &cnt);             //�����ֱ��ǣ�ѹ��������testbuf��ѹ�������ݳ���testbuf_len


	//��ֵ������ڴ�
	if (hwmimage)
	{
		GlobalFree(hwmimage);
		hwmimage = NULL;
	}
	hwmimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	lpwmimage = (unsigned char*)hwmimage;
	
	
	for (int i = 0; i < GrayHead; i++)
	{
		*(lpwmimage + i) = *(lporigimage + i);
	}
	
	for (int row = 0; row < ImageHeight; row++)
		for (int column = 0; column < ImageWidth; column++)
		{
			int i = column / 8;                                                 //��location map�Ŀ�
			int j = 7 + i * 8 - column;                                             //�������ƶ�Ӧ��λ����
			tmp = *(image + row * lmwidth / 8 + i);                               //��ȡ��Ӧ��Ԥ����location map���ݣ�һ��
			int m = (tmp >> j) & 1;                                               //mΪ��ȡ����location map��λ���ݣ�Ϊ0��1
			if (m == 1)
			{
				if (*(lp + row * ImageWidth + column) < 129)
					*(lpwmimage + GrayHead + row * ImageWidth + column) = *(lp + row * ImageWidth + column);
			}
			else if (m == 0)
			{
				if (*(lp + row * ImageWidth + column) > 128)
					*(lpwmimage + GrayHead + row * ImageWidth + column) = *(lp + row * ImageWidth + column);
			}
			else
				*(lpwmimage + GrayHead + row * ImageWidth + column) = 0;
		}

}

//����ʧ��
void CDeprejbgDoc::Yenhanced()
{
	//��¼lporigimage��ָ��
	lpoimage = lporigimage;

	//����ɫͼ��RGBͨ����ֵ�ֱ�ֵ���Ҷ�ͼ��ʽ
	int GrayHead = 1024 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);                 //�Ҷ�ͼ��λͼ�������
	int ColorHead = m_ColorImageSize - ImageWidth * ImageHeight * sizeof(unsigned char) * 3;   //��ɫͼ��λͼ���

	//��ʼ��
	if (hYchannel)
	{
		GlobalFree(hYchannel);
		hYchannel = NULL;
	}
	hYchannel = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	Ychannel = (unsigned char*)hYchannel;

	if (hUchannel)
	{
		GlobalFree(hUchannel);
		hUchannel = NULL;
	}
	hUchannel = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	Uchannel = (unsigned char*)hUchannel;

	if (hVchannel)
	{
		GlobalFree(hVchannel);
		hVchannel = NULL;
	}
	hVchannel = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	Vchannel = (unsigned char*)hVchannel;

	int z = 0;
	for (int j = ColorHead; j < m_ColorImageSize; j = j + 3)
	{
		*(Ychannel + GrayHead + z) = *(lporigimage + j) * 0.299 + *(lporigimage + j + 1) * 0.587 + *(lporigimage + j + 2) * 0.114;
		*(Uchannel + GrayHead + z) = *(lporigimage + j) * -0.169 + *(lporigimage + j + 1) * -0.331 + *(lporigimage + j + 2) * 0.5 + 128;
		*(Vchannel + GrayHead + z) = *(lporigimage + j) * 0.5 + *(lporigimage + j + 1) * -0.419 + *(lporigimage + j + 2) * -0.081 + 128;
		z++;
	}

	//��ǿYͨ��
	lporigimage = Ychannel;
	deori(1);
	memcpy(Ychannel, lpwmimage, m_ImageSize);

	//��ǿ���ʼ��
	if (hGchannelEnhanced)
	{
		GlobalFree(hGchannelEnhanced);
		hGchannelEnhanced = NULL;
	}
	hGchannelEnhanced = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	GchannelEnhanced = (unsigned char*)hGchannelEnhanced;

	if (hRchannelEnhanced)
	{
		GlobalFree(hRchannelEnhanced);
		hRchannelEnhanced = NULL;
	}
	hRchannelEnhanced = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	RchannelEnhanced = (unsigned char*)hRchannelEnhanced;

	if (hBchannelEnhanced)
	{
		GlobalFree(hBchannelEnhanced);
		hBchannelEnhanced = NULL;
	}
	hBchannelEnhanced = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	BchannelEnhanced = (unsigned char*)hBchannelEnhanced;

	for (int i = GrayHead; i < m_ImageSize; i++)
	{
		*(RchannelEnhanced + i) = *(Ychannel + i) + (*(Vchannel + i) - 128) * 1.13983;
		*(GchannelEnhanced + i) = *(Ychannel + i) + (*(Uchannel + i) - 128) * -0.39465 + (*(Vchannel + i) - 128) * -0.5806;
		*(BchannelEnhanced + i) = *(Ychannel + i) + (*(Uchannel + i) - 128) * 2.03211;
	}

	//���ļ�ͷд�����ָ��
	for (int m = 0; m < ColorHead; m++)
	{
		*(lpwmimage + m) = *(lpoimage + m);
	}

	z = GrayHead;
	for (int o = ColorHead; o < m_ColorImageSize; o = o + 3)
	{
		*(lpwmimage + o) = *(RchannelEnhanced + z);
		*(lpwmimage + o + 1) = *(GchannelEnhanced + z);
		*(lpwmimage + o + 2) = *(BchannelEnhanced + z);
		z++;
	}

	//��ָ
	lporigimage = lpoimage;

}

void CDeprejbgDoc::Yrecovery()
{
	int GrayHead = 1024 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);                 //�Ҷ�ͼ��λͼ�������
	int ColorHead = m_ColorImageSize - ImageWidth * ImageHeight * sizeof(unsigned char) * 3;   //��ɫͼ��λͼ���

	//��ʼ��
	if (hYchannel)
	{
		GlobalFree(hYchannel);
		hYchannel = NULL;
	}
	hYchannel = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	Ychannel = (unsigned char*)hYchannel;

	if (hUchannel)
	{
		GlobalFree(hUchannel);
		hUchannel = NULL;
	}
	hUchannel = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	Uchannel = (unsigned char*)hUchannel;

	if (hVchannel)
	{
		GlobalFree(hVchannel);
		hVchannel = NULL;
	}
	hVchannel = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	Vchannel = (unsigned char*)hVchannel;

	//����ɫͼ��RGBͨ����ֵתΪYUV��ʽ�����ֱ�ֵ���Ҷ�ͼ��ʽ
	int z = 0;
	for (int j = ColorHead; j < m_ColorImageSize; j = j + 3)
	{
		*(Ychannel + GrayHead + z) = *(lpwmimage + j) * 0.299 + *(lpwmimage + j + 1) * 0.587 + *(lpwmimage + j + 2) * 0.114;
		*(Uchannel + GrayHead + z) = *(lpwmimage + j) * -0.169 + *(lpwmimage + j + 1) * -0.331 + *(lpwmimage + j + 2) * 0.5 + 128;
		*(Vchannel + GrayHead + z) = *(lpwmimage + j) * 0.5 + *(lpwmimage + j + 1) * -0.419 + *(lpwmimage + j + 2) * -0.081 + 128;
		z++;
	}

	//Yͨ����ԭ
	memcpy(lppreimage, Ychannel, m_ImageSize);
	depre();
	memcpy(Ychannel, lpwmimage, m_ImageSize);

	//תΪRGB
	for (int i = GrayHead; i < m_ImageSize; i++)
	{
		*(RchannelEnhanced + i) = *(Ychannel + i) + (*(Vchannel + i) - 128) * 1.13983;
		*(GchannelEnhanced + i) = *(Ychannel + i) + (*(Uchannel + i) - 128) * -0.39465 + (*(Vchannel + i) - 128) * -0.5806;
		*(BchannelEnhanced + i) = *(Ychannel + i) + (*(Uchannel + i) - 128) * 2.03211;
	}

	//���ļ�ͷд�����ָ��
	for (int m = 0; m < ColorHead; m++)
	{
		*(lpwmimage + m) = *(lpoimage + m);
	}

	z = GrayHead;
	for (int o = ColorHead; o < m_ColorImageSize; o = o + 3)
	{
		*(lpwmimage + o) = *(RchannelEnhanced + z);
		*(lpwmimage + o + 1) = *(GchannelEnhanced + z);
		*(lpwmimage + o + 2) = *(BchannelEnhanced + z);
		z++;
	}

	//��ָ
	lporigimage = lpoimage;
}

//���ڳ���Ϊ�㣬��������⣬������û��ָ����δ���
void CDeprejbgDoc::SmallestRangeEnhanced()
{
	//��¼lporigimage��ָ��
	lpoimage = lporigimage;

	//����ɫͼ��RGBͨ����ֵ�ֱ�ֵ���Ҷ�ͼ��ʽ
	int GrayHead = 1024 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);                 //�Ҷ�ͼ��λͼ�������
	int ColorHead = m_ColorImageSize - ImageWidth * ImageHeight * sizeof(unsigned char) * 3;   //��ɫͼ��λͼ���

	//��ʼ��
	if (hRchannel)
	{
		GlobalFree(hRchannel);
		hRchannel = NULL;
	}
	hRchannel = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	Rchannel = (unsigned char*)hRchannel;

	if (hGchannel)
	{
		GlobalFree(hGchannel);
		hGchannel = NULL;
	}
	hGchannel = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	Gchannel = (unsigned char*)hGchannel;

	if (hBchannel)
	{
		GlobalFree(hBchannel);
		hBchannel = NULL;
	}
	hBchannel = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	Bchannel = (unsigned char*)hBchannel;

	int MaxR = 0, MaxG = 0, MaxB = 0;
	int MinR = 255, MinG = 255, MinB = 255;
	int z = 0;
	for (int j = ColorHead; j < m_ColorImageSize; j = j + 3)
	{
		*(Rchannel + GrayHead + z) = *(lporigimage + j);
		*(Gchannel + GrayHead + z) = *(lporigimage + j + 1);
		*(Bchannel + GrayHead + z) = *(lporigimage + j + 2);
		z++;
		if (*(lporigimage + j) > MaxR)
		{
			MaxR = *(lporigimage + j);
		}
		else if (*(lporigimage + j) < MinR)
		{
			MinR = *(lporigimage + j);
		}
		if (*(lporigimage + j + 1) > MaxG)
		{
			MaxG = *(lporigimage + j + 1);
		}
		else if (*(lporigimage + j + 1) < MinG)
		{
			MinG = *(lporigimage + j + 1);
		}
		if (*(lporigimage + j + 2) > MaxB)
		{
			MaxB = *(lporigimage + j + 2);
		}
		else if (*(lporigimage + j + 2) < MinB)
		{
			MinB = *(lporigimage + j + 2);
		}
	}

	//�����ͨ���ķ�Χ������С��
	int Rrange, Grange, Brange, Minrange;
	Rrange = MaxR - MinR;
	Grange = MaxG - MinG;
	Brange = MaxB - MinB;
	Minrange = Rrange;
	if (Rrange < Grange)
	{
		Minrange = Rrange;
	}
	if (Brange < Minrange)
	{
		Minrange = Brange;
	}

	//��ǿ���ʼ��
	if (hGchannelEnhanced)
	{
		GlobalFree(hGchannelEnhanced);
		hGchannelEnhanced = NULL;
	}
	hGchannelEnhanced = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	GchannelEnhanced = (unsigned char*)hGchannelEnhanced;

	if (hRchannelEnhanced)
	{
		GlobalFree(hRchannelEnhanced);
		hRchannelEnhanced = NULL;
	}
	hRchannelEnhanced = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	RchannelEnhanced = (unsigned char*)hRchannelEnhanced;

	if (hBchannelEnhanced)
	{
		GlobalFree(hBchannelEnhanced);
		hBchannelEnhanced = NULL;
	}
	hBchannelEnhanced = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	BchannelEnhanced = (unsigned char*)hBchannelEnhanced;

	//ѡ��Χ��С�Ľ�����ǿ
	int pairs = 50;
	if (Minrange == Rrange)
	{
		lporigimage = Rchannel;
		deori(pairs);
		memcpy(RchannelEnhanced, lpwmimage, m_ImageSize);
		for (int i = GrayHead; i < m_ImageSize; i++)
		{
			*(GchannelEnhanced + i) = *(RchannelEnhanced + i) * (*(Gchannel + i)) / (*(Rchannel + i));
			*(BchannelEnhanced + i) = *(RchannelEnhanced + i) * (*(Bchannel + i)) / (*(Rchannel + i));
		}
	}
	else if (Minrange == Grange)
	{
		lporigimage = Gchannel;
		deori(pairs);
		memcpy(GchannelEnhanced, lpwmimage, m_ImageSize);
		for (int i = GrayHead; i < m_ImageSize; i++)
		{
			*(RchannelEnhanced + i) = *(GchannelEnhanced + i) * (*(Rchannel + i)) / (*(Gchannel + i));
			*(BchannelEnhanced + i) = *(GchannelEnhanced + i) * (*(Bchannel + i)) / (*(Gchannel + i));
		}
	}
	else
	{
		lporigimage = Bchannel;
		deori(pairs);
		memcpy(BchannelEnhanced, lpwmimage, m_ImageSize);
		for (int i = GrayHead; i < m_ImageSize; i++)
		{
			*(RchannelEnhanced + i) = *(BchannelEnhanced + i) * (*(Rchannel + i)) / (*(Bchannel + i));
			*(GchannelEnhanced + i) = *(BchannelEnhanced + i) * (*(Gchannel + i)) / (*(Bchannel + i));
		}
	}

	//���ļ�ͷд�����ָ��
	for (int m = 0; m < ColorHead; m++)
	{
		*(lpwmimage + m) = *(lpoimage + m);
	}

	z = GrayHead;
	for (int o = ColorHead; o < m_ColorImageSize; o = o + 3)
	{
		*(lpwmimage + o) = *(RchannelEnhanced + z);
		*(lpwmimage + o + 1) = *(GchannelEnhanced + z);
		*(lpwmimage + o + 2) = *(BchannelEnhanced + z);
		z++;
	}

	//��ָ
	lporigimage = lpoimage;
}

void CDeprejbgDoc::GrayReference()
{
	//��ǿ����
	int pairs = 50;

	//��¼lporigimage��ָ��
	lpoimage = lporigimage;

	//����ɫͼ��RGBͨ����ֵ�ֱ�ֵ���Ҷ�ͼ��ʽ
	int GrayHead = 1024 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);                 //�Ҷ�ͼ��λͼ�������
	int ColorHead = m_ColorImageSize - ImageWidth * ImageHeight * sizeof(unsigned char) * 3;   //��ɫͼ��λͼ���

	//��ʼ��
	if (hRchannel)
	{
		GlobalFree(hRchannel);
		hRchannel = NULL;
	}
	hRchannel = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	Rchannel = (unsigned char*)hRchannel;

	if (hGchannel)
	{
		GlobalFree(hGchannel);
		hGchannel = NULL;
	}
	hGchannel = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	Gchannel = (unsigned char*)hGchannel;

	if (hBchannel)
	{
		GlobalFree(hBchannel);
		hBchannel = NULL;
	}
	hBchannel = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	Bchannel = (unsigned char*)hBchannel;

	if (hGrayscale)
	{
		GlobalFree(hGrayscale);
		hGrayscale = NULL;
	}
	hGrayscale = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	Grayscale = (unsigned char*)hGrayscale;

	int z = 0;
	for (int j = ColorHead; j < m_ColorImageSize; j = j + 3)
	{
		*(Rchannel + GrayHead + z) = *(lporigimage + j);
		*(Gchannel + GrayHead + z) = *(lporigimage + j + 1);
		*(Bchannel + GrayHead + z) = *(lporigimage + j + 2);
		*(Grayscale + GrayHead + z) = *(lporigimage + j)*0.299 + *(lporigimage + j + 1)*0.587 + *(lporigimage + j + 2)*0.114;     //��ɫתΪ�Ҷ�
		z++;
	}

	//��ǿ�Ҷ�ͼ�����ο�
	lporigimage = Grayscale;
	deori(pairs);
	if (hGrayscaleEnhanced)
	{
		GlobalFree(hGrayscaleEnhanced);
		hGrayscaleEnhanced = NULL;
	}
	hGrayscaleEnhanced = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	GrayscaleEnhanced = (unsigned char*)hGrayscaleEnhanced;
	memcpy(GrayscaleEnhanced, lpwmimage, m_ImageSize);

	//��ǿRͨ��
	lporigimage = Rchannel;
	deori(pairs);
	if (hRchannelEnhanced)
	{
		GlobalFree(hRchannelEnhanced);
		hRchannelEnhanced = NULL;
	}
	hRchannelEnhanced = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	RchannelEnhanced = (unsigned char*)hRchannelEnhanced;
	memcpy(RchannelEnhanced, lpwmimage, m_ImageSize);

	//��ǿBͨ��
	lporigimage = Bchannel;
	deori(pairs);
	if (hBchannelEnhanced)
	{
		GlobalFree(hBchannelEnhanced);
		hBchannelEnhanced = NULL;
	}
	hBchannelEnhanced = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	BchannelEnhanced = (unsigned char*)hBchannelEnhanced;
	memcpy(BchannelEnhanced, lpwmimage, m_ImageSize);

	//Gͨ��д��
	if (hGchannelEnhanced)
	{
		GlobalFree(hGchannelEnhanced);
		hGchannelEnhanced = NULL;
	}
	hGchannelEnhanced = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	GchannelEnhanced = (unsigned char*)hGchannelEnhanced;

	for (int i = GrayHead; i < m_ImageSize; i++)
	{
		*(GchannelEnhanced + i) = (*(GrayscaleEnhanced + i) - *(RchannelEnhanced + i)*0.299 - *(BchannelEnhanced + i)*0.114) / 0.587;
	}

	//���ļ�ͷд�����ָ��
	for (int m = 0; m < ColorHead; m++)
	{
		*(lpwmimage + m) = *(lpoimage + m);
	}

	//д������ֵ
	z = GrayHead;
	for (int o = ColorHead; o < m_ColorImageSize; o = o + 3)
	{
		*(lpwmimage + o) = *(RchannelEnhanced + z);
		*(lpwmimage + o + 1) = *(GchannelEnhanced + z);
		*(lpwmimage + o + 2) = *(BchannelEnhanced + z);
		z++;
	}

	//��ָ
	lporigimage = lpoimage;
}

void CDeprejbgDoc::threePreProcess()
{
	//Ԥ�������
	int pairs = 20;

	//������
	long pldUseless;
	int metInThreePro = pairs - 1;
	long payload;
	long g=0, el=0;
	long testbuf_lenR, testbuf_lenG, testbuf_lenB;

	//��¼lporigimage��ָ��
	lpoimage = lporigimage;

	//����ɫͼ��RGBͨ����ֵ�ֱ�ֵ���Ҷ�ͼ��ʽ
	int GrayHead = 1024 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);                 //�Ҷ�ͼ��λͼ�������
	int ColorHead = m_ColorImageSize - ImageWidth * ImageHeight * sizeof(unsigned char) * 3;   //��ɫͼ��λͼ���

	/*��¼λ��ͼ��������*/
	struct jbg85_enc_state s;
	int locationMapWidth;              //locationmap�Ŀ����뱣֤�ܱ�8����
	long locationMapSize;              //locationmap��С
	unsigned char location8;           //һ���ֽڣ���¼8��λ��

	locationMapWidth = 8 * ((ImageWidth + 7) / 8);           //����ȡ����8������
	locationMapSize = ImageHeight * locationMapWidth / 8;    //һ���ֽڴ�8��λ�����Ҫ�߳˿�����8

	//��ʼ��
	if (hRchannel)
	{
		GlobalFree(hRchannel);
		hRchannel = NULL;
	}
	hRchannel = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	Rchannel = (unsigned char*)hRchannel;

	if (hGchannel)
	{
		GlobalFree(hGchannel);
		hGchannel = NULL;
	}
	hGchannel = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	Gchannel = (unsigned char*)hGchannel;

	if (hBchannel)
	{
		GlobalFree(hBchannel);
		hBchannel = NULL;
	}
	hBchannel = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	Bchannel = (unsigned char*)hBchannel;

	if (hRchannelPreprocessed)
	{
		GlobalFree(hRchannelPreprocessed);
		hRchannelPreprocessed = NULL;
	}
	hRchannelPreprocessed = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	RchannelPreprocessed = (unsigned char*)hRchannelPreprocessed;

	if (hGchannelPreprocessed)
	{
		GlobalFree(hGchannelPreprocessed);
		hGchannelPreprocessed = NULL;
	}
	hGchannelPreprocessed = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	GchannelPreprocessed = (unsigned char*)hGchannelPreprocessed;

	if (hBchannelPreprocessed)
	{
		GlobalFree(hBchannelPreprocessed);
		hBchannelPreprocessed = NULL;
	}
	hBchannelPreprocessed = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	BchannelPreprocessed = (unsigned char*)hBchannelPreprocessed;

	//��ͨ����ֵ
	int z = 0;
	for (int j = ColorHead; j < m_ColorImageSize; j = j + 3)
	{
		*(Rchannel + GrayHead + z) = *(lporigimage + j + 2);
		*(Gchannel + GrayHead + z) = *(lporigimage + j + 1);
		*(Bchannel + GrayHead + z) = *(lporigimage + j);
		z++;
	}

	//����
	//deoriRGBPre();

	/*����ͨ���ֱ���Ԥ����,����¼�²�����λ����Ϣ*/
	onlyPreProcess = true;

	lporigimage = Rchannel;
	//lporigimage = lpMaximage;
	deori(pairs);
	memcpy(RchannelPreprocessed, lpwmimage, m_ImageSize);
	//lpprehist��locmap��zn��testbuf_len�ļ�¼
	exctR = zn;
	testbuf_lenR = testbuf_len;
	if (hprehistRchannel)
	{
		GlobalFree(hprehistRchannel);
		hprehistRchannel = NULL;
	}
	hprehistRchannel = GlobalAlloc(GMEM_FIXED, 2 * (metInThreePro + 2) * sizeof(unsigned char));
	lpprehistRchannel = (unsigned char *)hprehistRchannel;
	memcpy(lpprehistRchannel, lpprehist, 2 * (metInThreePro + 2) * sizeof(unsigned char));
	if (isMerge == true) 
	{
		if (hlocmapRchannel)
		{
			GlobalFree(hlocmapRchannel);
			hlocmapRchannel = NULL;
		}
		hlocmapRchannel = GlobalAlloc(GMEM_FIXED, lml);
		locmapRchannel = (unsigned char *)hlocmapRchannel;
		memcpy(locmapRchannel, locmap, lml);
	}

	lporigimage = Gchannel;
	//lporigimage = lpMediaimage;
	deori(pairs);
	memcpy(GchannelPreprocessed, lpwmimage, m_ImageSize);
	//lpprehist��locmap��zn�ļ�¼
	exctG = zn;
	testbuf_lenG = testbuf_len;
	if (hprehistGchannel)
	{
		GlobalFree(hprehistGchannel);
		hprehistGchannel = NULL;
	}
	hprehistGchannel = GlobalAlloc(GMEM_FIXED, 2 * (metInThreePro + 2) * sizeof(unsigned char));
	lpprehistGchannel = (unsigned char *)hprehistGchannel;
	memcpy(lpprehistGchannel, lpprehist, 2 * (metInThreePro + 2) * sizeof(unsigned char));
	if (isMerge == true)
	{
		if (hlocmapGchannel)
		{
			GlobalFree(hlocmapGchannel);
			hlocmapGchannel = NULL;
		}
		hlocmapGchannel = GlobalAlloc(GMEM_FIXED, lml);
		locmapGchannel = (unsigned char *)hlocmapGchannel;
		memcpy(locmapGchannel, locmap, lml);
	}

	lporigimage = Bchannel;
	//lporigimage = lpMinimage;
	deori(pairs);
	memcpy(BchannelPreprocessed, lpwmimage, m_ImageSize);
	//lpprehist��locmap�ļ�¼
	exctB = zn;
	testbuf_lenB = testbuf_len;
	if (hprehistBchannel)
	{
		GlobalFree(hprehistBchannel);
		hprehistBchannel = NULL;
	}
	hprehistBchannel = GlobalAlloc(GMEM_FIXED, 2 * (metInThreePro + 2) * sizeof(unsigned char));
	lpprehistBchannel = (unsigned char *)hprehistBchannel;
	memcpy(lpprehistBchannel, lpprehist, 2 * (metInThreePro + 2) * sizeof(unsigned char));
	if (isMerge == true)
	{
		if (hlocmapBchannel)
		{
			GlobalFree(hlocmapBchannel);
			hlocmapBchannel = NULL;
		}
		hlocmapBchannel = GlobalAlloc(GMEM_FIXED, lml);
		locmapBchannel = (unsigned char *)hlocmapBchannel;
		memcpy(locmapBchannel, locmap, lml);
	}
	onlyPreProcess = false;

	//��RGB��ֵ������ڴ�
	if (hwmimage)
	{
		GlobalFree(hwmimage);
		hwmimage = NULL;
	}
	hwmimage = GlobalAlloc(GMEM_FIXED, m_ColorImageSize);
	lpwmimage = (unsigned char*)hwmimage;

	//��ָ
	lporigimage = lpoimage;

	//���ļ�ͷд�����ָ��
	for (int i = 0; i < ColorHead; i++)
	{
		*(lpwmimage + i) = *(lporigimage + i);
	}

	//int max;
	//int t;
	int k = 0;
	int j = GrayHead;
	
	for (int i = ColorHead; i < m_ColorImageSize; i += 3)
	{
		*(lpwmimage + i + 2) = *(RchannelPreprocessed + j);
		*(lpwmimage + i + 1) = *(GchannelPreprocessed + j);
		*(lpwmimage + i) = *(BchannelPreprocessed + j);
		j++;
		k++;
	}
	
	/*
	for (int i = ColorHead; i < m_ColorImageSize; i += 3)
	{

		*(lpwmimage + i + *(lpMreimage + k)) = *(RchannelPreprocessed + j);
		*(lpwmimage + i + *(lpMereimage + k)) = *(GchannelPreprocessed + j);
		*(lpwmimage + i + *(lpMireimage + k)) = *(BchannelPreprocessed + j);

		j++;
		k++;
	}
	*/

	long exct = exctR + exctG + exctB;

	threePreProcessEnhanced(metInThreePro, testbuf_lenR);

}

void CDeprejbgDoc::threePreProcessEnhanced(int met, long testbuf_lenInPre)
{
	long el = 0, g = 0;

	//����ɫͼ��RGBͨ����ֵ�ֱ�ֵ���Ҷ�ͼ��ʽ
	int GrayHead = 1024 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);                 //�Ҷ�ͼ��λͼ�������
	int ColorHead = m_ColorImageSize - ImageWidth * ImageHeight * sizeof(unsigned char) * 3;   //��ɫͼ��λͼ���

	//����
	if (hpreimage)
	{
		GlobalFree(hpreimage);
		hpreimage = NULL;
	}
	hpreimage = GlobalAlloc(GMEM_FIXED, m_ColorImageSize);
	lppreimage = (unsigned char *)hpreimage;
	memcpy(lppreimage, lporigimage, m_ColorImageSize);
	memcpy(lporigimage, lpwmimage, m_ColorImageSize);
	deoriRGBPre(lporigimage);
	memcpy(lporigimage, lppreimage, m_ColorImageSize);

	if (hMaximageTemporary)
	{
		GlobalFree(hMaximageTemporary);
		hMaximageTemporary = NULL;
	}
	hMaximageTemporary = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	lpMaximageTemporary = (unsigned char*)hMaximageTemporary;

	memcpy(lpMaximageTemporary, lpMaximage, m_ImageSize);

	if (hMaxEnimage)
	{
		GlobalFree(hMaxEnimage);
		hMaxEnimage = NULL;
	}
	hMaxEnimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	lpMaxEnimage = (unsigned char*)hMaxEnimage;
	memcpy(lpMaxEnimage, lpMaximage, m_ImageSize);
	for (int m = 0; m < met + 1; m++)
	{
		el = derem(lpMaximageTemporary, lpMaxEnimage, m, met, testbuf_lenInPre, exctR, locmapRchannel, lpprehistRchannel);                  //����Ƕ��
		g = g + el;
		memcpy(lpMaxEnimage, lpMaximageTemporary, m_ImageSize);
	}

	//����Minͨ��
	if (hMinEnimage)
	{
		GlobalFree(hMinEnimage);
		hMinEnimage = NULL;
	}
	hMinEnimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	lpMinEnimage = (unsigned char*)hMinEnimage;
	int z = 0;
	for (int i = GrayHead; i < m_ImageSize; i++)
	{
		double c2 = *(lpC2image + z);
		double c = *(lpCimage + z);
		//double min = *(lpMinimage + i);
		double max = *(lpMaxEnimage + i);
		//double median = *(lpMediaimage + i);
		if (max != 0)
			*(lpMinEnimage + i) = max * c2 + 0.5;
		else
			*(lpMinEnimage + i) = 0;
		z++;
	}

	//����Mediaͨ��
	if (hMedianEnimage)
	{
		GlobalFree(hMedianEnimage);
		hMedianEnimage = NULL;
	}
	hMedianEnimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	lpMedianEnimage = (unsigned char*)hMedianEnimage;
	z = 0;
	for (int i = GrayHead; i < m_ImageSize; i++)
	{
		double c = *(lpCimage + z);
		double c2 = *(lpC2image + z);
		double max = *(lpMaxEnimage + i);
		double min = *(lpMinEnimage + i);
		//double median = *(lpMediaimage + i);
		if (max != min)
			*(lpMedianEnimage + i) = max * (c - c2 * c + c2) + 0.5;
		else
			*(lpMedianEnimage + i) = max;
		z++;
	}

	if (hlocationMapColor)
	{
		GlobalFree(hlocationMapColor);
		hlocationMapColor = NULL;
	}
	hlocationMapColor = GlobalAlloc(GMEM_FIXED, ImageWidth*ImageHeight);
	locationMapColor = (unsigned char*)hlocationMapColor;

	int maplen=0;
	for (int i = 0; i < ImageWidth*ImageHeight; i++)
	{
		double c, c2;
		double minRe, medianRe, maxRe;
		unsigned char maxEn, minEn, medianEn, maxReChar, minReChar, medianReChar, minOri, medianOri,maxOri;

		maxEn = *(lpMaxEnimage + GrayHead + i);
		medianEn = *(lpMedianEnimage + GrayHead + i);
		minEn = *(lpMinEnimage + GrayHead + i);

		c = ((double)(medianEn - minEn)) / ((double)(maxEn - minEn));
		c2 = ((double)minEn) / ((double)maxEn);

		maxReChar = *(lpMaximage + GrayHead + i);
		maxRe = (double)maxReChar;
		if (maxReChar != 0)
			minRe = maxRe * c2 + 0.5;
		else
			minRe = 0;
		minReChar = (unsigned char)minRe;
		if (maxReChar != minReChar)
			medianRe = maxRe * (c - c2 * c + c2) + 0.5;
		else
			medianRe = maxRe;
		medianReChar = (unsigned char)medianRe;

		minOri = *(lpMinimage + GrayHead + i);
		medianOri = *(lpMediaimage + GrayHead + i);
		if (minOri != minReChar || medianOri != medianReChar)
		{
			*(locationMapColor + i) = 1;
			maplen++;
		}
		else
		{
			*(locationMapColor + i) = 0;
		}
	}

	if (hMinPredictOri)
	{
		GlobalFree(hMinPredictOri);
		hMinPredictOri = NULL;
	}
	hMinPredictOri = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	MinPredictOri = (unsigned char*)hMinPredictOri;

	if (hMinPredictEn)
	{
		GlobalFree(hMinPredictEn);
		hMinPredictEn = NULL;
	}
	hMinPredictEn = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	MinPredictEn = (unsigned char*)hMinPredictEn;

	if (hMedianPredictOri)
	{
		GlobalFree(hMedianPredictOri);
		hMedianPredictOri = NULL;
	}
	hMedianPredictOri = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	MedianPredictOri = (unsigned char*)hMedianPredictOri;

	if (hMedianPredictEn)
	{
		GlobalFree(hMedianPredictEn);
		hMedianPredictEn = NULL;
	}
	hMedianPredictEn = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	MedianPredictEn = (unsigned char*)hMedianPredictEn;

	int maxMin = 0, maxMedian = 0;
	for (int row = 0; row < ImageHeight; row++)
		for (int column = 0; column < ImageWidth; column++)
		{
			int minNeighborOri[3], minNeighborEn[3], medianNeighborOri[3], medianNeighborEn[3]; 
			int minMedOri, minMedEn, medianMedOri, medianMedEn;
			if (*(locationMapColor + row * ImageWidth + column) == 0)
			{
				*(MinPredictOri + row * ImageWidth + column + GrayHead) = *(lpMinimage + row * ImageWidth + column + GrayHead);
				*(MinPredictEn + row * ImageWidth + column + GrayHead) = *(lpMinEnimage + row * ImageWidth + column + GrayHead);
				*(MedianPredictOri + row * ImageWidth + column + GrayHead) = *(lpMediaimage + row * ImageWidth + column + GrayHead);
				*(MedianPredictEn + row * ImageWidth + column + GrayHead) = *(lpMedianEnimage + row * ImageWidth + column + GrayHead);
			}
			else if (*(locationMapColor + row * ImageWidth + column) == 1)
			{
				if (row == 0 && column == 0)
				{
					*(MinPredictOri + row * ImageWidth + column + GrayHead) = met+1;
					*(MinPredictEn + row * ImageWidth + column + GrayHead) = *(lpMinEnimage + row * ImageWidth + column + GrayHead);
					*(MedianPredictOri + row * ImageWidth + column + GrayHead) = met + 1;
					*(MedianPredictEn + row * ImageWidth + column + GrayHead) = *(lpMedianEnimage + row * ImageWidth + column + GrayHead);
					continue;
				}
				int num = 0;
				for (int i = row - 1; i <= row; i++)
					for (int j = column - 1; j <= column; j++)
					{
						if (num < 3)
						{
							if (j < 0 || j >= ImageWidth || i < 0 || i >= ImageHeight)
							{
								num++;
								continue;
							}
							minNeighborOri[num] = *(lpMinimage + i * ImageWidth + j + GrayHead);
							minNeighborEn[num] = *(lpMinEnimage + i * ImageWidth + j + GrayHead);
							medianNeighborOri[num] = *(lpMediaimage + i * ImageWidth + j + GrayHead);
							medianNeighborEn[num] = *(lpMedianEnimage + i * ImageWidth + j + GrayHead);
							num++;
						}

					}

				minMedOri = MED(minNeighborOri[0], minNeighborOri[1], minNeighborOri[2]);
				minMedEn = MED(minNeighborEn[0], minNeighborEn[1], minNeighborEn[2]);
				medianMedOri = MED(medianNeighborOri[0], medianNeighborOri[1], medianNeighborOri[2]);
				medianMedEn = MED(medianNeighborEn[0], medianNeighborEn[1], medianNeighborEn[2]);

				*(MinPredictOri + row * ImageWidth + column + GrayHead) = minMedOri;
				*(MinPredictEn + row * ImageWidth + column + GrayHead) = minMedEn;
				*(MedianPredictOri + row * ImageWidth + column + GrayHead) = medianMedOri;
				*(MedianPredictEn + row * ImageWidth + column + GrayHead) = medianMedEn;

				int referOri1Val = *(lpMinimage + row * ImageWidth + column + GrayHead);
				int referOri2Val = *(lpMediaimage + row * ImageWidth + column + GrayHead);

				if ((minMedOri - referOri1Val) > 100 || (medianMedOri - referOri2Val) > 100)
				{
					int correlation1 = minMedEn + (minMedOri - referOri1Val);
					int correlation2 = medianMedEn + (medianMedOri - referOri2Val);
				}
				if ((minMedOri - referOri1Val) > maxMin)		maxMin = minMedOri - referOri1Val;
				if ((medianMedOri - referOri2Val) > maxMedian)		maxMedian = medianMedOri - referOri2Val;
			}
		}

	

	//��RGB��ֵ������ڴ�
	if (hwmimage)
	{
		GlobalFree(hwmimage);
		hwmimage = NULL;
	}
	hwmimage = GlobalAlloc(GMEM_FIXED, m_ColorImageSize);
	lpwmimage = (unsigned char*)hwmimage;

	//��ָ
	lporigimage = lpoimage;

	//���ļ�ͷд�����ָ��
	for (int i = 0; i < ColorHead; i++)
	{
		*(lpwmimage + i) = *(lporigimage + i);
	}

	int k = 0;
	int j = GrayHead;
	for (int i = ColorHead; i < m_ColorImageSize; i += 3)
	{

		*(lpwmimage + i + *(lpMreimage + k)) = *(lpMaxEnimage + j);
		*(lpwmimage + i + *(lpMereimage + k)) = *(lpMedianEnimage + j);
		*(lpwmimage + i + *(lpMireimage + k)) = *(lpMinEnimage + j);

		j++;
		k++;
	}

	z = 0;
	for (int j = ColorHead; j < m_ColorImageSize; j = j + 3)
	{
		*(Rchannel + GrayHead + z) = *(lporigimage + j + 2);
		*(Gchannel + GrayHead + z) = *(lporigimage + j + 1);
		*(Bchannel + GrayHead + z) = *(lporigimage + j);
		z++;
	}

}

void CDeprejbgDoc::threePreProcessRecover()
{
	int pairs = 1;
	int metInThreePro = pairs - 1;
	unsigned char *wpR, *wpG, *wpB;

	//��¼lporigimage��ָ��
	lpoimage = lporigimage;

	int GrayHead = 1024 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);                 //�Ҷ�ͼ��λͼ�������
	int ColorHead = m_ColorImageSize - ImageWidth * ImageHeight * sizeof(unsigned char) * 3;   //��ɫͼ��λͼ���

	//��ʼ��
	if (hRchannel)
	{
		GlobalFree(hRchannel);
		hRchannel = NULL;
	}
	hRchannel = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	Rchannel = (unsigned char*)hRchannel;

	if (hGchannel)
	{
		GlobalFree(hGchannel);
		hGchannel = NULL;
	}
	hGchannel = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	Gchannel = (unsigned char*)hGchannel;

	if (hBchannel)
	{
		GlobalFree(hBchannel);
		hBchannel = NULL;
	}
	hBchannel = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	Bchannel = (unsigned char*)hBchannel;

	if (hRchannelPreprocessed)
	{
		GlobalFree(hRchannelPreprocessed);
		hRchannelPreprocessed = NULL;
	}
	hRchannelPreprocessed = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	RchannelPreprocessed = (unsigned char*)hRchannelPreprocessed;

	if (hGchannelPreprocessed)
	{
		GlobalFree(hGchannelPreprocessed);
		hGchannelPreprocessed = NULL;
	}
	hGchannelPreprocessed = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	GchannelPreprocessed = (unsigned char*)hGchannelPreprocessed;

	if (hBchannelPreprocessed)
	{
		GlobalFree(hBchannelPreprocessed);
		hBchannelPreprocessed = NULL;
	}
	hBchannelPreprocessed = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	BchannelPreprocessed = (unsigned char*)hBchannelPreprocessed;

	/*RGBͨ���ֱ�ֵ*/
	int z = 0;
	for (int j = ColorHead; j < m_ColorImageSize; j = j + 3)
	{
		*(RchannelPreprocessed + GrayHead + z) = *(lpwmimage + j + 2);
		*(GchannelPreprocessed + GrayHead + z) = *(lpwmimage + j + 1);
		*(BchannelPreprocessed + GrayHead + z) = *(lpwmimage + j);
		z++;
	}

	if (hwmimage)
	{
		GlobalFree(hwmimage);
		hwmimage = NULL;
	}
	hwmimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	lpwmimage = (unsigned char*)hwmimage;

	wpR = RchannelPreprocessed + GrayHead;
	preProcessRecover(lpprehistRchannel, locmapRchannel, metInThreePro, exctR, wpR);
	memcpy(Rchannel, RchannelPreprocessed, m_ImageSize);

	wpG = GchannelPreprocessed + GrayHead;
	preProcessRecover(lpprehistGchannel, locmapGchannel, metInThreePro, exctG, wpG);
	memcpy(Gchannel, GchannelPreprocessed, m_ImageSize);

	wpB = BchannelPreprocessed + GrayHead;
	preProcessRecover(lpprehistBchannel, locmapBchannel, metInThreePro, exctB, wpB);
	memcpy(Bchannel, BchannelPreprocessed, m_ImageSize);

	if (hwmimage)
	{
		GlobalFree(hwmimage);
		hwmimage = NULL;
	}
	hwmimage = GlobalAlloc(GMEM_FIXED, m_ColorImageSize);   //��ɫͼ���С
	lpwmimage = (unsigned char*)hwmimage;

	//���ļ�ͷд�����ָ��
	for (int m = 0; m < ColorHead; m++)
	{
		*(lpwmimage + m) = *(lpoimage + m);
	}

	z = GrayHead;
	for (int o = ColorHead; o < m_ColorImageSize; o = o + 3)
	{
		*(lpwmimage + o + 2) = *(Rchannel + z);
		*(lpwmimage + o + 1) = *(Gchannel + z);
		*(lpwmimage + o) = *(Bchannel + z);
		z++;
	}


}

void CDeprejbgDoc::reservedOnePairs()
{
	//Ԥ�����������Ԥ����pairs��
	int pairs = 1;

	//������
	long pldUseless;
	int metInThreePro = pairs - 1;
	long payload;
	long g = 0, el = 0;
	long testbuf_lenR, testbuf_lenG, testbuf_lenB;

	//��¼lporigimage��ָ��
	lpoimage = lporigimage;

	//����ɫͼ��RGBͨ����ֵ�ֱ�ֵ���Ҷ�ͼ��ʽ
	int GrayHead = 1024 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);                 //�Ҷ�ͼ��λͼ�������
	int ColorHead = m_ColorImageSize - ImageWidth * ImageHeight * sizeof(unsigned char) * 3;   //��ɫͼ��λͼ���

	//��ʼ��
	if (hRchannel)
	{
		GlobalFree(hRchannel);
		hRchannel = NULL;
	}
	hRchannel = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	Rchannel = (unsigned char*)hRchannel;

	if (hGchannel)
	{
		GlobalFree(hGchannel);
		hGchannel = NULL;
	}
	hGchannel = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	Gchannel = (unsigned char*)hGchannel;

	if (hBchannel)
	{
		GlobalFree(hBchannel);
		hBchannel = NULL;
	}
	hBchannel = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	Bchannel = (unsigned char*)hBchannel;

	if (hRchannelPreprocessed)
	{
		GlobalFree(hRchannelPreprocessed);
		hRchannelPreprocessed = NULL;
	}
	hRchannelPreprocessed = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	RchannelPreprocessed = (unsigned char*)hRchannelPreprocessed;

	if (hGchannelPreprocessed)
	{
		GlobalFree(hGchannelPreprocessed);
		hGchannelPreprocessed = NULL;
	}
	hGchannelPreprocessed = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	GchannelPreprocessed = (unsigned char*)hGchannelPreprocessed;

	if (hBchannelPreprocessed)
	{
		GlobalFree(hBchannelPreprocessed);
		hBchannelPreprocessed = NULL;
	}
	hBchannelPreprocessed = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	BchannelPreprocessed = (unsigned char*)hBchannelPreprocessed;

	//��ͨ����ֵ
	int z = 0;
	for (int j = ColorHead; j < m_ColorImageSize; j = j + 3)
	{
		*(Rchannel + GrayHead + z) = *(lporigimage + j + 2);
		*(Gchannel + GrayHead + z) = *(lporigimage + j + 1);
		*(Bchannel + GrayHead + z) = *(lporigimage + j);
		z++;
	}

	/*����ͨ���ֱ���Ԥ����,����¼�²�����λ����Ϣ*/
	onlyPreProcess = true;

	lporigimage = Rchannel;
	//lporigimage = lpMaximage;
	deori(pairs);
	memcpy(RchannelPreprocessed, lpwmimage, m_ImageSize);
	//lpprehist��locmap��zn��testbuf_len�ļ�¼
	exctR = zn;
	testbuf_lenR = testbuf_len;
	if (hprehistRchannel)
	{
		GlobalFree(hprehistRchannel);
		hprehistRchannel = NULL;
	}
	hprehistRchannel = GlobalAlloc(GMEM_FIXED, 2 * (metInThreePro + 2) * sizeof(unsigned char));
	lpprehistRchannel = (unsigned char *)hprehistRchannel;
	memcpy(lpprehistRchannel, lpprehist, 2 * (metInThreePro + 2) * sizeof(unsigned char));
	if (isMerge == true)
	{
		if (hlocmapRchannel)
		{
			GlobalFree(hlocmapRchannel);
			hlocmapRchannel = NULL;
		}
		hlocmapRchannel = GlobalAlloc(GMEM_FIXED, lml);
		locmapRchannel = (unsigned char *)hlocmapRchannel;
		memcpy(locmapRchannel, locmap, lml);
	}

	lporigimage = Gchannel;
	//lporigimage = lpMediaimage;
	deori(pairs);
	memcpy(GchannelPreprocessed, lpwmimage, m_ImageSize);
	//lpprehist��locmap��zn�ļ�¼
	exctG = zn;
	testbuf_lenG = testbuf_len;
	if (hprehistGchannel)
	{
		GlobalFree(hprehistGchannel);
		hprehistGchannel = NULL;
	}
	hprehistGchannel = GlobalAlloc(GMEM_FIXED, 2 * (metInThreePro + 2) * sizeof(unsigned char));
	lpprehistGchannel = (unsigned char *)hprehistGchannel;
	memcpy(lpprehistGchannel, lpprehist, 2 * (metInThreePro + 2) * sizeof(unsigned char));
	if (isMerge == true)
	{
		if (hlocmapGchannel)
		{
			GlobalFree(hlocmapGchannel);
			hlocmapGchannel = NULL;
		}
		hlocmapGchannel = GlobalAlloc(GMEM_FIXED, lml);
		locmapGchannel = (unsigned char *)hlocmapGchannel;
		memcpy(locmapGchannel, locmap, lml);
	}

	lporigimage = Bchannel;
	//lporigimage = lpMinimage;
	deori(pairs);
	memcpy(BchannelPreprocessed, lpwmimage, m_ImageSize);
	//lpprehist��locmap�ļ�¼
	exctB = zn;
	testbuf_lenB = testbuf_len;
	if (hprehistBchannel)
	{
		GlobalFree(hprehistBchannel);
		hprehistBchannel = NULL;
	}
	hprehistBchannel = GlobalAlloc(GMEM_FIXED, 2 * (metInThreePro + 2) * sizeof(unsigned char));
	lpprehistBchannel = (unsigned char *)hprehistBchannel;
	memcpy(lpprehistBchannel, lpprehist, 2 * (metInThreePro + 2) * sizeof(unsigned char));
	if (isMerge == true)
	{
		if (hlocmapBchannel)
		{
			GlobalFree(hlocmapBchannel);
			hlocmapBchannel = NULL;
		}
		hlocmapBchannel = GlobalAlloc(GMEM_FIXED, lml);
		locmapBchannel = (unsigned char *)hlocmapBchannel;
		memcpy(locmapBchannel, locmap, lml);
	}
	onlyPreProcess = false;
	
	//��RGB��ֵ������ڴ�
	if (hwmimage)
	{
		GlobalFree(hwmimage);
		hwmimage = NULL;
	}
	hwmimage = GlobalAlloc(GMEM_FIXED, m_ColorImageSize);
	lpwmimage = (unsigned char*)hwmimage;

	//��ָ
	lporigimage = lpoimage;

	//���ļ�ͷд�����ָ��
	for (int i = 0; i < ColorHead; i++)
	{
		*(lpwmimage + i) = *(lporigimage + i);
	}

	//int max;
	//int t;
	int k = 0;
	int j = GrayHead;

	for (int i = ColorHead; i < m_ColorImageSize; i += 3)
	{
		*(lpwmimage + i + 2) = *(RchannelPreprocessed + j);
		*(lpwmimage + i + 1) = *(GchannelPreprocessed + j);
		*(lpwmimage + i) = *(BchannelPreprocessed + j);
		j++;
		k++;
	}

	reservedOnePairsEnMax();
}

void CDeprejbgDoc::reservedOnePairsEnMax()
{
	long el = 0, g = 0;
	          
	int pairsEn = 30;
	int pairsPre = pairsEn + 1;				//Ԥ����pairs��Ҫ����ǿpairs����1
	int metInMaxPre = pairsEn - 1;

	long testbuf_lenMax;

	//����ɫͼ��RGBͨ����ֵ�ֱ�ֵ���Ҷ�ͼ��ʽ
	int GrayHead = 1024 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);                 //�Ҷ�ͼ��λͼ�������
	int ColorHead = m_ColorImageSize - ImageWidth * ImageHeight * sizeof(unsigned char) * 3;   //��ɫͼ��λͼ���

	//����
	deoriRGBPre(lpwmimage);
	

	if (hmaxPreprocessed)
	{
		GlobalFree(hmaxPreprocessed);
		hmaxPreprocessed = NULL;
	}
	hmaxPreprocessed = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	maxPreprocessed = (unsigned char*)hmaxPreprocessed;

	/*maxͨ����Ԥ����,����¼�²�����λ����Ϣ*/
	onlyPreProcess = true;

	lporigimage = lpMaximage;
	deori(pairsPre);
	memcpy(maxPreprocessed, lpwmimage, m_ImageSize);
	//lpprehist��locmap��zn��testbuf_len�ļ�¼
	exctMax = zn;
	testbuf_lenMax = testbuf_len;
	if (hprehistMaxChannel)
	{
		GlobalFree(hprehistMaxChannel);
		hprehistMaxChannel = NULL;
	}
	hprehistMaxChannel = GlobalAlloc(GMEM_FIXED, 2 * (metInMaxPre + 2) * sizeof(unsigned char));
	lpprehistMaxChannel = (unsigned char *)hprehistMaxChannel;
	memcpy(lpprehistMaxChannel, lpprehist, 2 * (metInMaxPre + 2) * sizeof(unsigned char));
	if (isMerge == true)
	{
		if (hlocmapMaxChannel)
		{
			GlobalFree(hlocmapMaxChannel);
			hlocmapMaxChannel = NULL;
		}
		hlocmapMaxChannel = GlobalAlloc(GMEM_FIXED, lml);
		locmapMaxChannel = (unsigned char *)hlocmapMaxChannel;
		memcpy(locmapMaxChannel, locmap, lml);
	}
	onlyPreProcess = false;
	
	//����MAXͨ��
	if (hMaxEnimage)
	{
		GlobalFree(hMaxEnimage);
		hMaxEnimage = NULL;
	}
	hMaxEnimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	lpMaxEnimage = (unsigned char*)hMaxEnimage;
	memcpy(lpMaxEnimage, lpMaximage, m_ImageSize);

	if (hMaximageTemporary)
	{
		GlobalFree(hMaximageTemporary);
		hMaximageTemporary = NULL;
	}
	hMaximageTemporary = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	lpMaximageTemporary = (unsigned char*)hMaximageTemporary;

	memcpy(lpMaximageTemporary, lpMaximage, m_ImageSize);

	for (int m = 0; m < metInMaxPre + 1; m++)
	{
		el = derem(lpMaximageTemporary, lpMaxEnimage, m, metInMaxPre, testbuf_lenMax, exctMax, locmapMaxChannel, lpprehistMaxChannel);                  //����Ƕ��
		g = g + el;
		memcpy(lpMaxEnimage, lpMaximageTemporary, m_ImageSize);
	}

	//����Minͨ��
	if (hMinEnimage)
	{
		GlobalFree(hMinEnimage);
		hMinEnimage = NULL;
	}
	hMinEnimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	lpMinEnimage = (unsigned char*)hMinEnimage;
	int z = 0;
	for (int i = GrayHead; i < m_ImageSize; i++)
	{
		double c2 = *(lpC2image + z);
		double c = *(lpCimage + z);
		//double min = *(lpMinimage + i);
		double max = *(lpMaxEnimage + i);
		//double median = *(lpMediaimage + i);
		if (max != 0)
			*(lpMinEnimage + i) = max * c2 + 0.5;
		else
			*(lpMinEnimage + i) = 0;
		z++;
	}

	//����Mediaͨ��
	if (hMedianEnimage)
	{
		GlobalFree(hMedianEnimage);
		hMedianEnimage = NULL;
	}
	hMedianEnimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	lpMedianEnimage = (unsigned char*)hMedianEnimage;
	z = 0;
	for (int i = GrayHead; i < m_ImageSize; i++)
	{
		double c = *(lpCimage + z);
		double c2 = *(lpC2image + z);
		double max = *(lpMaxEnimage + i);
		double min = *(lpMinEnimage + i);
		//double median = *(lpMediaimage + i);
		if (max != min)
			*(lpMedianEnimage + i) = max * (c - c2 * c + c2) + 0.5;
		else
			*(lpMedianEnimage + i) = max;
		z++;
	}

	if (hlocationMapColor)
	{
		GlobalFree(hlocationMapColor);
		hlocationMapColor = NULL;
	}
	hlocationMapColor = GlobalAlloc(GMEM_FIXED, ImageWidth*ImageHeight);
	locationMapColor = (unsigned char*)hlocationMapColor;

	if (hlocationMapColor)
	{
		GlobalFree(hlocationMapColor);
		hlocationMapColor = NULL;
	}
	hlocationMapColor = GlobalAlloc(GMEM_FIXED, ImageWidth*ImageHeight);
	locationMapColor = (unsigned char*)hlocationMapColor;

	if (hminOverflowMap)
	{
		GlobalFree(hminOverflowMap);
		hminOverflowMap = NULL;
	}
	hminOverflowMap = GlobalAlloc(GMEM_FIXED, ImageWidth*ImageHeight);
	minOverflowMap = (unsigned char*)hminOverflowMap;

	if (hmedianOverflowMap)
	{
		GlobalFree(hmedianOverflowMap);
		hmedianOverflowMap = NULL;
	}
	hmedianOverflowMap = GlobalAlloc(GMEM_FIXED, ImageWidth*ImageHeight);
	medianOverflowMap = (unsigned char*)hmedianOverflowMap;

	if (hoverflowValue)
	{
		GlobalFree(hoverflowValue);
		hoverflowValue = NULL;
	}
	hoverflowValue = GlobalAlloc(GMEM_FIXED, ImageWidth*ImageHeight*sizeof(unsigned char));
	overflowValue = (unsigned char*)hoverflowValue;

	int overflowNum = 0;
	for (int i = 0; i < ImageWidth*ImageHeight; i++)
	{
		double c, c2;
		double minRe, medianRe, maxRe;
		unsigned char maxEn, minEn, medianEn, maxReChar, minReChar, medianReChar, minOri, medianOri, maxOri;
		int minSamePlus, medianSamePlus;               //ͬ��ͬ�����ֵ

		maxEn = *(lpMaxEnimage + GrayHead + i);
		medianEn = *(lpMedianEnimage + GrayHead + i);
		minEn = *(lpMinEnimage + GrayHead + i);

		c = ((double)(medianEn - minEn)) / ((double)(maxEn - minEn));
		c2 = ((double)minEn) / ((double)maxEn);

		maxReChar = *(lpMaximage + GrayHead + i);
		maxRe = (double)maxReChar;
		if (maxReChar != 0)
			minRe = maxRe * c2 + 0.5;
		else
			minRe = 0;
		minReChar = (unsigned char)minRe;
		if (maxReChar != minReChar)
			medianRe = maxRe * (c - c2 * c + c2) + 0.5;
		else
			medianRe = maxRe;
		medianReChar = (unsigned char)medianRe;

		maxOri = *(lpMaximage + GrayHead + i);
		minOri = *(lpMinimage + GrayHead + i);
		medianOri = *(lpMediaimage + GrayHead + i);
		if (minOri != minReChar || medianOri != medianReChar)
		{
			minSamePlus = minOri + (maxEn - maxOri);
			medianSamePlus = medianOri + (maxEn - maxOri);
			if (minSamePlus >= 0)                  //�������
			{
				*(locationMapColor + i) = 1;
				*(minOverflowMap + i) = 0;
				*(medianOverflowMap + i) = 0;
				*(lpMedianEnimage + GrayHead + i) = medianSamePlus;
				*(lpMinEnimage + GrayHead + i) = minSamePlus;
			}
			else
			{
				*(locationMapColor + i) = 0;
				*(minOverflowMap + i) = 1;
				*(overflowValue + overflowNum) = 0 - minSamePlus;
				overflowNum++;                 
				if (medianSamePlus <= 0)                 //��medianSamePlusҲ���
				{
					*(overflowValue + overflowNum) = 0 - medianSamePlus;
					overflowNum++;
					*(medianOverflowMap + i) = 1;
				}
				else
				{
					*(medianOverflowMap + i) = 0;
					*(lpMedianEnimage + GrayHead + i) = medianSamePlus;
				}
				*(lpMedianEnimage + GrayHead + i) = medianEn;
				*(lpMinEnimage + GrayHead + i) = minEn;
			}
			
		}
		else
		{
			*(locationMapColor + i) = 0;
			*(minOverflowMap + i) = 0;
			*(medianOverflowMap + i) = 0;
		}
	}

	/*
	for (int i = GrayHead; i < m_ImageSize; i++)          //С��127���ƣ�����128����
	{
		unsigned char maxEn, medianEn, minEn;
		maxEn = *(lpMaxEnimage + i);
		medianEn = *(lpMedianEnimage + i);
		minEn = *(lpMinEnimage + i);
		if (maxEn <= 127)
		{
			*(lpMaxEnimage + i) = maxEn - 1;
		}
		else
		{
			*(lpMaxEnimage + i) = maxEn + 1;
		}
			
		if (medianEn <= 127)
		{
			*(lpMedianEnimage + i) = medianEn - 1;
		}
		else
		{
			*(lpMedianEnimage + i) = medianEn + 1;
		}

		if (minEn <= 127)
		{
			*(lpMinEnimage + i) = minEn - 1;
		}
		else
		{
			*(lpMinEnimage + i) = minEn + 1;
		}
	}
	
	
	for (int i = 0; i < ImageWidth*ImageHeight; i++)
	{
		if (*(minOverflowMap + i) == 1)
		{
			*(lpMinEnimage + GrayHead + i) = 0;
			if (*(medianOverflowMap + i) == 1)
			{
				*(lpMedianEnimage + GrayHead + i) = 255;
			}
		}
	}
	*/
	//��RGB��ֵ������ڴ�
	if (hwmimage)
	{
		GlobalFree(hwmimage);
		hwmimage = NULL;
	}
	hwmimage = GlobalAlloc(GMEM_FIXED, m_ColorImageSize);
	lpwmimage = (unsigned char*)hwmimage;

	//��ָ
	lporigimage = lpoimage;

	//���ļ�ͷд�����ָ��
	for (int i = 0; i < ColorHead; i++)
	{
		*(lpwmimage + i) = *(lporigimage + i);
	}

	int k = 0;
	int j = GrayHead;
	for (int i = ColorHead; i < m_ColorImageSize; i += 3)
	{

		*(lpwmimage + i + *(lpMreimage + k)) = *(lpMaxEnimage + j);
		*(lpwmimage + i + *(lpMereimage + k)) = *(lpMedianEnimage + j);
		*(lpwmimage + i + *(lpMireimage + k)) = *(lpMinEnimage + j);

		j++;
		k++;
	}
}

void CDeprejbgDoc::samePlus()
{
	int mm = 50;

	//����ɫͼ��RGBͨ����ֵ�ֱ�ֵ���Ҷ�ͼ��ʽ
	int GrayHead = 1024 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);                 //�Ҷ�ͼ��λͼ�������
	int ColorHead = m_ColorImageSize - ImageWidth * ImageHeight * sizeof(unsigned char) * 3;   //��ɫͼ��λͼ���
	deoriRGBPre(lporigimage);
	//��¼lporigimage��ָ��
	lpoimage = lporigimage;

	//����MAXͨ��
	lporigimage = lpMaximage;
	deori(mm);

	if (hMaxEnimage)
	{
		GlobalFree(hMaxEnimage);
		hMaxEnimage = NULL;
	}
	hMaxEnimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	lpMaxEnimage = (unsigned char*)hMaxEnimage;
	memcpy(lpMaxEnimage, lpwmimage, m_ImageSize);

	if (hMinEnimage)
	{
		GlobalFree(hMinEnimage);
		hMinEnimage = NULL;
	}
	hMinEnimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	lpMinEnimage = (unsigned char*)hMinEnimage;

	if (hMedianEnimage)
	{
		GlobalFree(hMedianEnimage);
		hMedianEnimage = NULL;
	}
	hMedianEnimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	lpMedianEnimage = (unsigned char*)hMedianEnimage;

	if (hlocationMapColor)
	{
		GlobalFree(hlocationMapColor);
		hlocationMapColor = NULL;
	}
	hlocationMapColor = GlobalAlloc(GMEM_FIXED, ImageWidth*ImageHeight);
	locationMapColor = (unsigned char*)hlocationMapColor;

	int z = 0;
	int len = 0;
	int minMinSamePlus = 0, minMedianSamePlus = 0;
	for (int i = GrayHead; i < m_ImageSize; i++)
	{
		int minSamePlus, medianSamePlus;
		int minSamePlus2, medianSamePlus2;
		unsigned char maxEn, maxOri, minOri, medianOri;
		maxEn = *(lpMaxEnimage + i);
		maxOri = *(lpMaximage + i);
		minOri = *(lpMinimage + i);
		medianOri = *(lpMediaimage + i);
		minSamePlus = minOri + (maxEn - maxOri);
		medianSamePlus = medianOri + (maxEn - maxOri);
		minSamePlus2 = minSamePlus;
		medianSamePlus2 = medianSamePlus;
		if (medianSamePlus < 0)
		{
			medianSamePlus = abs(medianSamePlus);
		}

		if (minSamePlus < 0)
		{
			minSamePlus = abs(minSamePlus);
			*(locationMapColor + z) = 1;
			len++;
		}
		else 
		{
			*(locationMapColor + z) = 0;
		}
		z++;

		*(lpMinEnimage + i) = minSamePlus;
		*(lpMedianEnimage + i) = medianSamePlus;

		if (minMinSamePlus > minSamePlus2)
			minMinSamePlus = minSamePlus2;
		if (minMedianSamePlus > medianSamePlus2)
			minMedianSamePlus = medianSamePlus2;

	}

	//��RGB��ֵ������ڴ�
	if (hwmimage)
	{
		GlobalFree(hwmimage);
		hwmimage = NULL;
	}
	hwmimage = GlobalAlloc(GMEM_FIXED, m_ColorImageSize);
	lpwmimage = (unsigned char*)hwmimage;

	//��ָ
	lporigimage = lpoimage;

	//���ļ�ͷд�����ָ��
	for (int i = 0; i < ColorHead; i++)
	{
		*(lpwmimage + i) = *(lporigimage + i);
	}

	int k = 0;
	int j = GrayHead;
	for (int i = ColorHead; i < m_ColorImageSize; i += 3)
	{

		*(lpwmimage + i + *(lpMreimage + k)) = *(lpMaxEnimage + j);
		*(lpwmimage + i + *(lpMereimage + k)) = *(lpMedianEnimage + j);
		*(lpwmimage + i + *(lpMireimage + k)) = *(lpMinEnimage + j);

		j++;
		k++;
	}
}

long CDeprejbgDoc::colorToGrayOnlyPreprocess(unsigned char *originalImage, int pairs)
{
	long testbuf_lenThree;
	int metInColorToGray=pairs-1;

	//��¼lporigimage��ָ��
	lpoimage = lporigimage;

	//ת�Ҷ�ͼ
	unsigned long m_ImageSize_ori = 1024 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + ImageWidth * ImageHeight * sizeof(unsigned char);
	int ColorImageWidth = ImageWidth;
	ImageWidth = 3 * ImageWidth;   //RGB�ֱ𵥶������Ҷ�ͼ��һ������ֵ��ͼ���Ϊԭ��3��
	m_ImageSize = 1024 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + ImageWidth * ImageHeight * sizeof(unsigned char);

	//����ɫͼ��RGBͨ����ֵ�ֱ�ֵ���Ҷ�ͼ��ʽ
	int GrayHead = 1024 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);                 //�Ҷ�ͼ��λͼ�������
	int ColorHead = m_ColorImageSize - ColorImageWidth * ImageHeight * sizeof(unsigned char) * 3;   //��ɫͼ��λͼ���

	//��ʼ��
	if (hImage2D)
	{
		GlobalFree(hImage2D);
		hImage2D = NULL;
	}
	hImage2D = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	Image2D = (unsigned char*)hImage2D;

	//��RGB����Ҷ�
	int z = 0;
	for (int j = ColorHead; j < m_ColorImageSize; j = j + 3)
	{
		*(Image2D + GrayHead + z) = *(originalImage + j);
		*(Image2D + GrayHead + z + 1) = *(originalImage + j + 1);
		*(Image2D + GrayHead + z + 2) = *(originalImage + j + 2);
		z = z + 3;
	}

	//��ʼ��
	if (hwmimage)
	{
		GlobalFree(hwmimage);
		hwmimage = NULL;
	}
	hwmimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	lpwmimage = (unsigned char*)hwmimage;

	/*���������ص�rgbֵ��ϳ�һ��ֱ��ͼ��Ԥ��������¼�²�����λ����Ϣ*/
	onlyPreProcess = true;

	lporigimage = Image2D;
	deori(pairs);

	exctThree = zn;
	testbuf_lenThree = testbuf_len;
	if (hprehistColorToGray)
	{
		GlobalFree(hprehistColorToGray);
		hprehistColorToGray = NULL;
	}
	hprehistColorToGray = GlobalAlloc(GMEM_FIXED, 2 * (metInColorToGray + 2) * sizeof(unsigned char));
	lpprehistColorToGray = (unsigned char *)hprehistColorToGray;
	memcpy(lpprehistColorToGray, lpprehist, 2 * (metInColorToGray + 2) * sizeof(unsigned char));
	
	if (isMerge == true)
	{
		if (hlocmapColorToGray)
		{
			GlobalFree(hlocmapColorToGray);
			hlocmapColorToGray = NULL;
		}
		hlocmapColorToGray = GlobalAlloc(GMEM_FIXED, lml);
		locmapColorToGray = (unsigned char *)hlocmapColorToGray;
		memcpy(locmapColorToGray, locmap, lml);
	}

	onlyPreProcess = false;

	if (hImage2DEnhanced)
	{
		GlobalFree(hImage2DEnhanced);
		hImage2DEnhanced = NULL;
	}
	hImage2DEnhanced = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	Image2DEnhanced = (unsigned char*)hImage2DEnhanced;
	memcpy(Image2DEnhanced, lpwmimage, m_ImageSize);

	//��ʼ��
	if (hwmimage)
	{
		GlobalFree(hwmimage);
		hwmimage = NULL;
	}
	hwmimage = GlobalAlloc(GMEM_FIXED, m_ColorImageSize);
	lpwmimage = (unsigned char*)hwmimage;

	//���ļ�ͷд�����ָ��
	for (int m = 0; m < ColorHead; m++)
	{
		*(lpwmimage + m) = *(lpoimage + m);
	}

	z = GrayHead;
	for (int o = ColorHead; o < m_ColorImageSize; o = o + 3)
	{
		*(lpwmimage + o) = *(Image2DEnhanced + z);
		*(lpwmimage + o + 1) = *(Image2DEnhanced + z + 1);
		*(lpwmimage + o + 2) = *(Image2DEnhanced + z + 2);
		z = z + 3;
	}

	//��ָ
	lporigimage = lpoimage;

	//��д
	m_ImageSize = m_ImageSize_ori;
	ImageWidth = ColorImageWidth;

	return testbuf_lenThree;
}

void CDeprejbgDoc::colorToGrayOnlyPreprocessRecover(int pairs, unsigned char *lpprehistColorToGray, unsigned char *locmapColorToGray, long exctThree)
{
	int metInColorToGray = pairs - 1;

	unsigned char *wpColorToGray;

	//��¼lporigimage��ָ��
	lpoimage = lporigimage;

	//ת�Ҷ�ͼ
	unsigned long int m_ImageSize_ori = m_ImageSize;
	int ColorImageWidth = ImageWidth;
	ImageWidth = 3 * ImageWidth;
	m_ImageSize = 1024 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + ImageWidth * ImageHeight * sizeof(unsigned char);

	//����ɫͼ��RGBͨ����ֵ�ֱ�ֵ���Ҷ�ͼ��ʽ
	int GrayHead = 1024 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);                 //�Ҷ�ͼ��λͼ�������
	int ColorHead = m_ColorImageSize - ColorImageWidth * ImageHeight * sizeof(unsigned char) * 3;   //��ɫͼ��λͼ���

	//��ɫͼ��תΪ�Ҷ�ͼ��
	//��ʼ��
	if (hImage2DEnhanced)
	{
		GlobalFree(hImage2DEnhanced);
		hImage2DEnhanced = NULL;
	}
	hImage2DEnhanced = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	Image2DEnhanced = (unsigned char*)hImage2DEnhanced;

	//��RGB����Ҷ�
	int z = 0;
	for (int j = ColorHead; j < m_ColorImageSize; j = j + 3)
	{
		*(Image2DEnhanced + GrayHead + z) = *(lpwmimage + j);
		*(Image2DEnhanced + GrayHead + z + 1) = *(lpwmimage + j + 1);
		*(Image2DEnhanced + GrayHead + z + 2) = *(lpwmimage + j + 2);
		z = z + 3;
	}

	//��ʼ��
	if (hImage2D)
	{
		GlobalFree(hImage2D);
		hImage2D = NULL;
	}
	hImage2D = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	Image2D = (unsigned char*)hImage2D;

	//�ָ�
	wpColorToGray = Image2DEnhanced + GrayHead;
	preProcessRecover(lpprehistColorToGray, locmapColorToGray, metInColorToGray, exctThree, wpColorToGray);
	memcpy(Image2D, Image2DEnhanced, m_ImageSize);

	
	//��ǿ���������ʼ��
	if (hwmimage)
	{
		GlobalFree(hwmimage);
		hwmimage = NULL;
	}
	hwmimage = GlobalAlloc(GMEM_FIXED, m_ColorImageSize);   //��ɫͼ���С
	lpwmimage = (unsigned char*)hwmimage;

	//���ļ�ͷд�����ָ��
	for (int m = 0; m < ColorHead; m++)
	{
		*(lpwmimage + m) = *(lpoimage + m);
	}

	z = GrayHead;
	for (int o = ColorHead; o < m_ColorImageSize; o = o + 3)
	{
		*(lpwmimage + o) = *(Image2D + z);
		*(lpwmimage + o + 1) = *(Image2D + z + 1);
		*(lpwmimage + o + 2) = *(Image2D + z + 2);
		z = z + 3;
	}


	//��ָ
	lporigimage = lpoimage;

	//��д
	m_ImageSize = m_ImageSize_ori;
	ImageWidth = ColorImageWidth;
}

void CDeprejbgDoc::maxEnhancedWithSamePlus()
{
	int s =50;
	for (int pairs = s; pairs < s+1; pairs++)
	{
		int metInColorToGray = pairs - 1;
		long testbuf_lenThree;

		long el = 0, g = 0;

		//����ɫͼ��RGBͨ����ֵ�ֱ�ֵ���Ҷ�ͼ��ʽ
		int GrayHead = 1024 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);                 //�Ҷ�ͼ��λͼ�������
		int ColorHead = m_ColorImageSize - ImageWidth * ImageHeight * sizeof(unsigned char) * 3;   //��ɫͼ��λͼ���

		//Ԥ����
		testbuf_lenThree = colorToGrayOnlyPreprocess(lporigimage,pairs);

		//����
		deoriRGBPre(lpwmimage);

		if (hMaximageTemporary)
		{
			GlobalFree(hMaximageTemporary);
			hMaximageTemporary = NULL;
		}
		hMaximageTemporary = GlobalAlloc(GMEM_FIXED, m_ImageSize);
		lpMaximageTemporary = (unsigned char*)hMaximageTemporary;

		memcpy(lpMaximageTemporary, lpMaximage, m_ImageSize);

		if (hMaxEnimage)
		{
			GlobalFree(hMaxEnimage);
			hMaxEnimage = NULL;
		}
		hMaxEnimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
		lpMaxEnimage = (unsigned char*)hMaxEnimage;
		memcpy(lpMaxEnimage, lpMaximage, m_ImageSize);

		int maxPairs;
		for (int m = 0; m < metInColorToGray + 1; m++)
		{
			el = derem(lpMaximageTemporary, lpMaxEnimage, m, metInColorToGray, testbuf_lenThree, exctThree, locmapColorToGray, lpprehistColorToGray);                  //����Ƕ��
			g = g + el;
			memcpy(lpMaxEnimage, lpMaximageTemporary, m_ImageSize);
			/*
			if (failed == 1)
			{
				maxPairs = m;
				break;
			}
			*/
		}
		payload = g;

		int maxBinNum[256];
		histStatistics(lpMaximage, maxBinNum);
		long maxEmbedLenPermit=0;
		
		for (int i = 0; i < pairs; i=i+2)
		{
			maxEmbedLenPermit = maxEmbedLenPermit + maxBinNum[i] + maxBinNum[i+1];
		}
		maxEmbedLenPermit = 8 * (maxEmbedLenPermit / 8);
		
		//����Min��Medianͨ��
		if (hMinEnimage)
		{
			GlobalFree(hMinEnimage);
			hMinEnimage = NULL;
		}
		hMinEnimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
		lpMinEnimage = (unsigned char*)hMinEnimage;

		if (hMedianEnimage)
		{
			GlobalFree(hMedianEnimage);
			hMedianEnimage = NULL;
		}
		hMedianEnimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
		lpMedianEnimage = (unsigned char*)hMedianEnimage;

		for (int i = GrayHead; i < m_ImageSize; i++)
		{
			unsigned char maxOri, medianOri, minOri, maxEn;

			maxOri = *(lpMaximage + i);
			maxEn = *(lpMaxEnimage + i);
			medianOri = *(lpMediaimage + i);
			minOri = *(lpMinimage + i);

			*(lpMedianEnimage + i) = medianOri + (maxEn - maxOri);
			*(lpMinEnimage + i) = minOri + (maxEn - maxOri);
		}

		//��RGB��ֵ������ڴ�
		if (hwmimage)
		{
			GlobalFree(hwmimage);
			hwmimage = NULL;
		}
		hwmimage = GlobalAlloc(GMEM_FIXED, m_ColorImageSize);
		lpwmimage = (unsigned char*)hwmimage;

		//��ָ
		lporigimage = lpoimage;

		//���ļ�ͷд�����ָ��
		for (int i = 0; i < ColorHead; i++)
		{
			*(lpwmimage + i) = *(lporigimage + i);
		}

		int k = 0;
		int j = GrayHead;
		for (int i = ColorHead; i < m_ColorImageSize; i += 3)
		{

			*(lpwmimage + i + *(lpMreimage + k)) = *(lpMaxEnimage + j);
			*(lpwmimage + i + *(lpMereimage + k)) = *(lpMedianEnimage + j);
			*(lpwmimage + i + *(lpMireimage + k)) = *(lpMinEnimage + j);

			j++;
			k++;
		}

		psnr = psnrColorCalculate(lporigimage, lpwmimage);

		if (failed == 1)
		{
			//pairs = maxPairs;
			//s = maxPairs;
			//failed = 0;
 			break;
 			//maxNotEmbed(s);
		}

		bool batchImg,hidingRate;
		batchImg = false;
		hidingRate = false;
		
		//������ͼ
		
		if (strlen(FileTitle) == 11)       //ONLY FOR kodim+2λ��
		{
			FileTitle[7] = '\0';
		}
		else if (strlen(FileTitle) == 10)       //ONLY FOR kodim+1λ��
		{
			FileTitle[6] = '\0';
		}
		
		/*
		if (strlen(FileTitle) == 9)         //sipi+1λ��
		{
			FileTitle[5] = '\0';
		}
		else if (strlen(FileTitle) == 10)    //sipi+2λ��
		{
			FileTitle[6] = '\0';
		}
		*/
		if (batchImg == true)
		{
			

			char filename[500];

			filename[0] = '\0';

			sprintf(filename, "E:\\experiment\\task\\result-reversible\\sipi-new-pn\\%s_%d.bmp", FileTitle, pairs);

			FILE *fb;

			fb = fopen(filename, "wb");
			if (fb == NULL)
			{
				AfxMessageBox("Can Not Open File To Write");
				return;
			}
			fwrite(lpwmimage, sizeof(unsigned char), m_ColorImageSize, fb);
			fclose(fb);
		}
		
		
		//Ƕ����д���ļ�
		if (hidingRate == true)
		{
			//��Ƕ����pure hiding rateд���ļ�
			FILE *payloadtxt;
			payloadtxt = fopen("E:\\experiment\\task\\result-reversible\\sipi-new-pn\\pure hiding rate.txt", "a");
			if (payloadtxt != NULL)
				fprintf(payloadtxt, "\n %s_%d \t %6.4f", FileTitle, pairs, (double)payload / (double)(ImageHeight*ImageWidth));
			fclose(payloadtxt);

			//��Ƕ��д���ļ�
			FILE *embnbtxt;
			embnbtxt = fopen("E:\\experiment\\task\\result-reversible\\sipi-new-pn\\embnb hiding rate.txt", "a");
			if (embnbtxt != NULL)
				fprintf(embnbtxt, "\n %s_%d \t %6.4f", FileTitle, pairs, (double)embnb / (double)(ImageHeight*ImageWidth));
			fclose(embnbtxt);
			
		}
		

	}
}

void CDeprejbgDoc::maxEnhancedWithSamePlusRecover()
{
	/*����*/
	/*��ȡ*/
	/*ͬ��ͬ��*/
	/*Ԥ����ָ�*/
	int GrayHead = 1024 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);                 //�Ҷ�ͼ��λͼ�������
	int ColorHead = m_ColorImageSize - ImageWidth * ImageHeight * sizeof(unsigned char) * 3;   //��ɫͼ��λͼ���

	//����
	depreRGBPre(lpwmimage);

	if (hwmimage)
	{
		GlobalFree(hwmimage);
		hwmimage = NULL;
	}
	hwmimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	lpwmimage = (unsigned char*)hwmimage;

	//max channel��ȡ
	memcpy(lppreimage, lpMaximage, m_ImageSize);
	colorToGrayPreprocessRecover = true;
	depre();
	colorToGrayPreprocessRecover = false;
	if (hMaxReimage)
	{
		GlobalFree(hMaxReimage);
		hMaxReimage = NULL;
	}
	hMaxReimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	lpMaxReimage = (unsigned char*)hMaxReimage;

	memcpy(hMaxReimage, lpwmimage, m_ImageSize);

	//����Min median����,ͬ��ͬ��
	if (hMinReimage)
	{
		GlobalFree(hMinReimage);
		hMinReimage = NULL;
	}
	hMinReimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	lpMinReimage = (unsigned char*)hMinReimage;

	if (hMedianReimage)
	{
		GlobalFree(hMedianReimage);
		hMedianReimage = NULL;
	}
	hMedianReimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	lpMedianReimage = (unsigned char*)hMedianReimage;

	for (int i = GrayHead; i < m_ImageSize; i++)
	{
		unsigned char maxRe, medianEn, minEn, maxEn;

		maxRe = *(lpMaxReimage + i);
		maxEn = *(lpMaximage + i);
		medianEn = *(lpMediaimage + i);
		minEn = *(lpMinimage + i);

		*(lpMedianReimage + i) = medianEn - (maxEn - maxRe);
		*(lpMinReimage + i) = minEn - (maxEn - maxRe);
	}

	//��RGB��ֵ������ڴ�
	if (hwmimage)
	{
		GlobalFree(hwmimage);
		hwmimage = NULL;
	}
	hwmimage = GlobalAlloc(GMEM_FIXED, m_ColorImageSize);
	lpwmimage = (unsigned char*)hwmimage;

	//��ָ
	lporigimage = lpoimage;

	//���ļ�ͷд�����ָ��
	for (int i = 0; i < ColorHead; i++)
	{
		*(lpwmimage + i) = *(lporigimage + i);
	}

	int k = 0;
	int j = GrayHead;
	for (int i = ColorHead; i < m_ColorImageSize; i += 3)
	{

		*(lpwmimage + i + *(lpMreimage + k)) = *(lpMaxReimage + j);
		*(lpwmimage + i + *(lpMereimage + k)) = *(lpMedianReimage + j);
		*(lpwmimage + i + *(lpMireimage + k)) = *(lpMinReimage + j);

		j++;
		k++;
	}

	/*�ָ�Ԥ����ֱ��ͼ*/
	int pairs = mext + 1;
	colorToGrayOnlyPreprocessRecover(pairs, lphist, lpextmap, exct);	
}

long CDeprejbgDoc::threeOnOneOnlyLeftPreprocess(unsigned char *originalImage, int pairs)
{
	int doublePairs = pairs * 2;
	long testbuf_lenThree;
	int metInColorToGray = doublePairs - 1;

	//��¼lporigimage��ָ��
	lpoimage = lporigimage;

	//ת�Ҷ�ͼ
	unsigned long m_ImageSize_ori = 1024 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + ImageWidth * ImageHeight * sizeof(unsigned char);
	int ColorImageWidth = ImageWidth;
	ImageWidth = 3 * ImageWidth;   //RGB�ֱ𵥶������Ҷ�ͼ��һ������ֵ��ͼ���Ϊԭ��3��
	m_ImageSize = 1024 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + ImageWidth * ImageHeight * sizeof(unsigned char);

	//����ɫͼ��RGBͨ����ֵ�ֱ�ֵ���Ҷ�ͼ��ʽ
	int GrayHead = 1024 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);                 //�Ҷ�ͼ��λͼ�������
	int ColorHead = m_ColorImageSize - ColorImageWidth * ImageHeight * sizeof(unsigned char) * 3;   //��ɫͼ��λͼ���

	//��ʼ��
	if (hImage2D)
	{
		GlobalFree(hImage2D);
		hImage2D = NULL;
	}
	hImage2D = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	Image2D = (unsigned char*)hImage2D;

	//��RGB����Ҷ�
	int z = 0;
	for (int j = ColorHead; j < m_ColorImageSize; j = j + 3)
	{
		*(Image2D + GrayHead + z) = *(originalImage + j);
		*(Image2D + GrayHead + z + 1) = *(originalImage + j + 1);
		*(Image2D + GrayHead + z + 2) = *(originalImage + j + 2);
		z = z + 3;
	}

	//��ʼ��
	if (hwmimage)
	{
		GlobalFree(hwmimage);
		hwmimage = NULL;
	}
	hwmimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	lpwmimage = (unsigned char*)hwmimage;

	/*���������ص�rgbֵ��ϳ�һ��ֱ��ͼ��Ԥ��������¼�²�����λ����Ϣ*/
	onlyPreProcess = true;
	leftPreProcess = true;

	lporigimage = Image2D;
	deori(doublePairs);

	exctThree = zn;
	testbuf_lenThree = testbuf_len;
	if (hprehistColorToGray)
	{
		GlobalFree(hprehistColorToGray);
		hprehistColorToGray = NULL;
	}
	hprehistColorToGray = GlobalAlloc(GMEM_FIXED, 2 * (metInColorToGray + 2) * sizeof(unsigned char));
	lpprehistColorToGray = (unsigned char *)hprehistColorToGray;
	memcpy(lpprehistColorToGray, lpprehist, 2 * (metInColorToGray + 2) * sizeof(unsigned char));

	if (isMerge == true)
	{
		if (hlocmapColorToGray)
		{
			GlobalFree(hlocmapColorToGray);
			hlocmapColorToGray = NULL;
		}
		hlocmapColorToGray = GlobalAlloc(GMEM_FIXED, lml);
		locmapColorToGray = (unsigned char *)hlocmapColorToGray;
		memcpy(locmapColorToGray, locmap, lml);
	}

	leftPreProcess = false;
	onlyPreProcess = false;

	if (hImage2DEnhanced)
	{
		GlobalFree(hImage2DEnhanced);
		hImage2DEnhanced = NULL;
	}
	hImage2DEnhanced = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	Image2DEnhanced = (unsigned char*)hImage2DEnhanced;
	memcpy(Image2DEnhanced, lpwmimage, m_ImageSize);

	//��ʼ��
	if (hwmimage)
	{
		GlobalFree(hwmimage);
		hwmimage = NULL;
	}
	hwmimage = GlobalAlloc(GMEM_FIXED, m_ColorImageSize);
	lpwmimage = (unsigned char*)hwmimage;

	//���ļ�ͷд�����ָ��
	for (int m = 0; m < ColorHead; m++)
	{
		*(lpwmimage + m) = *(lpoimage + m);
	}

	z = GrayHead;
	for (int o = ColorHead; o < m_ColorImageSize; o = o + 3)
	{
		*(lpwmimage + o) = *(Image2DEnhanced + z);
		*(lpwmimage + o + 1) = *(Image2DEnhanced + z + 1);
		*(lpwmimage + o + 2) = *(Image2DEnhanced + z + 2);
		z = z + 3;
	}

	//��ָ
	lporigimage = lpoimage;

	//��д
	m_ImageSize = m_ImageSize_ori;
	ImageWidth = ColorImageWidth;

	return testbuf_lenThree;
}

void CDeprejbgDoc::threeOnOneOnlyLeftPreprocessRecover(int pairs, unsigned char *imageAfterPre, unsigned char *lpprehistColorToGray, unsigned char *locmapColorToGray, long exctThree)
{
	int doublePairs = pairs * 2;
	int metInColorToGray = doublePairs - 1;

	unsigned char *wpColorToGray;

	//��¼lporigimage��ָ��
	lpoimage = lporigimage;

	//ת�Ҷ�ͼ
	unsigned long int m_ImageSize_ori = m_ImageSize;
	int ColorImageWidth = ImageWidth;
	ImageWidth = 3 * ImageWidth;
	m_ImageSize = 1024 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + ImageWidth * ImageHeight * sizeof(unsigned char);

	int GrayHead = 1024 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);                 //�Ҷ�ͼ��λͼ�������
	int ColorHead = m_ColorImageSize - ColorImageWidth * ImageHeight * sizeof(unsigned char) * 3;   //��ɫͼ��λͼ���


	//��ʼ��
	if (hImage2DEnhanced)
	{
		GlobalFree(hImage2DEnhanced);
		hImage2DEnhanced = NULL;
	}
	hImage2DEnhanced = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	Image2DEnhanced = (unsigned char*)hImage2DEnhanced;

	//��RGB����Ҷ�
	int z = 0;
	for (int j = ColorHead; j < m_ColorImageSize; j = j + 3)
	{
		*(Image2DEnhanced + GrayHead + z) = *(imageAfterPre + j);
		*(Image2DEnhanced + GrayHead + z + 1) = *(imageAfterPre + j + 1);
		*(Image2DEnhanced + GrayHead + z + 2) = *(imageAfterPre + j + 2);
		z = z + 3;
	}

	//��ʼ��
	if (hImage2D)
	{
		GlobalFree(hImage2D);
		hImage2D = NULL;
	}
	hImage2D = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	Image2D = (unsigned char*)hImage2D;

	leftPreProcessRecover = true;
	wpColorToGray = Image2DEnhanced + GrayHead;
	preProcessRecover(lpprehistColorToGray, locmapColorToGray, metInColorToGray, exctThree, wpColorToGray);
	memcpy(Image2D, Image2DEnhanced, m_ImageSize);
	leftPreProcessRecover = false;

	//��ǿ���������ʼ��
	if (hwmimage)
	{
		GlobalFree(hwmimage);
		hwmimage = NULL;
	}
	hwmimage = GlobalAlloc(GMEM_FIXED, m_ColorImageSize);   //��ɫͼ���С
	lpwmimage = (unsigned char*)hwmimage;

	//���ļ�ͷд�����ָ��
	for (int m = 0; m < ColorHead; m++)
	{
		*(lpwmimage + m) = *(lpoimage + m);
	}

	z = GrayHead;
	for (int o = ColorHead; o < m_ColorImageSize; o = o + 3)
	{
		*(lpwmimage + o) = *(Image2D + z);
		*(lpwmimage + o + 1) = *(Image2D + z + 1);
		*(lpwmimage + o + 2) = *(Image2D + z + 2);
		z = z + 3;
	}

	//��ָ
	lporigimage = lpoimage;

	//��д
	m_ImageSize = m_ImageSize_ori;
	ImageWidth = ColorImageWidth;

}

void CDeprejbgDoc::twoSortAndPreprocess()
{
	int s =40;
	for (int pairs = s; pairs < s+1; pairs++)
	{
		int metInColorToGray = pairs - 1;
		
		long exctMaxChannel, testbuf_lenMaxChannel, testbuf_lenThree;

		int GrayHead = 1024 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);                 //�Ҷ�ͼ��λͼ�������
		int ColorHead = m_ColorImageSize - ImageWidth * ImageHeight * sizeof(unsigned char) * 3;   //��ɫͼ��λͼ���

		//�Զ���ͼ����
		bool batchImg, hidingRate;
		batchImg = false;
		hidingRate = false;

		
		if (strlen(FileTitle) == 11)       //ONLY FOR kodim+2λ��
		{
			FileTitle[7] = '\0';
		}
		else if (strlen(FileTitle) == 10)       //ONLY FOR kodim+1λ��
		{
			FileTitle[6] = '\0';
		}
		
		/*
		if (strlen(FileTitle) == 9)         //sipi+1λ��
		{
			FileTitle[5] = '\0';
		}
		else if (strlen(FileTitle) == 10)    //sipi+2λ��
		{
			FileTitle[6] = '\0';
		}
		*/
		//��¼lporigimage��ָ��
		lpoimage = lporigimage;

		//��һ������
		deoriRGBPre(lporigimage);

		//��ʼ��
		if (hMaxPreimage)
		{
			GlobalFree(hMaxPreimage);
			hMaxPreimage = NULL;
		}
		hMaxPreimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
		lpMaxPreimage = (unsigned char*)hMaxPreimage;

		if (hMedianPreimage)
		{
			GlobalFree(hMedianPreimage);
			hMedianPreimage = NULL;
		}
		hMedianPreimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
		lpMedianPreimage = (unsigned char*)hMedianPreimage;

		if (hMinPreimage)
		{
			GlobalFree(hMinPreimage);
			hMinPreimage = NULL;
		}
		hMinPreimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
		lpMinPreimage = (unsigned char*)hMinPreimage;

		/*��MaxChannelԤ����,����¼��ز���,�õ�lpMaxPreimage*/
		onlyPreprocessOneChannel(lpMaximage, pairs);
		exctMaxChannel = exctOneChannel;
		testbuf_lenMaxChannel = testbuf_lenOneChannel;
		if (hprehistMaxChannel)
		{
			GlobalFree(hprehistMaxChannel);
			hprehistMaxChannel = NULL;
		}
		hprehistMaxChannel = GlobalAlloc(GMEM_FIXED, 2 * (metInColorToGray + 2) * sizeof(unsigned char));
		lpprehistMaxChannel = (unsigned char *)hprehistMaxChannel;
		memcpy(lpprehistMaxChannel, lpprehistOneChannel, 2 * (metInColorToGray + 2) * sizeof(unsigned char));
		if (isMerge == true)
		{
			if (hlocmapMaxChannel)
			{
				GlobalFree(hlocmapMaxChannel);
				hlocmapMaxChannel = NULL;
			}
			hlocmapMaxChannel = GlobalAlloc(GMEM_FIXED, lml);
			locmapMaxChannel = (unsigned char *)hlocmapMaxChannel;
			memcpy(locmapMaxChannel, locmapOneChannel, lml);
		}

		memcpy(lpMaxPreimage, lpwmimage, m_ImageSize);

		//test
		int min = 0;
		for (int i = GrayHead; i < m_ImageSize; i++)
		{
			int maxOri, maxPre;
			maxOri = *(lpMaximage + i);
			maxPre = *(lpMaxPreimage + i);
			if (min < (maxPre - maxOri))
				min = maxPre - maxOri;
			//if (min == -30 && maxOri!=255 && min == (maxPre - maxOri))
			//	min = -30;
		}

		/*ͬ��ͬ������Median Channel��Min Channel��������������,�õ���һ��Ԥ������ͼ�񣬷���һ������ĺ��Ƶ��Ұ�Ŀ�bin*/
		/*
		int minPrenum = 0, medianPrenum = 0;
		for (int i = GrayHead; i < m_ImageSize; i++)
		{
			int maxOri, medianOri, minOri, maxPre, medianPre, minPre;

			maxOri = *(lpMaximage + i);
			medianOri = *(lpMediaimage + i);
			minOri = *(lpMinimage + i);

			maxPre = *(lpMaxPreimage + i);
			minPre = minOri + (maxPre - maxOri);
			medianPre = medianOri + (maxPre - maxOri);

			if (minPre < 0)
			{
				minPre = minPre + 255;
				//minPrenum++;
			}
			if (medianPre < 0)
			{
				medianPre = medianPre + 255;
				//medianPrenum++;
			}

			if (minPre < pairs)
			{
				minPrenum++;
			}
			if (medianPre < pairs)
			{
				medianPrenum++;
			}

			*(lpMedianPreimage + i) = medianPre;
			*(lpMinPreimage + i) = minPre;
		}
		*/

		/*ͬ��ͬ������Median Channel��Min Channel��������������,�õ���һ��Ԥ������ͼ�񣬷�������ֱ��ͼ�������һ��pairs����*/
		int minPrenum = 0, medianPrenum = 0;
		for (int i = GrayHead; i < m_ImageSize; i++)
		{
			int maxOri, medianOri, minOri, maxPre, medianPre, minPre;

			maxOri = *(lpMaximage + i);
			medianOri = *(lpMediaimage + i);
			minOri = *(lpMinimage + i);

			maxPre = *(lpMaxPreimage + i) + pairs;
			minPre = minOri + (maxPre - maxOri);
			medianPre = medianOri + (maxPre - maxOri);

			*(lpMaxPreimage + i) = maxPre;
			*(lpMedianPreimage + i) = medianPre;
			*(lpMinPreimage + i) = minPre;
		}

		//��Ԥ��������ֵ
		if (hwmimage)
		{
			GlobalFree(hwmimage);
			hwmimage = NULL;
		}
		hwmimage = GlobalAlloc(GMEM_FIXED, m_ColorImageSize);
		lpwmimage = (unsigned char*)hwmimage;

		//��ָ
		lporigimage = lpoimage;

		//���ļ�ͷд�����ָ��
		for (int i = 0; i < ColorHead; i++)
		{
			*(lpwmimage + i) = *(lporigimage + i);
		}

		int k = 0;
		int j = GrayHead;
		for (int i = ColorHead; i < m_ColorImageSize; i += 3)
		{

			*(lpwmimage + i + *(lpMreimage + k)) = *(lpMaxPreimage + j);
			*(lpwmimage + i + *(lpMereimage + k)) = *(lpMedianPreimage + j);
			*(lpwmimage + i + *(lpMireimage + k)) = *(lpMinPreimage + j);

			j++;
			k++;
		}

		if (hfirstPreimage)
		{
			GlobalFree(hfirstPreimage);
			hfirstPreimage = NULL;
		}
		hfirstPreimage = GlobalAlloc(GMEM_FIXED, m_ColorImageSize);
		firstPreimage = (unsigned char*)hfirstPreimage;
		memcpy(firstPreimage, lpwmimage, m_ColorImageSize);

		//�Զ���ͼ
		/*
		if (batchImg == true)
		{
			char filename[500];
			filename[0] = '\0';
			sprintf(filename, "E:\\experiment\\task\\result-reversible\\kodak-pre\\%s_firstpre_%d.bmp", FileTitle, pairs);
			FILE *fb;
			fb = fopen(filename, "wb");
			if (fb == NULL)
			{
				AfxMessageBox("Can Not Open File To Write");
				return;
			}
			fwrite(lpwmimage, sizeof(unsigned char), m_ColorImageSize, fb);
			fclose(fb);
		}
		*/

		/*�ϳ�һ����ֱ��ͼ����Ԥ��������¼��ز�����exctThree, locmapColorToGray, lpprehistColorToGrayΪȫ�֣�testbuf_lenThreeΪ�ֲ�*/
		//testbuf_lenThree = colorToGrayOnlyPreprocess(firstPreimage, pairs);
		testbuf_lenThree = threeOnOneOnlyLeftPreprocess(firstPreimage, pairs);


		/*ֱ��ͼ����ƽ�ƻص��м�*/
		if (hsecondPreimage)
		{
			GlobalFree(hsecondPreimage);
			hsecondPreimage = NULL;
		}
		hsecondPreimage = GlobalAlloc(GMEM_FIXED, m_ColorImageSize);
		secondPreimage = (unsigned char*)hsecondPreimage;
		memcpy(secondPreimage, lpwmimage, m_ColorImageSize);

		//�Զ���ͼ
		/*
		if (batchImg == true)
		{
			char filename[500];
			filename[0] = '\0';
			sprintf(filename, "E:\\experiment\\task\\result-reversible\\kodak-pre\\%s_secondpre_%d.bmp", FileTitle, pairs);
			FILE *fb;
			fb = fopen(filename, "wb");
			if (fb == NULL)
			{
				AfxMessageBox("Can Not Open File To Write");
				return;
			}
			fwrite(secondPreimage, sizeof(unsigned char), m_ColorImageSize, fb);
			fclose(fb);
		}
		*/
		for (int i = ColorHead; i < m_ColorImageSize; i++)
		{
			unsigned char secondPre;
			secondPre = *(secondPreimage + i);
			*(secondPreimage + i) = secondPre - pairs;
		}


		/*�ڶ������򣬶�����Ԥ�����Ĵ�ֱ��ͼ����*/
		deoriRGBPre(secondPreimage);

		/*��������max channel����Ƕ��*/
		long el = 0, g = 0;
		if (hMaximageTemporary)
		{
			GlobalFree(hMaximageTemporary);
			hMaximageTemporary = NULL;
		}
		hMaximageTemporary = GlobalAlloc(GMEM_FIXED, m_ImageSize);
		lpMaximageTemporary = (unsigned char*)hMaximageTemporary;

		memcpy(lpMaximageTemporary, lpMaximage, m_ImageSize);

		if (hMaxEnimage)
		{
			GlobalFree(hMaxEnimage);
			hMaxEnimage = NULL;
		}
		hMaxEnimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
		lpMaxEnimage = (unsigned char*)hMaxEnimage;
		memcpy(lpMaxEnimage, lpMaximage, m_ImageSize);

		//�������ݹ���
		extraBookkeepingDataEmbed(pairs, exctThree, testbuf_lenThree, locmapColorToGray, lpprehistColorToGray);
		int doubleMet = 2 * pairs - 1;
		int lpprehistLen = 2 * (doubleMet + 2);
		int addBookkeepingLen = (lpprehistLen + sizeof(long) * 2 + testbuf_lenThree) * 8;

		//����Ƕ��
		addDataEmbedJudge = true;
		for (int m = 0; m < metInColorToGray + 1; m++)
		{
			el = derem(lpMaximageTemporary, lpMaxEnimage, m, metInColorToGray, testbuf_lenMaxChannel, exctMaxChannel, locmapMaxChannel, lpprehistMaxChannel);                  //����Ƕ��
			g = g + el;
			memcpy(lpMaxEnimage, lpMaximageTemporary, m_ImageSize);
		}
		payload = g - addBookkeepingLen;
		addDataEmbedJudge = false;

		/*����min��median channel*/
		if (hMinEnimage)
		{
			GlobalFree(hMinEnimage);
			hMinEnimage = NULL;
		}
		hMinEnimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
		lpMinEnimage = (unsigned char*)hMinEnimage;

		if (hMedianEnimage)
		{
			GlobalFree(hMedianEnimage);
			hMedianEnimage = NULL;
		}
		hMedianEnimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
		lpMedianEnimage = (unsigned char*)hMedianEnimage;

		for (int i = GrayHead; i < m_ImageSize; i++)
		{
			unsigned char maxOri, medianOri, minOri, maxEn;

			maxOri = *(lpMaximage + i);
			maxEn = *(lpMaxEnimage + i);
			medianOri = *(lpMediaimage + i);
			minOri = *(lpMinimage + i);

			*(lpMedianEnimage + i) = medianOri + (maxEn - maxOri);
			*(lpMinEnimage + i) = minOri + (maxEn - maxOri);
		}

		/*��RGB��ֵ������ڴ�*/
		if (hwmimage)
		{
			GlobalFree(hwmimage);
			hwmimage = NULL;
		}
		hwmimage = GlobalAlloc(GMEM_FIXED, m_ColorImageSize);
		lpwmimage = (unsigned char*)hwmimage;

		//��ָ
		lporigimage = lpoimage;

		//���ļ�ͷд�����ָ��
		for (int i = 0; i < ColorHead; i++)
		{
			*(lpwmimage + i) = *(lporigimage + i);
		}

		k = 0;
		j = GrayHead;
		for (int i = ColorHead; i < m_ColorImageSize; i += 3)
		{

			*(lpwmimage + i + *(lpMreimage + k)) = *(lpMaxEnimage + j);
			*(lpwmimage + i + *(lpMereimage + k)) = *(lpMedianEnimage + j);
			*(lpwmimage + i + *(lpMireimage + k)) = *(lpMinEnimage + j);

			j++;
			k++;
		}

		met = metInColorToGray;
		psnr = psnrColorCalculate(lporigimage, lpwmimage);

		if (failed == 1)
		{
			//pairs = maxPairs;
			//s = maxPairs;
			//failed = 0;
			break;
			//maxNotEmbed(s);
		}

		//������ͼ
		if (batchImg == true)
		{
			char filename[500];

			filename[0] = '\0';

			sprintf(filename, "E:\\experiment\\task\\result-reversible\\huePreservation\\kodak-new\\%s_%d.bmp", FileTitle, pairs);

			FILE *fb;

			fb = fopen(filename, "wb");
			if (fb == NULL)
			{
				AfxMessageBox("Can Not Open File To Write");
				return;
			}
			fwrite(lpwmimage, sizeof(unsigned char), m_ColorImageSize, fb);
			fclose(fb);
		}


		//Ƕ����д���ļ�
		if (hidingRate == true)
		{
			//��Ƕ����pure hiding rateд���ļ�
			FILE *payloadtxt;
			payloadtxt = fopen("E:\\experiment\\task\\result-reversible\\huePreservation\\kodak-new\\pure hiding rate.txt", "a");
			if (payloadtxt != NULL)
				fprintf(payloadtxt, "\n %s_%d \t %6.4f", FileTitle, pairs, (double)payload / (double)(ImageHeight*ImageWidth));
			fclose(payloadtxt);

			//��Ƕ��д���ļ�
			FILE *embnbtxt;
			embnbtxt = fopen("E:\\experiment\\task\\result-reversible\\huePreservation\\kodak-new\\embnb hiding rate.txt", "a");
			if (embnbtxt != NULL)
				fprintf(embnbtxt, "\n %s_%d \t %6.4f", FileTitle, pairs, (double)embnb / (double)(ImageHeight*ImageWidth));
			fclose(embnbtxt);

		}

	}

}

/*���ô˺�����Ҫ��¼��Ӧ�Ĳ����ͼ�¼:exctOneChannel��testbuf_lenOneChannel��lpprehistOneChannel��locmapOneChannel*/
void CDeprejbgDoc::onlyPreprocessOneChannel(unsigned char *originalImage, int pairs)
{
	int metInColorToGray = pairs - 1;

	int GrayHead = 1024 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);                 //�Ҷ�ͼ��λͼ�������
	int ColorHead = m_ColorImageSize - ImageWidth * ImageHeight * sizeof(unsigned char) * 3;   //��ɫͼ��λͼ���

	//��¼lporigimage��ָ��
	lpoimage = lporigimage;

	//��ʼ��
	if (hwmimage)
	{
		GlobalFree(hwmimage);
		hwmimage = NULL;
	}
	hwmimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	lpwmimage = (unsigned char*)hwmimage;

	//����Ԥ����
	onlyPreProcess = true;

	lporigimage = originalImage;
	deori(pairs);

	exctOneChannel = zn;
	testbuf_lenOneChannel = testbuf_len;
	if (hprehistOneChannel)
	{
		GlobalFree(hprehistOneChannel);
		hprehistOneChannel = NULL;
	}
	hprehistOneChannel = GlobalAlloc(GMEM_FIXED, 2 * (metInColorToGray + 2) * sizeof(unsigned char));
	lpprehistOneChannel = (unsigned char *)hprehistOneChannel;
	memcpy(lpprehistOneChannel, lpprehist, 2 * (metInColorToGray + 2) * sizeof(unsigned char));

	if (isMerge == true)
	{
		if (hlocmapOneChannel)
		{
			GlobalFree(hlocmapOneChannel);
			hlocmapOneChannel = NULL;
		}
		hlocmapOneChannel = GlobalAlloc(GMEM_FIXED, lml);
		locmapOneChannel = (unsigned char *)hlocmapOneChannel;
		memcpy(locmapOneChannel, locmap, lml);
	}

	onlyPreProcess = false;

	//��ָ
	lporigimage = lpoimage;
}

void CDeprejbgDoc::twoSortAndPreprocessRecover()
{
	/*����*/
	/*��ȡ*/
	/*ͬ��ͬ��*/
	/*��ֱ��ͼԤ����ָ�*/
	/*����*/
	/*ƽ�ƣ�maxԤ����ָ�*/
	/*����ͨ��ͬ��ͬ�����������*/

	int GrayHead = 1024 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);                 //�Ҷ�ͼ��λͼ�������
	int ColorHead = m_ColorImageSize - ImageWidth * ImageHeight * sizeof(unsigned char) * 3;   //��ɫͼ��λͼ���

	//�Զ���ͼ����
	bool batchImg;
	batchImg = true;
	if (strlen(FileTitle) == 11)       //ONLY FOR kodim+2λ��
	{
		FileTitle[7] = '\0';
	}
	else if (strlen(FileTitle) == 10)       //ONLY FOR kodim+1λ��
	{
		FileTitle[6] = '\0';
	}

	//����
	depreRGBPre(lpwmimage);

	if (hwmimage)
	{
		GlobalFree(hwmimage);
		hwmimage = NULL;
	}
	hwmimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	lpwmimage = (unsigned char*)hwmimage;

	//��max channel����ȡ����,������������
	//��ȡ�������ݴ洢�ĳ�ʼ��
	if (hExtractData)
	{
		GlobalFree(hExtractData);
		hExtractData = NULL;
	}
	hExtractData = GlobalAlloc(GMEM_FIXED, m_ImageSize * 24);
	ExtractData = (unsigned char*)hExtractData;
	//�������ݳ��ȵļ�¼����
	for (int i = 0; i < 128; i++)
	{
		dataLen[i] = 0;
	}
	//��ʼ������
	haveStored = 0;
	//�����������ȡ
	memcpy(lppreimage, lpMaximage, m_ImageSize);
	colorToGrayPreprocessRecover = true;
	addDataEmbedJudge = true;
	depre();                                  
	colorToGrayPreprocessRecover = false;
	addDataEmbedJudge = false;
	if (hMaxReimage)
	{
		GlobalFree(hMaxReimage);
		hMaxReimage = NULL;
	}
	hMaxReimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	lpMaxReimage = (unsigned char*)hMaxReimage;

	memcpy(hMaxReimage, lpwmimage, m_ImageSize);

	//����Min median����,ͬ��ͬ��
	if (hMinReimage)
	{
		GlobalFree(hMinReimage);
		hMinReimage = NULL;
	}
	hMinReimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	lpMinReimage = (unsigned char*)hMinReimage;

	if (hMedianReimage)
	{
		GlobalFree(hMedianReimage);
		hMedianReimage = NULL;
	}
	hMedianReimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	lpMedianReimage = (unsigned char*)hMedianReimage;

	for (int i = GrayHead; i < m_ImageSize; i++)
	{
		unsigned char maxRe, medianEn, minEn, maxEn;

		maxRe = *(lpMaxReimage + i);
		maxEn = *(lpMaximage + i);
		medianEn = *(lpMediaimage + i);
		minEn = *(lpMinimage + i);

		*(lpMedianReimage + i) = medianEn - (maxEn - maxRe);
		*(lpMinReimage + i) = minEn - (maxEn - maxRe);
	}

	//��RGB��ֵ������ڴ�
	if (hwmimage)
	{
		GlobalFree(hwmimage);
		hwmimage = NULL;
	}
	hwmimage = GlobalAlloc(GMEM_FIXED, m_ColorImageSize);
	lpwmimage = (unsigned char*)hwmimage;

	//���ļ�ͷд�����ָ��
	for (int i = 0; i < ColorHead; i++)
	{
		*(lpwmimage + i) = *(lporigimage + i);
	}

	int k = 0;
	int j = GrayHead;
	for (int i = ColorHead; i < m_ColorImageSize; i += 3)
	{

		*(lpwmimage + i + *(lpMreimage + k)) = *(lpMaxReimage + j);
		*(lpwmimage + i + *(lpMereimage + k)) = *(lpMedianReimage + j);
		*(lpwmimage + i + *(lpMireimage + k)) = *(lpMinReimage + j);

		j++;
		k++;
	}

	if (hsecondPreReimage)
	{
		GlobalFree(hsecondPreReimage);
		hsecondPreReimage = NULL;
	}
	hsecondPreReimage = GlobalAlloc(GMEM_FIXED, m_ColorImageSize);
	secondPreReimage = (unsigned char*)hsecondPreReimage;
	memcpy(secondPreReimage, lpwmimage, m_ColorImageSize);

	/*�ָ�Ԥ����ǰ�Ĵ�ֱ��ͼ*/
	int pairs = mext + 1;

	//ֱ��ͼ����ƽ��
	for (int i = ColorHead; i < m_ColorImageSize; i++)
	{
		unsigned char pixelValue = *(secondPreReimage + i);
		*(secondPreReimage + i) = pixelValue + pairs;
	}

	//�Զ���ͼ
	/*
	if (batchImg == true)
	{
		char filename[500];
		filename[0] = '\0';
		sprintf(filename, "E:\\experiment\\task\\result-reversible\\kodak-pre\\%s_secondpreRe_%d.bmp", FileTitle, pairs);
		FILE *fb;
		fb = fopen(filename, "wb");
		if (fb == NULL)
		{
			AfxMessageBox("Can Not Open File To Write");
			return;
		}
		fwrite(secondPreReimage, sizeof(unsigned char), m_ColorImageSize, fb);
		fclose(fb);
	}
	*/

	extraBookkeepingRecover();

	//��ֱ��ͼ�ָ�
	threeOnOneOnlyLeftPreprocessRecover(pairs, secondPreReimage, lpprehistColorToGray, locmapColorToGray, exctThree);

	if (hfirstPreReimage)
	{
		GlobalFree(hfirstPreReimage);
		hfirstPreReimage = NULL;
	}
	hfirstPreReimage = GlobalAlloc(GMEM_FIXED, m_ColorImageSize);
	firstPreReimage = (unsigned char*)hfirstPreReimage;
	memcpy(firstPreReimage, lpwmimage, m_ColorImageSize);

	//�Զ���ͼ
	/*
	if (batchImg == true)
	{
		char filename[500];
		filename[0] = '\0';
		sprintf(filename, "E:\\experiment\\task\\result-reversible\\kodak-pre\\%s_firstpreRe_%d.bmp", FileTitle, pairs);
		FILE *fb;
		fb = fopen(filename, "wb");
		if (fb == NULL)
		{
			AfxMessageBox("Can Not Open File To Write");
			return;
		}
		fwrite(firstPreReimage, sizeof(unsigned char), m_ColorImageSize, fb);
		fclose(fb);
	}
	*/

	/*���򣬵�һ��Ԥ����ָ���ͬ��ͬ�����������*/
	//���򣬵õ�lpMaximage��lpMediaimage��lpMinimage
	depreRGBPre(firstPreReimage);

	if (hMaxReimage)
	{
		GlobalFree(hMaxReimage);
		hMaxReimage = NULL;
	}
	hMaxReimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	lpMaxReimage = (unsigned char*)hMaxReimage;

	//max channel�����ƽ��
	for (int i = GrayHead; i < m_ImageSize; i++)
	{
		unsigned char maxValue = *(lpMaximage + i);
		*(lpMaxReimage + i) = maxValue - pairs;
	}

	//��һ��Ԥ����max channel�ָ�
	unsigned char *wpMaxChannel;
	wpMaxChannel = lpMaxReimage + GrayHead;
	preProcessRecover(lphist, lpextmap, mext, exct, wpMaxChannel);


	if (hMinReimage)
	{
		GlobalFree(hMinReimage);
		hMinReimage = NULL;
	}
	hMinReimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	lpMinReimage = (unsigned char*)hMinReimage;

	if (hMedianReimage)
	{
		GlobalFree(hMedianReimage);
		hMedianReimage = NULL;
	}
	hMedianReimage = GlobalAlloc(GMEM_FIXED, m_ImageSize);
	lpMedianReimage = (unsigned char*)hMedianReimage;

	for (int i = GrayHead; i < m_ImageSize; i++)
	{
		int maxRe, medianRe, minRe, maxPre, medianPre, minPre;

		maxRe = *(lpMaxReimage + i);
		maxPre = *(lpMaximage + i);
		medianPre = *(lpMediaimage + i);
		minPre = *(lpMinimage + i);

		medianRe = medianPre - pairs;
		minRe = minPre - pairs;

		medianRe = (maxRe - maxPre) + medianPre;
		minRe = (maxRe - maxPre) + minPre;

		*(lpMedianReimage + i) = medianRe;
		*(lpMinReimage + i) = minRe;
	}

	/*��RGB��ֵ������ڴ�*/
	if (hwmimage)
	{
		GlobalFree(hwmimage);
		hwmimage = NULL;
	}
	hwmimage = GlobalAlloc(GMEM_FIXED, m_ColorImageSize);
	lpwmimage = (unsigned char*)hwmimage;

	//��ָ
	lporigimage = lpoimage;

	//���ļ�ͷд�����ָ��
	for (int i = 0; i < ColorHead; i++)
	{
		*(lpwmimage + i) = *(lporigimage + i);
	}

	k = 0;
	j = GrayHead;
	for (int i = ColorHead; i < m_ColorImageSize; i += 3)
	{

		*(lpwmimage + i + *(lpMreimage + k)) = *(lpMaxReimage + j);
		*(lpwmimage + i + *(lpMereimage + k)) = *(lpMedianReimage + j);
		*(lpwmimage + i + *(lpMireimage + k)) = *(lpMinReimage + j);

		j++;
		k++;
	}

	

}

void CDeprejbgDoc::extraBookkeepingDataEmbed(int pairs, long exct, long testbuf_len, unsigned char *locationMap, unsigned char*prehist)
{
	int met = pairs - 1;
	int shiftnumber, shiftvalue;
	unsigned char intermediary;
	unsigned char* lenmediary;
	long *locmapLen;
	long *lohismapLen;
	int parameterLen;

	/*Ƕ��˳��zn(exct)��testbuf_len��lpprehist��locmap(������testbuf_len)*/
	int doubleMet = 2 * (met + 1) - 1;
	int lpprehistLen = 2 * (doubleMet + 2);
	//��ʼ����ȷ������Ƕ�����ݴ�С
	if (haddDataEmbed)
	{
		GlobalFree(haddDataEmbed);
		haddDataEmbed = NULL;
	}
	haddDataEmbed = GlobalAlloc(GMEM_FIXED, (lpprehistLen + sizeof(long) * 2 + testbuf_len) * 8);
	addDataEmbed = (unsigned char *)haddDataEmbed;

	/*zn(exct)��ֵ��addDataEmbed��*/
	locmapLen = &exct;
	for (int i = 0; i < sizeof(long); i++)
	{
		lenmediary = (unsigned char *)locmapLen + i;
		intermediary = *lenmediary;
		for (int k = 0; k < 8; k++)
		{
			*(addDataEmbed + 8 * i + k) = 0;
			shiftnumber = k;
			shiftvalue = (intermediary >> shiftnumber) & 1;
			*(addDataEmbed + 8 * i + k) = shiftvalue;
		}
	}

	/*testbuf_len��ֵ��addDataEmbed��*/
	lohismapLen = &testbuf_len;
	for (int i = 0; i < sizeof(long); i++)
	{
		lenmediary = (unsigned char *)lohismapLen + i;
		intermediary = *lenmediary;
		for (int k = 0; k < 8; k++)
		{
			*(addDataEmbed + 8 * (i + sizeof(long)) + k) = 0;
			shiftnumber = k;
			shiftvalue = (intermediary >> shiftnumber) & 1;
			*(addDataEmbed + 8 * (i + sizeof(long)) + k) = shiftvalue;
		}
	}

	parameterLen = sizeof(long) * 2;

	/*lpprehist��ֵ��addDataEmbed��*/
	for (int i = 0; i < lpprehistLen; i++)
	{
		intermediary = *(prehist + i);
		for (int k = 0; k < 8; k++)
		{
			*(addDataEmbed + 8 * (i + parameterLen) + k) = 0;
			shiftnumber = k;
			shiftvalue = (intermediary >> shiftnumber) & 1;
			*(addDataEmbed + 8 * (i + parameterLen) + k) = shiftvalue;
		}
	}
	

	/*locationMap��ֵ��addDataEmbed��*/
	for (int i = 0; i < testbuf_len; i++)
	{
		intermediary = *(locationMap + i);
		for (int k = 0; k < 8; k++)
		{
			*(addDataEmbed + 8 * (i + parameterLen + lpprehistLen) + k) = 0;
			shiftnumber = k;
			shiftvalue = (intermediary >> shiftnumber) & 1;
			*(addDataEmbed + 8 * (i + parameterLen + lpprehistLen) + k) = shiftvalue;
		}
	}

	addDataLen = (sizeof(long) * 2) + lpprehistLen + testbuf_len;

}

void CDeprejbgDoc::extraBookkeepingRecover()
{
	int last;
	long start;
	unsigned char shiftvalue;
	unsigned char *intermediate;
	int parameterLen;
	exctThree = 0;

	/*�ҳ����һ����ȡ�������ݵ�����last*/
	for (int i = 128; i >= 0; i--)
	{
		if (dataLen[i] == 0)
			last = i - 1;
	}

	//��ʼ��
	long *locmapLen;
	HANDLE hlocmapLen = NULL;
	if (hlocmapLen)
	{
		GlobalFree(hlocmapLen);
		hlocmapLen = NULL;
	}
	hlocmapLen = GlobalAlloc(GMEM_FIXED, sizeof(long));
	locmapLen = (long *)hlocmapLen;

	long *lohismapLen;
	HANDLE hlohismapLen = NULL;
	if (hlohismapLen)
	{
		GlobalFree(hlohismapLen);
		hlohismapLen = NULL;
	}
	hlohismapLen = GlobalAlloc(GMEM_FIXED, sizeof(long));
	lohismapLen = (long *)hlohismapLen;

	if (hExtractDataOrder)
	{
		GlobalFree(hExtractDataOrder);
		hExtractDataOrder = NULL;
	}
	hExtractDataOrder = GlobalAlloc(GMEM_FIXED, m_ImageSize * 24);
	ExtractDataOrder = (unsigned char*)hExtractDataOrder;

	/*��ExtractData��˳�������*/
	start = 0;
	for (int i = last; i >= 0; i--)
	{
		haveStored = haveStored - dataLen[i];
		memcpy(ExtractDataOrder + start, ExtractData + haveStored, dataLen[i]);
		start = dataLen[i] + start;
	}

	/*�ӵ�����˳���ExtractDataOrderȡ������*/
	//exct
	intermediate = (unsigned char *)lohismapLen;
	for (int i = 0; i < sizeof(long); i++)
	{
		*(intermediate + i) = 0;
		for (int k = 0; k < 8; k++)
		{
			shiftvalue = *(ExtractDataOrder + 8 * i + 7 - k);
			*(intermediate + i) = (*(intermediate + i) << 1) + shiftvalue;
		}
	}
	exctThree = *lohismapLen;

	//testbuf_len
	long testbuf_lenThree;
	intermediate = (unsigned char *)locmapLen;
	for (int i = 0; i < sizeof(long); i++)
	{
		*(intermediate + i) = 0;
		for (int k = 0; k < 8; k++)
		{
			shiftvalue = *(ExtractDataOrder + 8 * sizeof(long) + 8 * i + 7 - k);
			*(intermediate + i) = (*(intermediate + i) << 1) + shiftvalue;
		}
	}
	testbuf_lenThree = *locmapLen;

	parameterLen = sizeof(long) * 2;

	/*�ӵ�����˳���ExtractDataOrderȡ��locmap��lpprehist*/
	//lpprehist
	int doubleMet = 2 * (met + 1) - 1;
	int lpprehistLen = 2 * (doubleMet + 2);
	if (hprehistColorToGrayExtract)
	{
		GlobalFree(hprehistColorToGrayExtract);
		hprehistColorToGrayExtract = NULL;
	}
	hprehistColorToGrayExtract = GlobalAlloc(GMEM_FIXED, lpprehistLen * 8);
	lpprehistColorToGrayExtract = (unsigned char*)hprehistColorToGrayExtract;
	memcpy(lpprehistColorToGrayExtract, ExtractDataOrder + parameterLen * 8, lpprehistLen * 8);

	//��λ�������õ�ԭ����lpprehistOneChannel
	if (hprehistColorToGray)
	{
		GlobalFree(hprehistColorToGray);
		hprehistColorToGray = NULL;
	}
	hprehistColorToGray = GlobalAlloc(GMEM_FIXED, lpprehistLen);
	lpprehistColorToGray = (unsigned char*)hprehistColorToGray;

	for (int i = 0; i < lpprehistLen; i++)
	{
		*(lpprehistColorToGray + i) = 0;
		for (int k = 0; k < 8; k++)
		{
			shiftvalue = *(lpprehistColorToGrayExtract + 7 - k + 8 * i);
			*(lpprehistColorToGray + i) = (*(lpprehistColorToGray + i) << 1) + shiftvalue;
		}
	}


	//locmap
	if (hlocmapColorToGrayExtract)
	{
		GlobalFree(hlocmapColorToGrayExtract);
		hlocmapColorToGrayExtract = NULL;
	}
	hlocmapColorToGrayExtract = GlobalAlloc(GMEM_FIXED, testbuf_lenThree * 8);
	locmapColorToGrayExtract = (unsigned char*)hlocmapColorToGrayExtract;
	memcpy(locmapColorToGrayExtract, ExtractDataOrder + (parameterLen + lpprehistLen) * 8, testbuf_lenThree * 8);

	//��λ�������õ�ԭ����lpprehistOneChannel
	if (hlocmapColorToGray)
	{
		GlobalFree(hlocmapColorToGray);
		hlocmapColorToGray = NULL;
	}
	hlocmapColorToGray = GlobalAlloc(GMEM_FIXED, testbuf_lenThree);
	locmapColorToGray = (unsigned char*)hlocmapColorToGray;

	for (int i = 0; i < testbuf_lenThree; i++)
	{
		*(locmapColorToGray + i) = 0;
		for (int k = 0; k < 8; k++)
		{
			shiftvalue = *(locmapColorToGrayExtract + 7 - k + 8 * i);
			*(locmapColorToGray + i) = (*(locmapColorToGray + i) << 1) + shiftvalue;
		}
	}


}

/*���µ�����*/
void CDeprejbgDoc::maxNotEmbed(int targetS)
{
	int maxPairs = 15;
	int metInColorToGray = maxPairs - 1;
	long testbuf_lenThree;
	bool totalEmbedJudge;

	long el = 0, g = 0;

	//����ɫͼ��RGBͨ����ֵ�ֱ�ֵ���Ҷ�ͼ��ʽ
	int GrayHead = 1024 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);                 //�Ҷ�ͼ��λͼ�������
	int ColorHead = m_ColorImageSize - ImageWidth * ImageHeight * sizeof(unsigned char) * 3;   //��ɫͼ��λͼ���

	
	//Ԥ����
	testbuf_lenThree = colorToGrayOnlyPreprocess(lporigimage,maxPairs);

	//�ж�maxPairs��targetS�Ƿ��Ƕ��
	totalEmbedJudge = embedJudge(maxPairs, targetS, testbuf_lenThree, lpwmimage);
	
	//enlargeInfoDiff(targetS);

}

void CDeprejbgDoc::enlargeInfoDiff(int targetS)
{
	long testbuf_lenThree;
	long necessaryLen;
	int combineBinNum[256];
	int threeCombineEmbedPairs, targetN;
	long embedLenPermit;
	long maxDiff=0;

	for (int i = targetS+1; i < 65; i++)
	{
		//Ԥ����
		testbuf_lenThree = colorToGrayOnlyPreprocess(lporigimage,i);

		//������ҪǶ�����
		threeCombineEmbedPairs = i - targetS;
		necessaryLen = exctThree + 2 * 8 * (threeCombineEmbedPairs - 1) + 8 * (6 + sizeof(long) * 3);

		//������Ƕ�����
		histStatistics(lpwmimage, combineBinNum);
		embedLenPermit = 0;
		for (int j = 0; j < threeCombineEmbedPairs; j++)
		{
			embedLenPermit = embedLenPermit + combineBinNum[j] + combineBinNum[j];
		}
		embedLenPermit = 8 * (embedLenPermit / 8);

		int diff = necessaryLen - embedLenPermit;
		if (maxDiff == 0)
		{
			maxDiff = diff;
		}
		if (diff <= maxDiff)
		{
			maxDiff = diff;
			targetN = threeCombineEmbedPairs;
		}
	}

}

void CDeprejbgDoc::BinarySearch(int targetS, int low, int high)
{

}

bool CDeprejbgDoc::embedJudge(int maxPairs,int targetS,long testbuf_lenThree,unsigned char *imageAfterPre)
{
	bool couldEmbed = false;
	long el = 0, g = 0;
	int metInTargetS = targetS - 1;
	int threeCombineEmbedPairs = maxPairs - targetS;
	int combineBinNum[256];
	int maxBinNum[256];

	histStatistics(imageAfterPre, combineBinNum);
	long embedLenPermit = 0;
	long maxEmbedLenPermit = 0;
	
	for (int i = 0; i < threeCombineEmbedPairs; i=i+2)
	{
		embedLenPermit = embedLenPermit + combineBinNum[i] + combineBinNum[i+1];
	}
	embedLenPermit = 8 * (embedLenPermit / 8);
	
	long necessaryLen = exctThree + 2 * 8 * (threeCombineEmbedPairs - 1) + 8 * (6 + sizeof(long) * 3);
	long maxEmbedLen;
	
	if (necessaryLen < embedLenPermit)
	{
		couldEmbed = true;
	}
	else
	{
		maxEmbedLen = necessaryLen - embedLenPermit;

		//��¼lporigimage��ָ��
		lpoimage = lporigimage;
		lporigimage = imageAfterPre;

		//ת�Ҷ�ͼ
		unsigned long m_ImageSize_ori = 1024 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + ImageWidth * ImageHeight * sizeof(unsigned char);
		int ColorImageWidth = ImageWidth;
		ImageWidth = 3 * ImageWidth;   //RGB�ֱ𵥶������Ҷ�ͼ��һ������ֵ��ͼ���Ϊԭ��3��
		m_ImageSize = 1024 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + ImageWidth * ImageHeight * sizeof(unsigned char);

		//����ɫͼ��RGBͨ����ֵ�ֱ�ֵ���Ҷ�ͼ��ʽ
		int GrayHead = 1024 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);                 //�Ҷ�ͼ��λͼ�������
		int ColorHead = m_ColorImageSize - ColorImageWidth * ImageHeight * sizeof(unsigned char) * 3;   //��ɫͼ��λͼ���

		//��ʼ��
		if (hImage2D)
		{
			GlobalFree(hImage2D);
			hImage2D = NULL;
		}
		hImage2D = GlobalAlloc(GMEM_FIXED, m_ImageSize);
		Image2D = (unsigned char*)hImage2D;

		//��RGB����Ҷ�
		int z = 0;
		for (int j = ColorHead; j < m_ColorImageSize; j = j + 3)
		{
			*(Image2D + GrayHead + z) = *(lporigimage + j);
			*(Image2D + GrayHead + z + 1) = *(lporigimage + j + 1);
			*(Image2D + GrayHead + z + 2) = *(lporigimage + j + 2);
			z = z + 3;
		}

		if (hTotalimageTemporary)
		{
			GlobalFree(hTotalimageTemporary);
			hTotalimageTemporary = NULL;
		}
		hTotalimageTemporary = GlobalAlloc(GMEM_FIXED, m_ImageSize);
		lpTotalimageTemporary = (unsigned char*)hTotalimageTemporary;

		memcpy(lpTotalimageTemporary, Image2D, m_ImageSize);

		if (hTotalimageEn)
		{ 
			GlobalFree(hTotalimageEn);
			hTotalimageEn = NULL;
		}
		hTotalimageEn = GlobalAlloc(GMEM_FIXED, m_ImageSize);
		lpTotalimageEn = (unsigned char*)hTotalimageEn;
		memcpy(lpTotalimageEn, Image2D, m_ImageSize);

		for (int m = 0; m < threeCombineEmbedPairs + 1; m++)
		{
			el = derem(lpTotalimageTemporary, lpTotalimageEn, m, threeCombineEmbedPairs, testbuf_lenThree, maxEmbedLen, locmapColorToGray, lpprehistColorToGray);                  //����Ƕ��
			g = g + el;
			memcpy(lpTotalimageEn, lpTotalimageTemporary, m_ImageSize);
		}
		payload = g;

		//��ָ
		lporigimage = lpoimage;

		//��д
		m_ImageSize = m_ImageSize_ori;
		ImageWidth = ColorImageWidth;

		/*�����totalǶ����max channel*/
		//���򣬵���deoriRGBPre
		if (horiginal)
		{
			GlobalFree(horiginal);
			horiginal = NULL;
		}
		horiginal = GlobalAlloc(GMEM_FIXED, m_ColorImageSize);
		lporiginal = (unsigned char *)horiginal;
		memcpy(lporiginal, lporigimage, m_ColorImageSize);
		memcpy(lporigimage, lpTotalimageEn, m_ColorImageSize);    //����ֵ
		deoriRGBPre(lporigimage);
		memcpy(lporigimage, lporiginal, m_ColorImageSize);

		histStatistics(lpMaximage, maxBinNum);

		for (int i = 0; i < targetS; i=i+2)
		{
			maxEmbedLenPermit = maxEmbedLenPermit + maxBinNum[i] + maxBinNum[i+1];
		}
		maxEmbedLenPermit = 8 * (maxEmbedLenPermit / 8);

		maxEmbedLen = maxEmbedLen + 2 * 8 * (targetS - 1) + 8 * (6 + sizeof(long) * 3);

		if (maxEmbedLen < maxEmbedLenPermit)
		{
			couldEmbed = true;
		}
	}

	return couldEmbed;
}

void CDeprejbgDoc::bubbleSort(int arr[], int n)
{
	for (int i = 0; i < n; i++) {
		//�Ƚ��������ڵ�Ԫ��   
		for (int j = 0; j < n - i - 1; j++) {
			if (arr[j] < arr[j + 1]) {
				int t = arr[j];
				arr[j] = arr[j + 1];
				arr[j + 1] = t;
			}
		}
	}
}

void CDeprejbgDoc::histStatistics(unsigned char *lpwmimage,int binNum[])
{
	unsigned char value;

	int GrayHead = 1024 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);                 //�Ҷ�ͼ��λͼ�������
	int ColorHead = m_ColorImageSize - ImageWidth * ImageHeight * sizeof(unsigned char) * 3;   //��ɫͼ��λͼ���

	for (int i = 0; i < 256; i++)
	{
		binNum[i] = 0;
	}

	for (int i = GrayHead; i < m_ImageSize; i++)
	{
		lm = *(lpwmimage + i);
		for (int j = 0; j < 256; j++)
		{
			if (lm == j)
			{
				binNum[j]++;
			}
		}
	}

	bubbleSort(binNum, 256);
}

void CDeprejbgDoc::Test2()
{
	unsigned char value;
	int binNum[256], leftBinNum[128], rightBinNum[128];

	int GrayHead = 1024 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);                 //�Ҷ�ͼ��λͼ�������
	int ColorHead = m_ColorImageSize - ImageWidth * ImageHeight * sizeof(unsigned char) * 3;   //��ɫͼ��λͼ���

	for (int i = 0; i < 256; i++)
	{
		binNum[i] = 0;
	}

	for (int i = GrayHead; i < m_ImageSize; i++)
	{
		lm = *(lporigimage + i);
		for (int j = 0; j < 256; j++)
		{
			if (lm == j)
			{
				binNum[j]++;
			}
		}
	}

	for (int i = 0; i < 128; i++)
	{
		leftBinNum[i] = binNum[i];
		int j = i + 128;
		rightBinNum[i] = binNum[j];
	}

	bubbleSort(leftBinNum, 128);
	bubbleSort(rightBinNum, 128);
}