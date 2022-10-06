#ifndef __OUTPUT_HPP__
#define __OUTPUT_HPP__

#include <vector>
#include <string>

using uint = unsigned;

const char CORNER[] = "+o*";

struct Output { // 输出类
	uint r, c;
	std::vector<std::string> arr; // 输出数组

	Output(uint, uint);

	void output_border(uint, uint, uint, uint, uint); // 起始位置，边框大小，边角
	void output_id(uint, uint, const std::string&); // 起始位置，编号
};

#endif
