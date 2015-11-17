#include <ros/ros.h>
#include <sensor_msgs/image_encodings.h>
#include <ras_object_lib/Image_Transfer.h>

// #include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/photo/photo.hpp>


#include <ras_object_lib/image_converter.h>
#include <ras_object_lib/image_utils.h>
#include <ras_object_lib/image_color_range.h>
#include <ras_object_lib/image_morph.h>


#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cmath>



using namespace std;
// using namespace cv;



// @TODO:  move all these blur constants to a header file in the cv library package.


// static const std::string WINDOW_NAME = "window1";

static const std::string BLUR_GAUSSIAN = "gaussian";
static const std::string BLUR_NORMAL = "blur";
static const std::string BLUR_MEDIAN = "median";
static const std::string POINT_WINDOW_NAME="tuneparams";
static const std::string WINDOW_NAME="masked image";
static const std::string TRACKBAR_WINDOW="Trackbars";

const int LOOP_RATE=10;
const int MAX_BUFFER= 5;

bool color_tune = false;
bool blob_tune = false;
bool morph_tune = false;
bool blur_tune = false;
bool detect_all = true;
std::string blur_type = BLUR_NORMAL;
std::string color_tune_type="hue";
int hl_def = 0;
int hh_def = 255;
int vl_def = 0;
int vh_def = 255;
int sl_def = 0;
int sh_def = 255;

bool tranform_to_hsv = true;
std::string morph_type = ras_cv::NO_MORPH;
std::string color=ras_cv::GREEN_DARK;
std::vector<std::string> object_colors;

// std::string color=ras_cv::RED;
// std::string blur_type = BLUR_NORMAL;
int morph_size = 5;
int blur_size = 5;

int mask_size_x = 3;
int mask_size_y = 3;

int min_area = 220;
int max_area = 7500;

bool circular = false;
bool filter_ratio = false;
bool filter_by_area = true;
bool filter_by_convexity = true;

int min_circularity = 15;
// int min_inertia = 55;
int min_convexity = 15;
int min_inertia_ratio = 15;


int min_distance_between_blobs= 50;
cv::SimpleBlobDetector::Params params;
cv::Mat thres_img;
std::vector<cv::Mat> thres_imgs;
std::vector<cv::KeyPoint> key_points;


static const int ROWS = 1;
static const int COLS = 8;
static const int X_START = 50;
static const int Y_START = 0;
static const int X_SIZE = 120;
static const int Y_SIZE = 120;
static const int X_OFF = 5;
static const int Y_OFF = 0;


void colorTuneCallback(){

}

void detectShapes(const cv::Mat &box_img, cv::Mat &dst);

