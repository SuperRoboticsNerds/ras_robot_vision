
#include <sensor_msgs/image_encodings.h>
// #include "features2d.h"

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <ras_object_lib/image_converter.h>
#include <string>
#include <sstream>
#include <vector>




using namespace std;
using namespace cv;


class ObjectDetector{

private:
	static const bool USE_SURF= false;
	static const bool USE_BLOB= true;
	static const cv::SimpleBlobDetector::Params params_default;


public:
	cv::SimpleBlobDetector::Params params_;

	ObjectDetector(const bool use_surf, const bool use_blob, cv::SimpleBlobDetector::Params &params){
		const SimpleBlobDetector detector; 	
		params_ = params;	
		
	}

	ObjectDetector(const bool use_surf, const bool use_blob){

	}

	~ObjectDetector(){

	}


};


void odCallback(const sensor_msgs::ImageConstPtr& inimg){
	cv::SimpleBlobDetector::Params params;

	params.minThreshold = 10;
	params.maxThreshold = 200;

	params.filterByArea = true;
	params.minArea = 1200;

	params.filterByCircularity = true;
	params.minCircularity = 0.1;

	params.filterByConvexity = true;
	params.minConvexity = 0.80;

	params.filterByInertia = true; 
	params.minInertiaRatio = 0.01;

	cv:SimpleBlobDetector detector(params);

	cv::Mat grayimg;
	cv::Mat outimg;
	cv::Mat finimg;

	std::vector<KeyPoint> key_points;

	// detector.detect()

	to_cv_copy(&outimg, inimg);

	cv::cvtColor(outimg, grayimg, CV_BGR2GRAY);
	detector.detect(grayimg, key_points);

	cv::drawKeypoints(grayimg, key_points, finimg);


	// Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);

}



int main(int argc, char ** argv){

	cout << "Hello People";	
	ros::init(argc, argv, "object_detection");

	ros::NodeHandle node("~");


	ros::Subscriber sub;
	sub = node.subscribe("/camera/rgb/image_raw", 10, odCallback);

	ros::Rate loop_rate(10);
	ros::spin();
	return 0;

}

