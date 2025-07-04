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

	// My values
	// Green Circle
	int lowH = 50;
	int highH = 70;
	int lowS = 100;
	int highS = 255;
	int lowV = 100;
	int highV = 255;

	int state = SEARCH;
	
	Mat originalImg, displayImg;
	Point lastCenter = Point(0, 0);
	int lastRadius = 200;

	
	while(true){
		if(state != TRAIN){
			// Test frames of the image
			bool testing = cap.read(originalImg);
			flip(originalImg, originalImg, 1);

			if(!testing){
				cout << "Failure to read frames from video stream" << endl;
				break;
			}
		}
		
		Mat circleImg = Mat::zeros(originalImg.size(), CV_8UC3);

		if(state == SEARCH){
			vector<Vec3f> circles;
			Mat imgGray;
			cvtColor(originalImg, imgGray, COLOR_BGR2GRAY);
			HoughCircles(imgGray, circles, HOUGH_GRADIENT, 2, imgGray.rows * 2, 250, 125, 0, 0);

			for(size_t i = 0; i < circles.size(); i++){
				Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
				int radius = cvRound(circles[i][2]);
				circle(circleImg, center, 3, Scalar(0, 255, 0), -1, 8, 0);
				circle(circleImg, center, radius, Scalar(0, 0, 255), 5, 16, 0);
				lastCenter = center;
				lastRadius = radius;
			}

			displayImg = originalImg + circleImg;
		}
		else if(state == TRAIN){

		}
		else if(state == TRACK){

		}

		imshow("Video Stream", displayImg);
		int key = waitKey(1);

		// Training
		if(key == 116){
			if(state == SEARCH){
				state = TRAIN;
			} else{
				state = SEARCH;
			}
		}

		if(key == 27) break;
	}
	cap.release();
	destroyAllWindows();
}