#ifndef __FRAMEWORK_CPP__
#define __FRAMEWORK_CPP__

#include "framework.hpp"
#include "message.cpp"
#include "output.cpp"

Framework::Framework() {
	mp_id_node["0"] = new Node("0");
}

Node* Framework::get_node(const std::string &id) const { // 根据编号获取 Node
	return mp_id_node.at(id);
}

bool Framework::exist(const std::string &id) const { // 是否存在
	return mp_id_node.count(id);
}

Message Framework::create(const std::string &id, const std::string &pid, uint corner) { // 创建
	if (exist(id)) return Message("已经存在这个组件的节点");
	if (!exist(pid)) return Message("不存在父亲组件");
	auto node = mp_id_node[id] = new Node(id, corner);
	auto p_node = get_node(pid);
	node->par = p_node;
	p_node->add_child(node);
	return Message();
}

ResultMessage<std::vector<CreateNode> > Framework::del(const std::string &id) { // 删除
	if (!exist(id)) return Message("这个组件不存在");
	if (id == "0") return Message("不能删除编号为 0 的组件");

	std::vector<CreateNode> vc;
	auto node = get_node(id);
	node->del_recursive(vc); // 将子树中的都删除

	for (const auto &e: vc) { // 从 mp_id_node 中删除
		mp_id_node.erase(mp_id_node.find(e.ch_id));
	}
	return ResultMessage(vc);
}

ResultMessage<std::string> Framework::mv(const std::string &cid, const std::string &pid) { // 移动
	if (!exist(cid)) return Message("x 组件不存在");
	if (cid == "0") return Message("x 不能为 0");
	if (!exist(pid)) return Message("y 组件不存在");

	auto node = get_node(cid), p_node = get_node(pid);
	if (node->has_in_subtree(pid)) return Message("y 组件在 x 组件下"); // 不能循环嵌套
	return ResultMessage<std::string>(node->change_par(p_node));
}

ResultMessage<uint> Framework::set_corner(const std::string &id, uint corner) { // 设置边角
	if (!exist(id)) return Message("组件不存在");
	return get_node(id)->change_corner(corner);
}

std::ostream& operator << (std::ostream &os, const Framework& comp) { // 输出
	const auto &child = comp.get_node("0")->child;
	for (const auto &[_, ch]: child) {
		ch->get_size(0);
		Output output(ch->rows, ch->cols);
		ch->output(output, 0, 0, 0);
		os << output << '\n';
	}
	return os;
}

#endif
