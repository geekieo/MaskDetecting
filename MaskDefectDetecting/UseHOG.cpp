#include "stdafx.h"
#include "UseHOG.h"
#include <cv.h>
#include <highgui.h>

#include <fstream> //文件写入和读出



CUseHOG::CUseHOG()
{
}


CUseHOG::~CUseHOG()
{
}

int CUseHOG::computeGradient(Mat src, Mat& gradImage, Mat &gradOrient)
{
	//==========================================================梯度幅值的计算
	cv::Mat grad_x, grad_y;
	cv::Mat abs_grad_x, abs_grad_y;
	cv::Mat kerl_x = (Mat_<float>(1, 3) << -1, 0, 1);
	cv::filter2D(src, grad_x, CV_16S, kerl_x, Point(-1, -1), 0, BORDER_DEFAULT);
	cv::convertScaleAbs(grad_x, abs_grad_x);

	cv::Mat kerl_y = (Mat_<float>(3, 1) << -1, 0, 1);
	cv::filter2D(src, grad_y, CV_16S, kerl_y, Point(-1, -1), 0, BORDER_DEFAULT);
	cv::convertScaleAbs(grad_y, abs_grad_y);
	gradImage.create(cv::Size(grad_x.cols, grad_y.rows), CV_8U);

	uchar *totllGrad = gradImage.ptr<uchar>(0);
	uchar *_x_grad = abs_grad_x.ptr<uchar>(0);
	uchar *_y_grad = abs_grad_y.ptr<uchar>(0);
	for (int i = 0; i < grad_x.rows*grad_x.cols; i++)
	{
		totllGrad[i] = sqrt((double)_x_grad[i] * _x_grad[i] + (double)_y_grad[i] * _y_grad[i]);
	}
	//==========================================================梯度幅值的计算完成

	//==========================================================梯度方向的计算
	gradOrient.create(cv::Size(grad_x.cols, grad_y.rows), CV_8U);
	cv::divide(grad_y, grad_x, gradOrient, 1, -1);

	uchar *gradOrData = gradOrient.ptr<uchar>(0);
	uchar *xgrad = grad_x.ptr<uchar>(0);
	uchar *ygrad = grad_y.ptr<uchar>(0);

	for (int i = 0; i < grad_x.rows*grad_x.cols; i++)
	{
		double orient = atan2((double)ygrad[i], (double)xgrad[i]);                  ///梯度值没有负的，导致方向全是
		//double orient=atan(double(gradOrData[i]));
		gradOrData[i] = 180 * orient / CV_PI + 180;                    //////////////////////////////不知道是不是正确 都是0-90， 和0-270之间 
	}
	//==========================================================梯度方向的计算完成
	//cout<<gradOrient<<endl;
	return 1;
}

int CUseHOG::TrainHOGwithSVM(string trainSetPath, int imgWidth, int imgHight, string SVMSavePath)
{
	//int imgWidth = 120;
	//int imgHight = 120;
	vector<string> imagePaths;
	vector<int> imageClass;
	string buffer;
	int numberOfLine = 0;


	//ifstream svm_data("E:/apple/SVM_DATA.txt");
	ifstream svm_data(trainSetPath);
	unsigned long n;

	while (svm_data)
	{
		if (getline(svm_data, buffer))
		{
			numberOfLine++;
			if (numberOfLine % 2 == 0)//读到样本类别
			{
				imageClass.push_back(atoi(buffer.c_str()));
			}
			else//图片路径
			{
				imagePaths.push_back(buffer);
			}
		}
	}
	svm_data.close();//关闭文件    

	Mat data_mat, res_mat;
	int nImgNum = numberOfLine / 2;            //读入样本数量    
	////样本矩阵，nImgNum：横坐标是样本数量， WIDTH * HEIGHT：样本特征向量，即图像大小    
	//data_mat = Mat::zeros( nImgNum, 12996, CV_32FC1 );      
	//类型矩阵,存储每个样本的类型标志    
	res_mat = Mat::zeros(nImgNum, 1, CV_32FC1);

	//Mat trainImg = Mat::zeros(imgHight, imgWidth, CV_8UC3);//需要分析的图片    
	Mat trainImg;
	for (string::size_type i = 0; i != imagePaths.size(); i++)
	{
		trainImg = imread(imagePaths[i].c_str(), 0);
		//cout << " processing " << imagePaths[i].c_str() << endl;
		resize(trainImg, trainImg, cv::Size(imgWidth, imgHight), 0, 0, INTER_CUBIC);

		HOGDescriptor *hog = new HOGDescriptor(Size(imgWidth, imgHight), Size(16, 16), Size(8, 8), Size(8, 8), 9);//(检测窗口大小，块大小，滑动步长，单元格大小只支持8*8，直方图bin数只支持9)
		vector<float>descriptors;//结果数组       
		hog->compute(trainImg, descriptors, Size(1, 1), Size(0, 0)); //调用计算函数开始计算 (训练图像，结果，步长，填充)  
		if (i == 0)
		{
			data_mat = Mat::zeros(nImgNum, descriptors.size(), CV_32FC1); //根据输入图片大小进行分配空间   
		}
		//cout << "HOG dims: " << descriptors.size() << endl;
		n = 0;
		for (vector<float>::iterator iter = descriptors.begin(); iter != descriptors.end(); iter++)
		{
			data_mat.at<float>(i, n) = *iter;
			n++;
		}
		//cout<<SVMtrainMat->rows<<endl;    
		res_mat.at<float>(i, 0) = imageClass[i];
		//cout << " end processing " << imagePaths[i].c_str() << " " << imageClass[i] << endl;
	}

	CvSVM SVM;
	CvSVMParams param;
	CvTermCriteria criteria;
	criteria = cvTermCriteria(CV_TERMCRIT_EPS, 1000, FLT_EPSILON);
	param = CvSVMParams(CvSVM::C_SVC, CvSVM::RBF, 10.0, 0.09, 1.0, 10.0, 0.5, 1.0, NULL, criteria);

	/*
	SVM种类：CvSVM::C_SVC
	Kernel的种类：CvSVM::RBF
	degree：10.0（此次不使用）
	gamma：8.0
	coef0：1.0（此次不使用）
	C：10.0
	nu：0.5（此次不使用）
	p：0.1（此次不使用）
	然后对训练数据正规化处理，并放在CvMat型的数组里。
	*/
	//☆☆☆☆☆☆☆☆☆(5)SVM学习☆☆☆☆☆☆☆☆☆☆☆☆           
	SVM.train(data_mat, res_mat, Mat(), Mat(), param);
	//☆☆利用训练数据和确定的学习参数,进行SVM学习☆☆☆☆       
	SVM.save(SVMSavePath.data());
	return 1;
}

