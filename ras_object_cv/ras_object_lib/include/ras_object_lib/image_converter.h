
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <sensor_msgs/image_encodings.h>

namespace ras_cv{
	void to_cv_copy(cv::Mat* cvimg, const sensor_msgs::Image::ConstPtr& img);

	void to_ros_copy(const cv::Mat& cvimg, sensor_msgs::ImagePtr& img, int bw);

	void to_cv_copy_depth(cv::Mat* cvimg, const sensor_msgs::Image::ConstPtr& depth_img);

}
