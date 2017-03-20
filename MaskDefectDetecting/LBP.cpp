#include "stdafx.h"
#include "LBP.h"


LBP::LBP()
{
}


LBP::~LBP()
{
}

//srcImage:灰度图
//LBPImage:LBP图
void LBP::LBP_256(const Mat &srcImage, Mat &LBPImage)
{
	// 参数检查，内存分配
	int heightOfLBP = srcImage.rows;
	int widthOfLBP = srcImage.cols;
	int depth = srcImage.depth();
	int channels = srcImage.channels();
	CV_Assert(depth == CV_8U && channels == 1);
	LBPImage.create(Size(widthOfLBP, heightOfLBP), CV_8UC1);

	// 计算LBP特征图
	// 扩充原图像边界，便于边界处理
	Mat extendedImage;
	copyMakeBorder(srcImage, extendedImage, 1, 1, 1, 1, BORDER_DEFAULT);

	// 计算LBP
	int heightOfExtendedImage = extendedImage.rows;
	int widthOfExtendedImage = extendedImage.cols;
	uchar *rowPointer_ToExtended = extendedImage.data + widthOfExtendedImage + 1;
	uchar *rowPointer_ToLBP = LBPImage.data;
	int LBPValue = 0;// 每个像素的LBP值
	for (int y = 1; y <= heightOfExtendedImage - 2; ++y)
	{
		// 列
		uchar *colPointer_ToExtended = rowPointer_ToExtended;
		uchar *colPointer_ToLBP = rowPointer_ToLBP;
		for (int x = 1; x <= widthOfExtendedImage - 2; ++x)
		{
			LBPValue = 0;
			if (colPointer_ToExtended[0 - widthOfExtendedImage - 1] >= colPointer_ToExtended[0])
				LBPValue += 128;
			if (colPointer_ToExtended[0 - widthOfExtendedImage] >= colPointer_ToExtended[0])
				LBPValue += 64;
			if (colPointer_ToExtended[0 - widthOfExtendedImage + 1] >= colPointer_ToExtended[0])
				LBPValue += 32;
			if (colPointer_ToExtended[0 + 1] >= colPointer_ToExtended[0])
				LBPValue += 16;
			if (colPointer_ToExtended[0 + widthOfExtendedImage + 1] >= colPointer_ToExtended[0])
				LBPValue += 8;
			if (colPointer_ToExtended[0 + widthOfExtendedImage] >= colPointer_ToExtended[0])
				LBPValue += 4;
			if (colPointer_ToExtended[0 + widthOfExtendedImage - 1] >= colPointer_ToExtended[0])
				LBPValue += 2;
			if (colPointer_ToExtended[0 - 1] >= colPointer_ToExtended[0])
				LBPValue += 1;

			// LBP图
			colPointer_ToLBP[0] = LBPValue;

			// 下一个像素
			colPointer_ToExtended++;
			colPointer_ToLBP++;

		}

		// 下一行
		rowPointer_ToExtended += widthOfExtendedImage;
		rowPointer_ToLBP += widthOfLBP;

	}// end of y


}

