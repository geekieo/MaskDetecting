
// MaskDefectDetectingDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MaskDefectDetecting.h"
#include "MaskDefectDetectingDlg.h"
#include "afxdialogex.h"
#include "LBP.h"
#include <fstream>
#include <ml.h> 


#ifdef _DEBUG
#define new DEBUG_NEW

//#define CELLSIZE_LBP  8  // LBP的窗口大小，4,8，16
#endif

using namespace cv;
using namespace std;
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMaskDefectDetectingDlg 对话框



CMaskDefectDetectingDlg::CMaskDefectDetectingDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMaskDefectDetectingDlg::IDD, pParent)//控件关联变量初始化

	, m_pathOpenBand(_T(""))
	//, m_pathSaveBandNeg(_T(""))
	//, m_pathSaveBandPos(_T(""))
	, m_pathTrainBand(_T(""))

	, num0(1)
	, num1(1)
	, numTestSet(1)
	, dstrows(128)
	, CELLSIZE_LBP(4)
	, TrainImgWidth(64)
	, m_checkUseGamma(1)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMaskDefectDetectingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//变量关联
	DDX_Control(pDX, IDC_INFO, m_info);
	DDX_Text(pDX, IDC_PATH_TRAIN_BAND, m_pathTrainBand);
	DDX_Text(pDX, IDC_PATH_OPEN_BAND, m_pathOpenBand);
	DDX_Text(pDX, IDC_EDIT_BANDNUM0, num0);
	DDX_Text(pDX, IDC_EDIT_BANDNUM1, num1);
	DDX_Text(pDX, IDC_EDIT_BANDNUMTESTSET, numTestSet);
	DDX_Text(pDX, IDC_EDIT_DSTROWS, dstrows);
	dstcols = 2 * dstrows;//宽要在这里赋值，否则每次刷新宽不会变
	DDX_Text(pDX, IDC_EDIT_LBPCELLSIZE, CELLSIZE_LBP);
	DDX_Text(pDX, IDC_EDIT_HOGIMGSIZE, TrainImgWidth);
	TrainImgHeight = TrainImgWidth;
}

BEGIN_MESSAGE_MAP(CMaskDefectDetectingDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_READIMG, &CMaskDefectDetectingDlg::OnBnClickedButtonReadimg)
	ON_BN_CLICKED(IDC_BUTTON_CORRECTION, &CMaskDefectDetectingDlg::OnBnClickedButtonCorrection)
	ON_BN_CLICKED(IDC_BUTTON_BANDPROCESS, &CMaskDefectDetectingDlg::OnBnClickedButtonBandprocess)
	ON_BN_CLICKED(IDC_BUTTON_HAIRPROCESS, &CMaskDefectDetectingDlg::OnBnClickedButtonHairprocess)
	ON_EN_CHANGE(IDC_PATH_OPEN_BAND, &CMaskDefectDetectingDlg::OnEnChangePathOpenBand)

	ON_BN_CLICKED(IDC_BUTTON_BATCH_BAND, &CMaskDefectDetectingDlg::OnBnClickedButtonBatchBand)
	ON_BN_CLICKED(IDC_BUTTON_BATCH_BAND2, &CMaskDefectDetectingDlg::OnBnClickedButtonBatchBand2)
	ON_BN_CLICKED(IDC_BUTTON_BATCH_BAND3, &CMaskDefectDetectingDlg::OnBnClickedButtonBatchBand3)
	ON_BN_CLICKED(IDC_BUTTON_BATCH_BAND4, &CMaskDefectDetectingDlg::OnBnClickedButtonBatchBand4)
	ON_BN_CLICKED(IDC_BUTTON_BATCH_BANDTESTSET, &CMaskDefectDetectingDlg::OnBnClickedButtonBatchBandtestset)
	ON_BN_CLICKED(IDC_BUTTON_READIMG_2, &CMaskDefectDetectingDlg::OnBnClickedButtonReadimg2)
	ON_BN_CLICKED(IDC_BUTTON_JUDGE, &CMaskDefectDetectingDlg::OnBnClickedButtonJudge)
	ON_BN_CLICKED(IDC_BUTTON_SVM, &CMaskDefectDetectingDlg::OnBnClickedButtonSvm)
	ON_BN_CLICKED(IDC_BUTTON_SVMPREDICT, &CMaskDefectDetectingDlg::OnBnClickedButtonSvmpredict)
	ON_EN_CHANGE(IDC_PATH_TRAIN_BAND, &CMaskDefectDetectingDlg::OnEnChangePathTrainBand)

	ON_BN_CLICKED(IDC_BUTTON_SVM_LBP_Rotation, &CMaskDefectDetectingDlg::OnBnClickedButtonSvmLbpRotation)
	ON_BN_CLICKED(IDC_BUTTON_SVM_LBP_Rotation_Predict, &CMaskDefectDetectingDlg::OnBnClickedButtonSvmLbpRotationPredict)
	ON_BN_CLICKED(IDC_BUTTON_TRAIN_HOG_SVM, &CMaskDefectDetectingDlg::OnBnClickedButtonTrainHogSvm)
	ON_BN_CLICKED(IDC_BUTTON_PREDICT_HOG_SVM, &CMaskDefectDetectingDlg::OnBnClickedButtonPredictHogSvm)
	ON_BN_CLICKED(IDC_BUTTON_PREHOG, &CMaskDefectDetectingDlg::OnBnClickedButtonPrehog)

	ON_BN_CLICKED(IDC_BUTTON_SAVESHOW2, &CMaskDefectDetectingDlg::OnBnClickedButtonSaveshow2)
	ON_BN_CLICKED(IDC_BUTTON_SAVESHOW3, &CMaskDefectDetectingDlg::OnBnClickedButtonSaveshow3)
	ON_BN_CLICKED(IDC_BUTTON_SAVESHOW4, &CMaskDefectDetectingDlg::OnBnClickedButtonSaveshow4)
	ON_BN_CLICKED(IDC_CHECK_USEGRAYIMG, &CMaskDefectDetectingDlg::OnBnClickedCheckUsegrayimg)
	ON_BN_CLICKED(IDC_CHECK_USEGAMMA, &CMaskDefectDetectingDlg::OnBnClickedCheckUsegamma)
	ON_BN_CLICKED(IDC_BUTTON_CREATDIR, &CMaskDefectDetectingDlg::OnBnClickedButtonCreatdir)
	ON_EN_CHANGE(IDC_EDIT_LBPCELLSIZE, &CMaskDefectDetectingDlg::OnEnChangeEditLbpcellsize)
	ON_EN_CHANGE(IDC_EDIT_HOGIMGSIZE, &CMaskDefectDetectingDlg::OnEnChangeEditHogimgsize)
	ON_EN_CHANGE(IDC_EDIT_DSTROWS, &CMaskDefectDetectingDlg::OnEnChangeEditDstrows)
	ON_BN_CLICKED(IDC_BUTTON_INFO_CLC, &CMaskDefectDetectingDlg::OnBnClickedButtonInfoClc)
	ON_BN_CLICKED(IDC_BUTTON_FUSIONTRAIN, &CMaskDefectDetectingDlg::OnBnClickedButtonFusiontrain)
	ON_BN_CLICKED(IDC_BUTTON_FUSIONPREDICT, &CMaskDefectDetectingDlg::OnBnClickedButtonFusionpredict)
END_MESSAGE_MAP()


// CMaskDefectDetectingDlg 消息处理程序

BOOL CMaskDefectDetectingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
	m_info.AddString(L"运行状态信息：");
	((CButton *)GetDlgItem(IDC_CHECK_USEGAMMA))->SetCheck(1);//灰度图Gamma校正初始化为选中
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMaskDefectDetectingDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMaskDefectDetectingDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMaskDefectDetectingDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//My Code Begin
string ws2s(const wstring& ws)//CString 转 string
{
	_bstr_t t = ws.c_str();
	char* pchar = (char*)t;
	string result = pchar;
	return result;
}

void CMaskDefectDetectingDlg::saveImgWithNoBand(Mat src, Mat srcOrigin)//保存预处理的有棉筋训练集
{
	CString cstrNum0;
	cstrNum0.Format(_T("%d"), num0);
	CString cstrSaveDir0 = m_pathSaveBandNeg + "\\" + "neg" + cstrNum0 + ".bmp";//预处理后保存路径
	CString cstrSaveDirOrigin = m_pathSaveBandNegGray + "\\" + "neg" + cstrNum0 + ".bmp";//原图保存路径
	string strSaveDir0 = ws2s(cstrSaveDir0.GetString());
	string strSaveDir0Origin = ws2s(cstrSaveDirOrigin.GetString());
	imwrite(strSaveDir0, src);
	imwrite(strSaveDir0Origin, srcOrigin);//保存原图左边
	num0++;
}

void CMaskDefectDetectingDlg::saveImgWithBand(Mat src, Mat srcOrigin)//保存预处理的无棉筋训练集
{
	CString cstrNum1;
	cstrNum1.Format(_T("%d"), num1);
	CString cstrSaveDir1 = m_pathSaveBandPos + "\\" + "pos" + cstrNum1 + ".bmp";
	CString cstrSaveDirOrigin = m_pathSaveBandPosGray + "\\" + "pos" + cstrNum1 + ".bmp";
	string strSaveDir1 = ws2s(cstrSaveDir1.GetString());
	string strSaveDir1Origin = ws2s(cstrSaveDirOrigin.GetString());
	imwrite(strSaveDir1, src);
	imwrite(strSaveDir1Origin, srcOrigin);
	num1++;
}

