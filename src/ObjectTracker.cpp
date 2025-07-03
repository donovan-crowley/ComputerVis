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

	namedWindow("Video Stream", WINDOW_AUTOSIZE);

	// OpenCV Hue, Saturation, and Value ranges
	int lastLowH = 0;
	int lastHighH = 179;
	int lastLowS = 0;
	int lastHighS = 255;
	int lastLowV = 0;
	int lastHighV = 0;

	int state = SEARCH;
	
	Mat originalImg;
	Point originalP(0, 0);
	int lastRadius = 200;

	
	while(true){
		flip(originalImg, originalImg, 1);
			
		// test frames of the image
		bool testing = cap.read(originalImg);
		if(!testing){
			cout << "Failure to read frames from video stream" << endl;
			break;
		}
		
		Mat circleImg;
		if(state == SEARCH){
			circleImg = Mat::zeros(originalImg.size(), CV_8UC3);
			circle(circleImg, originalP, lastRadius, Scalar(0, 255, 0), 2);
		}

		Mat imgDisplay = originalImg + circleImg;

		imshow("Video Stream", imgDisplay);

		if(waitKey(1) == 27) break;
	}
	cap.release();
	destroyAllWindows();
}