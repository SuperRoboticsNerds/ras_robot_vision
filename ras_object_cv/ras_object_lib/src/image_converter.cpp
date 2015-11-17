#include "ras_object_lib/image_converter.h"
#include <ros/ros.h>


void ras_cv::to_cv_copy(cv::Mat* cvimg, const sensor_msgs::Image::ConstPtr& img) {
  cv_bridge::CvImagePtr cv_ptr;
  try {
    cv_ptr = cv_bridge::toCvCopy(img, sensor_msgs::image_encodings::BGR8);
    *cvimg = cv_ptr->image;
  } catch (cv_bridge::Exception& e) {
    ROS_ERROR("cv_bridge exception: %s", e.what());
  }
} 



void ras_cv::to_ros_copy(const cv::Mat& cvimg, sensor_msgs::ImagePtr& img){

	ros::Time time = ros::Time::now();
	cv_bridge::CvImage cvi;
	cvi.header.stamp = time;
	cvi.header.frame_id = "image";
	cvi.encoding = "bgr8";
	cvi.image = cvimg;

	// cvi.toImageMsg(img);
	img = cvi.toImageMsg();

}


