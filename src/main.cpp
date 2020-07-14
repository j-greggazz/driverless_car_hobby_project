#include <iostream>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <direct.h>
#include <dashboardTracker.h>
#include <QApplication>
#include <QWidget>
#include <mainwindow.h>


using namespace cv;
using namespace std;

// Ctrl+k Ctrl+D align code
void runThreadsOnHeap(string video_path);
void runThreadsOnStack(string video_path);
void runStaticMethodThreads(string video_path, std::string cur_dir);

int main(int argc, char *argv[])
{
	std::string cur_dir(argv[0]);

	bool testContours = false;

	if (testContours) {


	}

	else {
		QApplication a(argc, argv);
		MainWindow w;
		w.show();
		string path;
		if (!a.exec()) {
			path = w.getVidPath();
			cout << path << endl;

			if (!w.getAbort()) {
				std::string cur_dir(argv[0]);
				runStaticMethodThreads(path, cur_dir);
			}
		};
	}

	return 1;
}





