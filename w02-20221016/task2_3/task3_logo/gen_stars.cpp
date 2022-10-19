#ifndef _GEN_STARS_CPP_
#define _GEN_STARS_CPP_

#include "gen_stars.hpp"

double rand_gen(double st, double ed) {
	return st + (ed - st) * rand() / RAND_MAX;
}

GenStars::GenStars() {}

double point_dis(const cv::Point2d &p1, const cv::Point2d &p2) {
	// 两点之间距离
	cv::Point2d d = p2 - p1;
	return sqrt(d.x * d.x + d.y * d.y);
}

std::vector<Star> GenStars::gen_stars(const cv::Mat &_img) {
	this->img = _img;
	this->width = _img.cols;
	this->height = _img.rows;
	this->get_contours();
	this->get_con_circ();
	this->get_oth_circ();

	return this->get_stars_list();
}

void GenStars::get_contours() {
	// 二值化
	cv::Mat part;
	cv::threshold(img, part, 100, 255, cv::THRESH_BINARY);

	// 提取轮廓
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(part, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);

	this->contours = contours;
}

double GenStars::get_max_radius(const cv::Point2d &center) {
	double ret = 1000;
	for (auto &pr: this->con_circ) {
		ret = std::min(ret, point_dis(pr.first, center) - pr.second);
	}
	for (auto &pr: this->oth_circ) {
		ret = std::min(ret, point_dis(pr.first, center) - pr.second);
	}
	return ret;
}

void GenStars::get_con_circ(const std::vector<cv::Point> &con) {
	// 绕着轮廓扫，确认半径是什么，如果当前画这个半径的圆不会与其他圆发生冲突，就画出这个圆
	double lst_r = -1;
	for (uint32_t i = 0; i < con.size(); ++i) {
		uint32_t ed = i + 10; // 要求这 50 个连续的点必须画出一个圆
		double r_coef = RADIUS_VAR_COEF * rand_gen(-.25, .25); // 预测半径与平均半径之差的百分比
		double expected_r = CONTOUR_CIRC_RADIUS * (1 + r_coef); // 预测半径
		if (lst_r > 0) {
			// 与上一个圆的半径取平均，确保相邻两个圆半径之差不会太大
			expected_r = RADIUS_VAR_COEF * expected_r + (1 - RADIUS_VAR_COEF) * lst_r;
		}
		double empty_r = expected_r / (1 + CONTOUR_CIRC_SPACE) * CONTOUR_CIRC_SPACE; // 空闲部分的长度
		double need_r = expected_r + empty_r * .5;

		int mn_x = con[i].x, mx_x = con[i].x; // 当前点集最小最大横座标
		int mn_y = con[i].y, mx_y = con[i].y; // 当前点集最小最大纵座标
		for (; i <= con.size(); ++i) {
			if (i < con.size()) {
				mn_x = std::min(mn_x, con[i].x); mx_x = std::max(mx_x, con[i].x);
				mn_y = std::min(mn_y, con[i].y); mx_y = std::max(mx_y, con[i].y);
			}

			cv::Point2d center(.5 * (mn_x + mx_x), .5 * (mn_y + mx_y));
			double can_r = this->get_max_radius(center);
			if (can_r > need_r || i == ed) { // 如果能容纳这个圆或者没有其他点
				// 画出这个圆
				double actual_r = std::min(can_r, need_r);
				double draw_r = actual_r * (1 - CONTOUR_CIRC_SPACE);
				if (draw_r > 0) {
					this->con_circ.emplace_back(center, draw_r);
					lst_r = draw_r;
				}
				break;
			}
		}
	}
}

void GenStars::get_con_circ() {
	for (uint32_t i = 0; i < contours.size(); ++i) {
		this->get_con_circ(contours[i]);
	}
}

void GenStars::get_oth_circ() {
	// 生成其他地方上的圆
	int num = OTHER_CIRC_NUM * img.rows * img.cols;

	while (num--) {
		double r_coef = RADIUS_VAR_COEF * rand_gen(-1, 1); // 预测半径与平均半径之差的百分比
		double radius = OTHER_CIRC_RADIUS * (1 + r_coef); // 预测半径

		cv::Point center(rand_gen(-4, 4) * this->width,
				rand_gen(-4, 2) * this->height);
		this->oth_circ.emplace_back(center, radius);
	}
}

std::vector<Star> GenStars::get_stars_list() {
	std::vector<Star> res;
	for (auto &[p, rad]: con_circ) {
		double x = (p.x - .5 * this->width) / SCALE;
		double y = (.5 * this->height - p.y) / SCALE;
		double r = rad / SCALE;
		if (rand() % 5 == 0) r *= 10;
		// 在最后一层的星星
		for (int _ = 0; _ < 3; ++_) {
			double scale = rand_gen(.75, 1.5);
			double z = -SCALE / AVG_Z_SCALE;
			res.emplace_back(z * scale, x * scale, y * scale, r * scale);
		}
		// 在前面通道的星星
		for (int _ = 0; _ < 3; ++_) {
			double z = -SCALE / AVG_Z_SCALE * rand_gen(-2, 1);
			res.emplace_back(z, x, y, 1 / SCALE);
		}
	}
	for (auto &[p, rad]: oth_circ) {
		double x = (p.x - .5 * this->width) / SCALE;
		double y = (.5 * this->height - p.y) / SCALE;
		double r = rad / SCALE;
		// 在前面通道的星星
		for (int _ = 0; _ < 3; ++_) {
			double z = -SCALE / AVG_Z_SCALE * rand_gen(-2, 1);
			res.emplace_back(z, x, y, 1 / SCALE);
		}
	}
	return res;
}

#endif
