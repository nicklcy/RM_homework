#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

struct ProcessVideo {
	int bullet_id = 0; // 子弹编号
	std::vector<std::pair<cv::Point2f, int> > lst_fr_bul; // 上一帧子弹位置

	cv::Mat get_bullet(const cv::Mat &img); // 提取子弹轨迹
	std::vector<cv::Point2f> get_vertices(const cv::RotatedRect &rect); // RotatedRect 四个顶点
	cv::Point2f get_mid_point(const cv::Mat &img, const std::vector<cv::Point> &contour); // 找出轮廓中点
	std::pair<cv::Point2f, int> process_bullet(cv::Mat &img, const std::vector<cv::Point> &contour); // 处理子弹
	cv::Mat process_new_frame(const cv::Mat &img); // 处理新一帧
};

int main(int argc, char *argv[]) {
	std::string inp_name = "../IMG_8511.mp4";
	if (argc > 1) inp_name = argv[1];
	std::cerr << "Read video from " << inp_name << std::endl;

	cv::VideoCapture capture(inp_name);
	assert(capture.isOpened());
	cv::Size SIZE = cv::Size(capture.get(cv::CAP_PROP_FRAME_WIDTH),
			capture.get(cv::CAP_PROP_FRAME_HEIGHT));
	double FPS_RATE = capture.get(cv::CAP_PROP_FPS);

	std::string out_name = "result.mp4";
	if (argc > 2) out_name = argv[2];
	cv::VideoWriter writer;
	writer.open(out_name, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'),
			FPS_RATE, SIZE, true);
	assert(writer.isOpened());

	ProcessVideo process;
	cv::Mat src;
	std::vector<cv::Mat> res;
	int frame_id = 0;
	while (true) {
		capture >> src;
		if (src.empty()) break;
		++frame_id;
		std::cerr << "Current frame id: " << frame_id << std::endl;
		res.emplace_back(process.process_new_frame(src));
	}

	// 1 倍速回放
	for (const cv::Mat &frame: res) {
		cv::Mat txt_fr = frame.clone();
		cv::putText(txt_fr, "Play speed: 1x", cv::Point(txt_fr.cols - 230, 20),
				cv::FONT_HERSHEY_SIMPLEX, 0.75, cv::Scalar(255, 255, 255), 2);
		writer << txt_fr;
	}

	// 0.25 倍速回放
	for (const cv::Mat &frame: res) {
		cv::Mat txt_fr = frame.clone();
		cv::putText(txt_fr, "Play speed: 0.25x", cv::Point(txt_fr.cols - 230, 20),
				cv::FONT_HERSHEY_SIMPLEX, 0.75, cv::Scalar(255, 255, 255), 2);
		writer << txt_fr;
		writer << txt_fr;
		writer << txt_fr;
		writer << txt_fr;
	}

	writer.release();

	return 0;
}

cv::Mat ProcessVideo::get_bullet(const cv::Mat &img) {
	// 提取子弹部分
	cv::Mat msk, res;
	cv::inRange(img, cv::Scalar(0, 100, 0), cv::Scalar(200, 255, 120), msk);
	return msk;
}

std::vector<cv::Point2f> ProcessVideo::get_vertices(const cv::RotatedRect &rect) {
	cv::Point2f vertices[4];
	rect.points(vertices);
	return std::vector<cv::Point2f>(vertices, vertices + 4);
}

cv::Point2f ProcessVideo::get_mid_point(const cv::Mat &img, const std::vector<cv::Point> &contour) { // 找出拐弯轮廓中点
	// 拟合成四边形
	std::vector<cv::Point> poly_vec;
	cv::approxPolyDP(contour, poly_vec, 5, true);
	uint32_t n = poly_vec.size();
	cv::Mat tmp = img.clone();
	for (uint8_t i = 0; i < n; ++i) {
		cv::line(tmp, poly_vec[i], poly_vec[(i + 1) % n], cv::Scalar(0, 0, 255), 1);
	}
	std::cerr << "==== " << n << std::endl;
	cv::imshow("tmp", tmp);
	cv::waitKey();
	if (n == 2) {
		return 0.5 * static_cast<cv::Point2f>(poly_vec[0] + poly_vec[1]);
	}
	// 找出总长度最大的相邻两条边（且这两条边夹角为钝角）
	double mx = 0, mx1, mx2; int id = -1;
	for (uint32_t i = 0; i < n; ++i) {
		cv::Point p = poly_vec[i] - poly_vec[(i + 1) % n];
		cv::Point q = poly_vec[(i + 2) % n] - poly_vec[(i + 1) % n];
		double len1 = sqrt(p.ddot(p)), len2 = sqrt(q.ddot(q));
		if (mx < len1 + len2 && p.cross(q) < 0) {
			mx = len1 + len2; mx1 = len1; mx2 = len2; id = i;
		}
	}
	// 寻找中点
	int id0 = id, id1 = (id + 1) % n, id2 = (id + 2) % n;
	if (mx1 < mx2) { // 中点在 id + 1 到 id + 2 的边上
		std::swap(id2, id0);
		std::swap(mx1, mx2);
	}
	cv::Point2f p = poly_vec[id0], q = poly_vec[id1];
	cv::Point2f center = p + (q - p) / mx1 * mx * 0.5;
	return center;
}

