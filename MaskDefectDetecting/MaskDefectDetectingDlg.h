
// MaskDefectDetectingDlg.h : 头文件
//

#pragma once
#include <process.h>
#include <string>
#include "cv.h"
#include "highgui.h"
#include "CvvImage.h"
#include <ctime> 
#include "afxwin.h"
#include "MaskDefectProcessing.h"
#include "UseHOG.h"

// CMaskDefectDetectingDlg 对话框
class CMaskDefectDetectingDlg : public CDialogEx
{
// 构造
public:
	CMaskDefectDetectingDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MASKDEFECTDETECTING_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	
// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()


//My Code Begin
public:
	void DrawPicToHDC(IplImage *img, UINT ID);//把图片显示到控件
	void DrawPicToHDC(Mat img, UINT ID);//把图片显示到控件
	void DrawPicToHDC(Mat img, UINT ID, Mat &imgID);//把图片显示到控件，并保存副本
	void SavePicToWin(Mat img);//保存显示区图片
	void saveImgWithNoBand(Mat src, Mat srcOrigin);//保存分割的半边无棉筋口罩图像
	void saveImgWithBand(Mat src, Mat srcOrigin);//保存分割的半边有棉筋口罩图像
	int SVMpredict(Mat src, CvSVM &SVM);//单幅图像预测，使用Binary样本提取的LBP特征分类器
	int binaryHogPredict(Mat src, CvSVM &SVM);//单幅图像预测，使用Binary样本提取的HOG特征分类器
	int GetDir(CString path, CString savePath, int cat);//生成目录，并写入标记，保存成txt
	int GetDir(CString path, CString savePath);//生成目录，保存成txt
	int DeleteLastLine(string path);//path为txt路径，删除txt文件的最后一个/n


public:
	CMaskDefectProcessing imgProcess;//生成一个图像处理类实例，主要的图像预处理方法写在这个类
	CUseHOG useHog;////生成一个HoG训练预测类实例
	Mat m_src;//源图像
	Mat m_srcImg;//输入图像
	Mat m_nowImg;//输出图像
	Mat m_show2;//第2个图像显示框图像备份
	Mat m_show3;//第3个图像显示框图像备份
	Mat m_show4;//第4个图像显示框图像备份
	int m_checkUseGamma = 0;//checkbox选中状态标记
	int m_checkUseGrayimg = 0;//checkbox选中状态标记
	bool b_correction = false;//姿态校正标识符
	
	double start_time, end_time;
	bool openfiles = false;
	CListBox m_info;//状态栏
	string m_path;//读取图片路径

	Mat m_imgL;//单幅对半裁切后的左边
	Mat m_imgLGray;//单幅对半裁切后的左边
	Mat m_imgR;//单幅对半裁切镜像翻转后的右边
	Mat m_imgRGray;//单幅对半裁切切镜像翻转后的右边

	CString m_pathOpenBand;
	CString m_pathTrainBand;// 棉筋检测训练根目录
	string m_strPathTrainBand;//m_pathTrainBand 的 string 格式
	CString m_pathSaveBandPos;//有棉筋保存路径
	CString m_pathSaveBandNeg;//无棉筋保存路径
	CString m_pathSaveBandTestSet;//测试集保存路径
	CString m_pathSaveBandPosGray;//有棉筋原图归一化保存路径
	CString m_pathSaveBandNegGray;//无棉筋原图归一化保存路径
	CString m_pathSaveBandTestSetGray;//测试集原图归一化保存路径

//以下参数将在 .cpp 的 CDialogEx 函数中再次赋值或初始化，以该函数中的赋值数值为准。
	int CELLSIZE_LBP=8;// LBP的窗口大小，4,8，16
	int dstrows = 128;//对半分割前，缩放分辨率的高,等于用于提取特征的图像高度,需设为2^n
	int dstcols = 2 * dstrows;//对半分割前，缩放分辨率的宽，两倍于用于提取特征的图像宽度
	int TrainImgWidth = 64;
	int TrainImgHeight = TrainImgWidth;
	int num0 = 1;//用于口罩对半分割后，无棉筋按顺序保存
	int num1 = 1;//用于口罩对半分割后，有棉筋按顺序保存
	int numTestSet = 1;//用于测试图像对半分割后，按顺序保存

	//MFC自动生成的控件响应函数
	afx_msg void OnBnClickedButtonReadimg();//打开图片
	afx_msg void OnBnClickedButtonCorrection();//矫正图像
	afx_msg void OnBnClickedButtonBandprocess();
	void Bandprocess();//功能与上述函数一致，用于批处理
	afx_msg void OnBnClickedButtonHairprocess();
	afx_msg void OnEnChangePathOpenBand();


	afx_msg void OnBnClickedButtonBatchBand();
	afx_msg void OnBnClickedButtonBatchBand2();
	afx_msg void OnBnClickedButtonBatchBand3();
	afx_msg void OnBnClickedButtonBatchBand4();
	afx_msg void OnBnClickedButtonBatchBandtestset();
	afx_msg void OnBnClickedButtonReadimg2();
	afx_msg void OnBnClickedButtonJudge();
	afx_msg void OnBnClickedButtonSvm();
	afx_msg void OnBnClickedButtonSvmpredict();
	afx_msg void OnEnChangePathTrainBand();

	afx_msg void OnBnClickedButtonSvmLbpRotation();
	afx_msg void OnBnClickedButtonSvmLbpRotationPredict();
	afx_msg void OnBnClickedButtonTrainHogSvm();
	afx_msg void OnBnClickedButtonPredictHogSvm();
	afx_msg void OnBnClickedButtonPrehog();
	afx_msg void OnBnClickedButtonSaveshow2();
	afx_msg void OnBnClickedButtonSaveshow3();
	afx_msg void OnBnClickedButtonSaveshow4();
	afx_msg void OnBnClickedCheckUsegrayimg();
	afx_msg void OnBnClickedCheckUsegamma();
	afx_msg void OnBnClickedButtonCreatdir();

	afx_msg void OnEnChangeEditLbpcellsize();
	afx_msg void OnEnChangeEditHogimgsize();
	afx_msg void OnEnChangeEditDstrows();
	afx_msg void OnBnClickedButtonInfoClc();
	afx_msg void OnBnClickedButtonFusiontrain();
	afx_msg void OnBnClickedButtonFusionpredict();
};