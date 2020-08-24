// deprejbgDoc.h : interface of the CDeprejbgDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEPREJBGDOC_H__6F28E294_16F3_4C5D_9FF6_332309A2E45C__INCLUDED_)
#define AFX_DEPREJBGDOC_H__6F28E294_16F3_4C5D_9FF6_332309A2E45C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdio.h"
#include "jbigt.h"
#include "math.h"
//#include "jbigtst.h"

class CDeprejbgDoc : public CDocument
{
protected: // create from serialization only
	CDeprejbgDoc();
	DECLARE_DYNCREATE(CDeprejbgDoc)

// Attributes
public:

	bool extracted, failed, phis[256], phist[256],lphf;

	long int ltime, ind, pct, lml, embnb, extnb;
	int MinBestS;
	int MedBestS;
	int MaxBestS;
	int RBestS;
	int GBestS;
	int BBestS;
	int MinDiff;						//����һ�µ���С����
	int Rless, Rmore, Bless, Bmore;    //������ص�����
	int Rlow, Rhigh, Blow, Bhigh;      //����������ֵ
	int channel;                       //enhancedMax�����¼��ǿ�����ĸ�ͨ����ȡֵ��RGBPre����һ��
	bool newPre;					//�Ƿ�ʹ���µ�Ԥ����
	
	bool onlyPreProcess;			//�ж��Ƿ���ֻ��Ԥ����������Ƕ����Ԥ����ֿ�
	bool leftPreProcess;            //�ж��Ƿ�ֻ�����Ԥ����
	bool leftPreProcessRecover;     //�ж��Ƿ�ֻ�����Ԥ����ָ�
	bool colorToGrayPreprocessRecover;   //�ж��Ƿ���colorToGrayֱ��ͼԤ����ָ�
	bool isMerge;						//�ж��Ƿ��н��
	bool addDataEmbedJudge;                   //�ж��Ƿ��õ�����Ƕ��
	long addDataLenRemain;            //��ɫͼ�����Ƕ����ϢǶ������е�ʣ�೤��
	long addDataLen;                  //��ɫͼ�����Ƕ����Ϣ���ܳ���
	long exctR, exctG, exctB, exctMax,exctThree,exctOneChannel;			//rgbͨ����zn

	long testbuf_lenOneChannel;

	unsigned char *lpkey;				//Define the pointer of the original image which points the data of the original image in memory
	HANDLE hkey;

	double *originalimage, psnr, sg, sm;
	unsigned long int m_ImageSize;		//ͼ���ļ���С,����������Ϣ

	//�������е�ͼ��ָ�붼�ǰ�����λͼ��������Ϣ,�������ļ�ͷ����Ϣͷ��λͼ����ֵ��
	//ָ��ԭͼ�ļ�,ʼ�����ṩ���ݶ�����ı�����,����ʱ�����ݸ��Ƶ���һ���ڴ���ȥ����
	unsigned char *lporigimage;				//Define the pointer of the original image which points the data of the original image in memory
	HANDLE horigimage;
	
	unsigned char *lpwmimage;				//ָ��Ƕ��ˮӡ����ļ���ָ��ָ��׶ο�ʼ����ǿͼ��
	HANDLE hwmimage;

	unsigned char *lppwmimage;
	HANDLE hpwimage;

	//�ָ��׶ε�����ͼ��
	unsigned char *lppreimage;				//Define the pointer of the original image which points the data of the original image in memory
	HANDLE hpreimage;

	//���ɵ�ԭʼͼ��
	unsigned char *lporiginal;
	HANDLE horiginal;

	unsigned char *lpprehist;				//ָ���¼Ԥ����ı��ָ�룬����ԭͼ���bin�Ķ�Ӧ����ֵ��bin���ʱ���bin������ֵ��bin value
	HANDLE hprehist;

	unsigned char *lphist;				//����ȡʱ��lpprehist
	HANDLE hphist;

