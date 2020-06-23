#include <lineDetector.h>
#include <chrono>
#include <iostream>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <direct.h>

//#define _USE_MATH_DEFINES

using namespace cv;
using namespace std;
using namespace cv::dnn;

void setup(LineDetector& ld_, Mat& img);
int loadVideo(bool multithreading = true);
int trackCars_haar_clf();
int test_mobileNet();
void detectAndDisplay_cascade_clf(Mat& frame, CascadeClassifier& car_clf);
int trackCars_mobileNet();
void lineDetectionTest() {
	//LineDetector::PrintFullPath(".\\");
	loadVideo();
	//trackCars_haar_clf();

	//trackCars_mobileNet();
}

int loadVideo(bool multithreading) {

	LineDetector ld;

	if (multithreading) {

		//--- INITIALIZE VIDEOCAPTURE
		Mat frame;
		VideoCapture vCap;
		vCap.open("../data/dashboardVid.mp4");
		bool firstFrame = false;

		// atomic variable to stop threads
		std::atomic<bool> stop_threading = false;

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
			char quit = 0; 

			int i = 0;
			const int num_threads = 3;
			vector<LineDetector> lineDetectors; 
			lineDetectors.resize(num_threads); 
			vector<Net> mobileNets;
			mobileNets.resize(num_threads);
			thread frameThreads[num_threads];
			int detectorDisplayed = 0;
			//bool frameProcessed = true;
			int frameCount = 0;
			int processedFrames = 0;
			int noFrame = 10800;//15000;
			bool success = vCap.set(CAP_PROP_POS_FRAMES, noFrame);
			while (vCap.isOpened() && quit != 113) {

				if (quit == 'q') {
					for (int i = 0; i < 3; i++) {
						frameThreads[i].join();
					}
					break;
				}
				
				if (firstFrame) {
					setup(ld, frame);
					firstFrame = false;
				}

				else if (i < 3) {
					vCap.read(frame);
					lineDetectors[i].configParams.edgeParams.id = i;
					lineDetectors[i].configParams.edgeParams.currImg = frame;
					lineDetectors[i].configParams.edgeParams.newImgAvailable = true;
					lineDetectors[i].configParams.edgeParams.continueProcessing = true;
					CV_TRACE_FUNCTION();
					String modelTxt = "../models/MobileNetSSD_deploy.prototxt.txt";
					String modelBin = "../models/MobileNetSSD_deploy.caffemodel";
					Net net;

					try {
						mobileNets[i] = dnn::readNetFromCaffe(modelTxt, modelBin);

						cerr << "loaded successfully" << endl;
					}
					catch (cv::Exception& e)
					{
						std::cerr << "Exception: " << e.what() << std::endl;

					}

					frameThreads[i] = std::thread(LineDetector::processImg_thread, std::ref(lineDetectors[i]), std::ref(stop_threading), std::ref(mobileNets[i]));
					i++;
				}

				else if ((vCap.get(CAP_PROP_POS_FRAMES) + 1) < vCap.get(CAP_PROP_FRAME_COUNT)) {       // continue processing as long as further frames are available
					//frameProcessed = false;
					if (processedFrames == frameCount){
						vCap.read(frame);
						frameCount++;
					}
					
					

					if (i % num_threads == 0) {
						try {
							if (lineDetectors[0].configParams.edgeParams.linesDrawn == true & lineDetectors[0].configParams.edgeParams.detectionComplete == true & detectorDisplayed == 0) {
								imshow("Sample", lineDetectors[0].configParams.edgeParams.currImg.clone());
								lineDetectors[0].configParams.edgeParams.detectionComplete == false;
								lineDetectors[0].configParams.edgeParams.linesDrawn == false;
								cout << "%3 == 0, frame " << i << endl;
								lineDetectors[0].configParams.edgeParams.currImg = frame;
								lineDetectors[0].configParams.edgeParams.newImgAvailable = true;
								detectorDisplayed = 1;	
								processedFrames++;
							}
						}
						catch (cv::Exception& e) {
							int val = 0;
						}
					}

					if (i % num_threads == 1) {
						try {
							if (lineDetectors[1].configParams.edgeParams.linesDrawn == true & lineDetectors[1].configParams.edgeParams.detectionComplete == true & detectorDisplayed == 1) {
								imshow("Sample", lineDetectors[1].configParams.edgeParams.currImg.clone());
								lineDetectors[1].configParams.edgeParams.linesDrawn == false;
								lineDetectors[1].configParams.edgeParams.detectionComplete == false;
								cout << "%3 == 1, frame " << i << endl;
								lineDetectors[1].configParams.edgeParams.currImg = frame;
								lineDetectors[1].configParams.edgeParams.newImgAvailable = true;
								detectorDisplayed = 2;		
								processedFrames++;
							}
						}
						catch (cv::Exception& e) {
							int val = 0;
						}
					
					}

					if (i % num_threads == 2) {
						try {
							if (lineDetectors[2].configParams.edgeParams.linesDrawn == true & lineDetectors[2].configParams.edgeParams.detectionComplete == true & detectorDisplayed == 2) {
								imshow("Sample", lineDetectors[2].configParams.edgeParams.currImg.clone());
								lineDetectors[2].configParams.edgeParams.linesDrawn == false;
								lineDetectors[2].configParams.edgeParams.detectionComplete == false;
								cout << "%3 == 2, frame " << i << endl;
								lineDetectors[2].configParams.edgeParams.currImg = frame;
								lineDetectors[2].configParams.edgeParams.newImgAvailable = true;
								detectorDisplayed = 0;
								processedFrames++;
							}
						}
						catch (cv::Exception& e) {
							int val = 0;
						}					
					}
					
					i++;
					quit = waitKey(100);
				}
			}

			if (quit == 'q') {
				stop_threading = true;
				for (int i = 0; i < 3; i++) {
					frameThreads[i].join();
				}
			}
			destroyWindow("Sample");
			return 1;
		}
	}

	else {
		Mat frame;
		VideoCapture vCap;
		vCap.open("../data/dashboardVid.mp4");

		CV_TRACE_FUNCTION();
		String modelTxt = "../models/MobileNetSSD_deploy.prototxt.txt";
		String modelBin = "../models/MobileNetSSD_deploy.caffemodel";
		Net net;
		string CLASSES[] = { "background", "aeroplane", "bicycle", "bird", "boat",
		"bottle", "bus", "car", "cat", "chair", "cow", "diningtable",
		"dog", "horse", "motorbike", "person", "pottedplant", "sheep",
		"sofa", "train", "tvmonitor" };

		try {
			net = dnn::readNetFromCaffe(modelTxt, modelBin);

			cerr << "loaded successfully" << endl;
		}
		catch (cv::Exception& e)
		{
			std::cerr << "Exception: " << e.what() << std::endl;

		}


		bool firstFrame = false;
		int noFrame = 10000;
		bool success = vCap.set(CAP_PROP_POS_FRAMES, noFrame);
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

			int i = 0;
			int noFrame = 10000;
			bool success = vCap.set(CAP_PROP_POS_FRAMES, noFrame);
			while (vCap.isOpened() && quit != 113) {
				
				if (quit == 'q') {
					quit = true;
					break;
				}
				if (firstFrame) {
					setup(ld, frame);
					waitKey();
					firstFrame = false;
				}

				else if ((vCap.get(CAP_PROP_POS_FRAMES) + 1) < vCap.get(CAP_PROP_FRAME_COUNT)) {       // continue processing as long as further frames are available

					vCap.read(frame);

					ld.configParams.edgeParams.currImg = frame;

					ld.processImg();
					Mat img = frame(ld.configParams.edgeParams.roi_Box_car);
					//Mat img = ld.configParams.edgeParams.roiImg_car;
					Mat img2;
					resize(img, img2, Size(300, 300));
					//cv::resize(img, img2, cv::Size(), 0.2, 1);
					Mat inputBlob = blobFromImage(img2, 0.007843, Size(300, 300), Scalar(127.5, 127.5, 127.5), false);  // 1. Mean subtraction is used to help combat illumination changes in the input images in our dataset
																														// 2. Scaling 
					net.setInput(inputBlob, "data");
					Mat detection = net.forward("detection_out");
					Mat detectionMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());

					ostringstream ss;
					float confidenceThreshold = 0.2;
					for (int i = 0; i < detectionMat.rows; i++)
					{
						float confidence = detectionMat.at<float>(i, 2);

						if (confidence > 0) {
							cout << "Confidence = " << confidence << endl;
						}
						if (confidence > confidenceThreshold)
						{
							int idx = static_cast<int>(detectionMat.at<float>(i, 1));
							int xLeftBottom = static_cast<int>(detectionMat.at<float>(i, 3) * img.cols);
							int yLeftBottom = static_cast<int>(detectionMat.at<float>(i, 4) * img.rows);
							int xRightTop = static_cast<int>(detectionMat.at<float>(i, 5) * img.cols);
							int yRightTop = static_cast<int>(detectionMat.at<float>(i, 6) * img.rows);

							Rect object((int)xLeftBottom, (int)yLeftBottom,
								(int)(xRightTop - xLeftBottom),
								(int)(yRightTop - yLeftBottom));

							rectangle(img, object, Scalar(0, 255, 0), 2);

							cout << CLASSES[idx] << ": " << confidence << endl;

							ss.str("");
							ss << confidence;
							String conf(ss.str());
							String label = CLASSES[idx] + ": " + conf;
							int baseLine = 0;
							Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
							putText(img, label, Point(xLeftBottom, yLeftBottom), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0));
						}
					}
					imshow("detections", img);

					//waitKey(100);
					imshow("Sample", frame);

					quit = waitKey(500);

					/*auto start = std::chrono::high_resolution_clock::now();
					vCap.read(frame);
					auto finish = std::chrono::high_resolution_clock::now();
					std::chrono::duration<double> elapsed = finish - start;
					cout << "Time taken to read frame = " << elapsed.count() << endl;

					ld.configParams.edgeParams.currImg = frame;
					start = std::chrono::high_resolution_clock::now();
					ld.processImg();
					finish = std::chrono::high_resolution_clock::now();
					elapsed = finish - start;
					cout << "Time taken for image processing = " << elapsed.count() << endl;
					start = std::chrono::high_resolution_clock::now();
					imshow("Sample", frame);
					finish = std::chrono::high_resolution_clock::now();
					elapsed = finish - start;
					cout << "Time taken for imshow = " << elapsed.count() << endl;
					waitKey(100);*/
				}


			}
			destroyWindow("Sample");
			return 1;
		}
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
	//ld_.setLane_ROIBox();
	ld_.setCarDetectionROIBox();
	//LineDetector::PrintFullPath(".\\");
	//LineDetector::func();
}

