#ifndef _GEN_STARS_HPP_
#define _GEN_STARS_HPP_

#include <opencv2/opencv.hpp>
#include <vector>

#include "../star.hpp"

const float CONTOUR_CIRC_RADIUS = 10; // 轮廓上圆的平均半径（与整个轮廓长度相比）
const float RADIUS_VAR_COEF = .95; // 轮廓半径在平均半径 * 1 加减该参数下随机生成
const float RADIUS_ADJ_COEF = .3; // 越大代表轮廓上相邻两个圆差越大
const float CONTOUR_CIRC_SPACE = .25; // 轮廓上有多少是空闲的（没有被圆覆盖）
const float OTHER_CIRC_RADIUS = .5; // 其他地方圆的平均半径
const float OTHER_CIRC_NUM = 3E-3; // 其他地方圆的个数占总像素数的比例
const float OTHER_MIN_DIS = 10; // 其他地方圆的最小间距
const double SCALE = 100; // 缩放比例
const double AVG_Z_SCALE = 20; // Z 的缩放比例（倒数）
const double VAR_Z_SCALE = 1000; // logo 层 Z 的缩放比例（倒数）

class GenStars {
	cv::Mat img;
	uint32_t width, height;
	std::vector<std::vector<cv::Point>> contours;
	std::vector<std::pair<cv::Point, double>> con_circ;
	std::vector<std::pair<cv::Point, double>> oth_circ;

	void get_contours(); // 获取轮廓
	double get_max_radius(const cv::Point2d &center);
	void get_con_circ(const std::vector<cv::Point> &con); // 在轮廓上画圆
	void get_con_circ(); // 在所有轮廓上画圆
	void get_oth_circ(); // 在其他地方画圆
	std::vector<Star> get_stars_list();

public:
	GenStars();

	std::vector<Star> gen_stars(const cv::Mat &_img);
};

#endif
