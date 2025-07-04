#include "ObjectTracker.h"

const static int SEARCH = 0;
const static int TRAIN = 1;
const static int TRACK = 2;
const static int FIND = 3;

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
	int lowH = 70;
	int highH = 102;
	int lowS = 52;
	int highS = 255;
	int lowV = 72;
	int highV = 255;

	int state = SEARCH;
	
	Mat originalImg, displayImg;
	Point lastCenter = Point(0, 0);
	int lastRadius = 200;
	Point mid(cap.get(CAP_PROP_FRAME_WIDTH) / 2, cap.get(CAP_PROP_FRAME_HEIGHT) / 2);
	
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
		
		/*Mat threshold = */
		Mat circleImg = Mat::zeros(originalImg.size(), CV_8UC3);

		ostringstream os;

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
			displayImg = originalImg;
		}
		else if(state == TRACK){
			displayImg = originalImg;
		} 
		else if(state == FIND){
			Mat imgHSV;
			cvtColor(originalImg, imgHSV, COLOR_BGR2HSV);
			int radius = 250;
			circle(circleImg, mid, radius, Scalar(0, 0, 255), 5, 16, 0);

			vector<int> hue, sat, val;
			// For loop through entire circle
			for(int x = mid.x - radius; x <= mid.x + radius; x++){
				for(int y = mid.y - radius; y <= mid.y + radius; y++){
					if(x >= 0 && y >= 0 && x < imgHSV.cols && y < imgHSV.rows && 
						(pow(x - mid.x, 2) + pow(y - mid.y, 2) <= pow(radius, 2))){
						Vec3b pixelHSV = imgHSV.at<Vec3b>(y, x);
						hue.push_back(pixelHSV[0]);
						sat.push_back(pixelHSV[1]);
						val.push_back(pixelHSV[2]);
					}
				}
			}

			int hueTot = 0, satTot = 0, valTot = 0;
			for(int i = 0; i < hue.size(); i++){
				hueTot += hue[i];
				satTot += sat[i];
				valTot += val[i];
			}
			int hueAvg = hueTot / hue.size();
			int satAvg = satTot / sat.size();
			int valAvg = valTot / val.size();


			displayImg = originalImg + circleImg;

			os.str("");
			putText(displayImg, os.str(), Point(40, 60), FONT_HERSHEY_PLAIN, 2.5, Scalar(255, 255, 255), 2);
			os.str("");
			os << "H: " << hueAvg;
			putText(displayImg, os.str(), Point(40, 100), FONT_HERSHEY_PLAIN, 2.5, Scalar(255, 255, 255), 2);
			os.str("");
			os << "S: " << satAvg;
			putText(displayImg, os.str(), Point(40, 140), FONT_HERSHEY_PLAIN, 2.5, Scalar(255, 255, 255), 2);
			os.str("");
			os << "V: " << valAvg;
			putText(displayImg, os.str(), Point(40, 180), FONT_HERSHEY_PLAIN, 2.5, Scalar(255, 255, 255), 2);
			os.str("");
		}

		lastCenter = state == FIND ? mid : lastCenter;
		os << "Coordinates: " << lastCenter;
		putText(displayImg, os.str(), Point(40, 60), FONT_HERSHEY_PLAIN, 2.5, Scalar(255, 255, 255), 2);
		
		imshow("Video Stream", displayImg);
		int key = waitKey(1);

		// Training
		if(key == 116){ // t
			if(state == SEARCH){
				state = TRAIN;
			} else{
				state = SEARCH;
			}
		}
		else if(key == 32){ // Space Bar
			if(state == SEARCH){
				state = FIND;
			} else{
				state = SEARCH;
			}
		}

		if(key == 27) break; // Esc key
	}
	cap.release();
	destroyAllWindows();
}