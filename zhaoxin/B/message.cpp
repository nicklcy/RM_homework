#ifndef __MESSAGE_CPP__
#define __MESSAGE_CPP__

#include "message.hpp"
#include <iostream>

Message::Message() {}

Message::Message(const std::string &_msg) {
	msg = _msg;
}

bool Message::has_error() const {
	return msg.size() > 0;
}

bool Message::print_error() const {
	if (msg.size() > 0) {
		std::cerr << "Error! " << msg << '\n' << std::endl;
		return true;
	}
	return false;
}

#endif
