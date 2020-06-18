#include <lineDetector.h>

//#define _USE_MATH_DEFINES


using namespace cv;
using namespace std;

void lineDetectionTest() {

	// SetUp 
	
	//LineDetector::PrintFullPath(".\\");
	//LineDetector::func();

	LineDetector ld;
	
	Mat img = imread("../data/sample_car.jpg");
	/*
	double img_width = img.cols;  //obtain size of image and halve it in order to fit 
	double img_height = img.rows;
	int maxHeight = int(ld.configParams.edgeParams.screenHeight / 2);
	int maxWidth = int(ld.configParams.edgeParams.screenWidth / 2);

	int newCols;
	int newRows;
	int diff_height = maxHeight - img_height;
	int diff_width = maxWidth - img_width;

	if (diff_height < diff_width) {
		newCols = 0.5 * maxHeight;
		newRows = img.rows * newCols / img.cols;
	}
	else {
		newRows = 0.5 * maxWidth;
		newCols = img.cols * newRows / img.rows;
	}

	ld.configParams.edgeParams.newCols = newCols;
	ld.configParams.edgeParams.newRows = newRows;
	*/
	int maxHeight = int(ld.configParams.edgeParams.screenHeight / 2);
	int maxWidth = int(ld.configParams.edgeParams.screenWidth / 3);


	int new_cols = int(float(maxHeight) / float(img.rows) * img.cols);
	int new_rows = int(float(maxWidth) / float(img.cols) * img.rows);

	if (new_cols > maxWidth) {
		new_cols = maxWidth;
	}

	else {
		new_rows = maxHeight;
	}

	cv::Mat m;
	const std::string winName = "Background";
	cv::namedWindow(winName, cv::WINDOW_AUTOSIZE);
	m.create(int(ld.configParams.edgeParams.screenHeight*0.95), ld.configParams.edgeParams.screenWidth, CV_32FC3);
	m.setTo(cv::Scalar(255, 255, 255));
	moveWindow(winName, 0, 0);
	cv::imshow(winName, m);
	
	//cv::namedWindow(winName, cv::WINDOW_AUTOSIZE);

	ld.configParams.edgeParams.newCols = new_cols;
	ld.configParams.edgeParams.newRows = new_rows;

	ld.configParams.edgeParams.origImg = img.clone();
	ld.edgeParametersP();
	waitKey();
	LineDetector::displayImg(ld.configParams.edgeParams.cannyImg, "FINISHED", ld.configParams.edgeParams.screenWidth, ld.configParams.edgeParams.screenHeight, -1);
	cout << "here" << endl;
	cout << "Canny settings: " << ld.configParams.edgeParams.highThresh << endl;

	waitKey();
	
	
	
}
