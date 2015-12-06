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
#include <ras_msgs/Shape.h>


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

// params for hough circles 
int hough_threshold1 = 130;
int hough_threshold2 = 30;

// std::string color=ras_cv::RED;
// std::string blur_type = BLUR_NORMAL;
int morph_size = 5;
int blur_size = 5;

int mask_size_x = 3;
int mask_size_y = 3;

int min_area = 420;
int max_area = 9500;

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

//hough transform canny params
int threshold1 = 30;
int threshold2 = 150;


cv::Mat thres_img;
std::vector<cv::Mat> thres_imgs;
std::vector<cv::KeyPoint> key_points;
std::list<int> votelist;
std::list<int> votelist2;

int votes[7]={0};
int votes2[7]={0};
ros::Publisher shape_pub;



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


double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0){

	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);

}



void publishMessage(int shape, double xdist, double ydist){
	ras_msgs::Shape sid;
	sid.shape = shape;
	sid.x = xdist;
	sid.y = ydist;
	shape_pub.publish(sid);

}




int decisionRules(int *votes, int size){
	int finshape =2;
	std:cout << "looool";
	if(size != 7){
		return -1;
	}

	if(votes[1] >= 2){
		finshape = 1;
	}

//sphere
	if(votes[5] >= 3){
		finshape = 5;
	}

//prism
	if(votes[3] >= 2){
		finshape = 3;
	}

//cube
	if(votes[4] >= 4){
		finshape = 4;
	}


	return finshape;

}



void setLabel(cv::Mat& im, const std::string label, std::vector<cv::Point>& contour)
{
	int fontface = cv::FONT_HERSHEY_SIMPLEX;
	double scale = 0.4;
	int thickness = 1;
	int baseline = 0;

	cv::Size text = cv::getTextSize(label, fontface, scale, thickness, &baseline);
	cv::Rect r = cv::boundingRect(contour);

	cv::Point pt(r.x + ((r.width - text.width) / 2), r.y + ((r.height + text.height) / 2));
	cv::rectangle(im, pt + cv::Point(0, baseline), pt + cv::Point(text.width, -text.height), CV_RGB(255,255,255), CV_FILLED);
	cv::putText(im, label, pt, fontface, scale, CV_RGB(0,0,0), thickness, 8);
}



void detectShapes(cv::Mat box_img, cv::Mat &dst);


void detectShapes1(cv::Mat col_img,const cv::Mat &bw_img, cv::Mat &dst);



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
	printf ("Cloud: width = %d, height = %d\n", pr_img.rows, pr_img.cols);

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
	ras_cv::morph_tranform(blurred_img, morphed_img, ras_cv::CLOSE, cv::MORPH_RECT, morph_size);
	ras_cv::morph_tranform(blurred_img, blurred_img, ras_cv::OPEN, cv::MORPH_RECT, morph_size);
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


	if(key_points.size() >= 1){
		ROS_INFO("Distance from center: %f", pr_img.at<float>(240,320));
		// std::cout <<
		// ras_cv::writeMatrixAsString<cv::Vec3b>(hsv_img(ras_cv::get_bounding_box(key_points[0], pr_img.size[1], pr_img.size[0], 0.55)))
		// << std::endl;

	}

	cv::Mat dstshape;
	cv::Mat dstshape1;

	for(int i =0; i < key_points.size(); i++){

   		// cv::imshow(
     //    POINT_WINDOW_NAME + " " + ras_cv::writeAsString(i), 
     //    pr_img(ras_cv::get_bounding_box(key_points[i], pr_img.cols, pr_img.rows)));

        detectShapes(pr_img(ras_cv::get_bounding_box(key_points[i], pr_img.cols, pr_img.rows, 2.5)), dstshape);
        // detectShapes1(hsv_img(ras_cv::get_bounding_box(key_points[i], pr_img.cols, pr_img.rows)), fin_thres_img(ras_cv::get_bounding_box(key_points[i], pr_img.cols, pr_img.rows, 2.5)), dstshape1);


	}
	

  	cv::imshow(WINDOW_NAME, fin_img);
  	// cv::imshow(WINDOW_NAME, thres_img);
  	cv::waitKey(1);

}