// 使用查找表，获取LBP特征图,注意,为了方便表示特征图，58种等价模式表示为1~58,第59种混合模式表示为0
void LBP::UniformLBP_LUT(const Mat &srcImage, Mat &LBPImage)
{
	// 参数检查，内存分配
	int heightOfLBP = srcImage.rows;
	int widthOfLBP = srcImage.cols;
	int depth = srcImage.depth();
	int channels = srcImage.channels();
	CV_Assert(depth == CV_8U && channels == 1);
	LBPImage.create(Size(widthOfLBP, heightOfLBP), CV_8UC1);

	// 计算LBP图
	// 扩充原图像边界，便于边界处理
	Mat extendedImage;
	copyMakeBorder(srcImage, extendedImage, 1, 1, 1, 1, BORDER_DEFAULT);

	// LUT
	static const int table[256] = { 1, 2, 3, 4, 5, 0, 6, 7, 8, 0, 0, 0, 9, 0, 10, 11, 12, 0, 0, 0, 0, 0, 0, 0, 13, 0, 0, 0, 14, 0, 15, 16, 17, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 18, 0, 0, 0, 0, 0, 0, 0, 19, 0, 0, 0, 20, 0, 21, 22, 23, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 25,
		0, 0, 0, 0, 0, 0, 0, 26, 0, 0, 0, 27, 0, 28, 29, 30, 31, 0, 32, 0, 0, 0, 33, 0, 0, 0, 0, 0, 0, 0, 34, 0, 0, 0, 0
		, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 35, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 36, 37, 38, 0, 39, 0, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 42
		, 43, 44, 0, 45, 0, 0, 0, 46, 0, 0, 0, 0, 0, 0, 0, 47, 48, 49, 0, 50, 0, 0, 0, 51, 52, 53, 0, 54, 55, 56, 57, 58 };

	// 计算LBP
	int heightOfExtendedImage = extendedImage.rows;
	int widthOfExtendedImage = extendedImage.cols;
	uchar *rowPointer_ToExtended = extendedImage.data + widthOfExtendedImage + 1;
	uchar *rowPointer_ToLBP = LBPImage.data;
	int LBPValue = 0;// 每个像素的LBP值
	for (int y = 1; y <= heightOfExtendedImage - 2; ++y)
	{
		// 列
		uchar *colPointer_ToExtended = rowPointer_ToExtended;
		uchar *colPointer_ToLBP = rowPointer_ToLBP;
		for (int x = 1; x <= widthOfExtendedImage - 2; ++x)
		{
			// 计算LBP
			LBPValue = 0;
			if (colPointer_ToExtended[0 - widthOfExtendedImage - 1] >= colPointer_ToExtended[0])
				LBPValue += 128;
			if (colPointer_ToExtended[0 - widthOfExtendedImage] >= colPointer_ToExtended[0])
				LBPValue += 64;
			if (colPointer_ToExtended[0 - widthOfExtendedImage + 1] >= colPointer_ToExtended[0])
				LBPValue += 32;
			if (colPointer_ToExtended[0 + 1] >= colPointer_ToExtended[0])
				LBPValue += 16;
			if (colPointer_ToExtended[0 + widthOfExtendedImage + 1] >= colPointer_ToExtended[0])
				LBPValue += 8;
			if (colPointer_ToExtended[0 + widthOfExtendedImage] >= colPointer_ToExtended[0])
				LBPValue += 4;
			if (colPointer_ToExtended[0 + widthOfExtendedImage - 1] >= colPointer_ToExtended[0])
				LBPValue += 2;
			if (colPointer_ToExtended[0 - 1] >= colPointer_ToExtended[0])
				LBPValue += 1;

			colPointer_ToLBP[0] = table[LBPValue];

			// 下一个像素
			colPointer_ToExtended++;
			colPointer_ToLBP++;

		}

		// 下一行
		rowPointer_ToExtended += widthOfExtendedImage;
		rowPointer_ToLBP += widthOfLBP;

	}// end of y
}

//获取i中0,1的跳变次数
int LBP::GetHopCount(int i)
{
	// 转换为二进制
	int a[8] = { 0 };
	int k = 7;
	while (i)
	{
		// 除2取余
		a[k] = i % 2;
		i /= 2;
		--k;
	}

	// 计算跳变次数
	int count = 0;
	for (int k = 0; k<8; ++k)
	{
		// 注意，是循环二进制
		if (a[k] != a[k + 1 == 8 ? 0 : k + 1])
		{
			++count;
		}
	}
	return count;

}

// 建立等价模式表
// 这里为了便于建立LBP特征图，58种等价模式序号从1开始:1~58,第59类混合模式映射为0
void LBP::BuildUniformPatternTable(int *table)
{
	if (table == NULL)
	{
		return;
	}
	memset(table, 0, 256 * sizeof(int));
	uchar temp = 1;
	for (int i = 0; i<256; ++i)
	{
		if (GetHopCount(i) <= 2)
		{
			table[i] = temp;
			temp++;
		}
	}

	// 输出表格
	//for (int i = 0; i < 256;++i)
	//printf("%d,",table[i]);

}

// 获取一幅图像LBP特征
// cellSize:每个cell的大小,如16*16
void LBP::ComputerLBPFeatureVector(const Mat &srcImage, Size cellSize, vector<float> &featureVector)
{
	// 设置每个窗口大小
	int widthOfCell = cellSize.width;
	int heightOfCell = cellSize.height;
	int numberOfCell_X = srcImage.cols / widthOfCell;// X方向cell的个数
	int numberOfCell_Y = srcImage.rows / heightOfCell;

	// 特征向量的个数
	int numberOfDimension = 58 * numberOfCell_X*numberOfCell_Y;
	featureVector.resize(numberOfDimension, 0);

	//每个cell
	Mat ROI, cell, LBPImage;
	IplImage iplImage;

	// 计算LBP特征向量
	for (int y = 0; y <= numberOfCell_Y - 1; ++y)
	{
		for (int x = 0; x <= numberOfCell_X - 1; ++x)
		{
			// 每个cell
			ROI = srcImage(Rect(x * widthOfCell, y * heightOfCell, widthOfCell, heightOfCell));
			// 拷贝每个cell
			iplImage = ROI;
			cell = cvarrToMat(&iplImage, true);
			// 计算
			UniformLBP(cell, LBPImage, featureVector, (y*numberOfCell_X + x));
		}
	}
}