	int *lpdifimage;				//Define the pointer of the original image which points the data of the original image in memory
	HANDLE hdifimage;
	
	unsigned int *lpblkhis;        //��¼ֱ��ͼ����bin�ĸ߶�
	HANDLE hblkhis;

	int *lpblkval;                  //��¼ֱ��ͼbin����ֵ
	HANDLE hblkval;

	unsigned char *lpblkflg;
	HANDLE hblkflg;

	unsigned char *lpembmap;
	HANDLE hembmap;

	unsigned char *lplbldbn;
	HANDLE hlbldbn;

	unsigned char *lpembovh;
	HANDLE hembovh;

	unsigned char *lpbkimage;				//�����ָ�
	HANDLE hbkimage;

	unsigned char *lpextmap;               //����ȡʱ��locmap
	HANDLE hextmap;

	unsigned char *lpprehistRchannel;				//RchannelԤ����ʱ������λ����Ϣ
	HANDLE hprehistRchannel;
	unsigned char *lpprehistGchannel;				//GchannelԤ����ʱ������λ����Ϣ
	HANDLE hprehistGchannel;
	unsigned char *lpprehistBchannel;				//BchannelԤ����ʱ������λ����Ϣ
	HANDLE hprehistBchannel;
	
	unsigned char *lpprehistMaxChannel;				//MaxԤ����ʱ������λ����Ϣ
	HANDLE hprehistMaxChannel;
	unsigned char *lpprehistColorToGray;				//תΪ�Ҷ�Ԥ����ʱ������λ����Ϣ
	HANDLE hprehistColorToGray;
	unsigned char *lpprehistOneChannel;				//תΪ�Ҷ�Ԥ����ʱ������λ����Ϣ
	HANDLE hprehistOneChannel;
	unsigned char *lpprehistColorToGrayExtract;				//תΪ�Ҷ�Ԥ����ʱ������λ����Ϣ
	HANDLE hprehistColorToGrayExtract;

	unsigned char *locmapRchannel;								//RchannelԤ����ʱ������locmap
	HANDLE hlocmapRchannel;
	unsigned char *locmapGchannel;								//GchannelԤ����ʱ������locmap
	HANDLE hlocmapGchannel;
	unsigned char *locmapBchannel;								//BchannelԤ����ʱ������locmap
	HANDLE hlocmapBchannel;
	
	unsigned char *locmapMaxChannel;								//MaxChannelԤ����ʱ������locmap
	HANDLE hlocmapMaxChannel;	
	unsigned char *locmapColorToGray;								//תΪ�Ҷ�Ԥ����ʱ������locmap
	HANDLE hlocmapColorToGray;
	unsigned char *locmapOneChannel;								//��ͨ��Ԥ����ʱ������locmap
	HANDLE hlocmapOneChannel;
	unsigned char *locmapColorToGrayExtract;								//��ͨ��Ԥ����ʱ������locmap
	HANDLE hlocmapColorToGrayExtract;



	unsigned char *lp;                    //����ǰͼ���������
	unsigned char *wp;					  //�����ͼ���������
	unsigned char *lpbk;
	unsigned char lm, met, lb, rb, la, ra, ela, era, lbn;
	unsigned char mext;                   //��ȡ��S
	unsigned char ohpn;                   //��ʾ��Ҫ���ٴ�S����ȫ����¼��bookkeeping��Ϣ

	int bknum;         //����ɫ����

	int bkpixel[256];

	double percent;    //����ɫ��ռ�ٷֱ�

	LPBITMAPFILEHEADER bitfile;
    LPBITMAPINFOHEADER bitinfo;

	int ImageWidth,ImageHeight;
	long tstbfloc;                                      //��¼ʣ�໹�ж���bookkeeping��Ҫ��¼
	long lpoh;											//��¼���һ��bookkeepingǶ��ʱ�ĳ���
		
	unsigned char *lphismap;                            //��¼Ԥ����bookkeeping
	HANDLE hhismap;
	
