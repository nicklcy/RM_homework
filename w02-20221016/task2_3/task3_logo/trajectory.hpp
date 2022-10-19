#ifndef _TRAJECTORY_HPP_
#define _TRAJECTORY_HPP_

#include <bits/stdc++.h>

#include <Eigen/Dense>

class GetTrajectory {
public:
	std::vector<std::pair<Eigen::Vector3d, Eigen::Quaterniond>> cam; // 每一帧摄像机位置和四元数

	void get_trajectory(uint32_t frames, double R);
};

#endif
