# 第一次作业 1. shell

---

### 使用方式

```bash
chmod +x ./doit.sh
./doit.sh 查找的文件夹路径
```

---

### 思路

1. 用 find 和 grep 找出文件夹下所有以 .c 或 .h 结尾的文件；
2. 对这些文件使用 for 循环，逐一 if 判断里面是否存在 hello；
3. 计时直接用 time 完成。
