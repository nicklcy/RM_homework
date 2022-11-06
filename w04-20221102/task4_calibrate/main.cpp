/* usage: ./main directory_path chessboard_width chessboard_height
 * Ex: ./main ../data/calib1 11 8
 *     ./main ../data/calib2 9 6
 */

#include <filesystem>
#include <iostream>
#include <opencv2/highgui.hpp>
#include <string>
#include <vector>

#include <opencv2/opencv.hpp>

const cv::Size FIND_CORNER_SZ(10, 10);

class Calibrate {
  std::vector<std::vector<cv::Point3f>> obj_coors;
  std::vector<std::vector<cv::Point2f>> img_coors;
  cv::Size img_size;
  size_t img_cnt = 0;

public:
  cv::Mat cam_mat, dist_mat;
  std::vector<cv::Mat> rvecs, tvecs;

  Calibrate() {
    this->cam_mat = cv::Mat(3, 3, CV_32FC1);
    this->dist_mat = cv::Mat(1, 5, CV_32FC1);
  }

  // 添加新的棋盘图像
  bool add_new_image(const cv::Mat &img, const cv::Size &siz) {
    std::vector<cv::Point2f> corners;
    if (!cv::findChessboardCorners(img, siz, corners)) {
      return false;
    }
    assert(cv::find4QuadCornerSubpix(img, corners, FIND_CORNER_SZ));

    if (!this->img_size.empty()) {
      assert(this->img_size == img.size());
    }
    ++this->img_cnt;
    this->img_size = img.size();

    std::reverse(corners.begin(), corners.end());
    this->img_coors.emplace_back(corners);

    cv::Mat real;
    // cv::cvtColor(img, real, cv::COLOR_GRAY2BGR);
    // for (const cv::Point2f &pt : corners) {
    //   cv::circle(real, pt, 5, cv::Scalar(0, 0, 255), -1);
    // }
    // cv::imshow("board", real);
    // cv::waitKey(0);

    std::vector<cv::Point3f> obj_coor;
    for (uint32_t x = 0; x < siz.width; ++x) {
      for (uint32_t y = 0; y < siz.height; ++y) {
        obj_coor.emplace_back(x, y, 0);
      }
    }
    obj_coors.emplace_back(obj_coor);

    return true;
  }

  // 标定
  void do_calibrate() {
    cv::calibrateCamera(this->obj_coors, this->img_coors, this->img_size,
                        this->cam_mat, this->dist_mat, this->rvecs,
                        this->tvecs);
    std::cerr << "cam_mat:\n" << this->cam_mat << std::endl;
    std::cerr << "dist_mat:\n" << this->dist_mat << std::endl;
  }

  // 重投影误差
  double reproject_error() {
    double err = 0;
    for (size_t i = 0; i < img_cnt; ++i) {
      std::vector<cv::Point2f> proj_coor;
      cv::projectPoints(this->obj_coors[i], this->rvecs[i], this->tvecs[i],
                        this->cam_mat, this->dist_mat, proj_coor);
      err += cv::norm(this->img_coors[i], proj_coor) / proj_coor.size();
    }
    err /= img_cnt;
    return err;
  }
};

std::vector<std::string> get_files(const std::string &dir,
                                   const std::string &ext);
cv::Mat get_mask(const cv::Mat &img);

int main(int argc, char **argv) {
  Calibrate calib;

  std::vector<std::string> files = get_files(argv[1], ".jpg"); // 文件夹

  size_t board_width = std::atoi(argv[2]);  // 棋盘格子宽度
  size_t board_height = std::atoi(argv[3]); // 棋盘格子高度
  cv::Size board_size(board_width, board_height);

  for (const std::string &file : files) {
    std::cerr << "File: " << file << ": ";
    cv::Mat img = cv::imread(file);
    cv::Mat part = get_mask(img);
    if (!calib.add_new_image(part, board_size)) {
      std::cerr << "cannot find chessboard." << std::endl;
    } else {
      std::cerr << "ok." << std::endl;
    }
  }

  calib.do_calibrate();

  std::cerr << "Reprojection error: " << calib.reproject_error() << std::endl;

  return 0;
}

// 找出 dir 文件夹下所有扩展名为 ext 的文件名
std::vector<std::string> get_files(const std::string &dir,
                                   const std::string &ext) {
  std::vector<std::string> ret;
  for (const auto &p : std::filesystem::recursive_directory_iterator(dir)) {
    if (p.path().extension() == ext) {
      ret.emplace_back(p.path().string());
    }
  }
  return ret;
}

// 二值化
cv::Mat get_mask(const cv::Mat &img) {
  cv::Mat gray;
  cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);

  cv::Mat part;
  cv::adaptiveThreshold(gray, part, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C,
                        cv::THRESH_BINARY, 201, 0);
  // cv::imshow("part", part);
  // cv::waitKey();
  return part;
}
