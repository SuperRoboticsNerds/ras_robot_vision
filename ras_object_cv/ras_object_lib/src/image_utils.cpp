#include "ras_object_lib/image_utils.h"
#include <ros/ros.h>



template<typename T> std::string ras_cv::writeAsString(T text){
	std::ostringstream oss;
	oss  << text;
	return oss.str();

}


template std::string ras_cv::writeAsString(int);
template std::string ras_cv::writeAsString(double);
template std::string ras_cv::writeAsString(float);