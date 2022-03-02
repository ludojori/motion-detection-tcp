#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <fstream>
#include <cstring>


enum class Command : int  {
	FILE,
	MESSAGE
};

struct Request {
	Command command;
	char data[1000];
};

std::istream& operator>>(std::istream& stream, Request& request) 
{
	int command = 0;
	stream >> command;
	request.command = (Command)command;
	stream >> request.data;
	return stream;
}

std::ostream& operator<<(std::ostream& stream, Request& request)
{
	stream << (int)request.command;
	stream << request.data;
	return stream;
}

#endif
