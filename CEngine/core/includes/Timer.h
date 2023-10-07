#pragma once
#include <chrono>


class Timer
{
public:
	Timer();
	float Mark();
	float Peek() const;
	float GetTime(std::chrono::steady_clock::time_point start);
private:
	std::chrono::steady_clock::time_point last;
};

