#include <Eigen/Dense>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <vector>

#include "data/big_armor_scale.hpp"

Eigen::Quaterniond imu_quat;
std::vector<cv::Point2d> armor_img;
cv::Mat camera_matrix;
cv::Mat dist_coeffs;

double read_double(FILE *f);
void read_imu_and_armor();
void read_matrices();
void doit();

int main() {
  read_imu_and_armor();
  read_matrices();

  doit();

  return 0;
}

void doit() {
  cv::Mat rvec, tvec;
  cv::solvePnP(PW_BIG, armor_img, camera_matrix, dist_coeffs, rvec, tvec);
	std::cerr << "dist_coeffs: " << dist_coeffs << '\n';
  std::cerr << "* tvec: " << tvec << '\n';

  Eigen::Matrix3d mat;
  mat << -9.99945760e-01, -9.96630453e-03, 2.95451283e-03,
		3.08115920e-03, -1.27160549e-02, 9.99914289e-01,
		-9.92788188e-03, 9.99869347e-01, 1.27462745e-02;

  Eigen::Vector3d vec{tvec.at<double>(0, 0), tvec.at<double>(1, 0),
                      tvec.at<double>(2, 0)};
  Eigen::Vector3d res = imu_quat.matrix() * mat.transpose() * vec;
  std::cerr << res << '\n';
}

double read_double(FILE *f) {
  char c = getc(f);
  bool pos = true;
  while (true) {
    if (c == '-') {
      c = getc(f);
      if (c >= '0' && c <= '9') {
        pos = false;
        break;
      }
    } else if (c >= '0' && c <= '9') {
      break;
    }
    c = getc(f);
  }
  double ret = c - '0';
  while ((c = getc(f))) {
    if (c < '0' || c > '9')
      break;
    ret = ret * 10 + c - '0';
  }
  if (c == '.') {
    double mul = .1;
    for (; (c = getc(f)); mul *= .1) {
      if (c < '0' || c > '9')
        break;
      ret += (c - '0') * mul;
    }
  }
  if (fabs(ret) > 1E-9 && c == 'e') {
    bool exp_pos = true;
    if (getc(f) == '-')
      exp_pos = false;
    int exp = 0;
    while ((c = getc(f))) {
      if (c < '0' || c > '9')
        break;
      (exp *= 10) += c - '0';
    }
    while (exp--)
      ret *= exp_pos ? 10 : .1;
  }
  return pos ? ret : -ret;
}

void read_imu_and_armor() {
  FILE *f = fopen("../data/imu_and_armor.txt", "r");
  std::vector<double> imu_vec(4);
  for (double &x : imu_vec) {
    x = read_double(f);
  }
  imu_quat = {imu_vec[3], imu_vec[0], imu_vec[1], imu_vec[2]};
  for (uint32_t i = 0; i < 4; ++i) {
    double x = read_double(f), y = read_double(f);
    armor_img.emplace_back(x, y);
  }
}

void read_matrices() {
  FILE *f = fopen("../data/f_mat_and_c_mat.yml", "r");
  for (uint8_t i = 0; i < 3; ++i)
    read_double(f);
  camera_matrix = cv::Mat::zeros(3, 3, CV_64F);
  for (uint8_t i = 0; i < 3; ++i) {
    for (uint8_t j = 0; j < 3; ++j) {
      camera_matrix.at<double>(cv::Point(j, i)) = read_double(f);
    }
  }
  for (uint8_t i = 0; i < 2; ++i)
    read_double(f);
  dist_coeffs = cv::Mat::zeros(1, 5, CV_64F);
  for (uint8_t j = 0; j < 5; ++j) {
    dist_coeffs.at<double>(cv::Point(j, 0)) = read_double(f);
  }
}
