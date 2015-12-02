
#include <iostream>
#include <ros/ros.h>

#include <image_transport/image_transport.h>

#include <sensor_msgs/image_encodings.h>
#include <sensor_msgs/Image.h>

#include <cv_bridge/cv_bridge.h>
#include <ras_object_lib/image_converter.h>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>



using namespace std;
using namespace cv;

static const std::string OPENCV_WINDOW="Color Detection";
	int iLowH = 0;
 	int iHighH = 255;

	int iLowS = 0; 
	int iHighS = 255;
	int iLowV = 0;
	int iHighV = 255;

	cv::Mat imgHSV;
	cv::Mat imgThresholded;

void varyParamsCallback(int, void *){
	inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded);
	// cv::imshow("Thresholded Image", imgThresholded);

}



class ColorDetectRT{

public:
	ros::NodeHandle n;
	image_transport::ImageTransport it;
	image_transport::Subscriber image_sub;
	image_transport::Publisher image_pub;
	// cv::Mat imgHSV;
	// cv::Mat imgThresholded;
	// cv::Mat 
	// int iLowH = 0;
 // 	int iHighH = 250;

	// int iLowS = 0; 
	// int iHighS = 255;
	// int iLowV = 0;
	// int iHighV = 255;


	ColorDetectRT()
	: it(n)
	{
		image_sub = it.subscribe("/camera/rgb/image_raw", 3, &ColorDetectRT::videoCallback, this);
		image_pub = it.advertise("/image_converter/output_video", 1);

		cv::namedWindow(OPENCV_WINDOW);

	}


	~ColorDetectRT(){
		cv::destroyWindow(OPENCV_WINDOW);
	}


	// void lowHueCallback(int, void *){
	// 	inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded);


	// }


	void videoCallback(const sensor_msgs::ImageConstPtr & msg){
		cv_bridge::CvImagePtr cv_ptr;


		try{
			// cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
			cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
		}
		catch(cv_bridge::Exception& e){
			ROS_ERROR("cv bridge exception error: %s", e.what());
			return ;
		}

		cv::Mat srcimg = cv_ptr->image;
		// convert the bgr image into grayscale;
		cv::Mat grayimg;
		// cv::Mat imgHSV;
		// cv::Mat imgThresholded;


		cv::namedWindow("Control", CV_WINDOW_AUTOSIZE); //create a window called "Control"

		 //Create trackbars in "Control" window
		const int lowHueSliderMax = 255;
		const int highHueSliderMax = 255;
		const int lowSaturationSliderMax = 255;
		const int highSaturationSliderMax = 255;
		const int lowValueSliderMax = 255;
		const int highValueSliderMax = 255;


 		createTrackbar("LowH", "Control", &iLowH, lowHueSliderMax, varyParamsCallback); //Hue (0 - 179)
 		createTrackbar("HighH", "Control", &iHighH, highHueSliderMax, varyParamsCallback);

 		createTrackbar("LowS", "Control", &iLowS, lowSaturationSliderMax, varyParamsCallback); //Saturation (0 - 255)
 		createTrackbar("HighS", "Control", &iHighS, highSaturationSliderMax, varyParamsCallback);

 		createTrackbar("LowV", "Control", &iLowV, lowValueSliderMax, varyParamsCallback);//Value (0 - 255)
 		createTrackbar("HighV", "Control", &iHighV, highValueSliderMax, varyParamsCallback);

 		int iLastX = -1; 
 		int iLastY = -1;

 		//Capture a temporary image from the camera
 		Mat imgTmp;


		grayimg = cv::Mat(srcimg.size(), CV_8UC1);

		// cv::cvtColor(srcimg, grayimg, CV_BGR2GRAY);
		cv::cvtColor(srcimg, imgHSV, COLOR_BGR2HSV);

		cv::convertScaleAbs(srcimg, grayimg, 100, 0.0);

		cv::namedWindow( "Contours", CV_WINDOW_AUTOSIZE );
    	cv::imshow( "Contours", grayimg );
    	cv::waitKey(3);

    	inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded);

    	//morphological closing.
    	cv::erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5,5)));
    	cv::dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5,5)));


    	//morphological closing.
    	cv::dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5,5)));
    	cv::erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5,5)));


    	ros::Rate loop_rate(10);

    	// image_pub.publish(cv_ptr->toImageMsg());
    	cv::imshow("Thresholded Image", imgThresholded);

	}




};





int main(int argc, char ** argv){
	ros::init(argc, argv, "color_detect");

	// ros::NodeHandle n;
	ColorDetectRT  cdrt;
	ros::spin();
	return 0;

}
