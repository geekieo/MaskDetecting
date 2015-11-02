
// MaskDefectDetectingDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MaskDefectDetecting.h"
#include "MaskDefectDetectingDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace cv;

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
	: CDialogEx(CMaskDefectDetectingDlg::IDD, pParent)
	, m_editKmax(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMaskDefectDetectingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_KMAX, m_editKmax);
}

BEGIN_MESSAGE_MAP(CMaskDefectDetectingDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CMaskDefectDetectingDlg::OnTcnSelchangeTab1)
	ON_BN_CLICKED(IDC_BUTTON_READIMG, &CMaskDefectDetectingDlg::OnBnClickedButtonReadimg)
	ON_BN_CLICKED(IDC_BUTTON_CORRECTION, &CMaskDefectDetectingDlg::OnBnClickedButtonCorrection)
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
void CMaskDefectDetectingDlg::DrawPicToHDC(Mat img1, UINT ID)//将 Mat 类图片绘制到显示控件
{
	IplImage *img = (&(IplImage)img1);//Mat → Iplimage*
	CDC *pDC = GetDlgItem(ID)->GetDC();
	HDC hDC = pDC->GetSafeHdc();
	CRect rect;
	GetDlgItem(ID)->GetClientRect(&rect);
	CvvImage cimg;
	cimg.CopyOf(img); // 复制图片
	cimg.DrawToHDC(hDC, &rect); // 将图片绘制到显示控件的指定区域内
	ReleaseDC(pDC);
}

void CMaskDefectDetectingDlg::OnBnClickedButtonReadimg()
{
	////手动打开图片
	//// IDB_SHOW1 为Picture控件的ID号
	//CFileDialog dlg(TRUE);
	//if (dlg.DoModal() == IDOK)
	//{
	//	CString FilePathName;
	//	FilePathName = dlg.GetPathName();
	//	//CString到string的转换
	//	CStringA stra(FilePathName.GetBuffer(0));
	//	FilePathName.ReleaseBuffer();
	//	path = stra.GetBuffer(0);
	//	stra.ReleaseBuffer();
	//	openfiles = true;
	//}
	//if (pSrcImg) cvReleaseImage(&pSrcImg);
	//const char *charpath = path.c_str();
	//pSrcImg = cvLoadImage(charpath, 1); //读图
	//if (pSrcImg)
	//{
	//	//pSrcImgCopy = cvCloneImage(pSrcImg);//拷贝当前图像
	//	DrawPicToHDC(pSrcImg, IDB_SHOW1);
	//	pNowImage = pSrcImg;//当前图片
	//}

	pSrcImg = cvLoadImage("D:\\[Experiment]\\口罩图像\\pylon_2ed\\quemianjin\\10.tiff", 1); //读图
	if (pSrcImg)
	{
		DrawPicToHDC(pSrcImg, IDB_SHOW1);
		pNowImage = pSrcImg;//当前图片
	}
	else
	{
		CString str("初始化未读到图，问题出在 MaskDefectDetectingDlg.cpp");
		MessageBox(str);
	}
}

IplImage *g_pGrayImage = NULL;
IplImage *g_pBinaryImage = NULL;
const char *pstrWindowsBinaryTitle = "阈值分割（二值图）";
//void on_trackbar(int pos)
//{
//	// 转为二值图  
//	cvThreshold(g_pGrayImage, g_pBinaryImage, pos, 255, CV_THRESH_BINARY);
//	// 显示二值图  
//	cvShowImage(pstrWindowsBinaryTitle, g_pBinaryImage);
//}

void   Delay(int   time)//time*1000为秒数 
{
	clock_t   now = clock();
	while (clock() - now   <   time);
}

void CMaskDefectDetectingDlg::OnBnClickedButtonCorrection()
{
	// TODO:  在此添加控件通知处理程序代码
	if (pSrcImg == NULL)
		return;
	start_time = clock();//程序开始计时

	Mat src(pSrcImg, true);//pSrcImg拷贝
	//	imgProcessing.Transformation(src);
	imgProcessing.SkewCorrection(src);//矫正输入src
	DrawPicToHDC(imgProcessing.pNowImage, IDB_SHOW2);//矫正输出pNowImage
	m_editKmax = imgProcessing.kmax;//给编辑框传递kmax值
	UpdateData(false);//更新编辑框数据
	end_time = (clock() - start_time) / 1000;//用时
	std::cout << "用时 " << end_time << " 秒" << std::endl;
}

void CMaskDefectDetectingDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO:  在此添加控件通知处理程序代码
	*pResult = 0;
}
//My Code End