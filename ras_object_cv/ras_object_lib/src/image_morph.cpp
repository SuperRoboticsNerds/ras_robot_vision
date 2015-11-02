#include "ras_object_lib/image_morph.h"
#include <ros/ros.h>


cv::Mat ras_cv::get_structuring_element(int ele_type, int size){
	return cv::getStructuringElement(ele_type, cv::Size(2*size +1, 2*size +1));

}


void ras_cv::morph_tranform(cv::Mat& src, cv::Mat& dst, const std::string & morph_tr_type, int element_type, int element_size){
	cv::Mat element = ras_cv::get_structuring_element(element_type, element_size);
	if(morph_tr_type == ras_cv::ERODE){
		cv::erode(src, dst, element);
	} else if(morph_tr_type == ras_cv::DILATE){
		cv::dilate(src, dst, element);
	} else if(morph_tr_type == ras_cv::OPEN){
		cv::erode(src, dst, element);
		cv::dilate(dst, dst, element);
	} else if(morph_tr_type == ras_cv::CLOSE){
		cv::dilate(src, dst, element);
		cv::erode(dst, dst, element);
	} else{
		dst = src;
	}

}



cv::Rect  ras_cv::get_bounding_box(cv::KeyPoint& point, int width, int height){
	int size = (int) point.size;
	int x1_ = std::max<int>(0, (int) point.pt.x -  size);
	int x2_ = std::min<int>(width - 1, (int) point.pt.x + size);
	int y1_ = std::max<int>(0, (int) point.pt.y - size);
	int y2_ = std::min<int>(height - 1,(int) point.pt.y + size);

	return cv::Rect(x1_, y1_, x2_ - x1_, y2_ - y1_);

}

cv::Rect  ras_cv::get_bounding_box(cv::KeyPoint& point, int width, int height, float scaling){
	int size = (int) point.size;
	int x1_ = std::max<int>(0, (int) point.pt.x -  (scaling)*size);
	int x2_ = std::min<int>(width - 1, (int) point.pt.x + (scaling) *size);
	int y1_ = std::max<int>(0, (int) point.pt.y - (scaling) *size);
	int y2_ = std::min<int>(height - 1,(int) point.pt.y + (scaling) *size);

	return cv::Rect(x1_, y1_, x2_ - x1_, y2_ - y1_);

}
 


template <typename T>  std::vector<T> ras_cv::getPointsofLine(const cv::Mat &img, int x_start, int x_end, int y_start, int y_end, int n){
	int x_max = img.size[0];
	int y_max = img.size[1];

	double x_diff = (double) (x_end - x_start);
	double y_diff = (double) (y_end - y_start);

	double x_i = x_start;
	double y_i = y_start;

	std::vector<T> points;

	for(int i =0; i<n; i++){
		points.push_back(img.at<T>(std::min((int) x_i, x_max)), std::min((int) y_i, y_max));
		x_i += x_diff;
		y_i += y_diff;
	}

	return points;

}