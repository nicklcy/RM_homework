# 罗成元 交龙战队视觉部笔试解题报告

## 第一题：实现一个表格处理类

---

## 介绍

本程序实现了全部三个梯度，还有其他特殊功能：

1. 实现了单元格内的居中对齐；
2. 判断在合并单元格时是否是一个矩形。
3. 添加数据时可添加任意长度且 `cin` 能识别的 `string`；
4. 使用差分约束系统确定表格每一列的宽度；
5. 加了 `quit` 指令，输入 `quit` 时退出程序。

---

## 如何使用这个程序

### create 指令

输入方式同题目要求，输入 `create` $n$ $m$。

#### 要求：

$n, m$ 均为整数，$n, m > 0$ 且 $n \times m \le 10^6$。

如果 $n, m \le 0$ 或 $n \times m > 10^6$ 该程序会报错（但是并没有判断 $n, m$ 是否为 `int` 范围内的整数）。

注意如果当且已经有一个表格，然后再进行 `create` 操作，则会把之前的表格覆盖掉。

### add 指令

输入方式同题面要求，输入 `add` $i$ $j$ $s$。

#### 要求：

$0 \le i < n, 0 \le j < m$，$s$ 为任意 `cin` 能识别的 `string`（长度不限）。

如果不满足上述条件程序会报错。

### merge 指令

输入方式同题目要求，输入 `merge` $i_1$ $j_1$ $i_2$ $j_2$。合并后单元格的数据将于原来左上角单元格的数据一致。

#### 要求：

$0 \le i_1 \le i_2 < n, 0 \le j_1 \le j_2 < m$，且能合并成一个矩形。

如果不满足上述条件会报错。

### split 指令

输入方式同题目要求，输入 `split` $i$ $j$。将把 $(i, j)$ 所在的单元格全部取消合并，并且将当前的数据赋予新的单元格。

#### 要求：

$0 \le i < n, 0 \le j < m$。

如果不满足上述条件会报错。

### remove 指令

输入方式同题面要求，输入 `remove` $i$ $j$，将把 $(i, j)$ 所在单元格的数据删除。注意如果 $(i, j)$ 已被合并，则删除合并后单元格的数据。

#### 要求：

$0 \le i < n, 0 \le j < m$。

如果不满足上述条件会报错。

### clear 指令

输入方式同题目要求。

#### 要求：

无。

### quit 指令

输入 `quit` 即退出程序。

---

## 我的实现

对于每一个单元格，存储其数据，以及合并后所在单元格的左上角、右下角坐标。每次操作时，把其在内的数据、左上角、右下角坐标全部更改。

输出方式：首先用差分约束系统确定每一列的宽度。因为有边权为负的边，所以使用 `SPFA` 求最短路。

接下来，开一个 `string` 数组表示最终要输出的表格。先把表格的分界线确定下来，再考虑每个单元格的内容，居中放入数组中。

~~虽然这样做效率不高，但实现起来比较简单（关键是最近有点忙）~~