#pragma once
#include <cv.h>
#include <highgui.h>

using namespace cv;

class CMaskDefectProcessing
{
public:
	CMaskDefectProcessing();
	~CMaskDefectProcessing();

public:
	//int ThresholdOtsu(IplImage * src);// OTSU最大类间方差自适应阀值分割(灰度图),返回阈值
	///*===============================================*/
	//int BlockDetect(Mat src, int width, int height, int &x, int &y);//判断滑块图像是否是角。
	//int TotalData(Mat src);//统计灰度值
	//int BlockSearchCorner(Mat src);//滑块寻角（二值图），得到m_fourCorner[]并矫正归一化
	////=========
	//Point2f computeIntersect(Vec4i a, Vec4i b);//计算间隔
	//void sortCorners(std::vector<Point2f>& corners, Point2f center);//角点排序
	//int Transformation(Mat src);// 矫正变换
	////=========
	//bool LineDetect(Mat line,int coordinate);//线判断，返回角点坐标。
	//int LineScanCorrect(Mat src);//线扫描矫正归一化
	////=========
	int SkewCorrection(Mat src);//倾斜矫正




	/*===============================================*/
public:
	Mat m_NowImage;//show1当前显示图片
	//std::vector<Mat> pImageHistory;//图片数组
	//int m_HistoryNum;//初始化图片数组数
	//Point2f m_Corner[4];//四个角的坐标
	//vector<Point2f> m_corners;////四个角的坐标
	Point2f m_center;//中心点坐标

	//float carCos[181];
	//float carSin[181];


	RotatedRect m_maskrect;//口罩最小外接矩形

};

