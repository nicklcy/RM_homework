#ifndef _TRANS_HPP_
#define _TRANS_HPP_

#include <opencv2/opencv.hpp>
#include <Eigen/Dense>
#include "star.hpp"

// 给定相机内参矩阵，相机位置和旋转四元数，将所有点的坐标投影到相机上，并输出 cv::Mat

class DoTrans {
	uint32_t width, height;
	std::vector<Star> points;
	Eigen::Matrix<double, 3, 4> cam_f, trans_mat;

	void get_trans_mat(const Eigen::Vector3d &cam_pos, const Eigen::Quaterniond &rot_q);

public:
	DoTrans(uint32_t _width, uint32_t _height);
	void init(const Eigen::Matrix<double, 3, 4> &_cam_f, const std::vector<Star> &_points);
	cv::Mat get_image(const Eigen::Vector3d &cam_pos, const Eigen::Quaterniond &rot_q, bool need_shift = false);
};

#endif
