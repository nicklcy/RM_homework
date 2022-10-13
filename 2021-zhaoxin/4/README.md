# 罗成元 交龙战队视觉部笔试解题报告

## 第四题：OpenCV 实践题

---

## 介绍

本程序实现了全部 $3$ 个梯度。

---

## 如何使用这个程序

代码在 `source` 下的 `main.cpp`。

编译出的文件已经在 `source/build` 文件夹下了。如果需要重新编译，可以清空 `build` 文件夹，并在 `build` 文件夹内 `cmake ..; make`。

编译出的可执行文件为 `source/build/main`。使用方式：先进入 `source/build`，`./main $1`，其中 `$1` 为输入视频文件名。输出的视频文件为 `source/build/output.mp4`。如果需要用下发的视频来评测，可以使用 `./main ../video.mp4`。

---

## 实现方式

首先注意到能量机关都是红色的，所以先将图片中红色的部分提取出来。

接下来使用 `findContours` 求出这些红色部分的轮廓。因为图片中可能存在比较细小的红点，所以要将点集大小比较小的 `contours` 忽略。

剩下来的轮廓中，点集大小最小的即是 R。用 `minEnclosingCircle` 求出最小包围 R 的圆，并用 `circle` 画出这个圆。

然后考虑所有的扇叶。注意到待激活扇叶，考虑用 `approxPolyDp` 求出外轮廓的多边形，则多边形上有 $1$ 个点离其他点较远。然后我们可以找到这个扇叶内部的轮廓中，点集大小较大的那个，即为装甲板。在已激活扇叶中，我们可以通过与待激活扇叶的装甲板，进行点集大小的比较，最接近的那个就是已激活扇叶内的装甲板。

还有一个问题，是如何找到一个扇叶的五个角点。我的方法是用 `approxPolyDP` 求出装甲板的矩形，随后对于 4 个角点，求出与 R 圆心距离最近的两个点，并求出对于这个扇叶外围轮廓中距离 R 圆心最近的那个点进行连边。

---

## 参考

[提取红色的部分](https://stackoverflow.com/questions/32522989/opencv-better-detection-of-red-color/32523532#32523532)

[findContours](https://blog.csdn.net/laobai1015/article/details/76400725)

[minEnclosingCircle 与多边形逼近](https://blog.csdn.net/qq_41553038/article/details/80213038)

[画直线](https://opencv-tutorial.readthedocs.io/en/latest/draw/draw.html)

[Point](https://docs.opencv.org/3.4/db/d4e/classcv_1_1Point__.html)

[视频读写](https://docs.opencv.org/4.5.3/d3/d9c/samples_2cpp_2tutorial_code_2videoio_2video-write_2video-write_8cpp-example.html#_a8)
