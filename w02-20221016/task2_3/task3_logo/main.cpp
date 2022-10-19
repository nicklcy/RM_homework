#include <iostream>
#include <vector>

#include <opencv2/opencv.hpp>
#include <Eigen/Dense>

#include "gen_stars.cpp"
#include "../star.hpp"
#include "../trans.cpp"
#include "trajectory.cpp"

const uint32_t HEIGHT = 750;
const uint32_t WIDTH = 1000;

int main(int argc, char *argv[]) {
	// 摄像机内参矩阵
	Eigen::Matrix<double, 3, 4> cam_f;
	cam_f << 400., 0., 0., 0.,
		0., 400., 0., 0.,
		0., 0., 1., 0.;

	std::string inp_name = "../logo.png";
	if (argc > 1) inp_name = argv[1];
	std::cerr << "Read logo from " << inp_name << std::endl;

	cv::Mat logo_img = cv::imread(inp_name, cv::IMREAD_GRAYSCALE);

	// 获取星星坐标
	GenStars gen_stars;
	std::vector<Star> stars = gen_stars.gen_stars(logo_img);

	// 获取摄像机轨迹
	GetTrajectory get_traject;
	get_traject.get_trajectory(24 * 6, 2.5);

	// 初始化变换
	DoTrans do_trans(WIDTH, HEIGHT);
	do_trans.init(cam_f, stars);

	std::vector<cv::Mat> res;
	for (auto &[cam_pos, rot_q]: get_traject.cam) {
		res.emplace_back(do_trans.get_image(cam_pos, rot_q, true));
	}

	std::string out_name = "../result.mp4";
	if (argc > 2) out_name = argv[2];
	std::cerr << "Write video to " << out_name << std::endl;

	cv::VideoWriter writer;
	writer.open(out_name, cv::VideoWriter::fourcc('m', 'p', '4', 'v'),
			24, cv::Size(WIDTH, HEIGHT), true);
	assert(writer.isOpened());

	for (const auto &img: res) writer << img;

	writer.release();

	return 0;
}
