#include <lineDetector.h>

//#define _USE_MATH_DEFINES


using namespace cv;
using namespace std;

void lineDetectionTest() {

	// SetUp 
	
	//LineDetector::PrintFullPath(".\\");
	//LineDetector::func();
	cout << "Hi" << endl;
	LineDetector ld;
	
	Mat image = imread("../data/sample_car.jpg");
	
	ld.configParams.edgeParams.origImg = image.clone();
	ld.edgeParametersP();
	
	LineDetector::displayImg(ld.configParams.edgeParams.cannyImg, "FINISHED", "center", 1080, 1650, -1);
	cout << "here" << endl;
	cout << "Canny settings: " << ld.configParams.edgeParams.highThresh << endl;

	waitKey();
	
	
	
}