std::pair<cv::Point2f, int> ProcessVideo::process_bullet(
		cv::Mat &img, const std::vector<cv::Point> &contour) {
	cv::RotatedRect rect = cv::minAreaRect(contour);
	cv::Size2f rotat_size = rect.size;
	double rotat_h = rotat_size.height;
	double rotat_w = rotat_size.width;
	if (rotat_h > rotat_w) std::swap(rotat_h, rotat_w);
	float rotat_area = rotat_size.height * rotat_size.width;

	bool is_turning = cv::contourArea(contour) / rotat_area < 0.4; // 是否为拐弯
	// std::cerr << "- " << rotat_w / rotat_h << ' ' << cv::contourArea(contour) / rotat_area << '\n';

	if (rotat_w / rotat_h < 8 && !is_turning && rect.center.y < img.rows / 2) { // 不是刚发射的弹丸
		// 这一步主要卡掉对第 7, 8 发弹丸的干扰
		return std::make_pair(cv::Point2f(), int());
	}

	// 绘制外接矩形
	std::vector<cv::Point2f> vertices = this->get_vertices(rect);
	for (uint8_t i = 0; i < 4; ++i) {
		cv::line(img, vertices[i], vertices[(i + 1) & 3], cv::Scalar(255, 255, 255));
	}

	// 通过实际轮廓面积与外接矩形面积比，判断是否拐弯
	cv::Point2f center;
	if (is_turning) { // 是拐弯
		center = this->get_mid_point(img, contour);
	} else {
		center = rect.center; // 非拐弯，中点即为矩形中心
	}
	cv::circle(img, center, 5, cv::Scalar(0, 255, 255), cv::FILLED);

	int bul_id = 0;
	// 获取子弹编号
	if (!this->lst_fr_bul.empty()) {
		// 查找上一帧子弹轨迹中心最近的子弹
		double mn = 1E30; int id = -1;
		for (uint32_t i = 0; i < this->lst_fr_bul.size(); ++i) {
			const std::pair<cv::Point2f, int> &pr = this->lst_fr_bul[i];
			double dis = sqrt((pr.first - center).dot(pr.first - center));
			if (mn > dis) {
				mn = dis; id = i;
			}
		}
		// std::cerr << "mn = " << mn << '\n';
		if (mn < 200) { // 距离不能太远
			bul_id = this->lst_fr_bul[id].second;
		}
	}
	// std::cerr << "** " << center.x << ' ' << center.y << ' ' << img.rows << ' ' << img.cols << '\n';
	if (!bul_id && img.rows - center.y < img.rows * 0.4) { // 找不到子弹，可能是刚刚发射的
		bul_id = ++this->bullet_id;
	}
	if (bul_id) { // 打印子弹编号
		cv::putText(img, std::to_string(bul_id), center - cv::Point2f(0, 10),
				cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 2);
	}

	return std::make_pair(center, bul_id);
}

cv::Mat ProcessVideo::process_new_frame(const cv::Mat &img) {
	std::cerr << "========= " << std::endl;
	cv::Mat res = img.clone();
	cv::Mat mask = this->get_bullet(img);

	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(mask, contours, hierarchy,
			cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

	std::vector<std::pair<cv::Point2f, int> > new_fr_bul;
	for (uint32_t i = 0; i < contours.size(); ++i) {
		if (cv::contourArea(contours[i]) < 80) continue; // 面积太小，肯定不符
		std::pair<cv::Point2f, int> pr = this->process_bullet(res, contours[i]);
		if (pr.second) new_fr_bul.emplace_back(pr);
	}
	this->lst_fr_bul = new_fr_bul;
	// if (this->bullet_id >= 15 && this->bullet_id <= 16) {
	// 	cv::imshow("result", res);
	// 	cv::waitKey();
	// }

	if (this->bullet_id) {
		cv::putText(res, "ID: " + std::to_string(this->bullet_id), cv::Point(0, 20),
				cv::FONT_HERSHEY_SIMPLEX, 0.75, cv::Scalar(255, 255, 255), 2);
	}

	return res;
}
