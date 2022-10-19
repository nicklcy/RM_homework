// 每个点的坐标、半径等信息

#ifndef _POINT_HPP_
#define _POINT_HPP_

#include <iostream>
#include <Eigen/Dense>

class Star {
public:
	double x, y, z, rad;

	Star(double x = 0, double y = 0, double z = 0, double rad = 0):
		x(x), y(y), z(z), rad(rad) {}

	Eigen::Vector3d get_vec() const {
		Eigen::Vector3d res;
		res << x, y, z;
		return res;
	}
};

std::ostream& operator << (std::ostream &os, const Star &s) {
	return os << "(" << s.x << ", " << s.y << ", " << s.z << ")";
}

#endif