void CMaskDefectDetectingDlg::DrawPicToHDC(IplImage *img, UINT ID)//将 Iplimage* 图片绘制到显示控件
{
	CDC *pDC = GetDlgItem(ID)->GetDC();
	HDC hDC = pDC->GetSafeHdc();
	CRect rect;
	GetDlgItem(ID)->GetClientRect(&rect);
	CvvImage cimg;
	cimg.CopyOf(img); // 复制图片
	cimg.DrawToHDC(hDC, &rect); // 将图片绘制到显示控件的指定区域内
	ReleaseDC(pDC);
}
void CMaskDefectDetectingDlg::DrawPicToHDC(Mat src, UINT ID)//将 Mat 类图片绘制到显示控件
{
	IplImage *img = (&(IplImage)src);//Mat → Iplimage*
	CDC *pDC = GetDlgItem(ID)->GetDC();
	HDC hDC = pDC->GetSafeHdc();
	CRect rect;
	GetDlgItem(ID)->GetClientRect(&rect);
	CvvImage cimg;
	cimg.CopyOf(img); // 复制图片
	cimg.DrawToHDC(hDC, &rect); // 将图片绘制到显示控件的指定区域内
	ReleaseDC(pDC);
}
void CMaskDefectDetectingDlg::DrawPicToHDC(Mat src, UINT ID, Mat &imgID)//将 Mat 类图片绘制到显示控件
{
	imgID = src.clone();
	IplImage *img = (&(IplImage)src);//Mat → Iplimage*
	CDC *pDC = GetDlgItem(ID)->GetDC();
	HDC hDC = pDC->GetSafeHdc();
	CRect rect;
	GetDlgItem(ID)->GetClientRect(&rect);
	CvvImage cimg;
	cimg.CopyOf(img); // 复制图片
	cimg.DrawToHDC(hDC, &rect); // 将图片绘制到显示控件的指定区域内
	ReleaseDC(pDC);
}

