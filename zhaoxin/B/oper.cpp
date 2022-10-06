#ifndef __OPER_CPP__
#define __OPER_CPP__

#include "oper.hpp"
#include <iostream>
#include <string>

Oper::Oper() {}

Oper& Oper::make_create(const std::string& _x, const std::string &_y, uint _corner) { // 创建 create 的 Oper，以下同理
	type = OperType::Create;
	x = _x; y = _y; corner = _corner;
	return *this;
}

Oper& Oper::make_del(const std::string& id) {
	type = OperType::Del;
	x = id;
	return *this;
}

Oper& Oper::make_mv(const std::string &_x, const std::string &_y) {
	type = OperType::Mv;
	x = _x; y = _y;
	return *this;
}

Oper& Oper::make_set(const std::string &id, uint _corner) {
	type = OperType::Set;
	x = id; corner = _corner;
	return *this;
}

OperList::OperList() {
	cur_pos = 0;
}

void OperList::do_oper(const Oper& oper, const std::vector<Oper>& oper_undo) { // 添加操作
	while (oper_list.size() > cur_pos) { // 添加新操作，把原来被撤回的操作删掉
		oper_list.pop_back();
		oper_undo_list.pop_back();
	}
	oper_list.emplace_back(oper);
	oper_undo_list.emplace_back(oper_undo);
	++cur_pos;
}

void OperList::do_oper(const Oper& oper, const Oper& oper_undo) { // 添加操作
	do_oper(oper, std::vector<Oper>(1, oper_undo));
}

bool OperList::do_undo(std::vector<Oper> &oper_undo) { // 撤回
	if (cur_pos == 0) {
		std::cerr << "没有上一步！\n" << std::endl;
		return false;
	}
	oper_undo = oper_undo_list[--cur_pos];
	return true;
}

bool OperList::do_redo(Oper &oper_redo) { // 重做
	if (cur_pos == oper_list.size()) {
		std::cerr << "没有下一步！\n" << std::endl;
		return false;
	}
	oper_redo = oper_list[cur_pos++];
	return true;
}

#endif
