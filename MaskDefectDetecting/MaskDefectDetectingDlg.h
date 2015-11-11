
// MaskDefectDetectingDlg.h : 头文件
//

#pragma once
#include <process.h>
#include <string>
#include "cv.h"
#include "highgui.h"
#include "CvvImage.h"
#include "MaskDefectProcessing.h"
#include <ctime> 

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
public:	
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
//My Code Begin
public:
	void DrawPicToHDC(IplImage *img, UINT ID);//把图片显示到控件
	void DrawPicToHDC(Mat img, UINT ID);//把图片显示到控件

public:
	CMaskDefectProcessing imgProcessing;
	IplImage *pNowImage=NULL;//当前图片，一般为结果图像
	double start_time, end_time;

	afx_msg void OnBnClickedButtonReadimg();//打开图片
	bool openfiles = false;
	std::string path;//图片路径
	IplImage * pSrcImg=NULL;//读取图像

	afx_msg void OnBnClickedButtonCorrection();//矫正图像

};