int trackCars_haar_clf() {
	//CvHaarClassifierCascade *cascade;
	CascadeClassifier car_clf;
	car_clf.load("../models/cars.xml");
	LineDetector::PrintFullPath(".\\");

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
		//vCap.read(frame);
		char quit = 0; // Ascii value is 113

		int i = 0;
		while (vCap.isOpened() && quit != 113) {

			if (quit == 'q') {
				quit = true;
				break;
			}

			else if ((vCap.get(CAP_PROP_POS_FRAMES) + 1) < vCap.get(CAP_PROP_FRAME_COUNT)) {       // continue processing as long as further frames are available

				vCap.read(frame);
				detectAndDisplay_cascade_clf(frame, car_clf);
				waitKey(100);
			}


		}
		destroyAllWindows();
		return 1;
	}
}

int test_mobileNet() {

	string CLASSES[] = { "background", "aeroplane", "bicycle", "bird", "boat",
	"bottle", "bus", "car", "cat", "chair", "cow", "diningtable",
	"dog", "horse", "motorbike", "person", "pottedplant", "sheep",
	"sofa", "train", "tvmonitor" };
	
	//.load("../models/cars.xml");

	CV_TRACE_FUNCTION();
	String modelTxt = "../models/MobileNetSSD_deploy.prototxt.txt";
	String modelBin = "../models/MobileNetSSD_deploy.caffemodel";
	Net net;

	try{
		net = dnn::readNetFromCaffe(modelTxt, modelBin);
		
		cerr << "loaded successfully" << endl;
	}
	catch (cv::Exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;

	}


	// 1. Test model:
	vector<cv::String> fn;
	LineDetector::PrintFullPath(".\\");
	glob("../models/images/*.jpg", fn, false);

	vector<Mat> images;
	size_t count = fn.size(); //number of png files in images folder
	for (size_t i = 0; i < count; i++){
		Mat img = imread(fn[i]);
		if (img.empty())
		{
			std::cerr << "Can't read image from the file: " << std::endl;
			exit(-1);
		}

		Mat img2;
		resize(img, img2, Size(300, 300));
		Mat inputBlob = blobFromImage(img2, 0.007843, Size(300, 300), Scalar(127.5, 127.5, 127.5), false);  // 1. Mean subtraction is used to help combat illumination changes in the input images in our dataset
		                                                                                                    // 2. Scaling 
		net.setInput(inputBlob, "data");
		Mat detection = net.forward("detection_out");
		Mat detectionMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());

		ostringstream ss;
		float confidenceThreshold = 0.2;
		for (int i = 0; i < detectionMat.rows; i++)
		{
			float confidence = detectionMat.at<float>(i, 2);

			if (confidence > confidenceThreshold)
			{
				int idx = static_cast<int>(detectionMat.at<float>(i, 1));
				int xLeftBottom = static_cast<int>(detectionMat.at<float>(i, 3) * img.cols);
				int yLeftBottom = static_cast<int>(detectionMat.at<float>(i, 4) * img.rows);
				int xRightTop = static_cast<int>(detectionMat.at<float>(i, 5) * img.cols);
				int yRightTop = static_cast<int>(detectionMat.at<float>(i, 6) * img.rows);

				Rect object((int)xLeftBottom, (int)yLeftBottom,
					(int)(xRightTop - xLeftBottom),
					(int)(yRightTop - yLeftBottom));

				rectangle(img, object, Scalar(0, 255, 0), 2);

				cout << CLASSES[idx] << ": " << confidence << endl;

				ss.str("");
				ss << confidence;
				String conf(ss.str());
				String label = CLASSES[idx] + ": " + conf;
				int baseLine = 0;
				Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
				putText(img, label, Point(xLeftBottom, yLeftBottom), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0));
			}
		}
		imshow("detections", img);
		
		waitKey();



	}
	return 1;
};