void CMaskDefectDetectingDlg::SavePicToWin(Mat img)
{
	// 设置过滤器   
	TCHAR szFilter[] = _T("图像文件(*.bmp)|*.bmp|所有文件(*.*)|*.*||");
	// 构造保存文件对话框   
	CFileDialog fileDlg(FALSE, _T("bmp"), _T("saveShow"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);
	CString FilePath;

	// 显示保存文件对话框   
	if (IDOK == fileDlg.DoModal())
	{
		// 如果点击了文件对话框上的“保存”按钮，则将选择的文件路径显示到编辑框里   
		FilePath = fileDlg.GetPathName();

		CStringA stra(FilePath.GetBuffer(0));
		string strFilePath = stra.GetBuffer(0);
		imwrite(strFilePath, img);
	}
}

void CMaskDefectDetectingDlg::OnBnClickedButtonReadimg()
{
	//手动打开图片
	// IDB_SHOW1 为Picture控件的ID号
	CFileDialog dlg(TRUE);
	if (dlg.DoModal() == IDOK)
	{
		CString FilePathName;
		FilePathName = dlg.GetPathName();
		//CString到string的转换
		CStringA stra(FilePathName.GetBuffer(0));
		FilePathName.ReleaseBuffer();
		m_path = stra.GetBuffer(0);
		stra.ReleaseBuffer();
		openfiles = true;
	}
	if (!m_src.empty()) m_src.release();
	const char *charpath = m_path.c_str();
	m_src = imread(charpath, 0); //读图

	////m_src = imread("D:\\Pictures\\口罩图像\\pylon_2ed\\quemianjin\\10.bmp", 0); //读大倾斜图,0为单通道灰度图，1为原图
	//m_src = imread("D:\\Pictures\\口罩图像\\band01\\10.tiff", 0);
	////m_src = imread("D:\\Pictures\\口罩图像\\gn_1st\\IMG_20141229_105945.jpg", 0); //读手机拍的图
	////m_src = imread("D:\\Pictures\\口罩图像\\pylon_2ed\\toufa\\10.tiff", 0);//读头发图
	////m_src = imread("D:\\Pictures\\口罩图像\\pylon_1st\\4ul3.bmp", 0);

	if (!m_src.empty())
	{
		DrawPicToHDC(m_src, IDB_SHOW1);
		b_correction = false;
	}
	else
	{
		CString str("初始化未读到图，问题出在 OnBnClickedButtonReadimg");
		MessageBox(str);
	}
}


void   Delay(int   time)//time*1000为秒数 
{
	clock_t   now = clock();
	while (clock() - now < time);
}

void CMaskDefectDetectingDlg::OnBnClickedButtonCorrection()//对m_src矫正归一化，输出到m_nowImg
{
	// TODO:  在此添加控件通知处理程序代码
	if (m_src.empty())
		return;
	//start_time = clock();//程序开始计时
	Mat src = m_src;
	imgProcess.GetMaskRect(src);//获得矫正变换矩阵
	imgProcess.Normalization(src, imgProcess.m_maskrect, imgProcess.m_normalrows, imgProcess.m_normalcols, m_nowImg);
	DrawPicToHDC(m_nowImg, IDB_SHOW2, m_show2);//矫正输出m_nowImg
	b_correction = true;
	//Mat L, R;
	//imgProcess.Cut2half(m_nowImg,L,R);
	//imgProcess.MirrorTrans(R);
	//imwrite("切分左.jpg",L);
	//imwrite("切分右.jpg", R);
	//imwrite("矫正归一化.jpg",m_nowImg);
	//end_time = (clock() - start_time) / 1000;//用时
	//std::cout << "用时 " << end_time << " 秒" << std::endl;
}

void CMaskDefectDetectingDlg::OnBnClickedButtonBandprocess()//对m_src处理，输出到m_nowImg,m_imgL和m_imgR
{
	// TODO:  在此添加控件通知处理程序代码
	int blockSize = 25;
	int constValue = 10;

	if (m_src.empty())
		return;
	if (b_correction == false)
	{
		MessageBox(L"未进行姿态校正！");
		return;
	}
	//OnBnClickedButtonCorrection();//得出矫正变换的maskrect，m_nowImg此时为矫正归一化后的图像


	////1.固定阈值法
	//threshold(m_src, m_nowImg, 110, 255, CV_THRESH_BINARY);//CV_THRESH_BINARY  ,CV_THRESH_OTSU    实验最好
	//imgProcess.Normalization(m_nowImg, imgProcess.m_maskrect, imgProcess.m_normalrows, imgProcess.m_normalcols, m_nowImg);

	////2.局部自适应阈值法
	////adaptiveThreshold(m_src, m_nowImg, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY_INV, blockSize, constValue);
	//////imwrite("adaptiveThreshold.jpg", m_nowImg);
	////imgProcess.Normalization(m_nowImg, imgProcess.m_maskrect, imgProcess.m_normalrows, imgProcess.m_normalcols, m_nowImg);

	//imgProcess.Normalization(m_src, imgProcess.m_maskrect, imgProcess.m_normalrows, imgProcess.m_normalcols, m_nowImg);
	//Mat gammaOutput, gammaInput;
	//m_nowImg.convertTo(gammaInput, CV_32FC1);
	//float gamma = 1 / 2.2;
	//pow(gammaInput, gamma, gammaOutput);
	//normalize(gammaOutput, m_nowImg, 0, 255, NORM_MINMAX, CV_8UC1);
	//DrawPicToHDC(m_nowImg, IDB_SHOW3,m_show3);
	//adaptiveThreshold(m_nowImg, m_nowImg, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY_INV, blockSize, constValue);
	////imwrite("normalization_adaptiveThreshold.jpg", m_nowImg);
	////m_info.AddString(L"局部自适应阈值处理完毕");
	////UpdateData(FALSE);

	//3.平均阈值法
	imgProcess.Normalization(m_src, imgProcess.m_maskrect, imgProcess.m_normalrows, imgProcess.m_normalcols, m_nowImg);
	Mat gammaOutput, gammaInput, dst;
	m_nowImg.convertTo(gammaInput, CV_32FC1);
	float gamma = 1 / 2.2;
	pow(gammaInput, gamma, gammaOutput); 
	normalize(gammaOutput, m_nowImg, 0, 255, NORM_MINMAX, CV_8UC1);
	DrawPicToHDC(m_nowImg, IDB_SHOW3,m_show3);
	Scalar cs = mean(m_nowImg);
	cv::threshold(m_nowImg, m_nowImg, cs.val[0], 255, CV_THRESH_BINARY);
	m_info.AddString(L"平均阈值法处理完毕");
	UpdateData(FALSE);

	//imshow("before resize", m_nowImg);
	//resize(m_nowImg, m_nowImg, Size(dstcols, dstrows));
	DrawPicToHDC(m_nowImg, IDB_SHOW4, m_show4);
	cv::resize(m_nowImg, m_nowImg, Size(dstcols, dstrows));
	//图像裁切，左右归一化
	imgProcess.Cut2half(m_nowImg, m_imgL, m_imgR);
}

void CMaskDefectDetectingDlg::Bandprocess()
{
	int blockSize = 25;
	int constValue = 10;

	if (m_src.empty())
		return;
	if (b_correction == false)
	{
		MessageBox(L"未进行姿态校正！");
		return;
	}
	//3.平均阈值法
	imgProcess.Normalization(m_src, imgProcess.m_maskrect, imgProcess.m_normalrows, imgProcess.m_normalcols, m_nowImg);
	Scalar cs = mean(m_nowImg);
	cv::threshold(m_nowImg, m_nowImg, cs.val[0], 255, CV_THRESH_BINARY);

	DrawPicToHDC(m_nowImg, IDB_SHOW4, m_show4);
	cv::resize(m_nowImg, m_nowImg, Size(dstcols, dstrows));
	//图像裁切，左右归一化
	imgProcess.Cut2half(m_nowImg, m_imgL, m_imgR);
}


void CMaskDefectDetectingDlg::OnBnClickedButtonHairprocess()
{
	// TODO:  在此添加控件通知处理程序代码
	if (m_src.empty())
		return;
	Mat src = m_src;
	OnBnClickedButtonCorrection();//输出m_nowImg
	imgProcess.PreprocessHair(m_nowImg, m_nowImg);
	DrawPicToHDC(m_nowImg, IDB_SHOW4, m_show4);//矫正输出pNowImage
	m_nowImg.release();
}

void CMaskDefectDetectingDlg::OnEnChangePathOpenBand()
{
	UpdateData();
	UpdateData(FALSE);
}




void CMaskDefectDetectingDlg::OnEnChangePathTrainBand()
{
	//此处必须保证路径下有这四个文件夹，因为没写判断和生成文件夹的代码。
	UpdateData();//空间变量为 m_pathTrainBand
	m_pathTrainBand += "\\";
	m_pathSaveBandPos = m_pathTrainBand + _T("pos");
	m_pathSaveBandNeg = m_pathTrainBand + _T("neg");
	m_pathSaveBandTestSet = m_pathTrainBand + _T("TestSet");
	m_pathSaveBandPosGray = m_pathTrainBand + _T("posGray");;//有棉筋原图归一化保存路径
	m_pathSaveBandNegGray = m_pathTrainBand + _T("negGray");;//无棉筋原图归一化保存路径
	m_pathSaveBandTestSetGray = m_pathTrainBand + _T("TestSetGray");

	m_pathTrainBand = m_pathTrainBand.Left(m_pathTrainBand.GetLength() - 1);//删除最后的字符"//"
	CStringA stra(m_pathTrainBand.GetBuffer(0));
	m_strPathTrainBand = stra.GetBuffer(0);
	stra.ReleaseBuffer();
	UpdateData(FALSE);
}



void CMaskDefectDetectingDlg::OnBnClickedButtonBatchBand()//左无右无
{
	// TODO:  在此添加控件通知处理程序代码
	CFileFind finder;
	CString fileName;//如果找到的是文件夹 存放文件夹路径

	UpdateData();
	BOOL haveNext = finder.FindFile(m_pathOpenBand + "\\*.*"); //判断是否成功找到文件,修改" "内内容给限定查找文件类型

	//检查文件夹路径
	if (haveNext)
	{
		if (!PathIsDirectory(m_pathOpenBand))
		{
			MessageBox(L"图片源目录路径不正确！");
			return;
		}
		if (!PathIsDirectory(m_pathSaveBandNeg) && !CreateDirectory(m_pathSaveBandNeg, NULL))
		{
			MessageBox(L"结果保存文件夹不正确！");
			return;
		}
		if (!PathIsDirectory(m_pathSaveBandNegGray) && !CreateDirectory(m_pathSaveBandNegGray, NULL))
		{
			MessageBox(L"结果保存文件夹不正确！");
			return;
		}
	}
	else
	{
		MessageBox(L"没有发现图片文件！");
		return;
	}

	//开始遍历处理
	while (haveNext)
	{
		haveNext = finder.FindNextFile();//第一次执行FindNextFile是选择到第一个文件，以后执行为选择
		if (finder.IsDirectory())//找到的是文件夹，则遍历该文件夹下的文件
			continue;
		fileName = finder.GetFilePath();
		// 在此处添加对找到文件的处理
		m_src = imread(ws2s(fileName.GetString()), 0);//读取灰度图
		if (!m_src.empty())
		{
			DrawPicToHDC(m_src, IDB_SHOW1);
			b_correction = false;

			//灰度图校正缩放裁切，输出灰度图m_nowImg，m_imgLGray，m_imgRGray
			OnBnClickedButtonCorrection();//输出灰度图m_nowImg
			cv::resize(m_nowImg, m_nowImg, Size(dstcols, dstrows));
			imgProcess.Cut2half(m_nowImg, m_imgLGray, m_imgRGray);
			if (m_checkUseGamma == true)//勾选Gamma校正
			{
				//	imgProcess.GammaProcess(m_nowImg, m_nowImg);
				Mat gammaOutput, gammaInput, dst;
				m_nowImg.convertTo(gammaInput, CV_32FC1);
				float gamma = 1 / 2.2;
				pow(gammaInput, gamma, gammaOutput);
				normalize(gammaOutput, dst, 0, 255, NORM_MINMAX, CV_8UC1);
				DrawPicToHDC(dst, IDB_SHOW3, m_show3);
			}

			//归一化预处理并分割，输入灰度图m_src，输出二值增强图m_nowImg，m_imgL，m_imgR
			Bandprocess();
			//保存图片，左右都无棉筋
			saveImgWithNoBand(m_imgL, m_imgLGray);
			saveImgWithNoBand(m_imgR, m_imgRGray);	

		}
	}
	finder.Close();
	m_info.AddString(L"左无右无处理完毕");
	UpdateData(FALSE);
	return;
}

void CMaskDefectDetectingDlg::OnBnClickedButtonBatchBand2()//左无右有
{
	// TODO:  在此添加控件通知处理程序代码
	CFileFind finder;
	CString fileName;//如果找到的是文件夹 存放文件夹路径

	UpdateData();
	BOOL haveNext = finder.FindFile(m_pathOpenBand + "\\*.*"); //判断是否成功找到文件,修改" "内内容给限定查找文件类型

	//检查文件夹路径
	if (haveNext)
	{
		if (!PathIsDirectory(m_pathOpenBand))
		{
			MessageBox(L"图片源目录路径不正确！");
			return;
		}
		if (!PathIsDirectory(m_pathSaveBandNeg) && !CreateDirectory(m_pathSaveBandNeg, NULL))
		{
			MessageBox(L"结果保存文件夹不正确！");
			return;
		}

		if (!PathIsDirectory(m_pathSaveBandPos) && !CreateDirectory(m_pathSaveBandPos, NULL))
		{
			MessageBox(L"结果保存文件夹不正确！");
			return;
		}
		if (!PathIsDirectory(m_pathSaveBandNegGray) && !CreateDirectory(m_pathSaveBandNegGray, NULL))
		{
			MessageBox(L"结果保存文件夹不正确！");
			return;
		}
		if (!PathIsDirectory(m_pathSaveBandPosGray) && !CreateDirectory(m_pathSaveBandPosGray, NULL))
		{
			MessageBox(L"结果保存文件夹不正确！");
			return;
		}
	}
	else
	{
		MessageBox(L"没有发现图片文件！");
		return;
	}

	//开始遍历处理
	while (haveNext)
	{
		haveNext = finder.FindNextFile();//第一次执行FindNextFile是选择到第一个文件，以后执行为选择
		if (finder.IsDirectory())//找到的是文件夹，则遍历该文件夹下的文件
			continue;
		fileName = finder.GetFilePath();
		// 在此处添加对找到文件的处理
		m_src = imread(ws2s(fileName.GetString()), 0);//读取灰度图
		if (!m_src.empty())
		{
			DrawPicToHDC(m_src, IDB_SHOW1);
			b_correction = false;

			//原灰度图校正缩放裁切，不做二值化，输出灰度图m_nowImg，m_imgLGray，m_imgRGray
			OnBnClickedButtonCorrection();//输出灰度图m_nowImg
			cv::resize(m_nowImg, m_nowImg, Size(dstcols, dstrows));
			imgProcess.Cut2half(m_nowImg, m_imgLGray, m_imgRGray);
			if (m_checkUseGamma == true)//勾选Gamma校正
			{
				//	imgProcess.GammaProcess(m_nowImg, m_nowImg);
				Mat gammaOutput, gammaInput, dst;
				m_nowImg.convertTo(gammaInput, CV_32FC1);
				float gamma = 1 / 2.2;
				pow(gammaInput, gamma, gammaOutput);
				normalize(gammaOutput, dst, 0, 255, NORM_MINMAX, CV_8UC1);
				DrawPicToHDC(dst, IDB_SHOW3, m_show3);
			}
			//归一化预处理并分割，输入灰度图m_src，输出二值增强图m_nowImgm_imgL，m_imgR
			Bandprocess();
			//保存图片，左无右有
			saveImgWithNoBand(m_imgL, m_imgLGray);
			saveImgWithBand(m_imgR, m_imgRGray);
		}

	}
	finder.Close();
	m_info.AddString(L"左无右有处理完毕");
	UpdateData(FALSE);
	return;
}

void CMaskDefectDetectingDlg::OnBnClickedButtonBatchBand3()//左有右无
{
	// TODO:  在此添加控件通知处理程序代码
	CFileFind finder;
	CString fileName;//如果找到的是文件夹 存放文件夹路径

	UpdateData();
	BOOL haveNext = finder.FindFile(m_pathOpenBand + "\\*.*"); //判断是否成功找到文件,修改" "内内容给限定查找文件类型

	//检查文件夹路径
	if (haveNext)
	{
		if (!PathIsDirectory(m_pathOpenBand))
		{
			MessageBox(L"图片源目录路径不正确！");
			return;
		}
		if (!PathIsDirectory(m_pathSaveBandNeg) && !CreateDirectory(m_pathSaveBandNeg, NULL))
		{
			MessageBox(L"结果保存文件夹不正确！");
			return;
		}
		if (!PathIsDirectory(m_pathSaveBandPos) && !CreateDirectory(m_pathSaveBandPos, NULL))
		{
			MessageBox(L"结果保存文件夹不正确！");
			return;
		}
		if (!PathIsDirectory(m_pathSaveBandNegGray) && !CreateDirectory(m_pathSaveBandNegGray, NULL))
		{
			MessageBox(L"结果保存文件夹不正确！");
			return;
		}
		if (!PathIsDirectory(m_pathSaveBandPosGray) && !CreateDirectory(m_pathSaveBandPosGray, NULL))
		{
			MessageBox(L"结果保存文件夹不正确！");
			return;
		}
	}
	else
	{
		MessageBox(L"没有发现图片文件！");
		return;
	}

	//开始遍历处理
	while (haveNext)
	{
		haveNext = finder.FindNextFile();//第一次执行FindNextFile是选择到第一个文件，以后执行为选择
		if (finder.IsDirectory())//找到的是文件夹，则遍历该文件夹下的文件
			continue;
		fileName = finder.GetFilePath();
		// 在此处添加对找到文件的处理
		m_src = imread(ws2s(fileName.GetString()), 0);//读取灰度图
		if (!m_src.empty())
		{
			DrawPicToHDC(m_src, IDB_SHOW1);
			b_correction = false;

			//原灰度图校正缩放裁切，不做二值化，输出灰度图m_nowImg，m_imgLGray，m_imgRGray
			OnBnClickedButtonCorrection();//输出灰度图m_nowImg
			cv::resize(m_nowImg, m_nowImg, Size(dstcols, dstrows));
			imgProcess.Cut2half(m_nowImg, m_imgLGray, m_imgRGray);

			if (m_checkUseGamma == true)//勾选Gamma校正
			{
				//	imgProcess.GammaProcess(m_nowImg, m_nowImg);
				Mat gammaOutput, gammaInput, dst;
				m_nowImg.convertTo(gammaInput, CV_32FC1);
				float gamma = 1 / 2.2;
				pow(gammaInput, gamma, gammaOutput);
				normalize(gammaOutput, dst, 0, 255, NORM_MINMAX, CV_8UC1);
				DrawPicToHDC(dst, IDB_SHOW3, m_show3);
			}
			//归一化预处理并分割，输入灰度图m_src，输出二值增强图m_nowImgm_imgL，m_imgR
			Bandprocess();
			//保存图片，左有右无
			saveImgWithBand(m_imgL, m_imgLGray);
			saveImgWithNoBand(m_imgR, m_imgRGray);
		}

	}
	finder.Close();
	m_info.AddString(L"左有右无处理完毕");
	UpdateData(FALSE);
	return;
}

void CMaskDefectDetectingDlg::OnBnClickedButtonBatchBand4()//左有右有
{
	// TODO:  在此添加控件通知处理程序代码
	CFileFind finder;
	CString fileName;//如果找到的是文件夹 存放文件夹路径

	UpdateData();
	BOOL haveNext = finder.FindFile(m_pathOpenBand + "\\*.*"); //判断是否成功找到文件,修改" "内内容给限定查找文件类型

	//检查文件夹路径
	if (haveNext)
	{
		if (!PathIsDirectory(m_pathOpenBand))
		{
			MessageBox(L"图片源目录路径不正确！");
			return;
		}
		if (!PathIsDirectory(m_pathSaveBandPos) && !CreateDirectory(m_pathSaveBandPos, NULL))
		{
			MessageBox(L"结果保存文件夹不正确！");
			return;
		}
		if (!PathIsDirectory(m_pathSaveBandPosGray) && !CreateDirectory(m_pathSaveBandPosGray, NULL))
		{
			MessageBox(L"结果保存文件夹不正确！");
			return;
		}
	}
	else
	{
		MessageBox(L"没有发现图片文件！");
		return;
	}
	//开始遍历处理
	while (haveNext)
	{
		haveNext = finder.FindNextFile();//第一次执行FindNextFile是选择到第一个文件，以后执行为选择
		if (finder.IsDirectory())//找到的是文件夹，则遍历该文件夹下的文件
			continue;
		fileName = finder.GetFilePath();
		// 在此处添加对找到文件的处理
		m_src = imread(ws2s(fileName.GetString()), 0);//读取灰度图
		if (!m_src.empty())
		{
			DrawPicToHDC(m_src, IDB_SHOW1);
			b_correction = false;

			//原灰度图校正缩放裁切，不做二值化，输出灰度图m_nowImg，m_imgLGray，m_imgRGray
			OnBnClickedButtonCorrection();//输出灰度图m_nowImg
			cv::resize(m_nowImg, m_nowImg, Size(dstcols, dstrows));
			imgProcess.Cut2half(m_nowImg, m_imgLGray, m_imgRGray);

			if (m_checkUseGamma == true)//勾选Gamma校正
			{
				//	imgProcess.GammaProcess(m_nowImg, m_nowImg);
				Mat gammaOutput, gammaInput, dst;
				m_nowImg.convertTo(gammaInput, CV_32FC1);
				float gamma = 1 / 2.2;
				pow(gammaInput, gamma, gammaOutput);
				normalize(gammaOutput, dst, 0, 255, NORM_MINMAX, CV_8UC1);
				DrawPicToHDC(dst, IDB_SHOW3, m_show3);
			}
			//归一化预处理并分割，输入灰度图m_src，输出二值增强图m_nowImgm_imgL，m_imgR
			Bandprocess();
			//保存图片，左有右有
			saveImgWithBand(m_imgL, m_imgLGray);
			saveImgWithBand(m_imgR, m_imgRGray);
		}
	}
	finder.Close();
	m_info.AddString(L"左有右有处理完毕");
	UpdateData(FALSE);
	return;
}

void CMaskDefectDetectingDlg::OnBnClickedButtonBatchBandtestset()
{
	// TODO:  在此添加控件通知处理程序代码
	// TODO:  在此添加控件通知处理程序代码
	CFileFind finder;
	CString fileName;//如果找到的是文件夹 存放文件夹路径

	UpdateData();
	BOOL haveNext = finder.FindFile(m_pathOpenBand + "\\*.*"); //判断是否成功找到文件,修改" "内内容给限定查找文件类型

	//检查文件夹路径
	if (haveNext)
	{
		if (!PathIsDirectory(m_pathOpenBand))
		{
			MessageBox(L"图片源目录路径不正确！");
			return;
		}
		if (!PathIsDirectory(m_pathSaveBandTestSet) && !CreateDirectory(m_pathSaveBandTestSet, NULL))
		{
			MessageBox(L"结果保存文件夹不正确！");
			return;
		}
		if (!PathIsDirectory(m_pathSaveBandTestSetGray) && !CreateDirectory(m_pathSaveBandTestSetGray, NULL))
		{
			MessageBox(L"结果保存文件夹不正确！");
			return;
		}
	}
	else
	{
		MessageBox(L"没有发现图片文件！");
		return;
	}

	//开始遍历处理
	while (haveNext)
	{
		haveNext = finder.FindNextFile();//第一次执行FindNextFile是选择到第一个文件，以后执行为选择
		if (finder.IsDirectory())//找到的是文件夹，则遍历该文件夹下的文件
			continue;
		fileName = finder.GetFilePath();
		// 在此处添加对找到文件的处理
		m_src = imread(ws2s(fileName.GetString()), 0);//读取灰度图
		if (!m_src.empty())
		{

			DrawPicToHDC(m_src, IDB_SHOW1);
			b_correction = false;


			OnBnClickedButtonCorrection();//输出灰度图m_nowImg
			cv::resize(m_nowImg, m_nowImg, Size(dstcols, dstrows));
			imgProcess.Cut2half(m_nowImg, m_imgLGray, m_imgRGray);//原图校正缩放裁切，不做二值化
			// 归一化预处理并分割，输入灰度图m_src，输出二值增强图m_nowImg, m_imgL，m_imgR
			Bandprocess();

			//保存图片
			CString cstrNumTestSet;
			cstrNumTestSet.Format(_T("%d"), numTestSet);
			CString cstrSaveDir0 = m_pathSaveBandTestSet + "\\" + cstrNumTestSet + ".bmp";//预处理后保存路径
			CString cstrSaveDir0Gray = m_pathSaveBandTestSetGray + "\\" + cstrNumTestSet + ".bmp";//原图保存路径
			string strSaveDir0 = ws2s(cstrSaveDir0.GetString());
			string strSaveDirOrigin = ws2s(cstrSaveDir0Gray.GetString());
			imwrite(strSaveDir0, m_imgL);//保存预处理后的左边
			imwrite(strSaveDirOrigin, m_imgLGray);//保存原图左边
			numTestSet++;


			cstrNumTestSet.Format(_T("%d"), numTestSet);
			cstrSaveDir0 = m_pathSaveBandTestSet + _T("\\") + cstrNumTestSet + _T(".bmp");
			cstrSaveDir0Gray = m_pathSaveBandTestSetGray + _T("\\") + cstrNumTestSet + _T(".bmp");//原图保存路径
			strSaveDir0 = ws2s(cstrSaveDir0.GetString());
			strSaveDirOrigin = ws2s(cstrSaveDir0Gray.GetString());
			imwrite(strSaveDir0, m_imgR);//保存预处理后的右边
			imwrite(strSaveDirOrigin, m_imgRGray);//保存原图右边
			numTestSet++;
		}
	}
	finder.Close();
	m_info.AddString(L"棉筋检测测试集预处理完毕");
	UpdateData(FALSE);
	return;
}


void CMaskDefectDetectingDlg::OnBnClickedButtonReadimg2()
{
	//手动打开图片
	// IDB_SHOW1 为Picture控件的ID号
	CFileDialog dlg(TRUE);
	if (dlg.DoModal() == IDOK)
	{
		CString FilePathName;
		FilePathName = dlg.GetPathName();
		//CString到string的转换
		CStringA stra(FilePathName.GetBuffer(0));
		FilePathName.ReleaseBuffer();
		m_path = stra.GetBuffer(0);
		stra.ReleaseBuffer();
		openfiles = true;
	}
	if (!m_src.empty()) m_src.release();
	const char *charpath = m_path.c_str();
	m_src = imread(charpath, 0); //读图
	//m_src = imread("D:\\Pictures\\口罩图像\\pylon_2ed\\quemianjin\\10.bmp", 0); //读大倾斜图,0为单通道灰度图，1为原图
	//m_src = imread("D:\\Pictures\\口罩图像\\gn_1st\\IMG_20141229_105945.jpg", 0); //读手机拍的图
	//m_src = imread("D:\\Pictures\\口罩图像\\pylon_2ed\\toufa\\10.tiff", 0);//读头发图
	//m_src = imread("D:\\Pictures\\口罩图像\\pylon_1st\\4ul3.bmp", 0);

	if (!m_src.empty())
	{
		DrawPicToHDC(m_src, IDB_SHOW1);
		b_correction = false;
	}
}

int CMaskDefectDetectingDlg::SVMpredict(Mat src, CvSVM &SVM)//单张图像SVM判断返回值1为正，-1为负
{
	LBP lbp;
	int lengthOfFeatureVector = (TrainImgWidth / CELLSIZE_LBP)*(TrainImgHeight / CELLSIZE_LBP) * 58; // 特征向量的维数
	// 识别
	vector<float> featureVectorOfTestImage;
	Mat _featureVectorOfTestImage(1, lengthOfFeatureVector, CV_32FC1);
	char line[512];

	//ofstream resultOfPrediction(m_strPathTrainBand + "\\PredictResult.txt", ios::out);
	resize(src, src, cv::Size(TrainImgWidth, TrainImgHeight), 0, 0, INTER_CUBIC);//缩放样本
	// 计算LBP特征向量
	lbp.ComputerLBPFeatureVector(src, Size(CELLSIZE_LBP, CELLSIZE_LBP), featureVectorOfTestImage);
	for (vector<float>::size_type j = 0; j <= featureVectorOfTestImage.size() - 1; ++j)
	{
		_featureVectorOfTestImage.at<float>(0, j) = featureVectorOfTestImage[j];
	}

	int predict = SVM.predict(_featureVectorOfTestImage);

	return predict;

	////测试图像旋转不变等价模式计算
	//int lengthOfFeatureVector = (TrainImgWidth / CELLSIZE_LBP)*(TrainImgHeight / CELLSIZE_LBP) * 9; // 特征向量的维数，这里只有9种
	//LBP lbp;
	//vector<float> featureVectorOfTestImage;
	//Mat _featureVectorOfTestImage(1, lengthOfFeatureVector, CV_32FC1);
	//char line[512];
	//	resize(src, src, cv::Size(TrainImgWidth, TrainImgHeight), 0, 0, INTER_CUBIC);//缩放样本
	//	// 计算样本LBP特征向量
	//	lbp.ComputerLBPFeatureVector_Rotation(src, Size(CELLSIZE_LBP, CELLSIZE_LBP), featureVectorOfTestImage);
	//	for (vector<float>::size_type j = 0; j <= lengthOfFeatureVector - 1; ++j)
	//	{
	//		_featureVectorOfTestImage.at<float>(0, j) = featureVectorOfTestImage[j];
	//	}
	//	imshow("_featureVectorOfTestImage",_featureVectorOfTestImage);
	//	waitKey(0);
	//	FileStorage fs(".\\featureVectorOfTestImage.xml", FileStorage::WRITE);
	//	fs << "featureVectorOfTestImage" << _featureVectorOfTestImage;
	//	fs.release();
	//return 1;

}

int CMaskDefectDetectingDlg::binaryHogPredict(Mat src, CvSVM &SVM)//单张图像SVM判断返回值1为正，-1为负
{
	int predict = useHog.PredictSingleImage(src, TrainImgWidth, TrainImgWidth, SVM);
	return predict;	
}

void CMaskDefectDetectingDlg::OnBnClickedButtonJudge()
{
	// TODO:  在此添加控件通知处理程序代码
	if (m_src.empty())
		return;
	OnBnClickedButtonCorrection();

	//这一段伽马校正并用不到，只是为了运行界面美观；
	Mat gammaOutput, gammaInput, dst;
	m_nowImg.convertTo(gammaInput, CV_32FC1);
	float gamma = 1 / 2.2;
	pow(gammaInput, gamma, gammaOutput);
	normalize(gammaOutput, gammaOutput, 0, 255, NORM_MINMAX, CV_8UC1);
	DrawPicToHDC(gammaOutput, IDB_SHOW3, m_show3);
	//伽马校正代码结束

	Bandprocess();

	if (m_pathTrainBand.IsEmpty())
	{
		MessageBox(L"请选择训练根目录");
		return;
	}
	CvSVM SVM;
	//SVM.load((m_strPathTrainBand + "\\ClassifierBinaryLBP.xml").data());//载入训练好的SVM分类器
	//int predictBandL = SVMpredict(m_imgL, SVM);
	//int predictBandR = SVMpredict(m_imgR, SVM);
	SVM.load((m_strPathTrainBand + "\\HOGClassifierBinary.xml").data());//载入训练好的SVM分类器

	int predictBandL = binaryHogPredict(m_imgL, SVM);
	int predictBandR = binaryHogPredict(m_imgR, SVM);

	if (predictBandL == 1 && predictBandR == 1)
		m_info.AddString(L"口罩带齐全");
	if (predictBandL == -1 && predictBandR == 1)
		m_info.AddString(L"左口罩带缺失");
	if (predictBandL == 1 && predictBandR == -1)
		m_info.AddString(L"右口罩带缺失");
	if (predictBandL == -1 && predictBandR == -1)
		m_info.AddString(L"左右口罩带均缺失");

	UpdateData(FALSE);
}

int CMaskDefectDetectingDlg::GetDir(CString path, CString txtPath, int cat)
{
	CFileFind finder;
	CString fileName;//文件包含路径的全名
	ofstream output(ws2s(txtPath.GetString()), ios::app);//创建文件
	BOOL haveNext = finder.FindFile(path + "\\*.bmp"); //判断是否成功找到文件," *.*"为任意文件

	//开始遍历处理
	if (!haveNext)
	{
		m_info.AddString(L"无bmp图像文件");
		return 0;
	}
	while (haveNext)
	{
		haveNext = finder.FindNextFile();//第一次执行FindNextFile是选择到第一个文件，以后执行为选择
		//if (finder.IsDirectory())//找到的是文件夹，则遍历该文件夹下的文件
		//	continue;
		//fileName = finder.GetFilePath();
		// 在此处添加对找到文件的处理
		CString imgDir = finder.GetFilePath();
		string str = CStringA(imgDir);
		output << str.c_str() << "\n";
		output << cat << "\n";
	}
	finder.Close();
	//m_info.AddString(L"训练目录生成完毕");
	UpdateData(FALSE);
	return 1;
}

int CMaskDefectDetectingDlg::GetDir(CString path, CString txtPath)
{
	CFileFind finder;
	CString fileName;//文件包含路径的全名
	ofstream output(ws2s(txtPath.GetString()), ios::app);//创建文件
	BOOL haveNext = finder.FindFile(path + "\\*.bmp"); //判断是否成功找到文件," *.*"为任意文件

	//开始遍历处理
	if (!haveNext)
	{
		m_info.AddString(L"无bmp图像文件");
		return 0;
	}
	while (haveNext)
	{
		haveNext = finder.FindNextFile();//第一次执行FindNextFile是选择到第一个文件，以后执行为选择
		//if (finder.IsDirectory())//找到的是文件夹，则遍历该文件夹下的文件
		//	continue;
		//fileName = finder.GetFilePath();
		// 在此处添加对找到文件的处理
		CString imgDir = finder.GetFilePath();
		string str = CStringA(imgDir);
		output << str.c_str() << "\n";
	}

	finder.Close();
	//m_info.AddString(L"目录生成完毕");
	UpdateData(FALSE);
	return 1;
}

int CMaskDefectDetectingDlg::DeleteLastLine(string path)
{
	vector<string> tmp_files;
	ifstream winfile(path);
	if (!winfile)
	{
		cout << "fail" << endl;
		return 0;
	}
	string lineContent;
	while (getline(winfile, lineContent, '\n'))
	{
		tmp_files.push_back(lineContent + "\n");
	}

	winfile.close();

	tmp_files.back().erase(tmp_files.back().end() - 1);//删除最后一个\n

	ofstream offile(path, ios::out);
	vector<string>::iterator siter = tmp_files.begin();

	copy(tmp_files.begin(), tmp_files.end(), ostream_iterator<string>(offile));

	//cout << "ok!" << endl;

	offile.close();
	return 1;
}

void CMaskDefectDetectingDlg::OnBnClickedButtonSvm()//此处吧txtPath的代码缩短处理了一下
{
	// TODO:  在此添加控件通知处理程序代码
	// 读入训练样本路径和类别
	UpdateData();
	clock_t   startTime, endTime;
	startTime = clock();//开始时间
	vector<string> imagePaths;
	vector<int> imageClass;
	string buffer;
	int numberOfLine = 0;
	if (m_pathTrainBand.IsEmpty())
	{
		MessageBox(L"请选择训练根目录");
		return;
	}

	string TrainSetTxtPath;
	CString txtPath;
	if (m_checkUseGrayimg == 0)//使用二值增强图
	{
		 txtPath = m_pathTrainBand +_T("\\TrainSetBinary.txt");
	}
	else//使用灰度图
	{
		txtPath = m_pathTrainBand + _T("\\TrainSetGray.txt");
	}

	TrainSetTxtPath = ws2s(txtPath.GetString());//路径字符串类型转换
	ifstream file(TrainSetTxtPath, ios::in);
	
	if (file.peek() == EOF)
	{
		m_info.AddString(L"目录为空，请先生成样本目录");
		UpdateData(false);
		return;
	}
	while (!file.eof())
	{
		if (getline(file, buffer))
		{
			++numberOfLine;
			if (numberOfLine % 2 == 0)//读到样本类别
			{
				imageClass.push_back(atoi(buffer.c_str()));
			}
			else
			{
				imagePaths.push_back(buffer);
			}
		}
	}

	file.close();

	// 计算样本LBP特征向量矩阵和类别矩阵
	int lengthOfFeatureVector = (TrainImgWidth / CELLSIZE_LBP)*(TrainImgHeight / CELLSIZE_LBP) * 58; // 特征向量的维数()
	Mat featureVectorOfSample(imagePaths.size(), lengthOfFeatureVector, CV_32FC1); // 样本的特征向量矩阵
	Mat classOfSample(imagePaths.size(), 1, CV_32SC1);
	vector<string>::size_type numberOfSample = imagePaths.size();
	Mat srcImage;
	LBP lbp;
	vector<float> featureVector;
	for (vector<string>::size_type i = 0; i <= numberOfSample - 1; ++i)
	{
		// 读入图片ifstream
		srcImage = imread(imagePaths[i], 0);
		if (m_checkUseGrayimg == true) // 如果使用灰度图
		{
			if (m_checkUseGamma == true)//是否对灰度图做Gamma校正
			{
				//	imgProcess.GammaProcess(srcImage, srcImage);
				Mat gammaOutput, gammaInput;
				srcImage.convertTo(gammaInput, CV_32FC1);
				float gamma = 1 / 2.2;
				pow(gammaInput, gamma, gammaOutput);
				normalize(gammaOutput, srcImage, 0, 255, NORM_MINMAX, CV_8UC1);
			}
		}
		resize(srcImage, srcImage, cv::Size(TrainImgWidth, TrainImgHeight), 0, 0, INTER_CUBIC);//缩放样本
		// 计算样本LBP特征向量
		lbp.ComputerLBPFeatureVector(srcImage, Size(CELLSIZE_LBP, CELLSIZE_LBP), featureVector);
		for (vector<float>::size_type j = 0; j <= lengthOfFeatureVector - 1; ++j)
		{
			featureVectorOfSample.at<float>(i, j) = featureVector[j];
		}
		classOfSample.at<int>(i, 0) = imageClass[i];
	}

	// 使用SVM分类器训练
	// 参数设置
	CvSVMParams params;
	params.svm_type = CvSVM::C_SVC;
	params.kernel_type = CvSVM::LINEAR;
	params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 100, 1e-6);
	/*Ptr<SVM> svm = SVM::create();
	svm->setType(SVM::C_SVC);
	svm->setKernel(SVM::LINEAR);
	svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 100, 1e-6));*/

	//训练SVM
	CvSVM SVM;
	SVM.train(featureVectorOfSample, classOfSample, Mat(), Mat(), params);
	if (m_checkUseGrayimg == 0)//使用二值增强图
	{
		SVM.save((m_strPathTrainBand + "\\ClassifierBinaryLBP.xml").data());
		m_info.AddString(L"二值增强LBP分类训练完毕");
	}
	else
	{
		SVM.save((m_strPathTrainBand + "\\ClassifierGrayLBP.xml").data());
		m_info.AddString(L"灰度图LBP分类训练完毕");
	}
	//svm->train(featureVectorOfSample, ROW_SAMPLE, classOfSample);
	//svm->save(string(PATH) + "Classifier.xml");
	endTime = clock();//结束时间
	double runTime = (double)(endTime - startTime)/1000;
	//wstringstream ss;
	//wstring str;
	//ss << end_time;
	//ss >> str;	
	//str = "训练用时：" + str;
	//m_info.AddString(str.c_str());
	UpdateData(FALSE);

}