	unsigned char *locmap;								//��¼lohismap��locmapһλ��Ӧlohismap��λ
	HANDLE hlocmap;                                        

	unsigned char *locationCompression;					//ѹ��locationmap������
	HANDLE hlocationCompression;

	unsigned char *ExtractData;                         //��¼��bookkeeping���������ȡ����
	HANDLE hExtractData;

	unsigned char *ExtractMap;                          //����ȡ�����������ж�ȡmap����
	HANDLE hExtractMap;

	unsigned char *colorLocationMap;                    //��ȡ��ѹ����map
	HANDLE hcolorLocationMap;

	unsigned char *ExtractDataOrder;                         //��ExtractData˳�������
	HANDLE hExtractDataOrder;

	long dataLen[128];                                   //��¼ÿһ����ȡ�ĳ�bookkeeping����������ݳ���

	long haveStored;                                    //��¼ExtractData�Ѿ��洢�ĳ���
	long ExtractMapLen;                                 //��ȡ������map����
	int channelChosenExtract;                           //��ȡ��������ǿchannel��¼

	unsigned char *pp, *extlen;
	HANDLE hextlen;

	unsigned char m_CurrentDirectory[256*256+64];		//�ݴ浱ǰ�򿪵��ļ���Ϣ,֮�����ڵ�ǰ���ڱ�������ʾ���ļ�·����Ϣ
	unsigned char m_OriginalWatermark[256*256+64];		//ˮӡͼ��Ĵ�����ֵ
	CString CurrentDirectory;
	
	char watermarkedfilename[500];	
	char watermarkedfiletitle[500];

	unsigned int filesize,wmheight,wmwidth, mpv, lpv, rpv, plv, threshold;
	long payload, expld, zn, exbuflen;
	long exct;                                          //����ȡ��zn

	LPBITMAPFILEHEADER wmfile;
	LPBITMAPINFOHEADER wminfo;

	BOOLEAN isBackground[2140][2140];
	BOOLEAN doBksega;


	//��ɫͼ�����ӵı���
	//ָ��Rͨ������չͼ
	unsigned char* lpRimage;
	HANDLE hRimage;

	//ָ��Gͨ������չͼ
	unsigned char* lpGimage;
	HANDLE hGimage;

	//ָ��Bͨ������չͼ
	unsigned char* lpBimage;
	HANDLE hBimage;

	int* lpMreimage;              //��¼maxλ�������ĸ�RGBͨ��
	HANDLE hMreimage;
	int* lpMereimage;             //��¼medianλ�������ĸ�RGBͨ��
	HANDLE hMereimage;
	int* lpMireimage;             //��¼minλ�������ĸ�RGBͨ��
	HANDLE hMireimage;

	//MAX
	//Max
	unsigned char* lpMaximage;
	HANDLE hMaximage;
	//Media
	unsigned char* lpMediaimage;
	HANDLE hMediaimage;
	//MIN
	unsigned char* lpMinimage;
	HANDLE hMinimage;
	//CONST
	double *lpCimage;
	HANDLE hCimage;
	double *lpC2image;
	HANDLE hC2image;
	//record

	//Max
	unsigned char* lpMaximageTemporary;
	HANDLE hMaximageTemporary;

	//total
	unsigned char* lpTotalimageTemporary;
	HANDLE hTotalimageTemporary;
	unsigned char* lpTotalimageEn;
	HANDLE hTotalimageEn;

	//MaxEn
	unsigned char* lpMaxEnimage;
	HANDLE hMaxEnimage;
	unsigned char* lpMedianEnimage;
	HANDLE hMedianEnimage;
	unsigned char* lpMinEnimage;
	HANDLE hMinEnimage;


	//maxRe
	unsigned char* lpMaxReimage;
	HANDLE hMaxReimage;
	unsigned char* lpMedianReimage;
	HANDLE hMedianReimage;
	unsigned char* lpMinReimage;
	HANDLE hMinReimage;

