#include "ObjectTracker.h"

void ObjectTracker::run() {
	VideoCapture cap(0);

	if (!cap.isOpened()) {
		cout << "Failed to open default camera" << endl;
		exit(-1);
	}

	Mat edges, gray, frame;
	namedWindow("Control", WINDOW_AUTOSIZE);
	
	while(true){
		cap >> frame;

		if(frame.empty()){
			cerr << "Empty frame received" << endl;
			break;
		}

		cvtColor(frame, gray, COLOR_BGR2GRAY);
		Canny(gray, edges, 50, 150);
		imshow("Control", edges);

		if(waitKey(1) == 27) break;
	}
	cap.release();
	destroyAllWindows();
}