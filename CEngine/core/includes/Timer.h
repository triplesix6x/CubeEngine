//Класс для работы со временем

#pragma once
#include <chrono>


class Timer
{
public:
	Timer();
	float Mark();
	std::chrono::duration<float> MarkDuration();
	float Peek() const;
	float GetTime(std::chrono::steady_clock::time_point start);
private:
	std::chrono::steady_clock::time_point last;
};