	//YUV����
	//Y
	unsigned char* Ychannel;
	HANDLE hYchannel;
	//U
	unsigned char* Uchannel;
	HANDLE hUchannel;
	//V
	unsigned char* Vchannel;
	HANDLE hVchannel;

	/*GenhancedRe����*/
	unsigned char* RchannelEnhancedRe;
	HANDLE hRchannelEnhancedRe;
	unsigned char* GchannelEnhancedRe;
	HANDLE hGchannelEnhancedRe;
	unsigned char* BchannelEnhancedRe;
	HANDLE hBchannelEnhancedRe;
	unsigned char *enhancedImage;				
	HANDLE henhancedImage;

	//����2����
	//R
	unsigned char* Rchannel;
	HANDLE hRchannel;
	//G
	unsigned char* Gchannel;
	HANDLE hGchannel;
	//B
	unsigned char* Bchannel;
	HANDLE hBchannel;
	//����
	unsigned char* ChannelSort;
	HANDLE hChannelSort;
	//��ǿ������
	unsigned char* EnhancedSort;
	HANDLE hEnhancedSort;

	//��ǿ��RGB
	unsigned char* RchannelEnhanced;
	HANDLE hRchannelEnhanced;
	unsigned char* GchannelEnhanced;
	HANDLE hGchannelEnhanced;
	unsigned char* BchannelEnhanced;
	HANDLE hBchannelEnhanced;

	//Ԥ�����rgb
	unsigned char* RchannelPreprocessed;
	HANDLE hRchannelPreprocessed;
	unsigned char* GchannelPreprocessed;
	HANDLE hGchannelPreprocessed;
	unsigned char* BchannelPreprocessed;
	HANDLE hBchannelPreprocessed;
	//Ԥ�����max
	unsigned char* maxPreprocessed;
	HANDLE hmaxPreprocessed;

	//����Է���
	unsigned char* referOri1;
	HANDLE hreferOri1;
	unsigned char* referOri2;
	HANDLE hreferOri2;
	unsigned char* referEn1;
	HANDLE hreferEn1;
	unsigned char* referEn2;
	HANDLE hreferEn2;
	unsigned char* referPredict1Ori;
	HANDLE hreferPredict1Ori;
	unsigned char* referPredict1En;
	HANDLE hreferPredict1En;
	unsigned char* referPredict2Ori;
	HANDLE hreferPredict2Ori;
	unsigned char* referPredict2En;
	HANDLE hreferPredict2En;
	unsigned char* referHoldOri;
	HANDLE hreferHoldOri;
	unsigned char* referHoldEn;
	HANDLE hreferHoldEn;

	unsigned char* MinPredictOri;
	HANDLE hMinPredictOri;
	unsigned char* MinPredictEn;
	HANDLE hMinPredictEn;
	unsigned char* MedianPredictOri;
	HANDLE hMedianPredictOri;
	unsigned char* MedianPredictEn;
	HANDLE hMedianPredictEn;

	unsigned char* mapFlow;
	HANDLE hmapFlow;
	unsigned char* mapFlow2;
	HANDLE hmapFlow2;

	long mapFlowLen;

	//����1����
	//��¼��ͬ����Median֮��
	double *lpMedDifference;
	HANDLE hMedDifference;

	//��¼��ͬ����Min֮��
	double *lpMinDifference;
	HANDLE hMinDifference;

	//��¼ÿ�οɻָ���ǿ��MIN
	unsigned char* lpMinRecovery;
	HANDLE hMinRecovery;
	
	//��¼ÿ�οɻָ���ǿ��MIN
	unsigned char* lpMedRecovery;
	HANDLE hMedRecovery;

	//ת�Ҷȷ�������
	//ת����Ҷ�ͼ
	unsigned char* Image2D;
	HANDLE hImage2D;

	//�ҶȲο�����
	unsigned char* Grayscale;
	HANDLE hGrayscale;

