### 罗成元 - 云汉交龙战队 2023 视觉部招新

---

所有题目主程序均在各自题目文件夹下的 `main.cpp`。其中 A、B 和 C 题均可以直接使用 `g++ main.cpp -o main -std=c++17` 编译，D 题文件夹下有 `CMakeLists.txt`，需要先 cmake 再 make 编译。

A、B、C 题均使用标准输入输出流。

D 题使用方式为
```bash
./main input output
```
其中 `input` 和 `output` 分别为导入视频和导出视频的文件名。且 D 题会在标准输出流上输出小陀螺的角速度。在 D/build 文件夹中已经有识别后的视频 `output.mp4 `。

A 题还提供了 Bash 脚本 `test_all.sh`，将样例 `grad1`，`grad2`，`grad3` 放入 A/ 文件夹下，`test_all.sh` 就会自动识别文件夹下所有样例，并进行测试。
