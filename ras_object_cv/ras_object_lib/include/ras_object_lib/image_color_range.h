
#ifndef COLOR_RANGE
#define COLOR_RANGE


#include <opencv2/imgproc/imgproc.hpp>
#include <string>


namespace ras_cv{

	const int color_count = 7;
	static const  std::string RED = "red";
	static const  std::string BLUE_LIGHT = "blue_light";
	static const  std::string BLUE_DARK = "blue_dark";
	static const  std::string GREEN_DARK = "green_dark";
	static const  std::string GREEN_FL = "green_fl";
	static const  std::string VIOLET = "violet";
	static const  std::string ORANGE  = "orange";
	static const  std::string YELLOW = "yellow";
	static const  std::string NO_COL = "no_col";

	// static const std::string 

	std::string color_list[] = {RED, BLUE_LIGHT, BLUE_DARK, GREEN_DARK, GREEN_FL, VIOLET, ORANGE, YELLOW, NO_COL};

	std::map<std::string, int> colormap = {{RED, 0}, {BLUE_LIGHT, 1}, {BLUE_DARK, 2}, {GREEN_DARK, 3}, {GREEN_FL, 4}, {VIOLET, 5}, {ORANGE, 6}, {YELLOW, 7}};
	

	static const std::string RAS_CV_HUE="hue";
	static const std::string RAS_CV_SAT="sat";
	static const std::string RAS_CV_VAL="val";

	static const cv::Scalar SCALAR_RED = cv::Scalar(0, 0, 255);
	static const cv::Scalar SCALAR_GREEN = cv::Scalar(0, 255, 0);
	static const cv::Scalar SCALAR_BLUE = cv::Scalar(255, 0, 0);

	// static const  cv::Scalar BLUE_LIGHT_LOW = cv::Scalar(88, 90, 95);
	// static const  cv::Scalar BLUE_LIGHT_HIGH = cv::Scalar(105, 120, 120);
	

	static const  cv::Scalar BLUE_LIGHT_LOW = cv::Scalar(85, 45, 55);
	static const  cv::Scalar BLUE_LIGHT_HIGH = cv::Scalar(110, 255, 255);


	// static const  cv::Scalar BLUE_DARK_LOW = cv::Scalar(97, 80, 90);
	// static const  cv::Scalar  BLUE_DARK_HIGH = 	cv::Scalar(115, 105, 120);

	static const  cv::Scalar BLUE_DARK_LOW = cv::Scalar(70, 30, 30);
	static const  cv::Scalar  BLUE_DARK_HIGH = 	cv::Scalar(130, 250, 250);


	// static const cv::Scalar GREEN_DARK_LOW = cv::Scalar(54, 108, 100);
	// static const cv::Scalar GREEN_DARK_HIGH = cv::Scalar(65, 155, 140);
	static const cv::Scalar GREEN_DARK_LOW = cv::Scalar(40, 30, 30);
	static const cv::Scalar GREEN_DARK_HIGH = cv::Scalar(75, 255, 255);


	// static const cv::Scalar GREEN_FL_LOW = cv::Scalar(29, 145, 165);
	// static const cv::Scalar GREEN_FL_HIGH = cv::Scalar(65, 255, 255);

	static const cv::Scalar GREEN_FL_LOW = cv::Scalar(29, 55, 85);
	static const cv::Scalar GREEN_FL_HIGH = cv::Scalar(65, 255, 255);
// from gmm model
	// static const cv::Scalar VIOLET_LOW = cv::Scalar(138, 55, 90);
	// static const cv::Scalar VIOLET_HIGH = cv::Scalar(165, 100, 150);

	static const cv::Scalar VIOLET_LOW = cv::Scalar(120, 60, 100);
	static const cv::Scalar VIOLET_HIGH = cv::Scalar(165, 190, 255);

	// static const cv::Scalar YELLOW_LOW = cv::Scalar(14, 165, 185);
	// static const cv::Scalar YELLOW_HIGH = cv::Scalar(30, 195, 225);
	static const cv::Scalar YELLOW_LOW = cv::Scalar(10, 160, 160);
	static const cv::Scalar YELLOW_HIGH = cv::Scalar(40, 255, 255);

	static const cv::Scalar ORANGE_LOW = cv::Scalar(0, 150, 160);
	static const cv::Scalar ORANGE_HIGH = cv::Scalar(40, 255, 255);

	// static const cv::Scalar RED_LOW = cv::Scalar(162, 170, 135);
	// static const cv::Scalar RED_HIGH = cv::Scalar(185, 225, 220);

	static const cv::Scalar RED_LOW = cv::Scalar(155, 170, 145);
	static const cv::Scalar RED_HIGH = cv::Scalar(180, 255, 255);


	void thresholdColor(cv::Mat& src, cv::Mat& dst, const std::string& color);


}



#endif
