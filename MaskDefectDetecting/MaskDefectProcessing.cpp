#include "stdafx.h"
#include "MaskDefectProcessing.h"

#define max(a, b)  (((a) > (b)) ? (a) : (b));

using namespace std;

CMaskDefectProcessing::CMaskDefectProcessing()
{
}

CMaskDefectProcessing::~CMaskDefectProcessing()
{
}

//从原图得到m_maskrect旋转矩阵，可用于后续预处理图像的矫正。
int CMaskDefectProcessing::GetMaskRect(Mat src)
{
	Mat   src_bin, src_dilate, src_erode;
	//cvThreshold(g_pGrayImage, g_pBinaryImage, 0, 255, CV_THRESH_OTSU);//二值化
	threshold(src, src_bin, 0, 255, CV_THRESH_OTSU);//二值化，自适应阈值分割，
	blur(src_bin, src_bin, Size(3, 3));//模糊
	Mat element = getStructuringElement(MORPH_RECT, Size(15, 15));//腐蚀膨胀算子，15*15的矩形
	dilate(src_bin, src_dilate, element);//膨胀
	erode(src_dilate, src_erode, element);//腐蚀
	//imshow("src_erode", src_erode);
	//waitKey(0);

	vector<vector<Point> > contours;//轮廓
	cv::findContours(src_erode,
		contours, // a vector of contours 
		CV_RETR_EXTERNAL, // retrieve the external contours
		CV_CHAIN_APPROX_NONE); // retrieve all pixels of each contours 此函数为配合下面的 minAreaRect() 使用

	int area = 0;
	int maxArea=0;//最小外接矩形面积
	for (int i = 0; i < contours.size(); i++) // 核实最小外接矩形尺寸，只用面积最大的外接矩形
	{
		RotatedRect minRect = minAreaRect(Mat(contours[i]));//★计算最小面积的外接矩形，关键步骤
		area = (int)minRect.size.height *(int)minRect.size.width;
		if (area > maxArea)
		{
			maxArea = area;
			m_maskrect = minRect;
		}
	}
	////绘制最小外接矩形示意图
	//Mat src_erode_edge;
	//Canny(src_erode, src_erode_edge,4,8);
	//imshow("canny", src_erode_edge);
	//imwrite("canny_4_8.jpg", src_erode_edge);
	//drawContours(src_erode, contours, -1, Scalar(255, 0, 0), 1);
	//Point2f vertices[4];
	//m_maskrect.points(vertices);
	//for (int i = 0; i < 4; i++)
	//{
	//	line(src_erode_edge, vertices[i], vertices[(i + 1) % 4], Scalar(255, 0, 0), 2);
	//}
	//imshow("contours", src_erode_edge);
	//imwrite("minAreaRect.jpg", src_erode_edge);
	//waitKey(0);



	return 0;
}

//函数功能：倾斜矫正并归一化尺寸
//输入：src：8位单通道图像；maskrect：口罩最小外接矩形；Size()：归一化矩形尺寸。
//输出：Mat dst:切割旋转矫正的归一化口罩图像。
int CMaskDefectProcessing::Normalization(Mat src, RotatedRect maskrect, int rows, int cols,Mat &dst)
{
	//将偏斜口罩回正
	float r = (float)m_maskrect.size.width / (float)m_maskrect.size.height;//宽高比值
	if (r < 1)//宽比高短，交换宽高，偏转角加90度。
	{
		m_maskrect.angle = 90 + m_maskrect.angle;
		std::swap(m_maskrect.size.width, m_maskrect.size.height);

	}
	Size rect_size = m_maskrect.size;
	Mat rotmat;//旋转矩阵
	rotmat = getRotationMatrix2D(m_maskrect.center, m_maskrect.angle, 1);
	Mat img_rotated;//旋转后图像
	warpAffine(src, img_rotated, rotmat, src.size(), CV_INTER_CUBIC);//旋转矫正，输入为bw0
	//imshow("img_rotated", img_rotated);
	//waitKey(0);

	//将矩形内口罩 resize 归一化到统一尺寸的 mat 中，目的是获得变换矩阵，变换可复用于原图的其他预处理结果。
	Mat img_crop;//分割结果
	getRectSubPix(img_rotated, rect_size, m_maskrect.center, img_crop);//分割提取
	//imshow("img_crop", img_crop);
	//waitKey(0);

	Mat resultResized;//最后归一化的Mat
	resultResized.create(rows, cols, CV_8UC1);//修改此Mat，可以得到不同的归一化大小，对应的Resize矩阵
	resize(img_crop, resultResized, resultResized.size(), 0, 0, INTER_CUBIC);//缩放到统一大小
	//imshow("resultResized", resultResized);
	//waitKey(0);

	dst = resultResized;

	return 0;
}

