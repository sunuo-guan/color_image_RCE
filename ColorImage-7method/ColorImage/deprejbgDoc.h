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
	int MinDiff;						//排序不一致的最小次数
	int Rless, Rmore, Bless, Bmore;    //溢出像素的数量
	int Rlow, Rhigh, Blow, Bhigh;      //上下溢的最大值
	int channel;                       //enhancedMax里面记录增强的是哪个通道，取值与RGBPre保持一致
	bool newPre;					//是否使用新的预处理
	
	bool onlyPreProcess;			//判断是否是只做预处理，将数据嵌入与预处理分开
	bool leftPreProcess;            //判断是否只做左侧预处理
	bool leftPreProcessRecover;     //判断是否只做左侧预处理恢复
	bool colorToGrayPreprocessRecover;   //判断是否是colorToGray直方图预处理恢复
	bool isMerge;						//判断是否有结合
	bool addDataEmbedJudge;                   //判断是否用到额外嵌入
	long addDataLenRemain;            //彩色图像额外嵌入信息嵌入过程中的剩余长度
	long addDataLen;                  //彩色图像额外嵌入信息的总长度
	long exctR, exctG, exctB, exctMax,exctThree,exctOneChannel;			//rgb通道的zn

	long testbuf_lenOneChannel;

	unsigned char *lpkey;				//Define the pointer of the original image which points the data of the original image in memory
	HANDLE hkey;

	double *originalimage, psnr, sg, sm;
	unsigned long int m_ImageSize;		//图像文件大小,包括所有信息

	//下面所有的图像指针都是包含了位图的所有信息,包括了文件头、信息头、位图像素值等
	//指向原图文件,始终是提供数据而不会改变数据,处理时先数据复制到另一个内存中去处理
	unsigned char *lporigimage;				//Define the pointer of the original image which points the data of the original image in memory
	HANDLE horigimage;
	
	unsigned char *lpwmimage;				//指向嵌入水印后的文件，指向恢复阶段开始的增强图像
	HANDLE hwmimage;

	unsigned char *lppwmimage;
	HANDLE hpwimage;

	//恢复阶段的输入图像
	unsigned char *lppreimage;				//Define the pointer of the original image which points the data of the original image in memory
	HANDLE hpreimage;

	//过渡的原始图像
	unsigned char *lporiginal;
	HANDLE horiginal;

	unsigned char *lpprehist;				//指向记录预处理的标记指针，包括原图像空bin的对应像素值，bin结合时左边bin的像素值，bin value
	HANDLE hprehist;

	unsigned char *lphist;				//即提取时的lpprehist
	HANDLE hphist;

	int *lpdifimage;				//Define the pointer of the original image which points the data of the original image in memory
	HANDLE hdifimage;
	
	unsigned int *lpblkhis;        //记录直方图各个bin的高度
	HANDLE hblkhis;

	int *lpblkval;                  //记录直方图bin的数值
	HANDLE hblkval;

	unsigned char *lpblkflg;
	HANDLE hblkflg;

	unsigned char *lpembmap;
	HANDLE hembmap;

	unsigned char *lplbldbn;
	HANDLE hlbldbn;

	unsigned char *lpembovh;
	HANDLE hembovh;

	unsigned char *lpbkimage;				//背景分割
	HANDLE hbkimage;

	unsigned char *lpextmap;               //即提取时的locmap
	HANDLE hextmap;

	unsigned char *lpprehistRchannel;				//Rchannel预处理时产生的位置信息
	HANDLE hprehistRchannel;
	unsigned char *lpprehistGchannel;				//Gchannel预处理时产生的位置信息
	HANDLE hprehistGchannel;
	unsigned char *lpprehistBchannel;				//Bchannel预处理时产生的位置信息
	HANDLE hprehistBchannel;
	
	unsigned char *lpprehistMaxChannel;				//Max预处理时产生的位置信息
	HANDLE hprehistMaxChannel;
	unsigned char *lpprehistColorToGray;				//转为灰度预处理时产生的位置信息
	HANDLE hprehistColorToGray;
	unsigned char *lpprehistOneChannel;				//转为灰度预处理时产生的位置信息
	HANDLE hprehistOneChannel;
	unsigned char *lpprehistColorToGrayExtract;				//转为灰度预处理时产生的位置信息
	HANDLE hprehistColorToGrayExtract;

	unsigned char *locmapRchannel;								//Rchannel预处理时产生的locmap
	HANDLE hlocmapRchannel;
	unsigned char *locmapGchannel;								//Gchannel预处理时产生的locmap
	HANDLE hlocmapGchannel;
	unsigned char *locmapBchannel;								//Bchannel预处理时产生的locmap
	HANDLE hlocmapBchannel;
	
	unsigned char *locmapMaxChannel;								//MaxChannel预处理时产生的locmap
	HANDLE hlocmapMaxChannel;	
	unsigned char *locmapColorToGray;								//转为灰度预处理时产生的locmap
	HANDLE hlocmapColorToGray;
	unsigned char *locmapOneChannel;								//单通道预处理时产生的locmap
	HANDLE hlocmapOneChannel;
	unsigned char *locmapColorToGrayExtract;								//单通道预处理时产生的locmap
	HANDLE hlocmapColorToGrayExtract;



	unsigned char *lp;                    //处理前图像数据起点
	unsigned char *wp;					  //处理后图像数据起点
	unsigned char *lpbk;
	unsigned char lm, met, lb, rb, la, ra, ela, era, lbn;
	unsigned char mext;                   //提取的S
	unsigned char ohpn;                   //表示需要多少次S才能全部记录下bookkeeping信息

	int bknum;         //背景色数量

	int bkpixel[256];

	double percent;    //背景色所占百分比

	LPBITMAPFILEHEADER bitfile;
    LPBITMAPINFOHEADER bitinfo;

	int ImageWidth,ImageHeight;
	long tstbfloc;                                      //记录剩余还有多少bookkeeping需要记录
	long lpoh;											//记录最后一次bookkeeping嵌入时的长度
		
	unsigned char *lphismap;                            //记录预处理bookkeeping
	HANDLE hhismap;
	
	unsigned char *locmap;								//记录lohismap，locmap一位对应lohismap八位
	HANDLE hlocmap;                                        

	unsigned char *locationCompression;					//压缩locationmap的输入
	HANDLE hlocationCompression;

	unsigned char *ExtractData;                         //记录除bookkeeping数据外的提取数据
	HANDLE hExtractData;

	unsigned char *ExtractMap;                          //从提取出来的数据中读取map数据
	HANDLE hExtractMap;

	unsigned char *colorLocationMap;                    //提取的压缩后map
	HANDLE hcolorLocationMap;

	unsigned char *ExtractDataOrder;                         //将ExtractData顺序调整好
	HANDLE hExtractDataOrder;

	long dataLen[128];                                   //记录每一轮提取的除bookkeeping数据外的数据长度

	long haveStored;                                    //记录ExtractData已经存储的长度
	long ExtractMapLen;                                 //提取出来的map长度
	int channelChosenExtract;                           //提取出来的增强channel记录

	unsigned char *pp, *extlen;
	HANDLE hextlen;

	unsigned char m_CurrentDirectory[256*256+64];		//暂存当前打开的文件信息,之后是在当前窗口标题栏显示的文件路径信息
	unsigned char m_OriginalWatermark[256*256+64];		//水印图像的纯像素值
	CString CurrentDirectory;
	
	char watermarkedfilename[500];	
	char watermarkedfiletitle[500];

	unsigned int filesize,wmheight,wmwidth, mpv, lpv, rpv, plv, threshold;
	long payload, expld, zn, exbuflen;
	long exct;                                          //即提取的zn

	LPBITMAPFILEHEADER wmfile;
	LPBITMAPINFOHEADER wminfo;

	BOOLEAN isBackground[2140][2140];
	BOOLEAN doBksega;


	//彩色图像增加的变量
	//指向R通道的拓展图
	unsigned char* lpRimage;
	HANDLE hRimage;

	//指向G通道的拓展图
	unsigned char* lpGimage;
	HANDLE hGimage;

	//指向B通道的拓展图
	unsigned char* lpBimage;
	HANDLE hBimage;

	int* lpMreimage;              //记录max位置上是哪个RGB通道
	HANDLE hMreimage;
	int* lpMereimage;             //记录median位置上是哪个RGB通道
	HANDLE hMereimage;
	int* lpMireimage;             //记录min位置上是哪个RGB通道
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

	//YUV定义
	//Y
	unsigned char* Ychannel;
	HANDLE hYchannel;
	//U
	unsigned char* Uchannel;
	HANDLE hUchannel;
	//V
	unsigned char* Vchannel;
	HANDLE hVchannel;

	/*GenhancedRe定义*/
	unsigned char* RchannelEnhancedRe;
	HANDLE hRchannelEnhancedRe;
	unsigned char* GchannelEnhancedRe;
	HANDLE hGchannelEnhancedRe;
	unsigned char* BchannelEnhancedRe;
	HANDLE hBchannelEnhancedRe;
	unsigned char *enhancedImage;				
	HANDLE henhancedImage;

	//方案2定义
	//R
	unsigned char* Rchannel;
	HANDLE hRchannel;
	//G
	unsigned char* Gchannel;
	HANDLE hGchannel;
	//B
	unsigned char* Bchannel;
	HANDLE hBchannel;
	//排序
	unsigned char* ChannelSort;
	HANDLE hChannelSort;
	//增强后排序
	unsigned char* EnhancedSort;
	HANDLE hEnhancedSort;

	//增强后RGB
	unsigned char* RchannelEnhanced;
	HANDLE hRchannelEnhanced;
	unsigned char* GchannelEnhanced;
	HANDLE hGchannelEnhanced;
	unsigned char* BchannelEnhanced;
	HANDLE hBchannelEnhanced;

	//预处理后rgb
	unsigned char* RchannelPreprocessed;
	HANDLE hRchannelPreprocessed;
	unsigned char* GchannelPreprocessed;
	HANDLE hGchannelPreprocessed;
	unsigned char* BchannelPreprocessed;
	HANDLE hBchannelPreprocessed;
	//预处理后max
	unsigned char* maxPreprocessed;
	HANDLE hmaxPreprocessed;

	//相关性方法
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

	//方案1定义
	//记录不同方法Median之差
	double *lpMedDifference;
	HANDLE hMedDifference;

	//记录不同方法Min之差
	double *lpMinDifference;
	HANDLE hMinDifference;

	//记录每次可恢复增强的MIN
	unsigned char* lpMinRecovery;
	HANDLE hMinRecovery;
	
	//记录每次可恢复增强的MIN
	unsigned char* lpMedRecovery;
	HANDLE hMedRecovery;

	//转灰度方案定义
	//转换后灰度图
	unsigned char* Image2D;
	HANDLE hImage2D;

	//灰度参考方案
	unsigned char* Grayscale;
	HANDLE hGrayscale;

	unsigned char* GrayscaleEnhanced;
	HANDLE hGrayscaleEnhanced;
	//增强后
	unsigned char* Image2DEnhanced;
	HANDLE hImage2DEnhanced;

	//增强max最多方案
	unsigned char* channelEnhanced;
	HANDLE hchannelEnhanced;
	unsigned char* MaxEnhanced;
	HANDLE hMaxEnhanced;
	unsigned char* MedianEnhanced;
	HANDLE hMedianEnhanced;
	unsigned char* MinEnhanced;
	HANDLE hMinEnhanced;

	//还原所需的标记location map
	unsigned char* locationMapColor;           //标记位，若为1则表示使用了原值，若为0则表示使用了增强值
	HANDLE hlocationMapColor;

	//溢出标记
	unsigned char* overflowMap;           //标记位，若为1则表示使用了原值，若为0则表示使用了增强值
	HANDLE hoverflowMap;

	//溢出标记
	unsigned char* minOverflowMap;           
	HANDLE hminOverflowMap;
	unsigned char* medianOverflowMap;           
	HANDLE hmedianOverflowMap;
	unsigned char* overflowValue;
	HANDLE hoverflowValue;

	//压缩前的map
	unsigned char* mapBeforeCompression;
	HANDLE hmapBeforeCompression;

	//从图像提取出来的map
	unsigned char* colorMapRecover;
	HANDLE hcolorMapRecover;

	//Gchannel方法还原所需要的map
	unsigned char* GchannelMap;
	HANDLE hGchannelMap;

	//还原所需的矩阵记录
	unsigned char* channelRecovery;
	HANDLE hchannelRecovery;
	unsigned char* MaxRecover;
	HANDLE hMaxRecover;
	unsigned char* MedianRecover;
	HANDLE hMedianRecover;
	unsigned char* MinRecover;
	HANDLE hMinRecover;

	//嵌入时用于判断的标记
	unsigned char* addDataEmbed;
	HANDLE haddDataEmbed;

	//剩余需要嵌入的彩色图像map
	unsigned char* addDataRemain;
	HANDLE haddDataRemain;

	//指向原始彩色图片,记录原始彩色文件的内存首地址
	unsigned char* lpoimage;

	//指向base on hsv处理后的彩色图像
	unsigned char* lphsvimage;

	//彩色图像大小
	unsigned long m_ColorImageSize;

	//记录文件名
	char FileTitle[50];              

	//test-Genhanced
	long buflen;

	/*twoSortAndPreprocess定义*/
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
	void defix();	//打开待嵌入的水印文件
	void deori(int mm);	//嵌入
	void depre();	//提取/还原
	long deremb(unsigned char *lpwmimg, unsigned char *lporigimg, int t);                                                           //预处理
	long derem(unsigned char *lpwmimg, unsigned char *lporigimg, int t, int met, long testbuf_len, long zn, unsigned char *locmap, unsigned char *lpprehist);
	long dextr(unsigned char *lpwmimg, unsigned char *lporigimg, int t);
	long extrec(unsigned char *lpwmimg, unsigned char *lporigimg);
	long blkemb(unsigned char *tp, unsigned char *mp, int row, int column, int bs, long buf_len, long el, int t);
	long blkems(unsigned char *tp, unsigned char *mp, int row, int column, int bs, long buf_len, long el, int t);                   //第一次嵌入
	long blkext(unsigned char *mp, int row, int column, int bs, long el, int t);
	long blkexs(unsigned char *mp, int row, int column, int bs, long el, int t);
	void threePreProcess();
	void threePreProcessEnhanced(int met,long testbuf_lenInPre);
	void threePreProcessRecover();
	void reservedOnePairs();
	void reservedOnePairsEnMax();
	void samePlus();
	void preProcessRecover(unsigned char *lphist, unsigned char *lpextmap, unsigned char mext, long exct, unsigned char *wp);              //恢复预处理前的直方图 
	void bkgdseg();
	void desaveas();

	//彩色图像增加的函数
	void Init();
	
	//嵌入
	void deoriRGBPre(unsigned char *originalImage);
	void deoriRGB();          //基于hsv，不完全可逆
	void deoriRGBSamePlus();
	void Genhanced();         //只增强g
	void GenhancedRecovery();   //只增强g还原
	void Yenhanced();
	void Yrecovery();
	void SmallestRangeEnhanced();
	void ColorToThreeTimesGray();                //3D转2D
	void ColorToThreeTimesGrayRecovery(bool haveChannel);
	void GrayReference();
	void enhancedMax();
	void enhancedMaxRe();
	int MED(int a,int b, int c);       //MED预测
	int predict(unsigned char *refer, unsigned char *bePredicted, int row, int column);
	void Test();                      //压缩测试
	void Test2();
	void bubbleSort(int arr[], int n);       //冒泡排序
	void BinarySearch(int targetS,int low,int high);     //二分查找
	void ColorLocationMap(unsigned char *ColorMap, long MapLength, int channel);         //channel表示选择增强的通道，大于2表明不用记录
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

	//psnr计算
	double psnrColorCalculate(unsigned char *originalImage, unsigned char *enhancedImage);

	//提取
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
