

// single file point for all object detection modules.


#include <ros/ros.h>
#include <std_msgs/String.h>
#include <std_msgs/Float32MultiArray.h>
#include <std_msgs/Float64.h>
#include <image_transport/image_transport.h>

#include <sensor_msgs/image_encodings.h>
#include <sensor_msgs/Image.h>
#include <sensor_msgs/LaserScan.h>
#include <sensor_msgs/PointCloud2.h>
#include <cv_bridge/cv_bridge.h>
#include <ras_object_lib/image_converter.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "geometry_msgs/Twist.h"

#include <string>
#include <sstream>

static const std::string OPENCV_WINDOW="OpenCV Window";

void camImageCallback(const sensor_msgs::Image::ConstPtr &msg){
	int image_height = msg->height;
	int image_width = msg->width;

	int step = msg->step;
	int *image_data[image_height][step];
	cv::Mat *cvimg; 
	to_cv_copy(cvimg, msg);

	// image_data = msg->data;	
}

void camDepthCallback(const std_msgs::Float32MultiArray &msg){

}




class ImageManipulator{

public: 
	ros::NodeHandle n;
	image_transport::ImageTransport it;
	image_transport::Subscriber image_sub;
	image_transport::Publisher image_pub;
	ros::Publisher cc_pub;
	double linear_vel;
	double angular_vel;

	ImageManipulator()
	: it(n)
	{
		image_sub = it.subscribe("/camera/depth/image", 3, &ImageManipulator::imageCb, this);
		image_pub = it.advertise("/image_converter/output_video", 1);

		cv::namedWindow(OPENCV_WINDOW);

	} 

	~ImageManipulator(){
		cv::destroyWindow(OPENCV_WINDOW);
	}


	void findMinMax(double &minDist, double &maxDist, cv::Point2i &minInd, cv::Point2i &maxInd, cv::Mat &gray_img){

		for(int i = 0; i< gray_img.rows; i++){
			for(int j = 0;j < gray_img.cols; j++){
				int currDist = gray_img.at<uchar>(i,j);
				// std::cout << "100, 100 values"<< gray_img.at<uchar>(100,100);
				if (minDist > currDist){
					minDist = currDist;
					minInd.x = i;
					minInd.y = j;

				}

				if(maxDist < currDist){
					maxDist = currDist;
					maxInd.x = i;
					maxInd.y = j;
				}

			}
		}
		// std::cout << "minimumm distance is:" << minDist;
		// std::cout << "maximum distance is :" << maxDist;


	}


	void findMinMaxAngle(double& minAngle, double& maxAngle, cv::Point2i &minInd, cv::Point2i &maxInd, cv::Mat &gray_img){
		minAngle = 	-1 * 50 + minInd.y* 100/ gray_img.cols;
		maxAngle =  -1 * 50 + maxInd.y* 100/ gray_img.cols;
	}


	void  imageCb(const sensor_msgs::ImageConstPtr& msg){
		cv_bridge::CvImagePtr cv_ptr;

		try{
			// cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
			cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::TYPE_32FC1);
		}
		catch(cv_bridge::Exception& e){
			ROS_ERROR("cv bridge exception error: %s", e.what());
			return ;
		}

		cv::Mat srcimg = cv_ptr->image;
		// convert the bgr image into grayscale;
		cv::Mat grayimg;
		grayimg = cv::Mat(srcimg.size(), CV_8UC1);

		// cv::cvtColor(srcimg, grayimg, CV_BGR2GRAY);

		cv::convertScaleAbs(srcimg, grayimg, 100, 0.0);
		// int min_val = 0;
		int index_i = 0;
		int index_j = 0;

		int min_val = 255;
		const int MAX_INTENSITY = 255;
		const int MIN_INTENSITY = 1;
		int max_thresh = 80;
		int min_thresh = 5;
		int distance = 0;


		for(int i =0; i<grayimg.rows; i++){
			for(int j = 0; j < grayimg.cols; j++){
				int current_val = grayimg.at<uchar>(i,j);
				if(current_val > max_thresh){
					grayimg.at<uchar>(i,j) = MAX_INTENSITY;					
					// min_val = current_val;
				}

				if (current_val < min_thresh){
					grayimg.at<uchar>(i,j) = MAX_INTENSITY;
				}	

			}
		}

		double minVal;
		double maxVal;
		double minAngle; 
		double maxAngle;
		double P = 0.05;

		cv::Point2i minLoc, maxLoc;
		// findMinMax(minVal, maxVal, minIndex, maxIndex, grayimg);

		cv::minMaxLoc(grayimg, &minVal, &maxVal, &minLoc, &maxLoc);

		findMinMaxAngle(minAngle, maxAngle, minLoc, maxLoc, grayimg);
		// std::cout << "min index x value: " << minLoc.x ;
		

		linear_vel = 0.0;
		angular_vel = 0.0;

		// if(minVal >= 31 && minVal < 80){
		// 	linear_vel = 0.30;
		// } else{
		// 	linear_vel = 0;
		// }

		// if(minLoc.x >= 250  && minLoc.x <= 350){
		// 	angular_vel = 0;
		// } else{
		// 	angular_vel = (250 - minLoc.x) *0.2 ;
		// }


		if(minVal < 01 || minVal > 81){
			linear_vel = 0.0;
			angular_vel = 0.0;
		} else {
			if(fabs(minAngle) > 15){
				angular_vel = -1 * minAngle *P;
				linear_vel = 0.20;
			} else{
				angular_vel = 0.0;
				linear_vel = 0.30;
			}

		}


		std::cout << "angular velocity is :" << angular_vel << "\n" << "linear velocity is:" << linear_vel << "\n";

    	cv::namedWindow( "Contours", CV_WINDOW_AUTOSIZE );
    	cv::imshow( "Contours", grayimg );
    	cv::waitKey(3);

		cc_pub = n.advertise<geometry_msgs::Twist>("/motor_controller/twist", 10);

		ros::Rate loop_rate(10);

		while(ros::ok()){
			geometry_msgs::Twist msg;

			msg.linear.x = ImageManipulator::linear_vel;
			msg.angular.z = ImageManipulator::angular_vel;

			cc_pub.publish(msg);
			ros::spinOnce();

			loop_rate.sleep();
		}

		image_pub.publish(cv_ptr->toImageMsg());

	}

};



int main(int argc, char ** argv){

	ros::init(argc, argv, "image_manipulator");
	// Object_Detector od;
	ros::NodeHandle n;

	// TODO: put the name of the camera message type. 
	// ros::Subscriber cam_depth_image_raw = n.subscribe("camera/depth/image_raw", 100, &camImageRawCallback);
	// ros::Subscriber cam_sub_depth_uv = n.subscribe("camera/depth/uv", 100, &camDepthCallback);
	// ros::Subscriber cam_col_image = n.subscribe("camera/color/image_raw", 100, &camColorCallback);
	ImageManipulator iman;
	ros::spin();
	return 0;

}