void CMaskDefectDetectingDlg::OnBnClickedButtonSvmpredict()//
{
	// TODO:  在此添加控件通知处理程序代码
	// 使用训练好的分类器进行识别
	UpdateData();
	clock_t   startTime, endTime;
	startTime = clock();//开始时间

	if (m_pathTrainBand.IsEmpty())
	{
		MessageBox(L"请选择训练根目录");
		return;
	}
	string TestSetTxtPath;
	if (m_checkUseGrayimg == 0)//使用二值增强图
	{
		CString txtPath = m_pathTrainBand + "\\TestSetBinary.txt";
		TestSetTxtPath = ws2s(txtPath.GetString());//路径字符串类型转换
	}
	else//使用灰度图
	{
		CString txtPath = m_pathTrainBand + "\\TestSetGray.txt";
		TestSetTxtPath = ws2s(txtPath.GetString());//路径字符串类型转换
	}


	vector<string> testImagePath;
	string buffer;
	int lengthOfFeatureVector = (TrainImgWidth / CELLSIZE_LBP)*(TrainImgHeight / CELLSIZE_LBP) * 58; // 特征向量的维数
	LBP lbp;
	CvSVM SVM;

	if (m_checkUseGrayimg == 0)//使用二值增强图
	{
		SVM.load((m_strPathTrainBand + "\\ClassifierBinaryLBP.xml").data());//载入训练好的SVM分类器
	}
	else
	{
		SVM.load((m_strPathTrainBand + "\\ClassifierGrayLBP.xml").data());//载入训练好的SVM分类器
	}

	ifstream testFile(TestSetTxtPath, ios::in);// 注意要去掉最后一行的换行，否则最后一幅图片读出来就是空的
	if (!testFile)
	{
		m_info.AddString(L"请先对训练集做预处理");
		UpdateData(FALSE);
		return;
	}
	while (!testFile.eof())	{
		getline(testFile, buffer);
		testImagePath.push_back(buffer);
	}
	// 识别
	Mat testImage;
	vector<string>::size_type numberOfTestImage = testImagePath.size();
	vector<float> featureVectorOfTestImage;
	Mat _featureVectorOfTestImage(1, lengthOfFeatureVector, CV_32FC1);
	char line[512];

	string PredictTxtPath;
	if (m_checkUseGrayimg == 0)//使用二值增强图
	{
		CString txtPath = m_pathTrainBand + "\\PredictBinaryLBP.txt";
		PredictTxtPath = ws2s(txtPath.GetString());
	}
	else
	{
		CString txtPath = m_pathTrainBand + "\\PredictGrayLBP.txt";
		PredictTxtPath = ws2s(txtPath.GetString());
	}
	ofstream resultOfPrediction(PredictTxtPath, ios::out);

	// 注意将循环体内的耗时变量和操作提取到循环体内
	for (vector<string>::size_type i = 0; i <= numberOfTestImage - 1; ++i)
	{
		testImage = imread(testImagePath[i], 0);
		if (m_checkUseGrayimg == true) // 如果使用灰度图
		{
			if (m_checkUseGamma == true)//是否对灰度图做Gamma校正
			{
				//	imgProcess.GammaProcess(srcImage, srcImage);
				Mat gammaOutput, gammaInput;
				testImage.convertTo(gammaInput, CV_32FC1);
				float gamma = 1 / 2.2;
				pow(gammaInput, gamma, gammaOutput);
				normalize(gammaOutput, testImage, 0, 255, NORM_MINMAX, CV_8UC1);
			}
		}
		resize(testImage, testImage, cv::Size(TrainImgWidth, TrainImgHeight), 0, 0, INTER_CUBIC);//缩放样本
		// 计算LBP特征向量
		lbp.ComputerLBPFeatureVector(testImage, Size(CELLSIZE_LBP, CELLSIZE_LBP), featureVectorOfTestImage);
		for (vector<float>::size_type j = 0; j <= featureVectorOfTestImage.size() - 1; ++j)
		{
			_featureVectorOfTestImage.at<float>(0, j) = featureVectorOfTestImage[j];
		}

		int predict = SVM.predict(_featureVectorOfTestImage);
		sprintf(line, "%s %d\n", testImagePath[i].c_str(), predict);
		resultOfPrediction << line;
	}
	resultOfPrediction.close();

	m_info.AddString(L"等价LBP识别完毕");
	
	endTime = clock();//结束时间
	double runTime = (double)(endTime - startTime) / 1000;
	UpdateData(FALSE);
}