// 计算每个cell的特征图和特征向量
// LBPImage:LBP特征图(这里为了便于建立LBP特征图，58种等价模式序号从1开始:1~58,第59类混合模式映射为0)
// LBPFeature：每幅图的LBP特征
// indexOfCell:cell索引
// 注：你可以将第59类混合模式映射为任意数值，但是设置为0能够更好突出特征，因为要突出等价模式特征，所以非等价模式设置为0比较好
void LBP::UniformLBP(const Mat &srcImage, Mat &LBPImage, vector<float> &LBPFeatureVector, int indexOfCell)
{
	// 参数检查，内存分配
	int heightOfLBP = srcImage.rows;
	int widthOfLBP = srcImage.cols;
	int depth = srcImage.depth();
	int channels = srcImage.channels();
	CV_Assert(depth == CV_8U && channels == 1);
	LBPImage.create(Size(widthOfLBP, heightOfLBP), CV_8UC1);

	// 扩充原图像边界，便于边界处理
	Mat extendedImage;
	copyMakeBorder(srcImage, extendedImage, 1, 1, 1, 1, BORDER_DEFAULT);

	// 构建LBP 等价模式查找表
	int table[256];
	BuildUniformPatternTable(table);

	// 计算LBP特征图
	int heightOfExtendedImage = extendedImage.rows;
	int widthOfExtendedImage = extendedImage.cols;
	uchar *rowPointer_ToExtended = extendedImage.data + widthOfExtendedImage + 1;
	uchar *rowPointer_ToLBP = LBPImage.data;
	int LBPValue = 0;// 每个像素的LBP值
	for (int y = 1; y <= heightOfExtendedImage - 2; ++y)
	{
		// 列
		uchar *colPointer_ToExtended = rowPointer_ToExtended;
		uchar *colPointer_ToLBP = rowPointer_ToLBP;
		for (int x = 1; x <= widthOfExtendedImage - 2; ++x)
		{
			// 计算一般的256维LBP值
			LBPValue = 0;
			if (colPointer_ToExtended[0 - widthOfExtendedImage - 1] >= colPointer_ToExtended[0])
				LBPValue += 128;
			if (colPointer_ToExtended[0 - widthOfExtendedImage] >= colPointer_ToExtended[0])
				LBPValue += 64;
			if (colPointer_ToExtended[0 - widthOfExtendedImage + 1] >= colPointer_ToExtended[0])
				LBPValue += 32;
			if (colPointer_ToExtended[0 + 1] >= colPointer_ToExtended[0])
				LBPValue += 16;
			if (colPointer_ToExtended[0 + widthOfExtendedImage + 1] >= colPointer_ToExtended[0])
				LBPValue += 8;
			if (colPointer_ToExtended[0 + widthOfExtendedImage] >= colPointer_ToExtended[0])
				LBPValue += 4;
			if (colPointer_ToExtended[0 + widthOfExtendedImage - 1] >= colPointer_ToExtended[0])
				LBPValue += 2;
			if (colPointer_ToExtended[0 - 1] >= colPointer_ToExtended[0])
				LBPValue += 1;

			// 计算Uniform模式(等价模式)LBP值
			colPointer_ToLBP[0] = table[LBPValue];

			// 下一个像素
			++colPointer_ToExtended;
			++colPointer_ToLBP;

		}// end of x

		// 下一行
		rowPointer_ToExtended += widthOfExtendedImage;
		rowPointer_ToLBP += widthOfLBP;

	}// end of y

	// 计算cell的LBP特征
	uchar *imageDataOfLBP = LBPImage.data;
	int numberOfPixel = LBPImage.rows*LBPImage.cols;
	int index = 58 * indexOfCell;// 该cell的特征向量起始位置
	int sum = 0;
	for (int i = 0; i <= numberOfPixel - 1; ++i)
	{
		// 只统计等价模式
		if (imageDataOfLBP[i] != 0)
		{
			// 等价模式转化为0~57，所以是imageDataOfLBP[i] - 1
			++LBPFeatureVector[index + imageDataOfLBP[i] - 1];
			++sum;
		}
	}

	// 直方图归一化
	for (int i = 0; i <= 57; ++i)
	{
		LBPFeatureVector[index + i] /= sum;
	}
}

