#ifndef CV_NORMALISE
#define CV_NORMALISE


#include <opencv2/imgproc/imgproc.hpp>


namespace ras_cv{

void normalise(cv::Mat& img, cv::Mat& normimg);

void removeNans(cv::Mat& img, float val);


}






#endif