#ifndef __FRAMEWORK_HPP__
#define __FRAMEWORK_HPP__

#include <string>
#include <vector>
#include <set>
#include <map>
#include "message.cpp"
#include "output.cpp"
#include "node.cpp"

struct Framework { // 组件类
	std::map<std::string, Node*> mp_id_node; // 编号到指针的映射

	Framework();

	Node* get_node(const std::string&) const; // 获取指定编号的节点指针
	bool exist(const std::string&) const; // 是否存在

	Message create(const std::string&, const std::string&, uint); // 创建
	ResultMessage<std::vector<CreateNode> > del(const std::string&); // 删除
	ResultMessage<std::string> mv(const std::string&, const std::string&); // 移动
	ResultMessage<uint> set_corner(const std::string&, uint); // 设置边角
};

#endif
