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


double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0){

  double dx1 = pt1.x - pt0.x;
  double dy1 = pt1.y - pt0.y;
  double dx2 = pt2.x - pt0.x;
  double dy2 = pt2.y - pt0.y;
  return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);

}



int ras_cv::argmax(int* arr, int size){
  if(size == 0){
    return -1;
  }

  int max =-1;
  int idx = 0;
   for(int i = 0; i<size; i++){
    if(arr[i] > max){
      max = arr[i];
      idx = i;
    }

   }

   return idx; 

}



int ras_cv::findHoughCircles(cv::Mat& img, int threshold1, int threshold2){
  std::vector<cv::Vec3f> circles;
  cv::Mat hsv_channels[3];
  cv::split(img, hsv_channels);
  cv::Mat grayimg = hsv_channels[1];
  cv::HoughCircles(grayimg, circles, CV_HOUGH_GRADIENT, 1, 25, threshold1, threshold2, 0, 0);
  return circles.size();

}

int ras_cv::findHoughCirclesbw(const cv::Mat& img, int threshold1, int threshold2){
  std::vector<cv::Vec3f> circles;
  cv::HoughCircles(img, circles, CV_HOUGH_GRADIENT, 1, 25, threshold1, threshold2, 0, 0);
  return circles.size();

}


int ras_cv::maxval(int* arr, int size){
  if(size == 0){
    return 0;
  }

  int max = 0;
  for(int i =0; i < size; i++){
    if(arr[i] > max){
      max = arr[i];
    }

  }

  return max;

}





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