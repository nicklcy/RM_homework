#include <iostream>
#include <string>

#include <Eigen/Dense>
#include <opencv2/opencv.hpp>

#define DATA_PATH "../data/"
#define VIDEO_PATH DATA_PATH "video/"
#define LEFT_VIDEO_PATH VIDEO_PATH "left_10.mp4"
#define RIGHT_VIDEO_PATH VIDEO_PATH "right_10.mp4"
#define STEREO_PATH DATA_PATH "stereo.yaml"
#define EXTRINSICS_PATH DATA_PATH "extrinsics.yml"
#define OUTPUT_VIDEO_PATH "../result.mp4"

#include "detect_infantry.hpp"

cv::Mat K_1, C_1, K_2, C_2;
cv::Mat P_1, P_2;

void get_coeff_matrices();
cv::Mat do_trans(const cv::Mat &src1, const cv::Mat &src2);

int main(int argc, char **argv) {
  get_coeff_matrices();

  cv::VideoCapture capture1(LEFT_VIDEO_PATH);
  cv::VideoCapture capture2(RIGHT_VIDEO_PATH);

  assert(capture1.isOpened());
  assert(capture2.isOpened());
  cv::Size SIZE = cv::Size(capture1.get(cv::CAP_PROP_FRAME_WIDTH),
                           capture1.get(cv::CAP_PROP_FRAME_HEIGHT));
  double FPS_RATE = capture1.get(cv::CAP_PROP_FPS);

  cv::VideoWriter writer;
  writer.open(OUTPUT_VIDEO_PATH, cv::VideoWriter::fourcc('m', 'p', '4', 'v'),
              FPS_RATE, SIZE, true);
  assert(writer.isOpened());

  cv::Mat src1, src2;
  int id = 0;
  while (true) {
    capture1 >> src1;
    capture2 >> src2;
    id += 1;
    if (id < 0)
      continue;
    if (src1.empty() || src2.empty())
      break;
    std::cerr << "Frame id: " << id << std::endl;
    cv::Mat res = do_trans(src1, src2);
    cv::putText(res, "Frame id: " + std::to_string(id),
                cv::Point(res.cols - 230, 20), cv::FONT_HERSHEY_SIMPLEX, 0.75,
                cv::Scalar(255, 255, 255), 2);
    writer << res;
  }

  writer.release();

  return 0;
}

void get_coeff_matrices() {
  cv::FileStorage ste_params(STEREO_PATH, cv::FileStorage::READ);

  K_1 = ste_params["K_0"].mat();
  C_1 = ste_params["C_0"].mat();
  K_2 = ste_params["K_1"].mat();
  C_2 = ste_params["C_1"].mat();

  cv::FileStorage ext_params(EXTRINSICS_PATH, cv::FileStorage::READ);
  P_1 = ext_params["P1"].mat();
  P_2 = ext_params["P2"].mat();
}

cv::Mat do_trans(const cv::Mat &src1, const cv::Mat &src2) {
  std::vector<cv::Point2f> pts1 = detect_infantry(src1);
  std::vector<cv::Point2f> pts2 = detect_infantry(src2);

  if (pts1.empty() || pts2.empty())
    return src1;

  std::vector<cv::Point2f> undistort_pts1, undistort_pts2;
  cv::undistortPoints(pts1, undistort_pts1, K_1, C_1);
  cv::undistortPoints(pts2, undistort_pts2, K_2, C_2);

  if (undistort_pts1.size() > undistort_pts2.size()) {
    undistort_pts1.resize(undistort_pts2.size());
  } else {
    undistort_pts2.resize(undistort_pts1.size());
  }

  cv::Mat res;
  cv::triangulatePoints(P_1, P_2, undistort_pts1, undistort_pts2, res);

  float sum_depth = 0;
  for (size_t i = 0; i < res.cols; ++i) {
    sum_depth += res.at<float>(2, i) / res.at<float>(3, i);
  }
  float depth = sum_depth / res.cols * 1E-6;
  std::cerr << "Depth: " << depth << std::endl;
  return print(src1, pts1, depth);
}
