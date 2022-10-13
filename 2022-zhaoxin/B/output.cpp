#ifndef __OUTPUT_CPP__
#define __OUTPUT_CPP__

#include <iostream>
#include "output.hpp"

Output::Output(uint _r, uint _c) {
	r = _r; c = _c;
	arr = std::vector<std::string>(r, std::string(c, ' '));
}

void Output::output_border(uint si, uint sj, uint r, uint c, uint corner) { // 处理边界
	uint ei = si + r - 1, ej = sj + c - 1;
	arr[si][sj] = arr[ei][sj] = arr[si][ej] = arr[ei][ej] = CORNER[corner];
	for (uint i = si + 1; i < ei; ++i) arr[i][sj] = arr[i][ej] = '|';
	for (uint j = sj + 1; j < ej; ++j) arr[si][j] = arr[ei][j] = '-';
}

void Output::output_id(uint si, uint sj, const std::string &id) { // 处理编号
	for (char c : id) arr[si][sj++] = c;
}

std::ostream& operator << (std::ostream &os, const Output &output) { // 输出
	for (const auto &s: output.arr) os << s << '\n';
	return os;
}

#endif