	unsigned char* GrayscaleEnhanced;
	HANDLE hGrayscaleEnhanced;
	//��ǿ��
	unsigned char* Image2DEnhanced;
	HANDLE hImage2DEnhanced;

	//��ǿmax��෽��
	unsigned char* channelEnhanced;
	HANDLE hchannelEnhanced;
	unsigned char* MaxEnhanced;
	HANDLE hMaxEnhanced;
	unsigned char* MedianEnhanced;
	HANDLE hMedianEnhanced;
	unsigned char* MinEnhanced;
	HANDLE hMinEnhanced;

	//��ԭ����ı��location map
	unsigned char* locationMapColor;           //���λ����Ϊ1���ʾʹ����ԭֵ����Ϊ0���ʾʹ������ǿֵ
	HANDLE hlocationMapColor;

	//������
	unsigned char* overflowMap;           //���λ����Ϊ1���ʾʹ����ԭֵ����Ϊ0���ʾʹ������ǿֵ
	HANDLE hoverflowMap;

	//������
	unsigned char* minOverflowMap;           
	HANDLE hminOverflowMap;
	unsigned char* medianOverflowMap;           
	HANDLE hmedianOverflowMap;
	unsigned char* overflowValue;
	HANDLE hoverflowValue;

	//ѹ��ǰ��map
	unsigned char* mapBeforeCompression;
	HANDLE hmapBeforeCompression;

	//��ͼ����ȡ������map
	unsigned char* colorMapRecover;
	HANDLE hcolorMapRecover;

	//Gchannel������ԭ����Ҫ��map
	unsigned char* GchannelMap;
	HANDLE hGchannelMap;

	//��ԭ����ľ����¼
	unsigned char* channelRecovery;
	HANDLE hchannelRecovery;
	unsigned char* MaxRecover;
	HANDLE hMaxRecover;
	unsigned char* MedianRecover;
	HANDLE hMedianRecover;
	unsigned char* MinRecover;
	HANDLE hMinRecover;

	//Ƕ��ʱ�����жϵı��
	unsigned char* addDataEmbed;
	HANDLE haddDataEmbed;

	//ʣ����ҪǶ��Ĳ�ɫͼ��map
	unsigned char* addDataRemain;
	HANDLE haddDataRemain;

	//ָ��ԭʼ��ɫͼƬ,��¼ԭʼ��ɫ�ļ����ڴ��׵�ַ
	unsigned char* lpoimage;

	//ָ��base on hsv�����Ĳ�ɫͼ��
	unsigned char* lphsvimage;

	//��ɫͼ���С
	unsigned long m_ColorImageSize;

	//��¼�ļ���
	char FileTitle[50];              

	//test-Genhanced
	long buflen;

	/*twoSortAndPreprocess����*/
	unsigned char* lpMaxPreimage;
	HANDLE hMaxPreimage;
	unsigned char* lpMedianPreimage;
	HANDLE hMedianPreimage;
	unsigned char* lpMinPreimage;
	HANDLE hMinPreimage;
	unsigned char* firstPreimage;
	HANDLE hfirstPreimage;
	unsigned char* secondPreimage;
	HANDLE hsecondPreimage;
	unsigned char* secondPreReimage;
	HANDLE hsecondPreReimage;
	unsigned char* firstPreReimage;
	HANDLE hfirstPreReimage;