int trackCars_mobileNet() {

	string CLASSES[] = { "background", "aeroplane", "bicycle", "bird", "boat",
	"bottle", "bus", "car", "cat", "chair", "cow", "diningtable",
	"dog", "horse", "motorbike", "person", "pottedplant", "sheep",
	"sofa", "train", "tvmonitor" };

	//.load("../models/cars.xml");

	CV_TRACE_FUNCTION();
	String modelTxt = "../models/MobileNetSSD_deploy.prototxt.txt";
	String modelBin = "../models/MobileNetSSD_deploy.caffemodel";
	Net net;

	try {
		net = dnn::readNetFromCaffe(modelTxt, modelBin);

		cerr << "loaded successfully" << endl;
	}
	catch (cv::Exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;

	}

	Mat frame;
	VideoCapture vCap;
	vCap.open("../data/dashboardVid.mp4");

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
		
		char quit = 0; // Ascii value is 113
		int j = 0;
		// solution 1
		int noFrame = 10000;
		bool success = vCap.set(CAP_PROP_POS_FRAMES, noFrame);
		while (vCap.isOpened() && quit != 113) {
			vCap.read(frame);
			////j++;
			cout << j << endl;
			if (j > -1) {
				if (quit == 'q') {
					quit = true;
					break;
				}

				else if ((vCap.get(CAP_PROP_POS_FRAMES) + 1) < vCap.get(CAP_PROP_FRAME_COUNT)) {       // continue processing as long as further frames are available

					Mat img2;
					resize(frame, img2, Size(300, 300));
					Mat inputBlob = blobFromImage(img2, 0.007843, Size(300, 300), Scalar(127.5, 127.5, 127.5), false);  // 1. Mean subtraction is used to help combat illumination changes in the input images in our dataset
																														// 2. Scaling 
					net.setInput(inputBlob, "data");
					Mat detection = net.forward("detection_out");
					Mat detectionMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());

					ostringstream ss;
					float confidenceThreshold = 0.2;
					for (int i = 0; i < detectionMat.rows; i++)
					{
						float confidence = detectionMat.at<float>(i, 2);

						if (confidence > 0) {
							cout << "Confidence = " << confidence << endl;
						}
						if (confidence > confidenceThreshold)
						{
							int idx = static_cast<int>(detectionMat.at<float>(i, 1));
							int xLeftBottom = static_cast<int>(detectionMat.at<float>(i, 3) * frame.cols);
							int yLeftBottom = static_cast<int>(detectionMat.at<float>(i, 4) * frame.rows);
							int xRightTop = static_cast<int>(detectionMat.at<float>(i, 5) * frame.cols);
							int yRightTop = static_cast<int>(detectionMat.at<float>(i, 6) * frame.rows);

							Rect object((int)xLeftBottom, (int)yLeftBottom,
								(int)(xRightTop - xLeftBottom),
								(int)(yRightTop - yLeftBottom));

							rectangle(frame, object, Scalar(0, 255, 0), 2);

							cout << CLASSES[idx] << ": " << confidence << endl;

							ss.str("");
							ss << confidence;
							String conf(ss.str());
							String label = CLASSES[idx] + ": " + conf;
							int baseLine = 0;
							Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
							putText(frame, label, Point(xLeftBottom, yLeftBottom), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0));
						}
					}
					imshow("detections", frame);

					waitKey(100);

				}
			}
		}
	}
	return 1;
}

void detectAndDisplay_cascade_clf(Mat& frame, CascadeClassifier& car_clf)
{
	Mat frame_gray;
	cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);
	//-- Detect faces
	std::vector<Rect> cars;
	car_clf.detectMultiScale(frame_gray, cars);
	for (size_t i = 0; i < cars.size(); i++)
	{
		Point center(cars[i].x + cars[i].width / 2, cars[i].y + cars[i].height / 2);
		ellipse(frame, center, Size(cars[i].width / 2, cars[i].height / 2), 0, 0, 360, Scalar(255, 0, 255), 4);
		
		
	}
	//-- Show what you got
	imshow("Capture - Car detection", frame);
}