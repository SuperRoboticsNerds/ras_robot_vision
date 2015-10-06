
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <opencv2/imgproc/imgproc.hpp>


void to_cv_copy(cv::Mat* cvimg, const sensor_msgs::Image::ConstPtr& img);