void detectShapes1(cv::Mat col_img, const cv::Mat &bw_img, cv::Mat &dst){

	cv::Mat tempimg;
	// tempimg = cv::Scalar(255, 255, 255) - bw_img;
	std::vector<std::vector<cv::Point> > contours;
	cv::Mat bwCannyImg;
	int threshold1 = 30;
	int threshold2 = 150;

	cv::cvtColor(bw_img, tempimg, CV_RGB2GRAY);
	cv::Canny(col_img, bwCannyImg, threshold1, threshold2, 3);

	cv::findContours(tempimg, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);


	std::vector<cv::Point> approx;

	dst = bw_img.clone();
	cv::Mat bwtempimg = cv::Scalar(255, 255, 255) - bw_img;
	cv::cvtColor(bwtempimg, bwtempimg, CV_RGB2GRAY);
	int numCircles1;
	int shape = 5;


	for (int i = 0; i < contours.size(); i++)
	{
		// Approximate contour with accuracy proportional
		// to the contour perimeter

		if(votelist.size() >= 8){
			int finshape = decisionRules(votes, 7);
			double area = cv::contourArea(contours[i]);
			double distance =  1100/area;
			double angle = -10.0;

			//empty votes and votelist
			while(!votelist.empty()){
        		votelist.pop_back();        				
        	}	

        	for(int i = 0;i < 7; i++){
        		votes[i] = 0; 
        	}

			publishMessage(finshape, distance, angle);
			//publish message here for shape and area 
		}

		numCircles1 = ras_cv::findHoughCircles(col_img, hough_threshold1, hough_threshold2);
		// std::cout << numCircles1;
// 
		cv::approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true)*0.02, true);

		// Skip small or non-convex objects 
		if (std::fabs(cv::contourArea(contours[i])) < 80 || !cv::isContourConvex(approx))
			continue;


		// cv::Mat houghimg = hsv_img(ras_cv::get_bounding_box(key_points[i],  pr_img.cols, pr_img.rows, 2.50));
        int numCircles = ras_cv::findHoughCirclesbw(bwtempimg, hough_threshold1, hough_threshold2);

        double area = cv::contourArea(contours[i]);
		cv::Rect r = cv::boundingRect(contours[i]);
		int radius = r.width / 2;


        std::cout << numCircles << "\n";
        // if(numCircles > 0){
        // 	setLabel(dst, "CIR", contours[i]);
        // 	std::cout <"circle detected";
        // 	shape = 5;
        // 	votelist.push_back(shape);
        // 	continue;
        // }


		if (approx.size() == 3)
		{
			shape = 1;
			std::cout << "prism detected";
			// std:cout << "lololol";
			votelist.push_back(shape);
			setLabel(dst, "TRI", contours[i]);    // Triangles

		}


		if (std::abs(1 - ((double)r.width / r.height)) <= 0.2 &&
			std::abs(1 - (area / (CV_PI * std::pow(radius, 2)))) <= 0.2){
			setLabel(dst, "CIR", contours[i]);
			cout <<"circle detected";
			shape = 5;
			votes[shape]++;
			votelist.push_back(shape);

			continue;

		}



		if (approx.size() >= 4 && approx.size() <= 6)
		{
			// Number of vertices of polygonal curve
			int vtc = approx.size();

			// Get the cosines of all corners
			std::vector<double> cos;
			for (int j = 2; j < vtc+1; j++)
				cos.push_back(angle(approx[j%vtc], approx[j-2], approx[j-1]));

			// Sort ascending the cosine values
			std::sort(cos.begin(), cos.end());

			// Get the lowest and the highest cosine
			double mincos = cos.front();
			double maxcos = cos.back();

			// Use the degrees obtained above and the number of vertices
			// to determine the shape of the contour

			//pentagon
			if(mincos >= -0.34 && maxcos <= -0.27){
				setLabel(dst, "PENTA", contours[i]);
				shape = 3;				
			} else if(mincos >= -0.55 && maxcos <= -0.45){
				setLabel(dst, "HEXA", contours[i]);
				shape = 6;
			} else if(vtc == 4 && mincos >= -0.1 && maxcos <= 0.3){
				setLabel(dst, "CUBE", contours[i]);
				//cube dtected
				shape = 2;
			}
			votelist.push_back(shape);
			
		}
		else
		{
			// Detect and label circles
			double area = cv::contourArea(contours[i]);
			cv::Rect r = cv::boundingRect(contours[i]);
			int radius = r.width / 2;

			if (std::abs(1 - ((double)r.width / r.height)) <= 0.2 &&
			    std::abs(1 - (area / (CV_PI * std::pow(radius, 2)))) <= 0.2)
				setLabel(dst, "CIR", contours[i]);
		}

		votes[shape]++;
	}

	cv::imshow("dst", dst);

}



// shape detection function

void detectShapes(cv::Mat box_img, cv::Mat &dst){
	cv::Mat bw_img;
	int threshold1 = 30;
	int threshold2 = 150;

	cv::Canny(box_img, bw_img, threshold1, threshold2, 3);


	std::vector<std::vector<cv::Point> > contours;
	cv::findContours(bw_img.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	std::vector<cv::Point> approx;

	dst = box_img.clone();
	int shape = 5;

	for(int i =0; i < contours.size(); i++){
		cv::approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true) * 0.02, true);

       if ( !cv::isContourConvex(approx)){
       		// std::cout <<  
       		// std::cout << "no" << "\n"; 
			continue;
       }


       if(votelist2.size() >= 8){
			int finshape = decisionRules(votes2, 7);
			double area = cv::contourArea(contours[i]);
			double distance =  1100/area;
			double angle = -10.0;

			//empty votes and votelist
			while(!votelist2.empty()){
        		votelist2.pop_back();        				
        	}	

        	for(int i = 0;i < 7; i++){
        		votes2[i] = 0; 
        	}

			publishMessage(finshape, distance, angle);
			//publish message here for shape and area 
		}


		std::fabs(cv::contourArea(contours[i]));
       	
       	std::cout << "no of  corner kepoints" << approx.size() << "\n";
       if(approx.size() == 3){
       	 // annote this image with the label.
       		std::cout << "triangle detected";
       		shape = 1;
       		votes2[shape]++;
       		votelist2.push_back(shape);
       		continue;
       }	

       int numCircles1 = ras_cv::findHoughCircles(box_img, hough_threshold1, hough_threshold2);
       if(numCircles1 > 0){
       		shape = 5;
       		votes2[shape]++;
       		continue; 
       }

       if(approx.size() == 4){
       	  std::cout << "cube detected";
       	  shape = 2;
       }

       if(approx.size() == 5 || approx.size() == 10){
       		std::cout << "star detected";
       		shape = 5;
       }

       if(approx.size() > 12){
       	     std::cout << "sphere detected";
       	     shape = 5;
       }

       votelist2.push_back(shape);
       votes2[shape]++;
        // continue;
	}

}





int main(int argc, char ** argv){

	// cout << "Hello People";	
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

	
	ros::init(argc, argv, "shape_detection");

	ros::NodeHandle node("~");
	ros::Subscriber sub = node.subscribe("/camera/rgb/image_raw", MAX_BUFFER, tuneCallback);
	shape_pub = node.advertise<ras_msgs::Shape>("/object/shape", 1);

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