void CMaskDefectDetectingDlg::OnBnClickedButtonSvmLbpRotation()
{
	// TODO:  在此添加控件通知处理程序代码
	// 读入训练样本路径和类别
	UpdateData();
	clock_t   startTime, endTime;
	startTime = clock();//开始时间

	vector<string> imagePaths;
	vector<int> imageClass;
	string buffer;
	int numberOfLine = 0;
	if (m_pathTrainBand.IsEmpty())
	{
		MessageBox(L"请选择训练根目录");
		return;
	}
	string TrainSetTxtPath;
	if (m_checkUseGrayimg == 0)//使用二值增强图
	{
		CString txtPath = m_pathTrainBand + "\\TrainSetBinary.txt";
		TrainSetTxtPath = ws2s(txtPath.GetString());//路径字符串类型转换
	}
	else//使用灰度图
	{
		CString txtPath = m_pathTrainBand + "\\TrainSetGray.txt";
		TrainSetTxtPath = ws2s(txtPath.GetString());
	}

	ifstream file(TrainSetTxtPath, ios::in);

	if (file.peek() == EOF)
	{
		m_info.AddString(L"目录为空，请先生成样本目录");
		UpdateData(false);
		return;
	}
	while (!file.eof())
	{
		if (getline(file, buffer))
		{
			++numberOfLine;
			if (numberOfLine % 2 == 0)//读到样本类别
			{
				imageClass.push_back(atoi(buffer.c_str()));
			}
			else
			{
				imagePaths.push_back(buffer);
			}
		}
	}

	file.close();

	// 计算样本LBP特征向量矩阵和类别矩阵
	int lengthOfFeatureVector = (TrainImgWidth / CELLSIZE_LBP)*(TrainImgHeight / CELLSIZE_LBP) * 9; // 特征向量的维数，这里只有9种
	Mat featureVectorOfSample(imagePaths.size(), lengthOfFeatureVector, CV_32FC1); // 样本的特征向量矩阵
	Mat classOfSample(imagePaths.size(), 1, CV_32SC1);
	vector<string>::size_type numberOfSample = imagePaths.size();
	Mat srcImage;
	LBP lbp;
	vector<float> featureVector;
	for (vector<string>::size_type i = 0; i <= numberOfSample - 1; ++i)
	{
		// 读入图片
		srcImage = imread(imagePaths[i], 0);
		if (m_checkUseGrayimg == true) // 如果使用灰度图
		{
			if (m_checkUseGamma == true)//是否对灰度图做Gamma校正
			{
				//	imgProcess.GammaProcess(srcImage, srcImage);
				Mat gammaOutput, gammaInput;
				srcImage.convertTo(gammaInput, CV_32FC1);
				float gamma = 1 / 2.2;
				pow(gammaInput, gamma, gammaOutput);
				normalize(gammaOutput, srcImage, 0, 255, NORM_MINMAX, CV_8UC1);
			}
		}
		resize(srcImage, srcImage, cv::Size(TrainImgWidth, TrainImgHeight), 0, 0, INTER_CUBIC);//缩放样本
		// 计算样本LBP特征向量
		lbp.ComputerLBPFeatureVector_Rotation(srcImage, Size(CELLSIZE_LBP, CELLSIZE_LBP), featureVector);
		for (vector<float>::size_type j = 0; j <= lengthOfFeatureVector - 1; ++j)
		{
			featureVectorOfSample.at<float>(i, j) = featureVector[j];
		} 
		classOfSample.at<int>(i, 0) = imageClass[i];
	}

	// 使用SVM分类器训练
	// 参数设置
	CvSVMParams params;
	params.svm_type = CvSVM::C_SVC;
	params.kernel_type = CvSVM::LINEAR;
	params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 100, 1e-6);
	/*Ptr<SVM> svm = SVM::create();
	svm->setType(SVM::C_SVC);
	svm->setKernel(SVM::LINEAR);
	svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 100, 1e-6));*/

	//训练SVM
	CvSVM SVM;
	SVM.train(featureVectorOfSample, classOfSample, Mat(), Mat(), params);
	if (m_checkUseGrayimg == 0)//使用二值增强图
	{
		SVM.save((m_strPathTrainBand + "\\ClassifierBinaryRotLBP.xml").data());
		m_info.AddString(L"二值增强旋转不变LBP分类训练完毕");
	}
	else
	{
		SVM.save((m_strPathTrainBand + "\\ClassifierGrayRotLBP.xml").data());
		m_info.AddString(L"灰度图旋转不变LBP分类训练完毕");
	}
	//svm->train(featureVectorOfSample, ROW_SAMPLE, classOfSample);
	//svm->save(string(PATH) + "Classifier.xml");
	endTime = clock();//结束时间
	double runTime = (double)(endTime - startTime) / 1000;
	UpdateData(FALSE);
}


