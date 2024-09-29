#pragma once

#include <iosfwd>
#include <iostream>

using namespace std;

class PrintThread : public std::ostringstream
{
	public:
	PrintThread() = default;

	~PrintThread()
	{
		std::lock_guard<std::mutex> guard(_mutexPrint);
		std::cout << this->str();
	}

	private:
	static std::mutex _mutexPrint;
};