void  tuneCallback(const sensor_msgs::ImageConstPtr& inimg){

	params.minThreshold = 10;
	params.maxThreshold = 200;

	params.minDistBetweenBlobs = min_distance_between_blobs;
	params.filterByArea = filter_by_area;
	params.minArea = std::max(1, min_area);
  	params.maxArea = std::max(min_area, max_area);
	
	params.filterByCircularity = circular;
	params.minCircularity = min_circularity/ 100.;

	params.filterByConvexity = filter_by_convexity;
	params.minConvexity = min_convexity/ 100.;

	params.filterByInertia = true; 
	params.minInertiaRatio = min_inertia_ratio/ 100.;

	cv::Mat grayimg;
	cv::Mat pr_img;
	cv::Mat blurred_img;
	cv::Mat morphed_img;
	cv::Mat hsv_img;
	// cv::Mat thres_img;
	cv::Mat fin_img;

	std::vector<cv::KeyPoint> key_points;

	ras_cv::to_cv_copy(&pr_img, inimg);

	cv::cvtColor(pr_img, grayimg, CV_BGR2GRAY);


	if(blur_type == BLUR_GAUSSIAN){
		cv::GaussianBlur(pr_img, blurred_img, cv::Size(std::max(1, blur_size), std::max(1, blur_size)), 0, 0);
	} else if(blur_type == BLUR_MEDIAN){
		cv::medianBlur(pr_img, blurred_img, blur_size);
	} else if(blur_type == BLUR_NORMAL){
		cv::blur(pr_img, blurred_img, cv::Size(blur_size, blur_size), cv::Point(-1, -1));
	} else{
		blurred_img = pr_img;
	}


	ras_cv::morph_tranform(blurred_img, morphed_img, morph_type, cv::MORPH_RECT, morph_size);
	// detector.detect(grayimg, key_points);

	if(tranform_to_hsv){
		cv::cvtColor(morphed_img, hsv_img, cv::COLOR_BGR2HSV);
	}else{
		hsv_img = morphed_img;
	}


	cv::Mat thres_img_3;
	if(color_tune){
		cv::inRange(hsv_img, cv::Scalar(hl_def, sl_def, vl_def), cv::Scalar(hh_def, sh_def, vh_def), thres_img);
	}else{
		ras_cv::thresholdColor(hsv_img, thres_img, color);
		cv::cvtColor(thres_img, thres_img_3, CV_GRAY2BGR);
	}

	cv::Mat t1img[8];
	int cnt = 0;
	cv::Mat fin_thres_img;
	// cv::Mat fin_img;
	fin_thres_img = thres_img_3;
	cv::Mat thres_img_3_temp;

	for(std::vector<string>::iterator it = object_colors.begin(); it != object_colors.end(); ++it){
		ras_cv::thresholdColor(hsv_img, t1img[cnt], *it);
		cv::cvtColor(t1img[cnt], thres_img_3_temp, CV_GRAY2BGR);
		// cv::cvtColor(t1img[cnt], t1img[cnt], CV_GRAY2BGR);
		cnt++;
		cv::bitwise_or(fin_thres_img, thres_img_3_temp, fin_thres_img);
	}



	thres_img =  cv::Scalar(255, 255, 255) - thres_img;
	fin_thres_img = cv::Scalar(255, 255, 255) - fin_thres_img;
	if(detect_all == false){
		fin_thres_img = thres_img;
	}

	cv::SimpleBlobDetector detector(params);
	detector.detect(fin_thres_img, key_points);


	cv::drawKeypoints(fin_thres_img, key_points, fin_img, ras_cv::SCALAR_RED,  cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);


	// std::cout << "Number of detected key points:" << key_points.size() << "\n";


	if(key_points.size() >= 1){
		// std::cout <<
		// ras_cv::writeMatrixAsString<cv::Vec3b>(hsv_img(ras_cv::get_bounding_box(key_points[0], pr_img.size[1], pr_img.size[0], 0.55)))
		// << std::endl;

	}

	cv::Mat dstshape;

	for(int i =0; i < key_points.size(); i++){

   		cv::imshow(
        POINT_WINDOW_NAME + " " + ras_cv::writeAsString(i), 
        pr_img(ras_cv::get_bounding_box(key_points[i], pr_img.cols, pr_img.rows)));

        detectShapes(pr_img(ras_cv::get_bounding_box(key_points[i], pr_img.cols, pr_img.rows)), dstshape);
	}

  	cv::imshow(WINDOW_NAME, fin_img);
  	// cv::imshow(WINDOW_NAME, thres_img);
  	cv::waitKey(1);

}



void detectShapes(const cv::Mat &box_img, cv::Mat &dst){
	cv::Mat bw_img;
	int threshold1 = 10;
	int threshold2 = 160;

	cv::Canny(box_img, bw_img, threshold1, threshold2, 3);


	std::vector<std::vector<cv::Point> > contours;
	cv::findContours(bw_img.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	std::vector<cv::Point> approx;

	dst = box_img.clone();

	for(int i =0; i < contours.size(); i++){
		cv::approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true) * 0.02, true);

       if ( !cv::isContourConvex(approx)){
       		// std::cout <<  
       		std::cout << "no" << "\n"; 
			continue;
       }


		std::fabs(cv::contourArea(contours[i]));
       	
       	std::cout << "no of  corner kepoints" << approx.size() << "\n";
       if(approx.size() == 3){
       	 // annote this image with the label.
       		std::cout << "triangle detected";
       }

       if(approx.size() == 4){
       	  std::cout << "cube detected";
       }

       if(approx.size() == 8 || approx.size() == 12){
       		std::cout << "cross detected";
       }

       if(approx.size() == 5 || approx.size() == 10){
       		std::cout << "star detected";
       }

       if(approx.size() > 12){
       	     std::cout << "sphere detected";
       }
        // continue;

	}



}


