#include <iostream>
#include <string>

#include <opencv2/opencv.hpp>

cv::Mat do_match(const cv::Mat &img1, const cv::Mat &img2,
                 const double alpha = .4);

int main(int argc, char **argv) {
  std::string file_input1 = "../pictures/9407.png";
  std::string file_input2 = "../pictures/9408.png";
  std::string file_output = "../res.png";

  if (argc > 3) {
    file_input1 = argv[1];
    file_input2 = argv[2];
    file_output = argv[3];
  }

  std::cerr << "Pictures path: " << file_input1 << " and " << file_input2
            << std::endl;

  cv::Mat img1 = cv::imread(file_input1);
  cv::Mat img2 = cv::imread(file_input2);

  cv::Mat res = do_match(img1, img2);
  cv::imwrite(file_output, res);

  return 0;
}

cv::Mat do_match(const cv::Mat &img1, const cv::Mat &img2, const double alpha) {
  cv::Mat gray1, gray2;
  cv::cvtColor(img1, gray1, cv::COLOR_BGR2GRAY);
  cv::cvtColor(img2, gray2, cv::COLOR_BGR2GRAY);

  cv::Ptr<cv::ORB> orb = cv::ORB::create();

  std::cerr << "Detect key points..." << std::endl;
  std::vector<cv::KeyPoint> keypoints1, keypoints2;
  orb->detect(gray1, keypoints1);
  orb->detect(gray2, keypoints2);

  std::cerr << "Compute descriptors..." << std::endl;
  cv::Mat descriptor1, descriptor2;
  orb->compute(gray1, keypoints1, descriptor1);
  orb->compute(gray2, keypoints2, descriptor2);

  std::cerr << "Do matching..." << std::endl;
  cv::BFMatcher matcher(cv::NORM_HAMMING);
  std::vector<cv::DMatch> matches;
  matcher.match(descriptor1, descriptor2, matches);

  std::cerr << "matches size: " << matches.size() << std::endl;

  std::vector<double> distances;
  for (const cv::DMatch &match : matches) {
    distances.emplace_back(match.distance);
  }
  std::sort(distances.begin(), distances.end());
  double threshold = distances[(size_t)(alpha * distances.size())];

  std::vector<cv::DMatch> filter_match;
  for (const cv::DMatch &match : matches) {
    if (match.distance < threshold) {
      filter_match.emplace_back(match);
    }
  }

  std::cerr << "filter match size: " << filter_match.size() << std::endl;

  cv::Mat res;
  cv::drawMatches(img1, keypoints1, img2, keypoints2, filter_match, res);
  return res;
}