	int f;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDeprejbgDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	void defix();	//�򿪴�Ƕ���ˮӡ�ļ�
	void deori(int mm);	//Ƕ��
	void depre();	//��ȡ/��ԭ
	long deremb(unsigned char *lpwmimg, unsigned char *lporigimg, int t);                                                           //Ԥ����
	long derem(unsigned char *lpwmimg, unsigned char *lporigimg, int t, int met, long testbuf_len, long zn, unsigned char *locmap, unsigned char *lpprehist);
	long dextr(unsigned char *lpwmimg, unsigned char *lporigimg, int t);
	long extrec(unsigned char *lpwmimg, unsigned char *lporigimg);
	long blkemb(unsigned char *tp, unsigned char *mp, int row, int column, int bs, long buf_len, long el, int t);
	long blkems(unsigned char *tp, unsigned char *mp, int row, int column, int bs, long buf_len, long el, int t);                   //��һ��Ƕ��
	long blkext(unsigned char *mp, int row, int column, int bs, long el, int t);
	long blkexs(unsigned char *mp, int row, int column, int bs, long el, int t);
	void threePreProcess();
	void threePreProcessEnhanced(int met,long testbuf_lenInPre);
	void threePreProcessRecover();
	void reservedOnePairs();
	void reservedOnePairsEnMax();
	void samePlus();
	void preProcessRecover(unsigned char *lphist, unsigned char *lpextmap, unsigned char mext, long exct, unsigned char *wp);              //�ָ�Ԥ����ǰ��ֱ��ͼ 
	void bkgdseg();
	void desaveas();

	//��ɫͼ�����ӵĺ���
	void Init();
	
	//Ƕ��
	void deoriRGBPre(unsigned char *originalImage);
	void deoriRGB();          //����hsv������ȫ����
	void deoriRGBSamePlus();
	void Genhanced();         //ֻ��ǿg
	void GenhancedRecovery();   //ֻ��ǿg��ԭ
	void Yenhanced();
	void Yrecovery();
	void SmallestRangeEnhanced();
	void ColorToThreeTimesGray();                //3Dת2D
	void ColorToThreeTimesGrayRecovery(bool haveChannel);
	void GrayReference();
	void enhancedMax();
	void enhancedMaxRe();
	int MED(int a,int b, int c);       //MEDԤ��
	int predict(unsigned char *refer, unsigned char *bePredicted, int row, int column);
	void Test();                      //ѹ������
	void Test2();
	void bubbleSort(int arr[], int n);       //ð������
	void BinarySearch(int targetS,int low,int high);     //���ֲ���
	void ColorLocationMap(unsigned char *ColorMap, long MapLength, int channel);         //channel��ʾѡ����ǿ��ͨ��������2�������ü�¼
	void ColorMapRecovery(bool haveChannel);

	long colorToGrayOnlyPreprocess(unsigned char *originalImage,int pairs);
	long threeOnOneOnlyLeftPreprocess(unsigned char *originalImage, int pairs);
	void colorToGrayOnlyPreprocessRecover(int pairs, unsigned char *lpprehistColorToGray, unsigned char *locmapColorToGray, long exctThree);
	void threeOnOneOnlyLeftPreprocessRecover(int pairs, unsigned char *imageAfterPre, unsigned char *lpprehistColorToGray, unsigned char *locmapColorToGray, long exctThree);
	void maxEnhancedWithSamePlus();
	void maxEnhancedWithSamePlusRecover();
	void maxNotEmbed(int targetS);
	bool embedJudge(int maxPairs, int targetS, long testbuf_lenThree,unsigned char *imageAfterPre);
	void enlargeInfoDiff(int targetS);
	void histStatistics(unsigned char *lpwmimage, int binNum[]);

	void twoSortAndPreprocess();
	void onlyPreprocessOneChannel(unsigned char *originalImage,int pairs);
	void twoSortAndPreprocessRecover();
	void extraBookkeepingDataEmbed(int pairs, long exct, long testbuf_len, unsigned char *locationMap, unsigned char*prehist);
	void extraBookkeepingRecover();

	//psnr����
	double psnrColorCalculate(unsigned char *originalImage, unsigned char *enhancedImage);

	//��ȡ
	void depreRGBPre(unsigned char *originalImage);
	void depreRGB();
	
//	void output_bie(unsigned char *start, size_t len, void *file);
	virtual ~CDeprejbgDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CDeprejbgDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEPREJBGDOC_H__6F28E294_16F3_4C5D_9FF6_332309A2E45C__INCLUDED_)
