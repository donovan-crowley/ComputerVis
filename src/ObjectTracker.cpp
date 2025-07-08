#include "ObjectTracker.h"

const static int SEARCH = 0;
const static int TRAIN = 1;
const static int TRACK = 2;
const static int READ = 3;

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
	int lastHighV = 255;

	// My values
	int lowH = lastLowH;
	int highH = lastHighH;
	int lowS = lastLowS;
	int highS = lastHighS;
	int lowV = lastLowV;
	int highV = lastHighV;

	int state = SEARCH;
	int progress = 0, step = 2, initialStep = 2;
	Vec3b lastInternalAvg = Vec3b(0, 0, 0);
	Vec3b lastExternalAvg = Vec3b(255, 255, 255);
	
	Mat originalImg, displayImg, trainingImg;
	Point lastCenter = Point(0, 0);
	int lastRadius = 0;
	Point mid(cap.get(CAP_PROP_FRAME_WIDTH) / 2, cap.get(CAP_PROP_FRAME_HEIGHT) / 2);

	Mat imgTmp;
	cap.read(imgTmp);
	Mat imgLines = Mat::zeros(imgTmp.size(), CV_8UC3);

	int lastX = -1; 
	int lastY = -1;
	
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
		
		Mat thresholdImg = ObjectTracker::thresholding(originalImg, Scalar(lowH, lowS, lowV), Scalar(highH, highS, highV), true, false);
		Mat circleImg = Mat::zeros(originalImg.size(), CV_8UC3);
		ostringstream os;

		if(state == SEARCH){
			Mat imgGray;
			vector<Vec3f> circles;
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
			double morph = (double) lastInternalAvg[0];
			circle(circleImg, lastCenter, lastRadius, Scalar(0, morph, 255 - morph), 3, 8);
			thresholdImg = ObjectTracker::thresholding(trainingImg, Scalar(lowH, lowS, lowV), Scalar(highH, highS, highV), false, true);
			displayImg = thresholdImg + circleImg;

			vector<double> internalAvg = vector<double>(3);
			vector<double> externalAvg = vector<double>(3);
			int internalCount = 0, externalCount = 0;

			for(int x = 0; x < thresholdImg.rows; x++){
				for(int y = 0; y < thresholdImg.cols; y++){
					Vec3b color = thresholdImg.at<Vec3b>(x, y);
					// Inside circle
					if(pow(y - lastCenter.x, 2) + pow(x - lastCenter.y, 2) < pow(lastRadius, 2)){
						internalAvg[0] += color[0];
						internalAvg[1] += color[1];
						internalAvg[2] += color[2];
						internalCount++;
					} else{ // Outside circle
						externalAvg[0] += color[0];
						externalAvg[1] += color[1];
						externalAvg[2] += color[2];
						externalCount++;
					}
				}
			}
			
			for(int i = 0; i < 3; i++){
				internalAvg[i] /= internalCount;
				externalAvg[i] /= externalCount;
			}

			Vec3b internal = Vec3b(internalAvg[0], internalAvg[1], internalAvg[2]);
			Vec3b external = Vec3b(externalAvg[0], externalAvg[1], externalAvg[2]);
			double internalChange = ((double)internal[0] - lastInternalAvg[0] + internal[1] - lastInternalAvg[1] + internal[2] - lastInternalAvg[2]) / 3;
			double externalChange = ((double)external[0] - lastExternalAvg[0] + external[1] - lastExternalAvg[1] + external[2] - lastExternalAvg[2]) / 3;
			
			if(lastHighH != highH || lastLowH != lowH || lastHighS != highS || lastLowS != lowS || lastHighV != highV || lastLowV != lowV){
				cout << "VISIBLE Progress: " << progress << endl;
				cout << "Step: " << step << endl;
				cout << "Internal Change: " << internalChange << endl;
				cout << "External Change: " << externalChange << endl;
				cout << "New internal: " << internal << ", Old internal: " << lastInternalAvg << endl;
				cout << "New external: "  << external << ", Old external: " << lastExternalAvg << endl;
			}
			if(progress > 12){
				state = TRACK;
			}
			// Internal color increased or noise decreased or internal more stable than external
			else if((internalChange >= 0 && externalChange >= 0 && internalChange >= externalChange) || 
			(internalChange <= 0 && externalChange <= 0 && internalChange >= externalChange) || 
			(internalChange >= 0 && externalChange <= 0)){ 
				lastInternalAvg = internal;
				lastExternalAvg = external;
				lastLowH = lowH;
				lastHighH = highH;
				lastLowS = lowS;
				lastHighS = highS;
				lastLowV = lowV;
				lastHighV = highV;

				if(progress == 1){
					lowH = max(0, lowH - step);
				}
				else if(progress == 2){
					highH = min(179, highH + step);
				}
				else if(progress == 3){
					lowS = max(0, lowS - step);
				}
				else if(progress == 4){
					highS = min(255, highS + step);
				}
				else if(progress == 5){
					lowV = max(0, lowV - step);
				}
				else if(progress == 6){
					highV = min(255, highV + step);
				}
				else if(progress == 7){
					lowH = min(179, lowH + step);
				}
				else if(progress == 8){
					highH = max(0, highH - step);
				}
				else if(progress == 9){
					lowS = min(255, lowS + step);
				}
				else if(progress == 10){
					highS = max(0, highS - step);
				}
				else if(progress == 11){
					lowV = min(255, lowV + step);
				}
				else if(progress == 12){
					highV = max(0, highV - step);
				}
				
				if(lastHighH == highH && lastLowH == lowH && lastHighS == highS && lastLowS == lowS && lastHighV == highV && lastLowV == lowV){
					progress++;
				}
			}
			else if(step < 6){
				lowH = lastLowH;
				highH = lastHighH;
				lowS = lastLowS;
				highS = lastHighS;
				lowV = lastLowV;
				highV = lastHighV;
				step++;
			} else{
				step = initialStep;
				progress++;
			}
		}
		else if(state == TRACK){
			Moments m = moments(thresholdImg);

			double m00 = m.m00;
			double m10 = m.m10;
			double m01 = m.m01;

			// No objects in image possibly due to noise
			if(m00 > 100000){
				int posX = m10 / m00;
				int posY = m01 / m00;
				circle(circleImg, Point(posX, posY), 15, Scalar(255, 0, 0), 3, 8);
				
				if(lastX >= 0 && lastY >= 0 && posX >= 0 && posY >= 0){
					line(imgLines, Point(posX, posY), Point(lastX, lastY), Scalar(0, 0, 255), 1);
				}
				lastX = posX;
				lastY = posY;
			}



			displayImg = originalImg + circleImg + imgLines;
		} 
		else if(state == READ){
			Mat imgHSV;
			cvtColor(originalImg, imgHSV, COLOR_BGR2HSV);
			int radius = 250;
			circle(circleImg, mid, radius, Scalar(0, 0, 255), 5, 16, 0);

			int hue = 0, sat = 0, val = 0, count = 0;
			// For loop through entire circle
			for(int x = mid.x - radius; x <= mid.x + radius; x++){
				for(int y = mid.y - radius; y <= mid.y + radius; y++){
					if(pow(x - mid.x, 2) + pow(y - mid.y, 2) <= pow(radius, 2)){
						Vec3b pixelHSV = imgHSV.at<Vec3b>(y, x);
						hue += pixelHSV[0];
						sat += pixelHSV[1];
						val += pixelHSV[2];
						count++;
					}
				}
			}

			hue /= count;
			sat /= count;
			val /= count;

			displayImg = originalImg + circleImg;

			os.str("");
			os << "H: " << hue;
			putText(displayImg, os.str(), Point(40, 100), FONT_HERSHEY_PLAIN, 2.5, Scalar(255, 255, 255), 2);
			os.str("");
			os << "S: " << sat;
			putText(displayImg, os.str(), Point(40, 140), FONT_HERSHEY_PLAIN, 2.5, Scalar(255, 255, 255), 2);
			os.str("");
			os << "V: " << val;
			putText(displayImg, os.str(), Point(40, 180), FONT_HERSHEY_PLAIN, 2.5, Scalar(255, 255, 255), 2);
			os.str("");
		}
		
		Point trackCenter(lastX, lastY);
		lastCenter = state == READ ? mid : lastCenter;
		lastCenter = state == TRACK ? trackCenter : lastCenter;
		os << "Coordinates: " << lastCenter;
		putText(displayImg, os.str(), Point(40, 60), FONT_HERSHEY_PLAIN, 2.5, Scalar(255, 255, 255), 2);
		os.str("");
		if(state != READ){
			os << "H: (" << lowH << " - " << highH << ")";
			putText(displayImg, os.str(), Point(40, 100), FONT_HERSHEY_PLAIN, 2.5, Scalar(255, 255, 255), 2);
			os.str("");
			os << "S: (" << lowS << " - " << highS << ")";
			putText(displayImg, os.str(), Point(40, 140), FONT_HERSHEY_PLAIN, 2.5, Scalar(255, 255, 255), 2);
			os.str("");
			os << "V: (" << lowV << " - " << highV << ")";
			putText(displayImg, os.str(), Point(40, 180), FONT_HERSHEY_PLAIN, 2.5, Scalar(255, 255, 255), 2);
			os.str("");
		}

		imshow("Object Detection", displayImg);

		int key = waitKey(1);
		// Training
		if(key == 116){ // t
			if(state == SEARCH){
				imgLines = Mat::zeros(imgTmp.size(), CV_8UC3);
				state = TRAIN;
				progress = 1;

				lowH = 0;
				highH = 179;
				lowS = 0;
				highS = 255;
				lowV = 0;
				highV = 255;

				lastLowH = 0;
				lastHighH = 179;
				lastLowS = 0;
				lastHighS = 255;
				lastLowV = 0;
				lastHighV = 255;

				lastInternalAvg = Vec3b(0, 0, 0);
				lastExternalAvg = Vec3b(255, 255, 255);

				vector<double> average = vector<double>(3);
				int count = 0;
				for (int x = 0; x < originalImg.rows; x++) {
				    for (int y = 0; y < originalImg.cols; y++) {
				        if (pow(y - lastCenter.x, 2) + pow(x - lastCenter.y, 2) < pow(lastRadius, 2)) {
				        	Vec3b color = originalImg.at<Vec3b>(x, y);
				        	average[0] += color[0];
				        	average[1] += color[1];
				        	average[2] += color[2];
				        	count++;
				        }
				    }
				}
				average[0] /= count;
				average[1] /= count;
				average[2] /= count;

				trainingImg = Mat(originalImg);
				Vec3b hsv = ObjectTracker::RGB2HSV(average[0], average[1], average[2]);
				lowH = max(0, hsv[0] - 20);
				highH = min(179, hsv[0] + 20);
				lowS = max(0, hsv[1] - 20);
				highS = min(255, hsv[1] + 20);
				lowV = max(0, hsv[2] - 20);
				highV = min(255, hsv[2] + 20);
			} else{
				state = SEARCH;
			}
		}
		else if(key == 32){ // Space Bar
			if(state != READ){
				state = READ;
			} else{
				state = SEARCH;
			}
		}
		else if(key == 99){ // c
			imgLines = Mat::zeros(imgTmp.size(), CV_8UC3);
		}
		else if(key == 27) break; // Esc key
	}
	cap.release();
	destroyAllWindows();
}

