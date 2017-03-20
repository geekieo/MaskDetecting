#pragma once
#ifndef __LBP_H__
#define __LBP_H__
#include "opencv2/opencv.hpp"
#include<vector>
using namespace std;
using namespace cv;

class LBP
{
public:
	LBP();
	~LBP();

	// 灰度不变LBP(256种模式)
	void LBP_256(const Mat &srcImage, Mat &LBPImage);// 计算256维LBP特征图


	// 灰度不变+等价模式LBP(58种模式),没有考虑旋转不变
	void ComputerLBPFeatureVector(const Mat &srcImage, Size cellSize, vector<float> &featureVector);// 计算LBP特征
	void UniformLBP(const Mat &srcImage, Mat &LBPImage, vector<float> &LBPFeatureVector, int indexOfCell); // 计算每个cell的特征图和特征向量

	void BuildUniformPatternTable(int *table);
	int GetHopCount(int i);// 获取i中0,1的跳变次数
	void UniformLBP_LUT(const Mat &srcImage, Mat &LBPImage);// 使用查找表，获取LBP特征图

	// 灰度不变+旋转不变+等价模式LBP(9种模式)
	void ComputerLBPFeatureVector_Rotation(const Mat &srcImage, Size cellSize, vector<float> &featureVector);// 计算LBP特征
	void RotationUniformLBP(const Mat &srcImage, Mat &LBPImage, vector<float> &LBPFeatureVector, int indexOfCell);// 计算特征图
	int ComputerValue9(int value58);// 计算9种等价模式
	uchar GetMinBinary(uchar *binary);

	void Test();// 测试灰度不变+旋转不变+等价模式LBP
};

#endif