int main(int argc, char ** argv){

	// cout << "Hello People";	
	

	ras_cv::create_windows(POINT_WINDOW_NAME, ROWS, COLS, X_START, Y_START, X_SIZE, Y_SIZE, X_OFF, Y_OFF);
	cv::namedWindow(WINDOW_NAME, CV_WINDOW_AUTOSIZE);
  	cv::moveWindow(WINDOW_NAME, 400, 250);
  	cv::namedWindow(TRACKBAR_WINDOW, CV_WINDOW_NORMAL);
  	cv::moveWindow(TRACKBAR_WINDOW, 1050, 250);

  	// fill the vector of colors
  	object_colors.push_back(ras_cv::GREEN_DARK);
  	object_colors.push_back(ras_cv::GREEN_FL);
  	object_colors.push_back(ras_cv::BLUE_DARK);
  	object_colors.push_back(ras_cv::BLUE_LIGHT);
  	object_colors.push_back(ras_cv::RED);
  	object_colors.push_back(ras_cv::VIOLET);
  	object_colors.push_back(ras_cv::YELLOW);
  	object_colors.push_back(ras_cv::ORANGE);

	
	ros::init(argc, argv, "object_detection_tune");

	ros::NodeHandle node("~");
	ros::Subscriber sub = node.subscribe("/camera/rgb/image_raw", MAX_BUFFER, tuneCallback);

	ros::Rate loop_rate(LOOP_RATE);


	// node.param("morph_tune", morph_tune, true);
	// node.param<std::string>("morph_type", morph_type, ras_cv::DILATE);
	// node.param("blur_tune", blur_tune, true);
	// node.param<std::string>("blur_type", blur_type, BLUR_NORMAL);
	// node.param("color_tune", color_tune, false);
	// node.param<std::string>("color", color, ras_cv::BLUE_DARK);
	// node.param("convert_to_hsv", tranform_to_hsv, true);
	// node.param("blob_tune", blob_tune, true);
	// node.param("color_tune_type", color_tune_type, ras_cv::RAS_CV_HUE);


	bool morph_tune = true;
	// bool color_tune = true;

	
	if(blob_tune){
		cv::createTrackbar("min area", TRACKBAR_WINDOW, &min_area, 1000, NULL);
		cv::createTrackbar("max area", TRACKBAR_WINDOW, &max_area, 10000, NULL);
		cv::createTrackbar("min circularity", TRACKBAR_WINDOW, &min_circularity, 100, NULL);		
		cv::createTrackbar("min distance", TRACKBAR_WINDOW, &min_distance_between_blobs, 800, NULL);
		cv::createTrackbar("min convexity", TRACKBAR_WINDOW, &min_convexity, 100, NULL);
		cv::createTrackbar("min inertia", TRACKBAR_WINDOW, &min_inertia_ratio, 100, NULL);
	
	}

	if(morph_tune){
		cv::createTrackbar("Morph size", TRACKBAR_WINDOW, &morph_size, 31, NULL);
	}


	if(blur_tune){
		if(blur_type == BLUR_NORMAL){
			cv::createTrackbar("blur size", TRACKBAR_WINDOW, &blur_size, 31, NULL);	
		} else if(blur_type == BLUR_GAUSSIAN){
			cv::createTrackbar("blur size", TRACKBAR_WINDOW, &blur_size, 31, NULL);
		}
	}

		cv::createTrackbar("HL", TRACKBAR_WINDOW, &hl_def, 180, NULL);
		cv::createTrackbar("HH", TRACKBAR_WINDOW, &hh_def, 180, NULL);
		cv::createTrackbar("SL", TRACKBAR_WINDOW, &sl_def, 255, NULL);
		cv::createTrackbar("SH", TRACKBAR_WINDOW, &sh_def, 255, NULL);
		cv::createTrackbar("VL", TRACKBAR_WINDOW, &vl_def, 255, NULL);
		cv::createTrackbar("VH", TRACKBAR_WINDOW, &vh_def, 255, NULL);


	// if(color_tune){
	// 	if(color_tune_type == "hue"){
	// 		cv::createTrackbar("HL", TRACKBAR_WINDOW, &hl_def, 180, NULL);
	// 		cv::createTrackbar("HH", TRACKBAR_WINDOW, &hh_def, 255, NULL);
	// 	} else if(color_tune_type == "sat"){
	// 		cv::createTrackbar("SL", TRACKBAR_WINDOW, &sl_def, 50, NULL);
	// 		cv::createTrackbar("SH", TRACKBAR_WINDOW, &sh_def, 255, NULL);
	// 	} else if(color_tune_type == "val"){
	// 		cv::createTrackbar("VL", TRACKBAR_WINDOW, &vl_def, 50, NULL);
	// 		cv::createTrackbar("VH", TRACKBAR_WINDOW, &vh_def, 255, NULL);
	// 	}
	// }
             
	ros::spin();
	return 0;

}