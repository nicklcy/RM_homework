#ifndef __NODE_HPP__
#define __NODE_HPP__

#include <string>
#include <vector>
#include <set>
#include "output.cpp"

using uint = unsigned int;

struct CreateNode { // 表达一个创建节点的类
	std::string pa_id, ch_id;
	uint ch_corn;

	CreateNode(const std::string&, const std::string&, uint);
};

struct Node { // 组件节点类
	std::string id; // 编号
	uint corner; // 边角类别
	Node* par; // 父亲节点
	std::set<std::pair<std::string, Node*> > child; // 儿子节点
	uint rows, cols; // 大小

	Node();
	Node(const std::string&, uint);

	uint change_corner(const uint&); // 更改边角类型
	std::string change_par(Node*); // 更改父亲
	void add_child(Node*); // 添加儿子
	void del_child(Node*); // 删除儿子
	void del_recursive(std::vector<CreateNode>&); // 递归删除
	bool has_in_subtree(const std::string&);

	void get_size(uint); // 获取大小，参数表示方向（0 垂直，1 水平）
	void output(Output&, uint, uint, uint) const; // 输出到 Output，起始坐标，方向
};

#endif
