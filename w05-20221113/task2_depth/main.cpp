#define CAMERA_PATH "../data/camera.yaml"
#define DIR_PATH "../data/stereo-data/"

#include "VO.h"
#include <Eigen/Dense>

int main(int argc, char **argv) {
  cv::Mat img1 = cv::imread(DIR_PATH "0_orig.jpg");
  cv::Mat dpt1 = cv::imread(DIR_PATH "0_dpt.tiff", cv::IMREAD_ANYDEPTH);
  cv::Mat img2 = cv::imread(DIR_PATH "1_orig.jpg");
  cv::Mat dpt2 = cv::imread(DIR_PATH "1_dpt.tiff", cv::IMREAD_ANYDEPTH);

	cv::FileStorage params(CAMERA_PATH, cv::FileStorage::READ);
	cv::Mat K = params["K"].mat();

  cv::Mat rvec, tvec, R, t;
  find_PnP(img1, dpt1, img2, dpt2, K, rvec, tvec);
  find_use_E(img1, dpt1, img2, dpt2, K, R, t);

  process_Stitch_project(img1, dpt1, img2, dpt2, K, R, tvec, "result");

  return 0;
}
