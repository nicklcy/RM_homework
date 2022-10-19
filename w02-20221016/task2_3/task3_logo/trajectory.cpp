#ifndef _TRAJECTORY_CPP_
#define _TRAJECTORY_CPP_

#include <vector>
#include <algorithm>

#include "trajectory.hpp"

const double ANGLE = acos(-1) * .5; // 一开始摄像机朝向的角度
const double COEF = .5; // 调节速度变化

double sqr(double x) { return x * x; }

std::vector<double> get_speed_coef(int cnt) {
	// 获取每一帧角速度
	// 速度图像可以认为是四个 1/4 圆拼接起来，其中最后一个被拉长（速度变化更慢）
	std::vector<double> speed;
	int t = cnt / 4;
	for (int i = 1; i <= t; ++i) {
		speed.emplace_back(t - sqrt(sqr(t) - sqr(i)) * COEF);
	}
	for (int i = t; i--; ) {
		speed.emplace_back(t + sqrt(sqr(t) - sqr(i)) * COEF);
	}
	for (int i = 1; i <= t; ++i) {
		speed.emplace_back(t + sqrt(sqr(t) - sqr(i)) * COEF);
	}
	for (int i = t; i--; ) {
		speed.emplace_back(t - sqrt(sqr(t) - sqr(i)));
	}
	while (speed.size() < cnt) speed.emplace_back(0);
	std::partial_sum(begin(speed), end(speed), begin(speed));
	for (double &x: speed) x /= speed.back();
	return speed;
}

void GetTrajectory::get_trajectory(uint32_t frames, double R) {
	// 获取每一帧摄像机位置和四元数
	Eigen::Quaterniond rot_q(.5, -.5, -.5, .5);

	auto speed = get_speed_coef(frames);
	for (uint32_t i = 0; i < frames; ++i) {
		// 求当前帧姿态
		double ang = (1 - speed[i]) * ANGLE;
		double x = R * sin(ang);
		double y = R * (cos(ang) - 1);

		Eigen::AngleAxisd cam_rot1(Eigen::AngleAxisd(-ang, Eigen::Vector3d::UnitZ()));
		Eigen::AngleAxisd cam_rot2(Eigen::AngleAxisd(-ang, Eigen::Vector3d::UnitX()));
		this->cam.emplace_back(Eigen::Vector3d({x, y, x}), cam_rot2 * cam_rot1 * rot_q);
	}
}

#endif
