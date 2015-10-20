#ifndef CV_UTILS
#define CV_UTILS


#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/nonfree/features2d.hpp>

#include <string>
#include <sstream>
#include <vector>



namespace ras_cv{



	void makeBoxAroundObject(cv::Mat& src, cv::Mat& dst, int width, int height, int thickness);

	void makeBoxAroundObject(cv::Mat& src, cv::Mat& dst, int x_start, int x_end, int y_start, int y_end, int thickness);

	void makeManyWindows(const std::string& windowName, int rows, int cols, int x_start, int x_size, int y_start, int y_size, int x_offset, int y_offset);

	template <typename T> std::string writeAsString(T text);

	void  create_windows(
    const std::string &name, 
    int rows, 
    int cols, 
    int x_start, 
    int y_start,
    int x_size,
    int y_size,
    int x_offset,
    int y_offset);

}





#endif