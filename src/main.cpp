#include <server.h>
#include <pch2.h>
#include <dashboardTracker.h>

/*To use CUDA as the backend of OpenCV DNN module, you can simply add these two lines after you load the pre-trained model:
https://cuda-chen.github.io/image%20processing/programming/2020/02/22/build-opencv-dnn-module-with-nvidia-gpu-support-on-ubuntu-1804.html
// C++
net.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
net.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
*/

using namespace cv;


// Ctrl+k Ctrl+D align code
void runHeapObjectThreads(const std::string& video_path, const std::string& cur_dir);
void runStackObjectThreads(const std::string& video_path, const std::string& cur_dir);
void runStaticMethodThreads(const std::string& video_path, const std::string& cur_dir);
void singleThreadContourTest();
void testGPUFunctions();

int main(int argc, char* argv[])
{

	bool singleTest = false;

	if (singleTest) {
#if HAS_CUDA
		testGPUFunctions();
#endif
		//singleThreadContourTest();
	}

	else {
		std::string cur_dir(argv[0]);
		std::cout << cur_dir << std::endl;
		QApplication a(argc, argv);
		MainWindow w;
		w.show();
		std::string path;
		if (!a.exec()) {
			path = w.getVidPath();
			std::cout << path << std::endl;

			if (!w.getAbort()) {
				std::string cur_dir(argv[0]);
				runStackObjectThreads(path, cur_dir);
			}
		};
	}

	return 1;
}





