#include "ras_object_lib/image_color_range.h"
#include <ros/ros.h>


void ras_cv::thresholdColor(cv::Mat& src, cv::Mat& dst, const std::string& color){

	// switch(color)
	// {
	// 	case ras_cv::BLUE_LIGHT:
	// 		cv::inRange(src, ras_cv::BLUE_LIGHT_LOW, ras_cv::BLUE_LIGHT_HIGH, dst);
	// 		break;
	// 	case ras_cv::BLUE_DARK:
	// 		cv::inRange(src, ras_cv::BLUE_DARK_LOW, ras_cv::BLUE_DARK_HIGH, dst);
	// 		break;
	// 	case ras_cv::GREEN_DARK:
	// 		cv::inRange(src, ras_cv::GREEN_DARK_LOW, ras_cv::GREEN_DARK_HIGH, dst);
	// 		break;
	// 	case ras_cv::GREEN_FL:
	// 		cv::inRange(src, ras_cv::GREEN_FL_LOW, cv::GREEN_FL_HIGH, dst);
	// 		break;
	// 	case ras_cv::VIOLET:
	// 		cv::inRange(src, cv::VIOLET_LOW, cv::VIOLET_HIGH, dst);
	// 		break;
	// 	case ras_cv::ORANGE:
	// 		cv::inRange(src, cv::ORANGE_LOW, cv::ORANGE_HIGH, dst);
	// 		break;
	// 	case ras_cv::YELLOW:
	// 		cv::inRange(src, cv::YELLOW_LOW, cv::YELLOW_HIGH, dst);
	// 		break;
	// 	case ras_cv::RED:
	// 		cv::inRange(src, cv::RED_LOW, cv::RED_HIGH, dst);
	// 		break;
	// 	default:
	// 		src = dst;

	// }

	if(color == ras_cv::BLUE_LIGHT){
		cv::inRange(src, ras_cv::BLUE_LIGHT_LOW, ras_cv::BLUE_LIGHT_HIGH, dst);
	}else if(color == ras_cv::BLUE_DARK){
		cv::inRange(src, ras_cv::BLUE_DARK_LOW, ras_cv::BLUE_DARK_HIGH, dst);
	} else if(color == ras_cv::GREEN_DARK){
		cv::inRange(src, ras_cv::GREEN_DARK_LOW, ras_cv::GREEN_DARK_HIGH, dst);
	} else if (color == ras_cv::GREEN_FL){
		cv::inRange(src, ras_cv::GREEN_FL_LOW, ras_cv::GREEN_FL_HIGH, dst);
	} else if (color == ras_cv::VIOLET){
		cv::inRange(src, ras_cv::VIOLET_LOW, ras_cv::VIOLET_HIGH, dst);
	} else if (color == ras_cv::ORANGE){
		cv::inRange(src, ras_cv::ORANGE_LOW, ras_cv::ORANGE_HIGH, dst);
	} else if(color == ras_cv::YELLOW){
		cv::inRange(src, ras_cv::YELLOW_LOW, ras_cv::YELLOW_HIGH, dst);
	} else if(color == ras_cv::RED){
		cv::inRange(src, ras_cv::RED_LOW, ras_cv::RED_HIGH, dst);
	} else{
		dst = src;
	}



}
