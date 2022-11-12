#ifndef _DETECT_INFANTRY_CPP_
#define _DETECT_INFANTRY_CPP_

#include <opencv2/opencv.hpp>

std::vector<cv::Point2f> detect_infantry(const cv::Mat &img) {
  cv::Mat hsv;
  cv::cvtColor(img, hsv, cv::COLOR_BGR2HSV);

  cv::Mat part;
  cv::inRange(hsv, cv::Scalar(90, 150, 150), cv::Scalar(110, 255, 255), part);
  cv::medianBlur(part, part, 5);

  static cv::Mat kernel =
      cv::getStructuringElement(cv::MORPH_RECT, cv::Size(1, 3));
  cv::dilate(part, part, kernel);

  std::vector<std::vector<cv::Point>> contours;
  std::vector<cv::Vec4i> _hierarchy;

  cv::findContours(part, contours, _hierarchy, cv::RETR_LIST,
                   cv::CHAIN_APPROX_NONE);

  std::vector<std::pair<cv::Rect, size_t>> vec;
  for (size_t i = 0; i < contours.size(); ++i) {
    cv::Rect rect = cv::boundingRect(contours[i]);
    if (rect.x * 2 < img.cols || rect.y * 3 < img.rows)
      continue;
    vec.emplace_back(rect, i);
  }
  sort(vec.begin(), vec.end(), [](const auto &rc1, const auto &rc2) {
    if (fabs(rc1.first.x - rc2.first.x) > 1E-9)
      return rc1.first.x > rc2.first.x;
    return rc1.first.y > rc2.first.y;
  });

  std::vector<cv::Point2f> res;
  for (size_t i = 1; i < vec.size(); ++i) {
    const auto &[rc1, id1] = vec[i - 1];
    const auto &[rc2, id2] = vec[i];
    auto diff_x = rc1.x - (rc2.x + rc2.width);
    // std::cerr << "== " << diff_x << ' ' << rc1 << ' ' << rc2 << std::endl;
    if (5 < diff_x && diff_x < 40 && fabs(rc1.y - rc2.y) < 50) {
      for (const auto &pt : contours[id1]) {
        res.emplace_back(pt);
      }
      for (const auto &pt : contours[id2]) {
        res.emplace_back(pt);
      }
      cv::drawContours(img, contours, id1, cv::Scalar(0, 0, 255), 2);
      cv::drawContours(img, contours, id2, cv::Scalar(0, 0, 255), 2);
      break;
    }
  }

  return res;
}

cv::Mat print(const cv::Mat &img, const std::vector<cv::Point2f> &points,
              const float &dis) {
  cv::Mat res = img.clone();
  if (points.empty())
    return res;

  cv::RotatedRect rect = cv::minAreaRect(points);
  // cv::Point2f vertices[4];
  // rect.points(vertices);
  // for (uint8_t i = 0; i < 4; ++i) {
  //   cv::line(res, vertices[i], vertices[(i + 1) & 3], cv::Scalar(0, 0, 255));
  // }

  cv::putText(res, std::to_string(dis), rect.center + cv::Point2f(50, 0),
              cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255), 2);

  return res;
}

#endif
