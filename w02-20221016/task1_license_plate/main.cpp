#include <bits/stdc++.h>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>

const int PLATE_WIDTH = 440;
const int PLATE_HEIGHT = 140;
const float SCALE = 1;

std::vector<cv::Point2f> get_blue(const cv::Mat &img);
cv::Mat do_transform(const cv::Mat &img, const std::vector<cv::Point2f> &pts);

int main(int argc, char *argv[]) {
	std::string inp_name = "../license_plate.png";
	if (argc > 1) inp_name = argv[1];
	std::cerr << "Read png from " << inp_name << std::endl;

	cv::Mat inp_img = cv::imread(inp_name);
	auto points = get_blue(inp_img);
	auto res = do_transform(inp_img, points);
	cv::imshow("result", res);
	cv::waitKey(0);

	std::string out_name = "../result.png";
	if (argc > 2) out_name = argv[2];
	std::cerr << "Output png to " << out_name << std::endl;
	cv::imwrite(out_name, res);

	return 0;
}

std::vector<cv::Point2f> get_blue(const cv::Mat &img) {
	// 提取车牌顶点
	cv::Mat hsv, msk;
	cv::cvtColor(img, hsv, cv::COLOR_BGR2HSV);
	cv::inRange(hsv, cv::Scalar(100, 147, 100), cv::Scalar(124, 255, 150), msk);
	// cv::imshow("res", msk);
	// cv::waitKey(0);

	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(msk, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);
	std::cerr << hsv.at<cv::Vec3b>(cv::Point(953, 415)) << '\n';

	// 找到面积最大的轮廓即为车牌
	float mx_size = 0; int mx_id = -1;
	for (uint32_t i = 0; i < contours.size(); ++i) {
		float area = cv::contourArea(contours[i]);
		if (mx_size < area) {
			mx_size = area;
			mx_id = i;
		}
	}
	auto &contour = contours[mx_id];
	// cv::drawContours(img, contours, mx_id, cv::Scalar(0, 255, 0));

	std::vector<cv::Point2f> points;

	// 用 x +- 3 * y 的直线，截取车牌，得到四个顶点
	for (int k = -1; k <= 1; k += 2) {
		float mx_val = -1E9; cv::Point mx_pt;
		float mn_val = +1E9; cv::Point mn_pt;
		for (auto &p: contour) {
			float val = p.x + 3 * k * p.y;
			if (val > mx_val) { mx_val = val; mx_pt = p; }
			if (val < mn_val) { mn_val = val; mn_pt = p; }
		}
		points.emplace_back(mn_pt);
		points.emplace_back(mx_pt);
		std::cerr << mx_pt << ' ' << hsv.at<cv::Vec3b>(mx_pt) << '\n';
		std::cerr << mn_pt << ' ' << hsv.at<cv::Vec3b>(mn_pt) << '\n';
	}
	// for (auto &p: points) {
	// 	cv::circle(img, p, 2, cv::Scalar(0, 0, 255), -1);
	// }
	// cv::imshow("img", img);
	// cv::waitKey(0);

	// 按照逆时针的顺序输出
	return {points[2], points[0], points[3], points[1]};
}

cv::Mat do_transform(const cv::Mat &img, const std::vector<cv::Point2f> &pts) {
	// 做透视变换

	std::vector<cv::Point2f> plate;
	plate.emplace_back(SCALE * cv::Point(1, 1));
	plate.emplace_back(SCALE * cv::Point(1, PLATE_HEIGHT));
	plate.emplace_back(SCALE * cv::Point(PLATE_WIDTH, PLATE_HEIGHT));
	plate.emplace_back(SCALE * cv::Point(PLATE_WIDTH, 1));

	cv::Mat mat = cv::getPerspectiveTransform(pts, plate);
	cv::Mat res;
	cv::warpPerspective(img, res, mat, cv::Size(PLATE_WIDTH, PLATE_HEIGHT));

	return res;
}