Mat ObjectTracker::thresholding(Mat originalImg, Scalar lowHSV, Scalar highHSV, bool noise, bool color){
	Mat imgHSV;
	cvtColor(originalImg, imgHSV, COLOR_BGR2HSV);
	Mat thresholdImg;
	inRange(imgHSV, lowHSV, highHSV, thresholdImg);

	if(noise){
		// Morphological Opening
		erode(thresholdImg, thresholdImg, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		dilate(thresholdImg, thresholdImg, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		// Morphological Closing
		dilate(thresholdImg, thresholdImg, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		erode(thresholdImg, thresholdImg, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
	}

	GaussianBlur(thresholdImg, thresholdImg, Size(51, 51), 0);

	if(color){
		cvtColor(thresholdImg, thresholdImg, COLOR_GRAY2RGB);
	}

	return thresholdImg;
}

Vec3b ObjectTracker::RGB2HSV(float r, float g, float b){
	float h, s, v, minV, maxV, delta;
	r /= 255;
	g /= 255;
	b /= 255;
	minV = min(min(r, g), b);
	maxV = max(max(r, g), b);
	v = maxV;
	delta = maxV - minV;
	if(maxV != 0){
		s = delta / maxV;
	} else{
		return Vec3b(0, 0, 0); // Black
	}

	if(r == maxV){
		h = (g - b) / delta;
	}
	else if(g == maxV){
		h = 2 + (b - r) / delta;
	} else{
		h = 4 + (r - g) / delta;
	}
	h *= 60;
	if(h < 0){
		h += 360;
	}
	return Vec3b(h / 2, s * 255, v * 255);
}