void CMaskDefectDetectingDlg::OnBnClickedButtonSvmLbpRotationPredict()
{
	// 使用训练好的分类器进行识别
	UpdateData();
	clock_t   startTime, endTime;
	startTime = clock();//开始时间

	if (m_pathTrainBand.IsEmpty())
	{
		MessageBox(L"请选择训练根目录");
		return;
	}
	string TestSetTxtPath;
	if (m_checkUseGrayimg == 0)//使用二值增强图
	{
		CString txtPath = m_pathTrainBand + "\\TestSetBinary.txt";
		TestSetTxtPath = ws2s(txtPath.GetString());//路径字符串类型转换
	}
	else//使用灰度图
	{
		CString txtPath = m_pathTrainBand + "\\TestSetGray.txt";
		TestSetTxtPath = ws2s(txtPath.GetString());//路径字符串类型转换
	}


	vector<string> testImagePath;
	string buffer;
	int lengthOfFeatureVector = (TrainImgWidth / CELLSIZE_LBP)*(TrainImgHeight / CELLSIZE_LBP) * 9; // 特征向量的维数,这里只有9种
	LBP lbp;
	CvSVM SVM;
	//载入训练好的SVM分类器
	if (m_checkUseGrayimg == 0)//使用二值增强图
	{
		SVM.load((m_strPathTrainBand + "\\ClassifierBinaryRotLBP.xml").data());
	}
	else
	{
		SVM.load((m_strPathTrainBand + "\\ClassifierGrayRotLBP.xml").data());
	}

	ifstream testFile(TestSetTxtPath, ios::in);// 注意要去掉最后一行的换行，否则最后一幅图片读出来就是空的
	if (!testFile)
	{
		m_info.AddString(L"请先对训练集做预处理");
		UpdateData(FALSE);
		return;
	}
	while (!testFile.eof())
	{
		getline(testFile, buffer);
		testImagePath.push_back(buffer);
	}
	// 识别
	Mat testImage;
	vector<string>::size_type numberOfTestImage = testImagePath.size();
	vector<float> featureVectorOfTestImage;
	Mat _featureVectorOfTestImage(1, lengthOfFeatureVector, CV_32FC1);
	char line[512];
//结果保存路径
	string PredictTxtPath;
	if (m_checkUseGrayimg == 0)//使用二值增强图
	{
		CString txtPath = m_pathTrainBand + "\\PredictBinaryRotLBP.txt";
		PredictTxtPath = ws2s(txtPath.GetString());
	}
	else
	{
		CString txtPath = m_pathTrainBand + "\\PredictGrayRotLBP.txt";
		PredictTxtPath = ws2s(txtPath.GetString());
	}
	ofstream resultOfPrediction(PredictTxtPath, ios::out);


	// 注意将循环体内的耗时变量和操作提取到循环体内
	for (vector<string>::size_type i = 0; i <= numberOfTestImage - 1; ++i)
	{
		testImage = imread(testImagePath[i], 0);
		if (m_checkUseGrayimg == true) // 如果使用灰度图
		{
			if (m_checkUseGamma == true)//是否对灰度图做Gamma校正
			{
				//	imgProcess.GammaProcess(srcImage, srcImage);
				Mat gammaOutput, gammaInput;
				testImage.convertTo(gammaInput, CV_32FC1);
				float gamma = 1 / 2.2;
				pow(gammaInput, gamma, gammaOutput);
				normalize(gammaOutput, testImage, 0, 255, NORM_MINMAX, CV_8UC1);
			}
		}
		resize(testImage, testImage, cv::Size(TrainImgWidth, TrainImgHeight), 0, 0, INTER_CUBIC);//缩放样本
		// 计算LBP特征向量
		lbp.ComputerLBPFeatureVector_Rotation(testImage, Size(CELLSIZE_LBP, CELLSIZE_LBP), featureVectorOfTestImage);
		for (vector<float>::size_type j = 0; j <= featureVectorOfTestImage.size() - 1; ++j)
		{
			_featureVectorOfTestImage.at<float>(0, j) = featureVectorOfTestImage[j];
		}

		int predict = SVM.predict(_featureVectorOfTestImage);
		sprintf(line, "%s %d\n", testImagePath[i].c_str(), predict);
		resultOfPrediction << line;
	}
	resultOfPrediction.close();
	m_info.AddString(L"旋转不变等价LBP识别完毕");

	endTime = clock();//结束时间
	double runTime = (double)(endTime - startTime) / 1000;
	UpdateData(FALSE);
}


