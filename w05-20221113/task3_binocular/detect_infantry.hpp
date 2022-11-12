#ifndef _DETECT_INFANTRY_HPP_
#define _DETECT_INFANTRY_HPP_

#include <opencv2/opencv.hpp>

extern std::vector<cv::Point2f> detect_infantry(const cv::Mat &img);

extern cv::Mat print(const cv::Mat &img, const std::vector<cv::Point2f> &points, const float &dis);

#endif