// 计算9种等价模式
int LBP::ComputerValue9(int value58)
{
	int value9 = 0;
	switch (value58)
	{
	case 1:
		value9 = 1;
		break;
	case 2:
		value9 = 2;
		break;
	case 4:
		value9 = 3;
		break;
	case 7:
		value9 = 4;
		break;
	case 11:
		value9 = 5;
		break;
	case 16:
		value9 = 6;
		break;
	case 22:
		value9 = 7;
		break;
	case 29:
		value9 = 8;
		break;
	case 58:
		value9 = 9;
		break;
	}

	return value9;

}

// 获取循环二进制的最小二进制模式
uchar LBP::GetMinBinary(uchar *binary)
{
	// 计算8个二进制
	uchar LBPValue[8] = { 0 };
	for (int i = 0; i <= 7; ++i)
	{
		LBPValue[0] += binary[i] << (7 - i);
		LBPValue[1] += binary[(i + 7) % 8] << (7 - i);
		LBPValue[2] += binary[(i + 6) % 8] << (7 - i);
		LBPValue[3] += binary[(i + 5) % 8] << (7 - i);
		LBPValue[4] += binary[(i + 4) % 8] << (7 - i);
		LBPValue[5] += binary[(i + 3) % 8] << (7 - i);
		LBPValue[6] += binary[(i + 2) % 8] << (7 - i);
		LBPValue[7] += binary[(i + 1) % 8] << (7 - i);
	}

	// 选择最小的
	uchar minValue = LBPValue[0];
	for (int i = 1; i <= 7; ++i)
	{
		if (LBPValue[i] < minValue)
		{
			minValue = LBPValue[i];
		}
	}

	return minValue;

}

// cellSize:每个cell的大小,如16*16
void LBP::ComputerLBPFeatureVector_Rotation(const Mat &srcImage, Size cellSize, vector<float> &featureVector)
{
	// 设置每个窗口大小
	int widthOfCell = cellSize.width;
	int heightOfCell = cellSize.height;
	int numberOfCell_X = srcImage.cols / widthOfCell;// X方向cell的个数
	int numberOfCell_Y = srcImage.rows / heightOfCell;

	// 特征向量的个数
	int numberOfDimension = 9 * numberOfCell_X*numberOfCell_Y;
	featureVector.resize(numberOfDimension, 0);

	//每个cell
	Mat ROI, cell, LBPImage;
	IplImage iplImage;

	// 计算LBP特征向量
	for (int y = 0; y <= numberOfCell_Y - 1; ++y)
	{
		for (int x = 0; x <= numberOfCell_X - 1; ++x)
		{
			// 每个cell
			ROI = srcImage(Rect(x * widthOfCell, y * heightOfCell, widthOfCell, heightOfCell));

			// 拷贝每个cell
			iplImage = ROI;
			cell = cvarrToMat(&iplImage, true);

			// 计算
			RotationUniformLBP(cell, LBPImage, featureVector, (y*numberOfCell_X + x));
		}
	}
}

