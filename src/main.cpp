#include <iostream>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <direct.h>
#include <dashboardTracker.h>
using namespace cv;
using namespace std;

// Ctrl+k Ctrl+D align code
void runThreadsOnHeap();
void runThreadsOnStack();
void runStaticMethodThreads();

int main()
{
	runStaticMethodThreads();
	runThreadsOnStack();
    runThreadsOnHeap();

	return 1;
}





