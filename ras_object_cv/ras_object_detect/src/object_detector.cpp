#include <ros/ros.h>
#include <sensor_msgs/image_encodings.h>
#include <ras_object_lib/Image_Transfer.h>
#include <ras_msgs/Object_id.h>

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
#include <ras_object_lib/Image_Transfer.h>


#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cmath>
#include <queue>
#include <list>


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


std::string blur_type = BLUR_NORMAL;
std::string color_tune_type="hue";

int hl_def = 0;
int hh_def = 255;
int vl_def = 0;
int vh_def = 255;
int sl_def = 0;
int sh_def = 255;
int global_counter = 0;

bool tranform_to_hsv = true;
std::string morph_type_1 = ras_cv::CLOSE;
std::string morph_type_2 = ras_cv::OPEN;
std::string color=ras_cv::GREEN_DARK;
std::vector<std::string> object_colors;



// morphing params
double sigma = 5.0;
// std::string color=ras_cv::RED;
// std::string blur_type = BLUR_NORMAL;
int morph_size = 5;
int blur_size = 7;

int mask_size_x = 3;
int mask_size_y = 3;

int min_area = 500;
int max_area = 10500;

bool circular = false;
bool filter_ratio = false;
bool filter_by_area = true;
bool filter_by_convexity = true;

int min_circularity = 15;
// int min_inertia = 55;
int min_convexity = 15;
int min_inertia_ratio = 15;
bool detect_all = true;
int votes[7] = {0};
int shape_votes[7] = {0};
std::list<int> votelist;
std::list<int> shapevotelist;


int min_distance_between_blobs= 60;


// params for hough circles 
int hough_threshold1 = 130;
int hough_threshold2 = 30;
cv::SimpleBlobDetector::Params params;
cv::Mat thres_img;

std::vector<cv::Mat> thres_imgs;
std::vector<cv::KeyPoint> key_points;

// client for the classifier service.
ros::ServiceClient client_color;
ros::ServiceClient client_shape;
ros::ServiceClient client_material;
ros::Publisher object_pub;
ros::Publisher  obj_img_pub;


static const int ROWS = 1;
static const int COLS = 8;
static const int X_START = 50;
static const int Y_START = 0;
static const int X_SIZE = 120;
static const int Y_SIZE = 120;
static const int X_OFF = 5;
static const int Y_OFF = 0;


cv::Mat ros_depth_img;

void colorTuneCallback(){


}


ras_object_lib::Image_Transfer  transferImage(const cv::Mat& cvimg, int bw){
	sensor_msgs::ImagePtr rosimg;
	ras_cv::to_ros_copy(cvimg, rosimg, bw);
	ras_object_lib::Image_Transfer imgtransfer;
	imgtransfer.request.im = *rosimg;
	return imgtransfer;
}


void sendMessageToSpeaker(int col_index, int object_index, int shape_index){
	ras_msgs::Object_id oid;
    oid.object = object_index;
    oid.color = col_index;
    oid.shape = shape_index;
    oid.time = 1000;
    oid.x = 1;
    oid.y = 10;
    object_pub.publish(oid);

}



int decisiontree(int color_index, int shape_index ){
	// check if it is a circle/spehere
	int object_index = 100;

	if(shape_index == 1){
		if(color_index != 6 && color_index != 3){
			// red  ball
			object_index = 6;
		}else{
			// yellow ball
			object_index = 11; 
			return object_index;
		}
	}

	if(color_index == 6 && shape_index != 1){
		// yellow cube
		object_index = 9;
	}

	if(color_index == 0){
		// dark green cube
		object_index = 4;
	}

	if(color_index == 4){
		object_index = 5;
	}

	if(color_index == 5){
		// orange star
		object_index = 10;
	}

	return object_index;

}




void depthCallback(const sensor_msgs::ImageConstPtr& depthimg){
	ras_cv::to_cv_copy_depth(ros_depth_img, depthimg);

}


