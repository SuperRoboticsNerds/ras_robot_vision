
#ifndef COLOR_RANGE
#define COLOR_RANGE


#include <opencv2/imgproc/imgproc.hpp>
#include <string>


namespace ras_cv{
	static const  std::string RED = "red";
	static const  std::string BLUE_LIGHT = "blue_light";
	static const  std::string BLUE_DARK = "blue_dark";
	static const  std::string GREEN_DARK = "green_dark";
	static const  std::string GREEN_FL = "green_fl";
	static const  std::string VIOLET = "violet";
	static const  std::string ORANGE  = "orange";
	static const  std::string YELLOW = "yellow";

	static const  cv::Scalar BLUE_LIGHT_LOW = cv::Scalar(88, 90, 95);
	static const  cv::Scalar BLUE_LIGHT_HIGH = cv::Scalar(105, 120, 120);

	static const  cv::Scalar BLUE_DARK_LOW = cv::Scalar(97, 80, 90);
	static const  cv::Scalar  BLUE_DARK_HIGH = 	cv::Scalar(115, 105, 120);

	static const cv::Scalar GREEN_DARK_LOW = cv::Scalar(54, 108, 100);
	static const cv::Scalar GREEN_DARK_HIGH = cv::Scalar(65, 135, 140);

	static const cv::Scalar GREEN_FL_LOW = cv::Scalar(42, 160, 145);
	static const cv::Scalar GREEN_FL_HIGH = cv::Scalar(53, 210, 210);

	static const cv::Scalar VIOLET_LOW = cv::Scalar(138, 55, 90);
	static const cv::Scalar VIOLET_HIGH = cv::Scalar(165, 100, 150);

	static const cv::Scalar YELLOW_LOW = cv::Scalar(14, 165, 185);
	static const cv::Scalar YELLOW_HIGH = cv::Scalar(30, 195, 225);

	static const cv::Scalar ORANGE_LOW = cv::Scalar(6, 165, 160);
	static const cv::Scalar ORANGE_HIGH = cv::Scalar(15, 210, 215);

	static const cv::Scalar RED_LOW = cv::Scalar(162, 170, 135);
	static const cv::Scalar RED_HIGH = cv::Scalar(185, 225, 220);


	void thresholdColor(cv::Mat& src, cv::Mat& dst, const std::string& color);


}






#endif
