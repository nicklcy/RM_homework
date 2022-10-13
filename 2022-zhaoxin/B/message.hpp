#ifndef __MESSAGE_HPP__
#define __MESSAGE_HPP__

#include <string>

struct Message { // 返回错误信息类
	std::string msg;

	Message();
	Message(const std::string&);

	bool has_error() const; // 存在错误
	bool print_error() const; // 存在并输出错误
};

template <typename T>
struct ResultMessage : Message { // 返回错误信息或数据类
	T val;

	ResultMessage() { val = T(); }
	ResultMessage(const T& _val) {
		val = _val;
	}
	ResultMessage(const std::string &_msg, const T &_val) {
		msg = _msg; val = _val;
	}
	ResultMessage(const Message &_message) {
		msg = _message.msg;
		val = T();
	}

	T get_val() const {
		return val;
	}
};

#endif