void  tuneCallback(const sensor_msgs::ImageConstPtr& inimg){

	params.minThreshold = 10;
	params.maxThreshold = 170;

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
	int circularObject = 0;

	std::vector<cv::KeyPoint> key_points;
	// std::list<std::string> votelist;
	

	ras_cv::to_cv_copy(&pr_img, inimg);

	cv::cvtColor(pr_img, grayimg, CV_BGR2GRAY);


	// if(blur_type == BLUR_GAUSSIAN){
	// 	cv::GaussianBlur(pr_img, blurred_img, cv::Size(std::max(1, blur_size), std::max(1, blur_size)), 0, 0);
	// } else if(blur_type == BLUR_MEDIAN){
	// 	cv::medianBlur(pr_img, blurred_img, blur_size);
	// } else if(blur_type == BLUR_NORMAL){
	// 	cv::blur(pr_img, blurred_img, cv::Size(blur_size, blur_size), cv::Point(-1, -1));
	// } else{
	// 	blurred_img = pr_img;
	// }


	ras_cv::Blur blur_params = 	ras_cv::Blur(blur_size, blur_size, sigma);


	ras_cv::blur(pr_img, blurred_img, blur_type, blur_params);

	// ras_cv::morph_tranform(blurred_img, morphed_img, ras_cv::OPEN, cv::MORPH_RECT, morph_size);
	ras_cv::morph_tranform(blurred_img, morphed_img, morph_type_1, cv::MORPH_RECT, morph_size);
	ras_cv::morph_tranform(blurred_img, blurred_img, morph_type_2, cv::MORPH_RECT, morph_size);
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

	cv::Mat t1img[9];
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

	global_counter += 1;

	if(key_points.size() >= 1){
		// write the pixel values in the bounding box as a text file ....
		// std::cout <<
		// ras_cv::writeMatrixAsString<cv::Vec3b>(hsv_img(ras_cv::get_bounding_box(key_points[0], pr_img.size[1], pr_img.size[0], 0.55)))
		// << std::endl;

	}


  	for(int i =0; i < key_points.size(); i++){

  		sensor_msgs::ImagePtr sensimg;

  		ras_cv::to_ros_copy(pr_img(ras_cv::get_bounding_box(key_points[i],  pr_img.cols, pr_img.rows)), sensimg, 10);

  		obj_img_pub.publish(sensimg);	

   		// cv::imshow(
     //    POINT_WINDOW_NAME + " " + ras_cv::writeAsString(i), 
     //    pr_img(ras_cv::get_bounding_box(key_points[i],  pr_img.cols, pr_img.rows)));


        ras_object_lib::Image_Transfer  it = transferImage(pr_img(ras_cv::get_bounding_box(key_points[i], pr_img.cols, pr_img.rows, 0.60)), 0);
        // ras_object_lib::Image_Transfer  it_shape = transferImage(pr_img(ras_cv::get_bounding_box(key_points[i], pr_img.cols, pr_img.rows, 1.50)));
        // ras_object_lib::Image_Transfer  it_material = transferImage(pr_img(ras_cv::get_bounding_box(key_points[i], pr_img.cols, pr_img.rows, 1.50)));

        std::vector<string> colors_in_frames;
        int shape_index;

        if(client_color.call(it)){
        	if(votelist.size() >= 10){
        		for (std::list<int>::const_iterator it = votelist.begin(), end = votelist.end(); it != end; ++it){
        			int index = *it;
        			votes[index]++;
        		}

        		std::cout << "hiii";
        		std:cout << votes;
        		int col_index = ras_cv::argmax(votes, 7);
        		int maxval = ras_cv::maxval(votes, 7);

        		if(maxval >= 7){
        		while(!votelist.empty()){
        			votelist.pop_back();        				
        		}

        		for(int j =0; j< 7; j++){
        			votes[j] =0;
        		}


        			// some color detected yay !!!!!!
        			// publish here ..........

        			int object_index = decisiontree(col_index, shape_index);
        			sendMessageToSpeaker(col_index, 1, 1);
        			std::cout << col_index << "\n";
        			std::cout << "some color" << "\n";
        		}
        	}


        	int number = atoi(it.response.str.c_str());
        	// it.response.str << number;
        	votelist.push_back(number);
        	std::cout << it.response.str << "\n";
        	std::cout << votelist.size() << "\n";
        	string color = it.response.str;
        // 	// client_color.call(it);
        }

	}

	
  	// cv::imshow(WINDOW_NAME, fin_img);
  	cv::waitKey(1);

}








int main(int argc, char ** argv){
	// cout << "Hello People";	
	
	// use this part of code only for debugging...
	// ras_cv::create_windows(POINT_WINDOW_NAME, ROWS, COLS, X_START, Y_START, X_SIZE, Y_SIZE, X_OFF, Y_OFF);
	// cv::namedWindow(WINDOW_NAME, CV_WINDOW_AUTOSIZE);
 //  	cv::moveWindow(WINDOW_NAME, 400, 250);
 //  	cv::namedWindow(TRACKBAR_WINDOW, CV_WINDOW_NORMAL);
 //  	cv::moveWindow(TRACKBAR_WINDOW, 1050, 250);


  	// fill the vector of colors
  	object_colors.push_back(ras_cv::GREEN_DARK);
  	object_colors.push_back(ras_cv::GREEN_FL);
  	object_colors.push_back(ras_cv::BLUE_DARK);
  	object_colors.push_back(ras_cv::BLUE_LIGHT);
  	object_colors.push_back(ras_cv::RED1);
  	object_colors.push_back(ras_cv::RED2);

  	object_colors.push_back(ras_cv::VIOLET);
  	object_colors.push_back(ras_cv::YELLOW);
  	object_colors.push_back(ras_cv::ORANGE);

	
	ros::init(argc, argv, "object_detection_tune");

	ros::NodeHandle node("~");

	// first take the depth image and then the raw image later.
	ros::Subscriber sub_depth = node.subscribe("/camera/depth_registered/image_raw", MAX_BUFFER, depthCallback);
	ros::Subscriber sub_image = node.subscribe("/camera/rgb/image_raw", MAX_BUFFER, tuneCallback);

  	client_color = node.serviceClient<ras_object_lib::Image_Transfer>("/classify_objects/color");
  	// client_shape = node.serviceClient<ras_object_lib::Image_Transfer>("/classify_objects/shape");

  	object_pub = node.advertise<ras_msgs::Object_id>("/object/color", 1);
  	obj_img_pub = node.advertise<sensor_msgs::Image>("/object/small_img", 1);


	ros::Rate loop_rate(LOOP_RATE);


	bool morph_tune = true;

	ros::spin();
	return 0;

}