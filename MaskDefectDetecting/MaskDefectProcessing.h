#pragma once
#include <cv.h>
#include <highgui.h>

using namespace cv;

class CMaskDefectProcessing
{
public:
	CMaskDefectProcessing();
	~CMaskDefectProcessing();

	int GetMaskRect(Mat src);//输入单通道src，输出m_maskrect旋转矩阵，用于后续矫正。
	int Normalization(Mat src, RotatedRect maskrect, int rows, int cols,Mat &dst);//倾斜矫正+尺度归一化(8位单通道图像，m_maskrect)
	int PreprocessBand(Mat src, Mat dst);//棉筋预处理
	int PreprocessHair(Mat src, Mat &dst);//头发预处理	
	Mat MirrorTrans(Mat src);// 图像镜像变换
	int Cut2half(Mat src, Mat &imgL, Mat &imgR);//图像平均切成两半，右边向左翻转
	int GammaProcess(Mat src, Mat &dst);//Gamma校正

public:
	Mat m_NowImage;//当前结果图  
	int m_normalrows = 138;//归一化图像大小， rows:cols = 11:20
	int m_normalcols = 250;//归一化图像大小，rows:cols = 11:20
	RotatedRect m_maskrect;//口罩最小外接矩形，用于口罩所有处理图像的归一化。
};