void CMaskDefectDetectingDlg::OnBnClickedButtonTrainHogSvm()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData();
	clock_t   startTime, endTime;
	startTime = clock();//开始时间

	if (m_pathTrainBand.IsEmpty())
	{
		MessageBox(L"请选择训练根目录");
		return;
	}
	int imgWidth = TrainImgWidth;//初始值为64
	int imgHight = TrainImgHeight;
	string TrainSetTxtPath;
	string SVMSavePath;
	if (m_checkUseGrayimg == 0)//使用二值增强图
	{
		CString txtPath = m_pathTrainBand + "\\TrainSetBinary.txt";
		CString svmPath = m_pathTrainBand + "\\HOGClassifierBinary.xml";
		TrainSetTxtPath = ws2s(txtPath.GetString());//路径字符串类型转换
		SVMSavePath = ws2s(svmPath.GetString());
	}
	else//使用灰度图
	{
		CString txtPath = m_pathTrainBand + "\\TrainSetGray.txt";
		CString svmPath = m_pathTrainBand + "\\HOGClassifierGray.xml";
		TrainSetTxtPath = ws2s(txtPath.GetString());
		SVMSavePath = ws2s(svmPath.GetString());
	}
	ifstream file(TrainSetTxtPath, ios::in);
	if (file.peek() == EOF)
	{
		m_info.AddString(L"目录为空，请先生成样本目录");
		UpdateData(false);
		return;
	}
	useHog.TrainHOGwithSVM(TrainSetTxtPath, imgWidth, imgHight, SVMSavePath);
	if (m_checkUseGrayimg == 0)//使用二值增强图
	{
		m_info.AddString(L"二值增强样本HOG特征训练完毕");
	}
	else//使用灰度图
	{
		m_info.AddString(L"灰度样本HOG特征训练完毕");
	}

	endTime = clock();//结束时间
	double runTime = (double)(endTime - startTime) / 1000;
	UpdateData(FALSE);
}