int CUseHOG::Predict(string testSetPath, string SVMLoadPath, int imgWidth, int imgHight, string PredictSavePath)
{
	//检测样本    
	vector<string> testImagePath;
	//ifstream testFile("E:/apple/SVM_TEST.txt");
	ifstream testFile(testSetPath);

	string buffer;
	unsigned long n;

	while (testFile)
	{
		if (getline(testFile, buffer))
		{
			testImagePath.push_back(buffer);
		}
	}
	testFile.close();

	Mat testImg;
	char line[512];
	//ofstream predict_txt("E:/apple/SVM_PREDICT.txt");
	ofstream predict_txt(PredictSavePath);
	CvSVM SVM;
	SVM.load(SVMLoadPath.data());//载入训练好的SVM分类器

	for (string::size_type j = 0; j != testImagePath.size(); j++)
	{
		testImg = imread(testImagePath[j].c_str(), 0);//读入图像     
		resize(testImg, testImg, cv::Size(imgWidth, imgHight), 0, 0, INTER_CUBIC);//要搞成同样的大小才可以检测到         
		HOGDescriptor *hog = new HOGDescriptor(cvSize(imgWidth, imgHight), cvSize(16, 16), cvSize(8, 8), cvSize(8, 8), 9);  //具体意思见参考文章1,2       
		vector<float>descriptors;//结果数组       
		hog->compute(testImg, descriptors, Size(1, 1), Size(0, 0)); //调用计算函数开始计算   
		//cout << "The Detection Result:" << endl;
		//cout << "HOG dims: " << descriptors.size() << endl;
		Mat SVMtrainMat = Mat::zeros(1, descriptors.size(), CV_32FC1);
		n = 0;
		for (vector<float>::iterator iter = descriptors.begin(); iter != descriptors.end(); iter++)
		{
			SVMtrainMat.at<float>(0, n) = *iter;
			n++;
		}
		int ret = SVM.predict(SVMtrainMat);
		std::sprintf(line, "%s %d\n", testImagePath[j].c_str(), ret);
		printf("%s %d\n", testImagePath[j].c_str(), ret);
		getchar();
		predict_txt << line;
	}
	predict_txt.close();

	return 1;
}
int CUseHOG::PredictSingleImage(Mat src, int imgWidth, int imgHight, CvSVM &SVM)
{
	Mat testImg;
	unsigned long n;
	resize(src, testImg, cv::Size(imgWidth, imgHight), 0, 0, INTER_CUBIC);//要搞成同样的大小才可以检测到         
	HOGDescriptor *hog = new HOGDescriptor(cvSize(imgWidth, imgHight), cvSize(16, 16), cvSize(8, 8), cvSize(8, 8), 9);
	vector<float>descriptors;//结果数组       
	hog->compute(testImg, descriptors, Size(1, 1), Size(0, 0)); //调用计算函数开始计算   
	//cout << "The Detection Result:" << endl;
	//cout << "HOG dims: " << descriptors.size() << endl;
	Mat SVMtrainMat = Mat::zeros(1, descriptors.size(), CV_32FC1);
	n = 0;
	for (vector<float>::iterator iter = descriptors.begin(); iter != descriptors.end(); iter++)
	{
		SVMtrainMat.at<float>(0, n) = *iter;
		n++;
	}
	int predict = SVM.predict(SVMtrainMat);

	return predict;
}