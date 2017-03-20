//使用SVM训练HoG
#pragma once
#include <iostream>//输入输出流
#include "highgui.h"
#include <ml.h>//opencv机器学习头文件，包含SVM
using namespace std;
using namespace cv;
#pragma once
class CUseHOG
{
public:
	CUseHOG();
	~CUseHOG(); 
	int computeGradient(cv::Mat src, cv::Mat &gradImage, cv::Mat &gradOrient);//计算图像梯度
	int TrainHOGwithSVM(string trainSetPath, int imgWidth ,int imgHight, string SVMSavePath);//输入训练集目录、训练集样本长宽，提取HOG特征并训练SVM分类器
	int Predict(string testSetPath, string SVMLoadPath, int imgWidth, int imgHight, string PredictSavePath);//输入测试集目录、分类器目录、训练集样本长宽、预测结果输出路径，进行批量预测
	int PredictSingleImage(cv::Mat src, int imgWidth, int imgHight, CvSVM &SVM);//输入图像样本、样本长宽、SVM分类器，进行单幅图像预测
};

