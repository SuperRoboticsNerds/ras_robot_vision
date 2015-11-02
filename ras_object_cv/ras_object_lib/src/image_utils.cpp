#include "ras_object_lib/image_utils.h"
#include <ros/ros.h>



template<typename T> std::string ras_cv::writeAsString(T text){
	std::stringstream ss;
	ss  << text;
	return ss.str();

}


template<typename T> std::string ras_cv::writeVectorAsString(std::vector<T>& vec){
	std::stringstream ss;
	for(typename std::vector<T>::const_iterator  it = vec.begin(); it != vec.end(); ++it){
		ss << *it << " ";
	}
	return ss.str();

}


template<typename T> std::string ras_cv::writeMatrixAsString(const cv::Mat& mat){
	std::stringstream ss;
	for(int i = 0; i< mat.rows; i++){
		for(int j = 0; j < mat.cols; j++){
			ss << mat.at<T>(cv::Point(i,j)) << ";";
		}
		ss << std::endl;
	}

	ss << std::endl;

	return ss.str();
}


template std::string ras_cv::writeVectorAsString<cv::Vec3b>(cv::vector<cv::Vec3b> & vector);


// template<typename T> std::string ras_cv::


template std::string ras_cv::writeAsString(int text);
template std::string ras_cv::writeAsString(double text);
template std::string ras_cv::writeAsString(float text);
template std::string ras_cv::writeAsString(long text);

template std::string ras_cv::writeMatrixAsString<cv::Vec3b>(const cv::Mat& mat);
template std::string ras_cv::writeMatrixAsString<uchar>(const cv::Mat& mat);


void ras_cv::create_windows(
    const std::string &name, 
    int rows, 
    int cols, 
    int x_start, 
    int y_start,
    int x_size,
    int y_size,
    int x_offset,
    int y_offset) {
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      std::string name_current = name + " " + ras_cv::writeAsString(i*cols + j);
      cv::namedWindow(name_current, CV_WINDOW_NORMAL);
      cv::resizeWindow(name_current, x_size, y_size);
      cv::moveWindow(
          name_current, 
          x_start + (x_size + x_offset) * j, 
          y_start + (y_size + y_offset) * i);
    }
  }
}