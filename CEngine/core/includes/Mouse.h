#pragma once
#include <queue>
#include <optional>

class Mouse
{
	friend class Window;
public:
	struct RawDelta
	{
		int x, y;
	};
	class Event
	{
	public:
		enum class Type
		{
			LPress,
			LRelease,
			RPress,
			RRelease,
			WheelUp,
			WheelDown,
			Move,
			Enter,
			Leave,
			Invalid
		};
	private:
		Type type;
		bool leftIsPressed;
		bool rightIsPressed;
		int x;
		int y;
	public:
		Event() 
			:
			type( Type::Invalid),
			leftIsPressed(false),
			rightIsPressed(false),
			x(0),
			y(0)
		{}
		Event(Type type, const Mouse& parent) 
			:
			type(type),
			leftIsPressed(parent.leftIsPressed),
			rightIsPressed(parent.rightIsPressed),
			x(parent.x),
			y(parent.y)
		{}
		Type GetType() const 
		{
			return type;
		}
		std::pair<int, int> GetPos() const 
		{
			return{ x,y };
		}
		int GetPosX() const 
		{
			return x;
		}
		int GetPosY() const 
		{
			return y;
		}
		bool LeftIsPressed() const 
		{
			return leftIsPressed;
		}
		bool RightIsPressed() const 
		{
			return rightIsPressed;
		}
	};
public:
	Mouse() = default;
	std::pair<int, int> GetPos() const ;
	std::optional<RawDelta> ReadRawDelta();
	int GetPosX() const ;
	int GetPosY() const ;
	bool IsInWindow() const;
	bool LeftIsPressed() const ;
	bool RightIsPressed() const ;
	std::optional<Mouse::Event> Read();
	bool IsEmpty() const 
	{
		return buffer.empty();
	}
	void Flush() ;

public:
	void OnRawDelta(int dx, int dy);
	void OnMouseMove(int x, int y) ;
	void OnMouseLeave();
	void OnMouseEnter();
	void OnLeftPressed(int x, int y) ;
	void OnLeftReleased(int x, int y) ;
	void OnRightPressed(int x, int y) ;
	void OnRightReleased(int x, int y) ;
	void OnWheelUp(int x, int y) ;
	void OnWheelDown(int x, int y) ;
	void TrimBuffer() ;
	void TrimRawInputBuffer();
private:
	static constexpr unsigned int bufferSize = 16u;
	int x;
	int y;
	bool leftIsPressed = false;
	bool rightIsPressed = false;
	bool isInWindow = false;
	std::queue<Event> buffer;
	std::queue <RawDelta> rawDeltaBuffer;
};