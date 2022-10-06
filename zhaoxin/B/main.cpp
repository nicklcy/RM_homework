#include <cstdio>
#include <iostream>
#include <vector>
#include <string>
#include "oper.cpp"
#include "node.cpp"
#include "framework.cpp"

using uint = unsigned int;

bool is_corner(const std::string &s, uint &corner_type) { // 判断边角输入是否合法
	if (s.length() != 2) return false;
	if (s[0] != '-') return false;
	switch (s[1]) {
		case 'r':
			corner_type = 0; return true;
		case 'c':
			corner_type = 1; return true;
		case 'l':
			corner_type = 2; return true;
		default: return false;
	}
}

bool is_id(const std::string &s) { // 判断编号输入是否合法
	for (char c : s) if (c < '0' || c > '9') return false;
	return true;
}

std::vector<std::string> read_oper_vc() { // 输入命令，按照空格划分
	char c = getchar();
	if (c == EOF) exit(0);
	std::vector<std::string> ret;
	std::string cur = "";
	for (; c != '\n'; c = getchar()) {
		if (c == ' ') {
			if (cur.size() > 0) ret.emplace_back(cur);
			cur = "";
		} else {
			cur += c;
		}
	}
	if (cur.size() > 0) ret.emplace_back(cur);
	return ret;
}

OperList oper_li; // 操作列表
Framework frame; // 组件

bool do_create(const std::string &x, const std::string &y, uint corner, bool flag = 1) { // 添加
	auto msg = frame.create(x, y, corner);
	if (msg.print_error()) {
		return false;
	} else {
		if (flag) {
			Oper op = Oper().make_create(x, y, corner);
			Oper undo = Oper().make_del(x);
			oper_li.do_oper(op, undo);
		}
		return true;
	}
}

bool do_del(const std::string &x, bool flag = 1) {
	auto msg = frame.del(x);
	if (msg.print_error()) {
		return false;
	} else {
		if (flag) {
			auto vc = msg.get_val();
			std::vector<Oper> undo;
			for (const auto &e: vc) {
				undo.emplace_back(Oper().make_create(e.ch_id, e.pa_id, e.ch_corn));
			}
			oper_li.do_oper(Oper().make_del(x), undo);
		}
		return true;
	}
}

bool do_mv(const std::string &x, const std::string &y, bool flag = 1) {
	auto msg = frame.mv(x, y);
	if (msg.print_error()) {
		return false;
	} else {
		if (flag) {
			oper_li.do_oper(Oper().make_mv(x, y), Oper().make_mv(x, msg.get_val()));
		}
		return true;
	}
}

bool do_set(const std::string &x, uint corner, bool flag = 1) {
	auto msg = frame.set_corner(x, corner);
	if (msg.print_error()) {
		return false;
	} else {
		if (flag) {
			oper_li.do_oper(Oper().make_set(x, corner), Oper().make_set(x, msg.get_val()));
		}
		return true;
	}
}

bool test_create(const std::vector<std::string> &vc) {
	static const std::string usage = "Error! Usage: create x y -t";
	uint corner;
	if (vc.size() != 4 || !is_id(vc[1]) || !is_id(vc[2]) || !is_corner(vc[3], corner)) {
		std::cerr << usage << std::endl;
		return false;
	}
	return do_create(vc[1], vc[2], corner);
}

bool test_del(const std::vector<std::string> &vc) {
	static const std::string usage = "Error! Usage: del x";
	if (vc.size() != 2 || !is_id(vc[1])) {
		std::cerr << usage << std::endl;
		return false;
	}
	return do_del(vc[1]);
}

bool test_mv(const std::vector<std::string> &vc) {
	static const std::string usage = "Error! Usage: mv x y";
	if (vc.size() != 3 || !is_id(vc[1]) || !is_id(vc[2])) {
		std::cerr << usage << std::endl;
		return false;
	}
	return do_mv(vc[1], vc[2]);
}

bool test_set(const std::vector<std::string> &vc) {
	static const std::string usage = "Error! Usage: set x -t";
	uint corner;
	if (vc.size() != 3 || !is_id(vc[1]) || !is_corner(vc[2], corner)) {
		std::cerr << usage << std::endl;
		return false;
	}
	return do_set(vc[1], corner);
}

bool do_oper(const Oper &op) {
	switch (op.type) {
		case Create:
			return do_create(op.x, op.y, op.corner, false);
		case Del:
			return do_del(op.x, false);
		case Mv:
			return do_mv(op.x, op.y, false);
		case Set:
			return do_set(op.x, op.corner, false);
	}
	return false;
}

bool undo() { // 撤销
	std::vector<Oper> vc;
	if (!oper_li.do_undo(vc)) return false;
	for (const auto &op: vc) {
		if (!do_oper(op)) {
			std::cerr << "Program error!\n" << std::endl;
			return false;
		}
	}
	return true;
}

bool redo() { // 重做
	Oper op;
	if (!oper_li.do_redo(op)) return false;
	if (!do_oper(op)) {
		std::cerr << "Program error!\n" << std::endl;
		return false;
	}
	return true;
}

int main() {
	while (true) {
		auto vc = read_oper_vc();
		bool flag = false;
		if (vc[0] == "create") {
			flag = test_create(vc);
		} else if (vc[0] == "del") {
			flag = test_del(vc);
		} else if (vc[0] == "mv") {
			flag = test_mv(vc);
		} else if (vc[0] == "set") {
			flag = test_set(vc);
		} else if (vc[0] == "undo") {
			flag = undo();
		} else if (vc[0] == "redo") {
			flag = redo();
		} else {
			std::cerr << "Unknown command (should be create, del, mv, set, undo, redo)!" << std::endl;
		}
		if (flag) std::cout << frame << std::endl;
	}
	return 0;
}
