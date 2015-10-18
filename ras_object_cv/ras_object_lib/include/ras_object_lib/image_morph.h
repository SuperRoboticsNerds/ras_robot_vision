
#ifndef MORPH_CV_TRANS
#define MORPH_CV_TRANS

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/nonfree/features2d.hpp>

#include <string>
#include <sstream>




namespace ras_cv{

static const std::string ERODE = "erode";
static const std::string DILATE = "dilate";
static const std::string OPEN = "open";
static const std::string CLOSE = "close";



cv::Mat get_structuring_element(int ele_type, int size);

void morph_tranform(cv::Mat& src, cv::Mat& dst, const std::string & morph_tr_type, int element_type, int element_size);


cv::Rect get_bounding_box(cv::KeyPoint& point, int width, int height);


template <typename T>  std::vector<T>  getPointsofLine(const cv::Mat &img, int x_start, int x_end, int y_start, int y_end, int n);


}






#endif





