#include <ros/ros.h>
#include <sensor_msgs/image_encodings.h>

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
bool blob_tune = true;
bool morph_tune = true;
bool blur_tune = false;
std::string blur_type = BLUR_GAUSSIAN;
std::string color_tune_type="hue";
int hl_def = 0;
int hh_def = 255;
int vl_def = 0;
int vh_def = 255;
int sl_def = 0;
int sh_def = 255;

bool tranform_to_hsv = true;
std::string morph_type = ras_cv::NO_MORPH;
std::string color=ras_cv::ORANGE;
// std::string color=ras_cv::GREEN_FL;
// std::string blur_type = BLUR_NORMAL;
int morph_size = 5;
int blur_size = 5;

int mask_size_x = 3;
int mask_size_y = 3;

int min_area = 300;
int max_area = 4800;

bool circular = false;
bool filter_ratio = false;
bool filter_by_area = true;
bool filter_by_convexity = true;

int min_circularity = 25;
// int min_inertia = 55;
int min_convexity = 35;
int min_inertia_ratio = 25;


int min_distance_between_blobs= 50;
cv::SimpleBlobDetector::Params params;
cv::Mat thres_img;
std::vector<cv::KeyPoint> key_points;


static const int ROWS = 1;
static const int COLS = 8;
static const int X_START = 50;
static const int Y_START = 0;
static const int X_SIZE = 120;
static const int Y_SIZE = 120;
static const int X_OFF = 5;
static const int Y_OFF = 0;

// void blobTuneCallback(int, void*){
// 	params.minThreshold = 10;
// 	params.maxThreshold = 200;

// 	params.minDistBetweenBlobs = min_distance_between_blobs;
// 	params.filterByArea = filter_by_area;
// 	params.minArea = std::max(1, min_area);
//   	params.maxArea = std::max(min_area, max_area);
	
// 	params.filterByCircularity = circular;
// 	params.minCircularity = min_circularity/ 100.;

// 	params.filterByConvexity = filter_by_convexity;
// 	params.minConvexity = min_convexity/ 100.;

// 	params.filterByInertia = true; 
// 	params.minInertiaRatio = min_inertia_ratio/ 100.;

// 	cv::SimpleBlobDetector detector(params);
// 	detector.detect(thres_img, key_points);

// }


void colorTuneCallback(){

}

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


	if(color_tune){
		cv::inRange(hsv_img, cv::Scalar(hl_def, sl_def, vl_def), cv::Scalar(hh_def, sh_def, vh_def), thres_img);
	}else{
		ras_cv::thresholdColor(hsv_img, thres_img, color);
	}

	cv::SimpleBlobDetector detector(params);
	detector.detect(thres_img, key_points);


	cv::drawKeypoints(thres_img, key_points, fin_img, ras_cv::SCALAR_RED,  cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);


	std::cout << "Number of detected key points:" << key_points.size() << "\n";

	for(int i =0; i < key_points.size(); i++){
		// ras_cv::get_bounding_box(key_points[i], 11, 11);
		std::cout << "yaya";
		// cv::imshow(
   		cv::imshow(
        POINT_WINDOW_NAME + " " + ras_cv::writeAsString(i), 
        pr_img(ras_cv::get_bounding_box(key_points[i], pr_img.cols, pr_img.rows)));

	}

  	// cv::imshow(WINDOW_NAME, fin_img);
  	cv::imshow(WINDOW_NAME, fin_img);
  	cv::waitKey(1);
	// Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);

}



// class Object_Detection_Tune{

// public:
// 	ros::NodeHandle n_;
// 	image_transport::Subscriber image_sub; 


// 	Object_Detection_Tune()
// 	: it(n)
// 	{
// 		image_sub = it.subscribe("/camera/rgb/image_raw", MAX_BUFFER, &Object_Detection_Tune:callback, this);
		// image_pub = it.advertise("/image_converter/output_video", 1);
// 		cv::NamedWindow(WINDOW_NAME);

// 	}

// };



int main(int argc, char ** argv){

	cout << "Hello People";	
	
	// cv::namedWindow(WINDOW_NAME);

	ras_cv::create_windows(POINT_WINDOW_NAME, ROWS, COLS, X_START, Y_START, X_SIZE, Y_SIZE, X_OFF, Y_OFF);
	cv::namedWindow(WINDOW_NAME, CV_WINDOW_AUTOSIZE);
  	cv::moveWindow(WINDOW_NAME, 400, 250);
  	cv::namedWindow(TRACKBAR_WINDOW, CV_WINDOW_NORMAL);
  	cv::moveWindow(TRACKBAR_WINDOW, 1050, 250);
	
	ros::init(argc, argv, "object_detection_tune");

	ros::NodeHandle node("~");
	ros::Subscriber sub = node.subscribe("/camera/rgb/image_raw", MAX_BUFFER, tuneCallback);

	ros::Rate loop_rate(LOOP_RATE);


	// node.param("morph_tune", morph_tune, false);
	// node.param<std::string>("morph_type", morph_type, ras_cv::DILATE);
	// node.param("blur_tune", blur_tune, false);
	// node.param<std::string>("blur_type", blur_type, BLUR_NORMAL);
	// node.param("color_tune", color_tune, false);
	// node.param<std::string>("color", color, ras_cv::BLUE_DARK);
	// node.param("convert_to_hsv", tranform_to_hsv, false);
	// node.param("blob_tune", blob_tune, true);
	// node.param("color_tune_type", color_tune_type, ras_cv::RAS_CV_HUE);


	bool morph_tune = true;

	
	if(blob_tune){
		cv::createTrackbar("min area", TRACKBAR_WINDOW, &min_area, 1000, NULL);
		cv::createTrackbar("max area", TRACKBAR_WINDOW, &max_area, 10000, NULL);
		cv::createTrackbar("min circularity", TRACKBAR_WINDOW, &min_circularity, 100, NULL);		
		cv::createTrackbar("min distance", TRACKBAR_WINDOW, &min_distance_between_blobs, 800, NULL);
		cv::createTrackbar("min convexity", TRACKBAR_WINDOW, &min_convexity, 100, NULL);
		cv::createTrackbar("min inertia", TRACKBAR_WINDOW, &min_inertia_ratio, 100, NULL);
	
	}

		cv::createTrackbar("Morph size", "morph_tune", &morph_size, 31, NULL);


	if(morph_tune){
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

