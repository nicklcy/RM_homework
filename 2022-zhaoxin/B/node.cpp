#ifndef __NODE_CPP__
#define __NODE_CPP__

#include <vector>
#include <string>
#include "node.hpp"

CreateNode::CreateNode(const std::string &_pa_id, const std::string &_ch_id, uint _ch_corn) {
	pa_id = _pa_id;
	ch_id = _ch_id;
	ch_corn = _ch_corn;
}

Node::Node() {}

Node::Node(const std::string &_id, uint _corner = 0) {
	id = _id; corner = _corner;
	par = nullptr;
	rows = cols = 0;
}

uint Node::change_corner(const uint& new_corner) { // 更改边角类型
	uint ret = corner; // 返回原边角类型
	corner = new_corner;
	return ret;
}

std::string Node::change_par(Node* new_par) { // 更改父亲
	auto ret = par->id;
	par->del_child(this);
	par = new_par;
	par->add_child(this);
	return ret;
}

void Node::add_child(Node* ch) { // 添加儿子
	child.emplace(std::make_pair(ch->id, ch));
}

void Node::del_child(Node* ch) { // 删除儿子
	auto it = child.find(std::make_pair(ch->id, ch));
	if (it != child.end()) child.erase(it);
}

void Node::del_recursive(std::vector<CreateNode> &vc) { // 递归删除
	vc.emplace_back(CreateNode(par->id, id, corner));
	par->del_child(this);
	while (!child.empty()) {
		auto *ch = child.begin()->second;
		ch->del_recursive(vc);
	}
}

bool Node::has_in_subtree(const std::string &target) { // 在子树中存在 target
	if (id == target) return true;
	for (const auto &[_, ch]: child) {
		if (ch->has_in_subtree(target)) return true;
	}
	return false;
}

void Node::get_size(uint dir) { // 获取大小
	// 初始化大小
	rows = 5; cols = 4 + id.size();

	// 遍历子节点
	for (const auto &[_, ch]: child) {
		ch->get_size(dir ^ 1); // 对儿子遍历要换个排列方向
		if (dir) { // 水平排列
			rows = std::max(rows, ch->rows + 2);
			cols += ch->cols + 1;
		} else { // 垂直排列
			cols = std::max(cols, ch->cols + 2);
			rows += ch->rows + 1;
		}
	}
}

void Node::output(Output& output, uint si, uint sj, uint dir) const { // 输出
	output.output_border(si, sj, rows, cols, corner);
	if (dir) { // 水平
		output.output_id(si + (rows >> 1), sj + 2, id);
		sj += id.size() + 1;
	} else {
		output.output_id(si + 2, sj + ((cols - id.size()) >> 1), id);
		si += 2;
	}

	// 遍历子节点
	for (const auto &[_, ch]: child) {
		if (dir) { // 水平排列
			ch->output(output, si + ((rows - ch->rows) >> 1), sj + 2, dir ^ 1);
			sj += 1 + ch->cols;
		} else { // 垂直排列
			ch->output(output, si + 2, sj + ((cols - ch->cols) >> 1), dir ^ 1);
			si += 1 + ch->rows;
		}
	}
}

#endif
