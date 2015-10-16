#include "ras_object_lib/image_normalise.h"

#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>


void normalise(cv::Mat& img, cv::Mat& normimg){
	double minVal = 0;
	double maxVal = 0;
	cv::Point2i minLoc(0, 0);
	cv::Point2i maxLoc(0, 0);

	// cvMat normimg;
	img.convertTo(normimg, CV_32F);
	cv::minMaxLoc(normimg, &minVal, &maxVal, &minLoc, &maxLoc);
	normimg = normimg / maxVal;
	// img = normimg;

}


void removeNans(cv::Mat &img, float normval){
	for(int i = 0; i < img.rows; i++){
		for(int j = 0; j < img.cols; j++){
			if(isnan(img.at<float>(cv::Point(i,j)))){
				img.at<float>(cv::Point(i,j)) = normval;
			}
		}
	}

}