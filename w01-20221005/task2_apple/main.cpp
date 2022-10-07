#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

cv::Mat get_red(const cv::Mat& img);
cv::Mat get_shown(const cv::Mat &img, const cv::Mat &msk, int); // 调参时使用
void draw_apple(cv::Mat &img, const cv::Mat &msk);

int main(int argc, char *argv[]) {
	std::string name = "../apple.png";
	if (argc > 1) name = argv[1];
	std::cerr << "Read image from " << name << std::endl;

	cv::Mat img = cv::imread(name);
	cv::Mat msk = get_red(img);
	draw_apple(img, msk);

	cv::imshow("result", img);
	cv::waitKey();
	if (argc > 2) {
		std::cerr << "Write image to " << argv[2] << std::endl;
		cv::imwrite(argv[2], img);
	}
	return 0;
}

cv::Mat get_red(const cv::Mat &img) {
	// 将 G 通道变为 R - G
	cv::Mat channels[3];
	cv::split(img, channels);
	channels[1] = channels[2] - channels[1];
	cv::Mat new_img;
	cv::merge(channels, 3, new_img);

	// 提取苹果（只要大致轮廓存在即可）
	cv::Mat part1, part2;
	// 提取苹果上半红色的部分
	cv::inRange(new_img, cv::Scalar(0, 70, 130), cv::Scalar(150, 255, 255), part1);
	// 提取苹果下面黄色的部分
	cv::inRange(new_img, cv::Scalar(0, 10, 70), cv::Scalar(20, 70, 120), part2);

	// 形态学运算将上下连接起来
	cv::Mat msk = part1 | part2;
	cv::Mat kernel = getStructuringElement(cv::MORPH_CROSS, cv::Size(2, 3));
	cv::Mat res_msk;
	cv::morphologyEx(msk, res_msk, cv::MORPH_CLOSE, kernel); // 红色与黄色相连
	cv::morphologyEx(res_msk, res_msk, cv::MORPH_OPEN, kernel); // 去除苹果上方枝干的一小部分
	return res_msk;
}

// 突显出原图像 msk 为 t 的部分（调参时使用）
cv::Mat get_shown(const cv::Mat &img, const cv::Mat &msk, int t) {
	assert(img.size() == msk.size());
	// 转换为 HSV
	cv::Mat hsv_img;
	cv::Mat channels[3];
	cv::cvtColor(img, hsv_img, cv::COLOR_BGR2HSV);
	cv::split(hsv_img, channels);

	// 所有亮度变为一半，再对 msk 为 0 的部分改为原来的亮度
	cv::Mat real_msk = t * msk +
		(1 - t) * (255 * cv::Mat::ones(msk.size(), CV_8U) - msk);
	int channel_id = 2;
	channels[channel_id] *= 0.3;
	channels[channel_id] += channels[channel_id].mul(real_msk / 255) * 0.7 / 0.3;

	cv::Mat hsv_res, res;
	cv::merge(channels, 3, hsv_res);
	cv::cvtColor(hsv_res, res, cv::COLOR_HSV2BGR);
	return res;
}

void draw_apple(cv::Mat &img, const cv::Mat &msk) {
	// 求出所有轮廓
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(msk, contours, hierarchy, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);

	// 面积最大的即为苹果
	double mx_area = 0;
	int mx_id = -1;
	for (uint32_t i = 0; i < contours.size(); ++i) {
		double area = cv::contourArea(contours[i]);
		if (mx_area < area) {
			mx_area = area;
			mx_id = i;
		}
	}
	// 画出苹果轮廓
	drawContours(img, contours, mx_id, cv::Scalar(255, 0, 0), 1);

	// 画出外接矩形
	cv::Rect rect = cv::boundingRect(contours[mx_id]);
	cv::rectangle(img, rect, cv::Scalar(0, 255, 0), 2);
}