int CMaskDefectProcessing::PreprocessBand(Mat src, Mat dst)//棉筋预处理
{
	return 0;
}

//头发预处理
int CMaskDefectProcessing::PreprocessHair(Mat src,Mat &dst)
{
	int thresh = 110;
	int max_thresh = 255;
	RNG rng(12345);//随机数生成器
	Mat nowImg = src.clone();
	IplImage *pNowImg = (&(IplImage)nowImg);//Mat → Iplimage*，共享数据

	cvAdaptiveThreshold(pNowImg, pNowImg, 255,
		CV_ADAPTIVE_THRESH_GAUSSIAN_C,
		CV_THRESH_BINARY_INV, 3, 5);//自适应阈值法,阈值的象素邻域大小3,窗口尺寸5,发丝图像增强关键步骤
	Mat src_adaptive(pNowImg, 1);//Iplimage* → Mat，共享数据
	Mat element = getStructuringElement(MORPH_ELLIPSE, Size(4, 4));//腐蚀膨胀算子，15*15的矩形
	dilate(src_adaptive, src_adaptive, element);//膨胀.
	//blur(src_adaptive, src_adaptive, Size(3,3));//模糊

	////归一化
	//Normalization(src_adaptive, m_maskrect, m_normalrows, m_normalcols, src_adaptive);
	//imshow("src_adaptive", src_adaptive);

	////椭圆拟合开始
	//Mat threshold_output;
	//vector<vector<Point> > contours;
	//vector<Vec4i> hierarchy;
	///// 阈值化检测边界
	//threshold(src_adaptive, threshold_output, thresh, 255, THRESH_BINARY);
	///// 寻找轮廓
	//findContours(threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	///// 对每个找到的轮廓创建可倾斜的边界框和椭圆
	//vector<RotatedRect> minRect(contours.size());
	//vector<RotatedRect> minEllipse(contours.size());
	//for (int i = 0; i < contours.size(); i++)
	//{
	//	minRect[i] = minAreaRect(Mat(contours[i]));
	//	if (contours[i].size() > 5)
	//	{
	//		minEllipse[i] = fitEllipse(Mat(contours[i]));
	//	}
	//}
	//// 绘出轮廓及其可倾斜的边界框和边界椭圆
	//Mat drawing = Mat::zeros(threshold_output.size(), CV_8UC3);
	//for (int i = 0; i< contours.size(); i++)
	//{	//生成随机色
	//	Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
	//	// contour
	//	drawContours(drawing, contours, i, color, 1, 8, vector<Vec4i>(), 0, Point());
	//	// ellipse
	//	ellipse(drawing, minEllipse[i], color, 2, 8);
	//	// rotated rectangle
	//	Point2f rect_points[4]; minRect[i].points(rect_points);
	//	for (int j = 0; j < 4; j++)
	//		line(drawing, rect_points[j], rect_points[(j + 1) % 4], color, 1, 8);
	//}

	//dst = drawing;
	Mat tempImg;
	src.copyTo(tempImg, src_adaptive);//将边缘检测出的图像作为mask，扣出原图图像
	dst = tempImg;
	imshow("dst", dst);
	//特征提取

	return 0;
}

// 图像左右镜像变换
Mat CMaskDefectProcessing::MirrorTrans(Mat src)
{
	Mat dst;
	dst.create(src.rows, src.cols, src.type());

	int cols = src.cols;

	for (int i = 0; i < cols; i++)
		src.col(cols - i - 1).copyTo(dst.col(i));
	return dst;
}

//图像平均切成两半，右边向左翻转
int CMaskDefectProcessing::Cut2half(Mat src, Mat &imgL, Mat &imgR)
{
	int width = src.size().width;
	int height = src.size().height;
	Range R1, R2;
	R1.start = 0;
	R1.end = width / 2;
	imgL = Mat::Mat(src, Range::all(), R1);
	R2.start = width / 2;
	R2.end = width;
	imgR = Mat::Mat(src, Range::all(), R2);
	//imgR镜像变换
	imgR = MirrorTrans(imgR);
	return 0;
}

int GammaProcess(Mat src, Mat &dst)//Gamma校正
{
	Mat gammaOutput, gammaInput;
	src.convertTo(gammaInput, CV_32FC1);
	float gamma = 1 / 2.2;
	pow(gammaInput, gamma, gammaOutput);
	normalize(gammaOutput, dst, 0, 255, NORM_MINMAX, CV_8UC1);
	return 0;
}