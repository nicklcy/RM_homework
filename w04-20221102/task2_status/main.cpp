#include <cstdio>

#include <Eigen/Dense>

/* 卡尔曼滤波
   DIM1 所有变量个数
   DIM2 观测量个数
*/
template <size_t DIM1, size_t DIM2> class Kalman {
public:
  Eigen::Matrix<double, DIM1, 1> X;    // 当前值
  Eigen::Matrix<double, DIM1, DIM1> P; // 当前协方差矩阵

  Eigen::Matrix<double, DIM1, DIM1> A; // 转移矩阵
  Eigen::Matrix<double, DIM1, DIM2> C; // 提取有观测值的项
  Eigen::Matrix<double, DIM1, DIM1> Q; // 预测噪声矩阵
  Eigen::Matrix<double, DIM2, DIM2> R; // 观测噪声矩阵

private:
  bool has_data = false; // 是否开始

public:
  Kalman(const Eigen::Matrix<double, DIM1, DIM1> &A,
         const Eigen::Matrix<double, DIM1, DIM2> &C,
         const Eigen::Matrix<double, DIM1, DIM1> &Q,
         const Eigen::Matrix<double, DIM2, DIM2> &R)
      : A(A), C(C), Q(Q), R(R) {}

  // 更新预测
  void update_predict() {
    if (!has_data)
      return;
    this->X = this->A * this->X;
    this->P = this->A * this->P * this->A.transpose() + this->Q;
  }

  // 更新观测
  Eigen::Matrix<double, DIM2, DIM2>
  update_measure(const Eigen::Matrix<double, DIM2, 1> &mes_val) {
    this->has_data = true;
    Eigen::Matrix<double, DIM1, DIM2> K =
        this->P * this->C *
        (this->C.transpose() * this->P * this->C + this->R).inverse();
    this->X += K * (mes_val - this->C.transpose() * this->X);
    this->P = (Eigen::Matrix<double, DIM1, DIM1>::Identity() -
               K * this->C.transpose()) *
              this->P;
    return this->C.transpose() * this->X;
  }

  // 求当前预测值
  Eigen::Matrix<double, DIM2, DIM2> get_predict() const {
    return this->C.transpose() * this->X;
  }
};

int main() {
  const size_t DIM1 = 3, DIM2 = 1;
  Eigen::Matrix<double, DIM1, DIM1> A;
  A << 1, 1, 0, 0, 1, 1, 0, 0, 1;
  Eigen::Matrix<double, DIM1, DIM2> C;
  C << 1, 0, 0;
  Eigen::Matrix<double, DIM1, DIM1> Q;
  Q << 4, 0, 0, 0, 4, 0, 0, 0, 5;
  Eigen::Matrix<double, DIM2, DIM2> R;
  R << 10;

  Kalman<DIM1, DIM2> do_kalman(A, C, Q, R);

  FILE *f = fopen("../data/dollar.txt", "r");
  for (uint32_t i = 0; i < 30; ++i) {
    do_kalman.update_predict();

    double val;
    fscanf(f, "%lf", &val);
    Eigen::Matrix<double, DIM2, DIM2> mat;
    mat << val;

    do_kalman.update_measure(mat);
  }

  for (uint32_t i = 0; i < 10; ++i) {
    do_kalman.update_predict();
    printf("%f\n", do_kalman.get_predict()(0, 0));
  }

  return 0;
}
