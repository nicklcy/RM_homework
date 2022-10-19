#ifndef _TRANS_CPP_
#define _TRANS_CPP_

#include "trans.hpp"

const double RADIUS_SCALE = 300;

DoTrans::DoTrans(uint32_t _width, uint32_t _height) {
	this->width = _width;
	this->height = _height;
}

void DoTrans::init(const Eigen::Matrix<double, 3, 4> &_cam_f, const std::vector<Star> &_points) {
	this->cam_f = _cam_f;
	this->points = _points;
}

void DoTrans::get_trans_mat(const Eigen::Vector3d &cam_pos, const Eigen::Quaterniond &rot_q) {
	// 获取变换矩阵
	Eigen::Matrix3d mat_rot = rot_q.matrix().transpose().cast<double>();
	Eigen::Matrix4d converter = Eigen::Matrix4d::Zero();
	converter.block(0, 0, 3, 3) = mat_rot;
	converter.block(0, 3, 3, 1) = -mat_rot * cam_pos;
	converter(3, 3) = 1;
	this->trans_mat = this->cam_f * converter;
}

cv::Mat DoTrans::get_image(const Eigen::Vector3d &cam_pos, const Eigen::Quaterniond &rot_q, bool need_shift) {
	// 获取图像
	this->get_trans_mat(cam_pos, rot_q);

	cv::Mat img(height, width, CV_8UC3);
	img = cv::Mat::zeros(height, width, CV_8UC3);
	for (const auto &p: this->points) {
		Eigen::Vector4d vec;
		vec.block(0, 0, 3, 1) = p.get_vec();
		vec(3, 0) = 1;
		Eigen::Vector3d t_vec = this->trans_mat * vec;
		if (t_vec(2, 0) < 0) continue;
		double x = t_vec(0, 0) / t_vec(2, 0) + (need_shift ? .5 * this->width : 0);
		double y = t_vec(1, 0) / t_vec(2, 0) + (need_shift ? .5 * this->height : 0);
		double rad = p.rad * RADIUS_SCALE / t_vec(2, 0);
		rad = std::min(std::max(rad, 1.), 5.);

		cv::circle(img, cv::Point(x, y), rad, cv::Scalar(255, 255, 255), -1, cv::LINE_AA);
	}
	return img;
}

#endif
