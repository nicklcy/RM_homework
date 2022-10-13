#include <opencv2/opencv.hpp>
#include <vector>
using namespace std;
using namespace cv;

// 只保留 image 的红色部分
Mat1b get_red(const Mat3b &image) {
    Mat3b hsv;
    cvtColor(image, hsv, COLOR_BGR2HSV);
    Mat1b mask1, mask2;
    inRange(hsv, Scalar(0, 70, 50), Scalar(20, 255, 255), mask1);
    inRange(hsv, Scalar(170, 70, 50), Scalar(180, 255, 255), mask2);
    return mask1 | mask2;
}

// 求两点之间距离
float get_dis(const Point &p, const Point2f &q) {
    return sqrt((p.x-q.x)*(p.x-q.x) + (p.y-q.y)*(p.y-q.y));
}

// 将一个点集，按到某点的距离，从小到大排序
void sort_points_by_dis(vector<Point> &points, const Point2f &center) {
    using P = pair<double, Point>;
    vector<P> vec;
    for (const auto &p : points) {
        vec.emplace_back(get_dis(p, center), p);
    }
    sort(begin(vec), end(vec), [](const P &x, const P &y) {
        return x.first < y.first;
    });
    vector<Point> ret;
    for (const auto &p : vec) {
        ret.emplace_back(p.second);
    }
    points.swap(ret);
}

// 找到点集中距某个最短的点
Point get_closest(const vector<Point> &points, const Point2f &center) {
    Point ret = points[0];
    float dis = get_dis(points[0], center);
    for (const auto &p : points) {
        float tmp = get_dis(p, center);
        if (dis > tmp) {
            dis = tmp; ret = p;
        }
    }
    return ret;
}

// 画要求的五边形
void draw_shape(Mat3b &image, const vector<Point> &outside, const vector<Point> &inside, const Point2f &center, const Scalar &color) {
    vector<Point> poly;
    approxPolyDP(inside, poly, 5, true);
    vector<Point> rect = poly;
    sort_points_by_dis(rect, center);
    for (size_t i = 0; i < rect.size(); ++i) {
        Point a = poly[i], b = poly[(i + 1) % rect.size()];
        if (((a != rect[0]) || (b != rect[1])) &&
            ((a != rect[1]) || (b != rect[0]))) {
            line(image, a, b, color, 2);
        }
    }
    Point p = get_closest(outside, center);
    line(image, rect[0], p, color, 2);
    line(image, rect[1], p, color, 2);
}

// 处理
void do_frame(const Mat3b &in_frame, Mat3b &out_frame) {
    Mat1b red_image = get_red(in_frame);
    out_frame = in_frame.clone();
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(red_image, contours, hierarchy, RETR_TREE, CHAIN_APPROX_NONE, Point());

    // 找 R
    size_t min_size = numeric_limits<size_t>::max();
    for (auto contour : contours) {
        if (contour.size() > 40) {
            if (min_size > contour.size()) {
                min_size = contour.size();
            }
        }
    }
    vector<Point> contour_R;
    for (auto contour : contours) {
        if (contour.size() == min_size) {
            contour_R = contour;
        }
    }
    // 找包围 R 的圆
    Point2f center; float radius;
    minEnclosingCircle(contour_R, center, radius);
    circle(out_frame, center, radius, Scalar(255, 255, 255), 2);

    vector<vector<size_t>> belong(contours.size()); // belong[i] 哪些轮廓在 i 的内部，也即判断 i 是否激活
    for (size_t i = 0; i < contours.size(); ++i) {
        if (contours[i].size() > 20 && ~hierarchy[i][3]) {
            belong[hierarchy[i][3]].emplace_back(i);
        }
    }
    vector<bool> activated(contours.size());
    static size_t rect_size; // 扇片边上矩形的点击大小
    for (size_t i = 0; i < contours.size(); ++i) {
        if (!belong[i].size()) continue;
        if (contours[i].size() > min_size) {
            bool flag = false;
            vector<Point> poly;
            approxPolyDP(contours[i], poly, 15, true);
            for (size_t i = 0; i < poly.size(); ++i) {
                bool away = true;
                for (size_t j = 0; j < poly.size(); ++j) {
                    if (i == j) continue;
                    away &= get_dis(poly[i], poly[j]) > 60;
                }
                flag |= away;
            }
            if (flag) { // 未激活能量扇片
                size_t max_size = contours[belong[i][0]].size();
                for (size_t j : belong[i]) {
                    if (max_size < contours[j].size()) {
                        max_size = contours[j].size();
                    }
                }
                vector<Point> contour;
                for (size_t j : belong[i]) {
                    if (max_size == contours[j].size()) {
                        contour = contours[j];
                    }
                }
                rect_size = contour.size();
                draw_shape(out_frame, contours[i], contour, center, Scalar(0, 255, 0));
            } else {
                activated[i] = true;
            }
        }
    }
    for (size_t i = 0; i < contours.size(); ++i) {
        if (activated[i]) { // 已激活的能量扇片
            int diff = numeric_limits<int>::max(); // 为了找矩形，这里比较的是点集大小与 rect_size 的差
            for (size_t j : belong[i]) {
                int tmp = abs(static_cast<int>(contours[j].size()) - rect_size);
                if (diff > tmp) diff = tmp;
            }
            for (size_t j : belong[i]) {
                int tmp = abs(static_cast<int>(contours[j].size()) - rect_size);
                if (diff == tmp) { // 第 j 个轮廓是矩形
                    draw_shape(out_frame, contours[i], contours[j], center, Scalar(255, 0, 0));
                }
            }
        }
    }
}

int main(int argc, char** argv) {
    VideoCapture inputVideo(argv[1]);
    assert(inputVideo.isOpened());
    int ex = static_cast<int>(inputVideo.get(CAP_PROP_FOURCC));     // Get Codec Type- Int form
    Size S = Size(static_cast<int>(inputVideo.get(CAP_PROP_FRAME_WIDTH)),    // Acquire input size
                  static_cast<int>(inputVideo.get(CAP_PROP_FRAME_HEIGHT)));
    int frames = inputVideo.get(CAP_PROP_FRAME_COUNT);

    VideoWriter outputVideo;
    outputVideo.open("output.mp4", ex, inputVideo.get(CAP_PROP_FPS), S, true);

    int i = 0;
    Mat3b src, res;
    while (true) { //Show the image captured in the window and repeat
        inputVideo >> src;              // read
        if (src.empty()) break;         // check if at end
        ++i;
        cerr << "Processing frame " << i << ' ' << 10000 * i / frames * 0.01 << "%" << endl;
        do_frame(src, res);
        outputVideo << res;
    }
    cerr << "Finished writing" << endl;
    return 0;
}
