//Файл обработки мыши

#include "../includes/Mouse.h"


std::pair<int, int> Mouse::GetPos() const 
{
	return { x,y };
}

std::optional<Mouse::RawDelta> Mouse::ReadRawDelta()
{
	if (rawDeltaBuffer.empty())
	{
		return std::nullopt;
	}
	const RawDelta d = rawDeltaBuffer.front();
	rawDeltaBuffer.pop();
	return d;
}

int Mouse::GetPosX() const 
{
	return x;
}

int Mouse::GetPosY() const 
{
	return y;
}

bool Mouse::IsInWindow() const
{
	return isInWindow;
}

bool Mouse::LeftIsPressed() const 
{
	return leftIsPressed;
}

bool Mouse::RightIsPressed() const 
{
	return rightIsPressed;
}

std::optional<Mouse::Event> Mouse::Read()
{
	if (buffer.size() > 0u)
	{
		Mouse::Event e = buffer.front();
		buffer.pop();
		return e;
	}
	return {};
}

void Mouse::Flush() 
{
	buffer = std::queue<Event>();
}


void Mouse::OnRawDelta(int dx, int dy)
{
	rawDeltaBuffer.push({ dx, dy });
	TrimBuffer();
}

void Mouse::OnMouseMove(int newx, int newy) 
{
	x = newx;
	y = newy;

	buffer.push(Mouse::Event(Mouse::Event::Type::Move, *this));
	TrimBuffer();
}

void Mouse::OnMouseLeave()
{
	isInWindow = false;
	buffer.push(Mouse::Event(Mouse::Event::Type::Leave, *this));
	TrimBuffer();
}

void Mouse::OnMouseEnter()
{
	isInWindow = true;
	buffer.push(Mouse::Event(Mouse::Event::Type::Enter, *this));
	TrimBuffer();
}

void Mouse::OnLeftPressed(int x, int y) 
{
	leftIsPressed = true;

	buffer.push(Mouse::Event(Mouse::Event::Type::LPress, *this));
	TrimBuffer();
}

void Mouse::OnLeftReleased(int x, int y) 
{
	leftIsPressed = false;

	buffer.push(Mouse::Event(Mouse::Event::Type::LRelease, *this));
	TrimBuffer();
}

void Mouse::OnRightPressed(int x, int y) 
{
	rightIsPressed = true;

	buffer.push(Mouse::Event(Mouse::Event::Type::RPress, *this));
	TrimBuffer();
}

void Mouse::OnRightReleased(int x, int y) 
{
	rightIsPressed = false;

	buffer.push(Mouse::Event(Mouse::Event::Type::RRelease, *this));
	TrimBuffer();
}

void Mouse::OnWheelUp(int x, int y) 
{
	buffer.push(Mouse::Event(Mouse::Event::Type::WheelUp, *this));
	TrimBuffer();
}

void Mouse::OnWheelDown(int x, int y) 
{
	buffer.push(Mouse::Event(Mouse::Event::Type::WheelDown, *this));
	TrimBuffer();
}

void Mouse::TrimRawInputBuffer()
{
	while (rawDeltaBuffer.size() > bufferSize)
	{
		rawDeltaBuffer.pop();
	}
}

void Mouse::TrimBuffer() 
{
	while (buffer.size() > bufferSize)
	{
		buffer.pop();
	}
}

