#include "ObjectTracker.h"

int SEARCH = 0;
int TRAIN = 1;
int TRACK = 2;

void ObjectTracker::run() {
	VideoCapture cap(0);

	if (!cap.isOpened()) {
		cout << "Failed to open default camera" << endl;
		exit(-1);
	}

	namedWindow("Control", WINDOW_AUTOSIZE);

	// OpenCV Hue, Saturation, and Value ranges
	int lastLowH = 0;
	int lastHighH = 179;
	int lastLowS = 0;
	int lastHighS = 255;
	int lastLowV = 0;
	int lastHighV = 0;

	int state = SEARCH;
	
	Mat originalImg;
	
	while(true){
		bool testing = cap.read(originalImg);
		flip(originalImg, originalImg, 1);
		if(!testing){
			cout << "Failure to read frames from video stream" << endl;
		}
		

		imshow("Control", originalImg);

		if(waitKey(1) == 27) break;
	}
	cap.release();
	destroyAllWindows();
}