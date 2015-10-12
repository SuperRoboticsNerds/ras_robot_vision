

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

// void to_cv_copy(cv::Mat* cvimg, const sensor_msgs::Image::ConstPtr& img);

void camImageRawCallback(const std_msgs::Float32MultiArray &msg){

}


void camDepthCallback(const std_msgs::Float32MultiArray &msg){

}

void camColorCallback(const sensor_msgs::Image::ConstPtr &msg){


}




class ImageManipulator{

public: 
	ros::NodeHandle n;
	image_transport::ImageTransport it;
	image_transport::Subscriber image_sub;
	image_transport::Publisher image_pub;

	ImageManipulator()
	: it(n)
	{
		image_sub = it.subscribe("/camera/image_raw", 10, &ImageManipulator::imageCb, this);
		image_pub = it.advertise("/image_converter/output_video", 1);

		cv::namedWindow(OPENCV_WINDOW);

	} 

	~ImageManipulator(){
		cv::destroyWindow(OPENCV_WINDOW);
	}


	void findMinMax(double &minDist, double &maxDist, cv::Point2i &minInd, cv::Point2i &maxInd, cv::Mat &gray_img){

		for(int i = 0; i< gray_img.rows; i++){
			for(int j = 0; gray_img.cols; j++){
				int currDist = gray_img.at<uchar>(i,j);
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

	}


	void findMinMaxAngle(double& minAngle, double& maxAngle, cv::Point2i &minInd, cv::Point2i &maxInd, cv::Mat &gray_img){
		minAngle = 	-1 * 50 + minInd.y* 100/ gray_img.cols;
		maxAngle =  -1 * 50 + maxInd.y* 100/ gray_img.cols;
	}


	void  imageCb(const sensor_msgs::ImageConstPtr& msg){
		cv_bridge::CvImagePtr cv_ptr;

		try{
			cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
		}
		catch(cv_bridge::Exception& e){
			ROS_ERROR("cv bridge exception error: %s", e.what());
			return ;
		}

		cv::Mat srcimg = cv_ptr->image;
		// convert the bgr image into grayscale;
		cv::Mat grayimg;
		grayimg = cv::Mat(srcimg.size(), CV_8UC3);

		cv::cvtColor(srcimg, grayimg, CV_BGR2GRAY);

		// int min_val = 0;
		int index_i = 0;
		int index_j = 0;

		int min_val = 255;
		const int MAX_INTENSITY = 255;
		const int MIN_INTENSITY = 1;
		int max_thresh = 80;
		int min_thresh = 1;
		int distance = 0;


		for(int i =0; i<grayimg.rows; i++){
			for(int j = 0; j < grayimg.cols; j++){
				int current_val = grayimg.at<uchar>(i,j);
				if(current_val > max_thresh){
					grayimg.at<uchar>(i,j) = MAX_INTENSITY;					
					// min_val = current_val;
				}

				if (current_val < min_thresh){
					grayimg.at<uchar>(i,j) = MIN_INTENSITY;
				}	

			}
		}

		double minVal = 0;
		double maxVal = 0;
		double minAngle; 
		double maxAngle;

		cv::Point2i minIndex(0,0), maxIndex(0,0);		
		findMinMax(minVal, maxVal, minIndex, maxIndex, grayimg);

		findMinMaxAngle(minAngle, maxAngle, minIndex, maxIndex, grayimg);
		std::cout << "min index x coordinate: " << minVal << minIndex.x << minIndex.y;

		double linear_vel = 0.0;
		double angular_vel = 0.0;
		if(minVal > 39 && minVal < 81){
			linear_vel = 0.30;
		} else{
			linear_vel = 0.0;
		}

		if()


	}




};


















int main(int argc, char ** argv){

	ros::init(argc, argv, "object_detector");
	// Object_Detector od;
	ros::NodeHandle n;

	// TODO: put the name of the camera message type. 
	ros::Subscriber cam_depth_image_raw = n.subscribe("camera/depth/image_raw", 100, &camImageRawCallback);
	ros::Subscriber cam_depth_image = n.subscribe("camera/depth/image", 100, &camImageCallback);
	// ros::Subscriber cam_sub_depth_uv = n.subscribe("camera/depth/uv", 100, &camDepthCallback);
	ros::Subscriber cam_col_image = n.subscribe("camera/color/image_raw", 100, &camColorCallback);

	ros::Rate loop_rate(10);

	while(ros::ok()){


		ros::spinOnce();
		loop_rate.sleep();
	}


}