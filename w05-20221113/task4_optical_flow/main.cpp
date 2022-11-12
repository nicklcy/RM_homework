#include <iostream>
#include <string>

#include <opencv2/opencv.hpp>

#define INPUT_VIDEO_PATH "../lk.mp4"
#define OUTPUT_VIDEO_PATH "../result.mp4"

cv::Mat trans_gray(const cv::Mat &src);

int main(int argc, char **argv) {
  cv::VideoCapture capture(INPUT_VIDEO_PATH);

  assert(capture.isOpened());
  cv::Size SIZE = cv::Size(capture.get(cv::CAP_PROP_FRAME_WIDTH),
                           capture.get(cv::CAP_PROP_FRAME_HEIGHT));
  double FPS_RATE = capture.get(cv::CAP_PROP_FPS);

  cv::VideoWriter writer;
  writer.open(OUTPUT_VIDEO_PATH, cv::VideoWriter::fourcc('m', 'p', '4', 'v'),
              FPS_RATE, SIZE, true);
  assert(writer.isOpened());

  std::vector<cv::Point2f> prv_points;
  size_t lst_size = 1E9;

  cv::Mat src, prv_src;
  int id = 0;
  while (true) {
    capture >> src;
    if (src.empty())
      break;
    ++id;
    std::cerr << "Frame id: " << id << std::endl;
    cv::Mat res = src.clone();

    std::vector<cv::Point2f> cur_points;
    std::vector<uint8_t> status;
    std::vector<float> error;
    if (prv_points.size() * 2 < lst_size) {
      cv::Ptr<cv::ORB> orb = cv::ORB::create();
      std::vector<cv::KeyPoint> keypoints;
      orb->detect(trans_gray(src), keypoints);
      for (const cv::KeyPoint &keypoint : keypoints) {
        prv_points.emplace_back(keypoint.pt);
        cv::circle(res, keypoint.pt, 2, cv::Scalar(0, 0, 255), -1);
      }
      lst_size = prv_points.size();
    } else {
      cv::calcOpticalFlowPyrLK(prv_src, src, prv_points, cur_points, status,
                               error);

      std::vector<cv::Point2f> new_points;
      for (size_t i = 0; i < prv_points.size(); ++i) {
        if (status[i] && error[i] < 20) {
          const cv::Point2f prv = prv_points[i], cur = cur_points[i];
          // std::cerr << "== " << i << ' ' << prv << ' ' << cur << ' ' << error[i]
          //           << std::endl;
          cv::circle(res, cur, 2, cv::Scalar(0, 0, 255), -1);
          cv::line(res, prv, cur, cv::Scalar(0, 255, 0));
          new_points.emplace_back(cur);
        }
      }
      new_points.swap(prv_points);
    }
    // cv::imshow("res", res);
    // cv::waitKey(0);
    writer << res;

    prv_src = src.clone();
  }

  writer.release();

  return 0;
}

cv::Mat trans_gray(const cv::Mat &src) {
  cv::Mat gray;
  cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
  return gray;
}
