#include <bits/stdc++.h>
#include <opencv2/opencv.hpp>
#include <Eigen/Dense>

#include "../trans.cpp"
#include "../star.hpp"

const int WIDTH = 1200;
const int HEIGHT = 800;

std::vector<Star> get_points(const std::string &file_name);

int main(int argc, char *argv[]) {
	Eigen::Matrix<double, 3, 4> cam_f;
	cam_f << 400., 0., 190., 0.,
		0., 400., 160., 0.,
		0., 0., 1., 0.;

	Eigen::Vector3d cam_pos = {2., 2., 2.};
	Eigen::Quaterniond rot_q(-.5, .5, .5, -.5);
	DoTrans do_trans(WIDTH, HEIGHT);

	std::string inp_name = "../points.txt";
	if (argc > 1) inp_name = argv[1];
	std::cerr << "Read points list from " << inp_name << std::endl;

	std::vector<Star> points;
	std::ifstream fin(inp_name);
	int n;
	fin >> n;
	while (n--) {
		double x, y, z;
		fin >> x >> y >> z;
		points.emplace_back(x, y, z);
	}

	do_trans.init(cam_f, points);

	cv::Mat res = do_trans.get_image(cam_pos, rot_q);
	cv::imshow("result", res);
	cv::waitKey(0);

	std::string out_name = "../result.png";
	if (argc > 2) out_name = argv[2];
	std::cerr << "Output points png to " << out_name << std::endl;
	cv::imwrite(out_name, res);

	return 0;
}
