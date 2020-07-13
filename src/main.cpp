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
void runStaticMethodThreads(string video_path);

int main(int argc, char *argv[])
{

	bool testContours = true;

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
				runStaticMethodThreads(path);
			}
		};
	}
	return 1;
}





