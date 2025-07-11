#ifndef OBJECTTRACKER_H_
#define OBJECTTRACKER_H_

#include <opencv2/opencv.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <stdio.h>
#include <math.h>
#include <string>
#include <sstream>
using namespace std;
using namespace cv;

class ObjectTracker {
public:
	static void run();
private:
	static Mat thresholding(Mat originalImg, Scalar lowHSV, Scalar highHSV, bool noise, bool color);
	static Vec3b RGB2HSV(float r, float g, float b);
};

#endif