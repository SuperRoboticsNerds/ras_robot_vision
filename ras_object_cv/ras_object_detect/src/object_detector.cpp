

// single file point for all object detection modules.


#include <ros/ros.h>
#include <std_msgs/String.h>
#include <std_msgs/Float32MultiArray.h>
#include <std_msgs/Float64.h>
#include <sensor_msgs/Image.h>
#include <sensor_msgs/LaserScan.h>
#include <sensor_msgs/PointCloud2.h>
#include <opencv2/imgproc/imgproc.hpp>

#include <string>
#include <sstream>


void camImageCallback(const sensor_msgs::Image::ConstPtr &msg){
	int image_height = msg->height;
	int image_width = msg->width;

	int step = msg->step;
	int *image_data[image_height][step];

	// image_data = msg->data;	
}

// void to_cv_copy(cv::Mat* cvimg, const sensor_msgs::Image::ConstPtr& img);

void camDepthCallback(const std_msgs::Float32MultiArray &msg){

}

void camColorCallback(const sensor_msgs::Image::ConstPtr &msg){


}


int main(int argc, char ** argv){

	ros::init(argc, argv, "object_detector");
	// Object_Detector od;
	ros::NodeHandle n;

	// TODO: put the name of the camera message type. 
	ros::Subscriber cam_sub_image_raw = n.subscribe("camera/depth/image_raw", 100, &camImageCallback);
	ros::Subscriber cam_sub_depth_uv = n.subscribe("camera/depth/uv", 100, &camDepthCallback);
	ros::Subscriber cam_col_image = n.subscribe("camera/color/image_raw", 100, &camColorCallback);

	ros::Rate loop_rate(10);

	while(ros::ok()){


		ros::spinOnce();
		loop_rate.sleep();
	}


}
