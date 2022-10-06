#ifndef __OPER_HPP__
#define __OPER_HPP__

#include <vector>
#include <string>

enum OperType {
	Create, Del, Mv, Set
};

struct Oper { // 操作类
	OperType type;
	std::string x, y;
	uint corner;

	Oper();

	Oper& make_create(const std::string&, const std::string&, uint); // 创建 create 的 Oper，以下同理
	Oper& make_del(const std::string&);
	Oper& make_mv(const std::string&, const std::string&);
	Oper& make_set(const std::string&, uint);
};

struct OperList { // 操作列表类
	std::vector<Oper> oper_list;
	std::vector<std::vector<Oper> > oper_undo_list;
	uint cur_pos; // 当前列表位置

	OperList();

	void do_oper(const Oper&, const Oper&); // 添加操作
	void do_oper(const Oper&, const std::vector<Oper>&); // 添加操作
	bool do_undo(std::vector<Oper>&); // 撤回
	bool do_redo(Oper&); // 重做
};

#endif
