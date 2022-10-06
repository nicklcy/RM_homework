#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

using uint = unsigned int;

float FPS_RATE;

// 只保留 image 的蓝色部分
cv::Mat1b get_blue(const cv::Mat3b &image) {
	cv::Mat1b res(image.rows, image.cols);
	for (uint i = 0; i < image.rows; ++i) {
		for (uint j = 0; j < image.cols; ++j) {
			const cv::Vec3b &col = image[i][j];
			if ((col[0] > 125 && col[1] > 125 && col[2] > 150) ||
					(col[1] > 220 && col[2] > 220)) {
				// res[i][j][0] = res[i][j][1] = res[i][j][2] = 255;
				res[i][j] = 255;
			} else {
				// res[i][j][0] = res[i][j][1] = res[i][j][2] = 0;
				res[i][j] = 0;
			}
		}
	}
	return res;
}

float get_width(const cv::RotatedRect &rect) { // 获取矩形宽度
	return rect.angle < 45 ? rect.size.width : rect.size.height;
}

float get_height(const cv::RotatedRect &rect) { // 获取矩形高度
	return rect.angle < 45 ? rect.size.height : rect.size.width;
}

float get_angle(const cv::RotatedRect &rect) { // 获取矩形角度
	return rect.angle - (rect.angle > 45 ? 90 : 0);
}

void print_rot_rect(cv::Mat &out, const cv::RotatedRect &rect) { // 输出矩形
	static cv::Point2f pts[4];
	rect.points(pts);
	for (uint i = 0; i < 4; ++i) {
		cv::line(out, pts[i], pts[(i + 1) & 3], cv::Scalar(255, 122, 0), 3);
	}
}

bool try_rects(cv::Mat &out_frame, cv::RotatedRect r1, cv::RotatedRect r2, cv::RotatedRect &res) {
	if (r1.center.x > r2.center.x) swap(r1, r2); // 保持 r1 在左边
	auto dx = r2.center.x - r1.center.x;
	auto dy = fabs(r2.center.y - r1.center.y);
	auto h = .5 * (get_height(r1) + get_height(r2));
	auto px = dx / h, py = dy / h;
	if (px < 4 && py < 1 && px + py < 4) { // 能组成一个装甲板
		// 获取所有角点
		cv::Point2f pts[4];
		std::vector<cv::Point2f> all_pts;
		r1.points(pts); for (auto p : pts) all_pts.emplace_back(p);
		r2.points(pts); for (auto p : pts) all_pts.emplace_back(p);
		// 找出角点最小覆盖矩形
		res = cv::minAreaRect(all_pts);
		return true;
	}
	return false;
}

std::vector<int> frame_id_vc; // 识别到装甲板水平的帧的编号

void do_frame(int frame_id, const cv::Mat &in_frame, cv::Mat &out_frame) {
	out_frame = in_frame.clone();

	auto blue_image = get_blue(in_frame);

	// out_frame = blue_image;
	// return;
	// 寻找边缘
	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(blue_image, contours, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);

	std::vector<cv::RotatedRect> rects;
	for (uint i = 0; i < contours.size(); ++i) {
		if (contours[i].size() > 50) {
			auto rect = cv::minAreaRect(contours[i]);
			if (get_height(rect) > get_width(rect) * 2 && fabs(get_angle(rect)) < 20) {
				// 可能是装甲办两边的灯
				rects.emplace_back(rect);
			}
		}
	}

	cv::RotatedRect res;
	for (uint i = 0; i < rects.size(); ++i) {
		for (uint j = 0; j < i; ++j) {
			if (try_rects(out_frame, rects[i], rects[j], res)) {
				// 是一块装甲板
				print_rot_rect(out_frame, res); // 输出
				if (fabs(get_angle(res)) < 4) { // 如果装甲板是几乎水平的
					frame_id_vc.emplace_back(frame_id);
				}
			}
		}
	}
}

float get_rotate_frames_cnt() {
	const auto &id_vc = frame_id_vc;
	std::vector<float> frames; // 装甲板水平帧数
	for (uint i = 0, j; i < id_vc.size(); ) {
		// 求帧编号相近的范围
		for (j = i + 1; j < id_vc.size(); ++j) {
			if (id_vc[j] - id_vc[j - 1] > 2) break;
		}
		// 水平帧的编号为这些帧的平均值
		frames.emplace_back(.5 * (i + j - 1) * (j - i));
		i = j;
	}

	// 计算水平帧间隔的平均值
	return 4. * (frames.back() - frames.front()) / (frames.size() - 1);
}

int main(int argc, char *argv[]) {
	cv::VideoCapture inputVideo(argv[1]);
	if (!inputVideo.isOpened()) {
		std::cerr  << "Could not open the input video: " << argv[1] << std::endl;
		return -1;
	}
	int ex = static_cast<int>(inputVideo.get(cv::CAP_PROP_FOURCC));     // Get Codec Type- Int form
	// Transform from int to char via Bitwise operators
	char EXT[] = {(char)(ex & 0XFF) , (char)((ex & 0XFF00) >> 8),(char)((ex & 0XFF0000) >> 16),(char)((ex & 0XFF000000) >> 24), 0};
	cv::Size S = cv::Size((int) inputVideo.get(cv::CAP_PROP_FRAME_WIDTH),    // Acquire input size
			(int) inputVideo.get(cv::CAP_PROP_FRAME_HEIGHT));
	FPS_RATE = inputVideo.get(cv::CAP_PROP_FPS);
	cv::VideoWriter outputVideo;                                        // Open the output
	outputVideo.open(argv[2], ex, FPS_RATE, S, true);
	cv::Mat src, res;
	for (int frame_id = 0; ; ++frame_id) { //Show the image captured in the window and repeat
		inputVideo >> src;              // read
		if (src.empty()) break;         // check if at end
		do_frame(frame_id, src, res);
		outputVideo << res;
	}
	std::cerr << "Finished writing" << std::endl;

	float rotate_cnt = get_rotate_frames_cnt();
	float frequency = FPS_RATE / rotate_cnt;
	std::cout << "Rotating frequency: " << frequency << " Hz = " << 1 / frequency << " s/rnd\n";
	std::cout << "Angular speed: " << 2 * acos(-1) * frequency << " rad/s\n";
	return 0;
}
