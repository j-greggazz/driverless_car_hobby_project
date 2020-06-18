#include <lineDetector.h>

//#define _USE_MATH_DEFINES


using namespace cv;
using namespace std;

void setup(LineDetector& ld_, Mat& img);
int loadVideo();
void lineDetectionTest() {


	
	loadVideo();
	//Mat img = imread("../data/sample_car.jpg");
	//setup(ld, img);
	waitKey();

	//LineDetector::displayImg(ld.configParams.edgeParams.cannyImg, "FINISHED", ld.configParams.edgeParams.screenWidth, ld.configParams.edgeParams.screenHeight, -1);
	//cout << "here" << endl;
	//cout << "Canny settings: " << ld.configParams.edgeParams.highThresh << endl;
}

int loadVideo() {
	
	LineDetector ld;

	//--- INITIALIZE VIDEOCAPTURE
	Mat frame;
	VideoCapture vCap;
	vCap.open("../data/dashboardVid.mp4");
	bool firstFrame = false;

	if (!vCap.isOpened()) {
		cout << "Error reading file: Check filepath." << endl;
		waitKey();
		return 0;
	}

	else if (vCap.get(CAP_PROP_FRAME_COUNT) < 1) {
		cout << "Error: At least one frame is required" << endl;
		waitKey();
		return(0);
	}

	else {
		//cout << "Number of frames = " << vCap.get(CAP_PROP_FRAME_COUNT) << endl;
		vCap.read(frame);
		
		char quit = 0; // Ascii value is 113

		while (vCap.isOpened() && quit != 113) {
			
			if (firstFrame) {
				setup(ld, frame);
				firstFrame = false;
			}

			else if ((vCap.get(CAP_PROP_POS_FRAMES) + 1) < vCap.get(CAP_PROP_FRAME_COUNT)) {       // continue processing as long as further frames are available
				vCap.read(frame);
				ld.configParams.edgeParams.currImg = frame;
				ld.cleanImg();

				imshow("Sample", frame);
				waitKey(100);
			}

			
		}

		return 1;
	}



}

void setup(LineDetector& ld_, Mat& img) {

	int maxHeight = int(ld_.configParams.edgeParams.screenHeight / 2);
	int maxWidth = int(ld_.configParams.edgeParams.screenWidth / 3);

	int new_cols = int(float(maxHeight) / float(img.rows) * img.cols);
	int new_rows = int(float(maxWidth) / float(img.cols) * img.rows);

	if (new_cols > maxWidth) {
		new_cols = maxWidth;
	}

	else {
		new_rows = maxHeight;
	}

	ld_.configParams.edgeParams.newCols = new_cols;
	ld_.configParams.edgeParams.newRows = new_rows;
	ld_.configParams.edgeParams.recHeight = new_rows;
	ld_.configParams.edgeParams.recWidth = new_cols;

	cv::Mat m;
	const std::string winName = "Background";
	cv::namedWindow(winName, cv::WINDOW_AUTOSIZE);
	m.create(int(ld_.configParams.edgeParams.screenHeight*0.95), ld_.configParams.edgeParams.screenWidth, CV_32FC3);
	m.setTo(cv::Scalar(255, 255, 255));
	moveWindow(winName, 0, 0);
	cv::imshow(winName, m);

	ld_.configParams.edgeParams.origImg = img.clone();
	ld_.setROI_Box();
	//LineDetector::PrintFullPath(".\\");
	//LineDetector::func();
}