// 计算旋转不变等价模式的LBP值
void LBP::RotationUniformLBP(const Mat &srcImage, Mat &LBPImage, vector<float> &LBPFeatureVector, int indexOfCell)
{
	// 参数检查，内存分配
	CV_Assert(srcImage.depth() == CV_8U&&srcImage.channels() == 1);
	LBPImage.create(srcImage.size(), srcImage.depth());

	// 扩充图像，处理边界情况
	Mat extendedImage;
	copyMakeBorder(srcImage, extendedImage, 1, 1, 1, 1, BORDER_DEFAULT);

	// 构建LBP 等价模式查找表
	int table[256];
	BuildUniformPatternTable(table);

	uchar binary[8] = { 0 };// 记录每个像素的LBP值
	int heigthOfExtendedImage = extendedImage.rows;
	int widthOfExtendedImage = extendedImage.cols;
	int widthOfLBPImage = LBPImage.cols;
	// 行
	uchar *rowPointer_ToExtendedImage = extendedImage.data + widthOfExtendedImage + 1;
	uchar *rowPointer_ToLBPImage = LBPImage.data;
	for (int y = 1; y <= heigthOfExtendedImage - 2; ++y)
	{
		// 列
		uchar *colPointer_ToExtendedImage = rowPointer_ToExtendedImage;
		uchar *colPointer_ToLBPImage = rowPointer_ToLBPImage;
		for (int x = 1; x <= widthOfExtendedImage - 2; ++x)
		{
			// 计算旋转不变LBP(最小的二进制模式)
			binary[0] = colPointer_ToExtendedImage[0 - widthOfExtendedImage - 1] >= colPointer_ToExtendedImage[0] ? 1 : 0;
			binary[1] = colPointer_ToExtendedImage[0 - widthOfExtendedImage] >= colPointer_ToExtendedImage[0] ? 1 : 0;
			binary[2] = colPointer_ToExtendedImage[0 - widthOfExtendedImage + 1] >= colPointer_ToExtendedImage[0] ? 1 : 0;
			binary[3] = colPointer_ToExtendedImage[0 + 1] >= colPointer_ToExtendedImage[0] ? 1 : 0;
			binary[4] = colPointer_ToExtendedImage[0 + widthOfExtendedImage + 1] >= colPointer_ToExtendedImage[0] ? 1 : 0;
			binary[5] = colPointer_ToExtendedImage[0 + widthOfExtendedImage] >= colPointer_ToExtendedImage[0] ? 1 : 0;
			binary[6] = colPointer_ToExtendedImage[0 + widthOfExtendedImage - 1] >= colPointer_ToExtendedImage[0] ? 1 : 0;
			binary[7] = colPointer_ToExtendedImage[0 - 1] >= colPointer_ToExtendedImage[0] ? 1 : 0;
			int minValue = GetMinBinary(binary);

			// 计算58种等价模式LBP
			int value58 = table[minValue];

			// 计算9种等价模式
			colPointer_ToLBPImage[0] = ComputerValue9(value58);

			// 下一个像素
			++colPointer_ToExtendedImage;
			++colPointer_ToLBPImage;
		}

		// 下一行
		rowPointer_ToExtendedImage += widthOfExtendedImage;
		rowPointer_ToLBPImage += widthOfLBPImage;
	}

	// 计算cell的LBP特征
	uchar *imageDataOfLBP = LBPImage.data;
	int numberOfPixel = LBPImage.rows*LBPImage.cols;
	int index = 9 * indexOfCell;// 该cell的特征向量起始位置
	int sum = 0;
	for (int i = 0; i <= numberOfPixel - 1; ++i)
	{
		// 只统计等价模式
		if (imageDataOfLBP[i] != 0)
		{
			// 等价模式转化为0~8，所以是imageDataOfLBP[i] - 1
			++LBPFeatureVector[index + imageDataOfLBP[i] - 1];
			++sum;
		}
	}

	// 直方图归一化
	for (int i = 0; i <= 8; ++i)
	{
		LBPFeatureVector[index + i] /= sum;
	}
}

// 验证灰度不变+旋转不变+等价模式种类
void LBP::Test()
{
	uchar LBPValue[8] = { 0 };
	int k = 7, j;
	int temp;
	LBP lbp;
	int number[256] = { 0 };
	int numberOfMinBinary = 0;

	// 旋转不变
	for (int i = 0; i < 256; ++i)
	{
		k = 7;
		temp = i;
		while (k >= 0)
		{
			LBPValue[k] = temp & 1;
			temp = temp >> 1;
			--k;
		}
		int minBinary = lbp.GetMinBinary(LBPValue);

		// 查找有无重复的
		for (j = 0; j <= numberOfMinBinary - 1; ++j)
		{
			if (number[j] == minBinary)
				break;
		}
		if (j == numberOfMinBinary)
		{
			number[numberOfMinBinary++] = minBinary;
		}
	}
	cout << "旋转不变一共有：" << numberOfMinBinary << "种" << endl;

	// LUT
	static const int table[256] = { 1, 2, 3, 4, 5, 0, 6, 7, 8, 0, 0, 0, 9, 0, 10, 11, 12, 0, 0, 0, 0, 0, 0, 0, 13, 0, 0, 0, 14, 0, 15, 16, 17, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 18, 0, 0, 0, 0, 0, 0, 0, 19, 0, 0, 0, 20, 0, 21, 22, 23, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 25,
		0, 0, 0, 0, 0, 0, 0, 26, 0, 0, 0, 27, 0, 28, 29, 30, 31, 0, 32, 0, 0, 0, 33, 0, 0, 0, 0, 0, 0, 0, 34, 0, 0, 0, 0
		, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 35, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 36, 37, 38, 0, 39, 0, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 42
		, 43, 44, 0, 45, 0, 0, 0, 46, 0, 0, 0, 0, 0, 0, 0, 47, 48, 49, 0, 50, 0, 0, 0, 51, 52, 53, 0, 54, 55, 56, 57, 58 };
	cout << "旋转不变+等价模式：" << endl;
	for (int i = 0; i <= numberOfMinBinary - 1; ++i)
	{
		cout << number[i] << " " << table[number[i]] << endl;
	}

}