void CMaskDefectDetectingDlg::OnBnClickedButtonPredictHogSvm()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData();
	clock_t   startTime, endTime;
	startTime = clock();//开始时间

	if (m_pathTrainBand.IsEmpty())
	{
		MessageBox(L"请选择训练根目录");
		return;
	}
	int imgWidth = TrainImgWidth;//初始值为64
	int imgHight = TrainImgHeight;

	string TestSetTxtPath;
	string SVMLoadPath;
	string PredictSavePath;
	if (m_checkUseGrayimg == 0)//使用二值增强图
	{
		CString txtPath = m_pathTrainBand + "\\TestSetBinary.txt";
		CString svmPath = m_pathTrainBand + "\\HOGClassifierBinary.xml";
		CString predictPath = m_pathTrainBand + "\\HOGPredictBinary.txt";
		TestSetTxtPath = ws2s(txtPath.GetString());//路径字符串类型转换
		SVMLoadPath = ws2s(svmPath.GetString());
		PredictSavePath = ws2s(predictPath.GetString());
		m_info.AddString(L"二值图HOG特征判别完毕");
	}
	else//使用灰度图
	{
		CString txtPath = m_pathTrainBand + "\\TestSetGray.txt";
		CString svmPath = m_pathTrainBand + "\\HOGClassifierGray.xml";
		CString predictPath = m_pathTrainBand + "\\HOGPredictGray.txt";
		TestSetTxtPath = ws2s(txtPath.GetString());
		SVMLoadPath = ws2s(svmPath.GetString());
		PredictSavePath = ws2s(predictPath.GetString());
		m_info.AddString(L"灰度图HOG特征判别完毕");
	}

	useHog.Predict(TestSetTxtPath, SVMLoadPath, imgWidth, imgHight, PredictSavePath);

	endTime = clock();//结束时间
	double runTime = (double)(endTime - startTime) / 1000;
	UpdateData(FALSE);
}


void CMaskDefectDetectingDlg::OnBnClickedButtonPrehog()
{
	// TODO:  在此添加控件通知处理程序代码
	if (m_src.empty())
		return;
	OnBnClickedButtonCorrection();//得出矫正变换的maskrect，m_nowImg此时为矫正归一化后的图像
	//Gamma校正
	Mat gammaOutput, gammaInput, dst;
	m_nowImg.convertTo(gammaInput, CV_32FC1);
	float gamma = 1 / 2.2;
	pow(gammaInput, gamma, gammaOutput);
	normalize(gammaOutput, dst, 0, 255, NORM_MINMAX, CV_8UC1);
	//Gamma校正完毕
	DrawPicToHDC(dst, IDB_SHOW3, m_show3);
	//imwrite("Gamma.jpg", dst);
	m_info.AddString(L"Gamma校正完毕");
	UpdateData(FALSE);

	Mat gradImage, gradOrient;
	useHog.computeGradient(dst, gradImage, gradOrient);
	//imwrite("gradImage.jpg", gradImage);
	//imwrite("gradOrient.jpg", gradOrient);

	DrawPicToHDC(gradImage, IDB_SHOW4, m_show4);
	m_info.AddString(L"梯度计算完毕");
	UpdateData(FALSE);

}



void CMaskDefectDetectingDlg::OnBnClickedButtonSaveshow2()
{
	// TODO:  在此添加控件通知处理程序代码
	SavePicToWin(m_show2);
}


void CMaskDefectDetectingDlg::OnBnClickedButtonSaveshow3()
{
	// TODO:  在此添加控件通知处理程序代码
	SavePicToWin(m_show3);
}


void CMaskDefectDetectingDlg::OnBnClickedButtonSaveshow4()
{
	// TODO:  在此添加控件通知处理程序代码
	SavePicToWin(m_show4);
}


void CMaskDefectDetectingDlg::OnBnClickedCheckUsegrayimg()
{
	// TODO:  在此添加控件通知处理程序代码
	if (BST_CHECKED == IsDlgButtonChecked(IDC_CHECK_USEGRAYIMG))//被选中
	{
		m_checkUseGrayimg = 1;
	}
	else//没有选中
	{
		m_checkUseGrayimg = 0;
	}
}


void CMaskDefectDetectingDlg::OnBnClickedCheckUsegamma()
{
	// TODO:  在此添加控件通知处理程序代码
	if (BST_CHECKED == IsDlgButtonChecked(IDC_CHECK_USEGAMMA))//被选中
	{
		m_checkUseGamma = 1;
	}
	else//没有选中
	{
		m_checkUseGamma = 0;
	}
}

//生成训练集目录，二值增强目录为TrainSetBinary.txt，灰度图目录为TrainSetGray.txt
void CMaskDefectDetectingDlg::OnBnClickedButtonCreatdir()
{
	// TODO:  在此添加控件通知处理程序代码
	if (m_pathTrainBand.IsEmpty())
	{
		MessageBox(L"请选择训练根目录");
		return;
	}
	string TrainSetBinaryTxtPath;
	string TrainSetGrayTxtPath;

	CString txtPath = m_pathTrainBand + "\\TrainSetBinary.txt";
	ofstream output(ws2s(txtPath.GetString()), ios::out);//创建文件,如果有清空
	GetDir(m_pathSaveBandPos, txtPath, 1);
	GetDir(m_pathSaveBandNeg, txtPath, -1);
	DeleteLastLine(ws2s(txtPath.GetString()));
	m_info.AddString(L"二值增强训练集目录生成完毕");



	CString txtPath2 = m_pathTrainBand + "\\TrainSetGray.txt";
	ofstream output2(ws2s(txtPath2.GetString()), ios::out);//创建文件,如果有清空
	GetDir(m_pathSaveBandPosGray, txtPath2, 1);
	GetDir(m_pathSaveBandNegGray, txtPath2, -1);
	DeleteLastLine(ws2s(txtPath2.GetString()));
	m_info.AddString(L"灰度样本训练集目录生成完毕");


	////-----------------上为训练集目录；下为测试集目录----------------////

	string TestSetBinaryTxtPath;
	string TestSetGrayTxtPath;

	CString txtPath3 = m_pathTrainBand + "\\TestSetBinary.txt";
	ofstream output3(ws2s(txtPath3.GetString()), ios::out);//创建文件,如果有清空
	GetDir(m_pathSaveBandTestSet, txtPath3);
	DeleteLastLine(ws2s(txtPath3.GetString()));
	m_info.AddString(L"二值增强测试集目录生成完毕");


	CString txtPath4 = m_pathTrainBand + "\\TestSetGray.txt";
	ofstream output4(ws2s(txtPath4.GetString()), ios::out);//创建文件,如果有清空
	GetDir(m_pathSaveBandTestSet, txtPath4);
	DeleteLastLine(ws2s(txtPath4.GetString()));
	m_info.AddString(L"灰度样本测试集目录生成完毕");

	UpdateData(FALSE);
}


void CMaskDefectDetectingDlg::OnEnChangeEditLbpcellsize()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。
	UpdateData();
	UpdateData(FALSE);
}


void CMaskDefectDetectingDlg::OnEnChangeEditHogimgsize()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。
	UpdateData();
	UpdateData(FALSE);
}


void CMaskDefectDetectingDlg::OnEnChangeEditDstrows()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。
	UpdateData();
	UpdateData(FALSE);
}


void CMaskDefectDetectingDlg::OnBnClickedButtonInfoClc()
{
	// TODO:  在此添加控件通知处理程序代码
	m_info.ResetContent();
	m_info.AddString(L"运行状态信息：");
}


void CMaskDefectDetectingDlg::OnBnClickedButtonFusiontrain()
{
	// TODO:  在此添加控件通知处理程序代码

}


void CMaskDefectDetectingDlg::OnBnClickedButtonFusionpredict()
{
	// TODO:  在此添加控件通知处理程序代码
}
