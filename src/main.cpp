#include <pch2.h>
#include <dashboardTracker.h>


using namespace cv;
using namespace std;

// Ctrl+k Ctrl+D align code
void runHeapObjectThreads(const string& video_path, const string& cur_dir);
void runStackObjectThreads(const string& video_path, const string& cur_dir);
void runStaticMethodThreads(const string& video_path, const std::string& cur_dir);
void singleThreadContourTest();
void testGPUFunctions();

int main(int argc, char* argv[])
{
	
	/*To use CUDA as the backend of OpenCV DNN module, you can simply add these two lines after you load the pre-trained model:
	https://cuda-chen.github.io/image%20processing/programming/2020/02/22/build-opencv-dnn-module-with-nvidia-gpu-support-on-ubuntu-1804.html
	// C++
	net.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
	net.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
	*/
	bool singleTest = false;

	if (singleTest) {
#if HAS_CUDA
		testGPUFunctions();
#endif
		//singleThreadContourTest();
	}

	else {
		std::string cur_dir(argv[0]);
		cout << cur_dir << endl;
		QApplication a(argc, argv);
		MainWindow w;
		w.show();
		string path;
		if (!a.exec()) {
			path = w.getVidPath();
			cout << path << endl;

			if (!w.getAbort()) {
				std::string cur_dir(argv[0]);
				runStackObjectThreads(path, cur_dir);
			}
		};
	}

	return 1;
}





