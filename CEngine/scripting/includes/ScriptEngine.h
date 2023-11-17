// ласс реализации скриптового движка, позвол€ющий взаимодействовать C# и C++

#pragma once

class ScriptEngine
{
public:
	static void Init();
	static void Shutdown();
private:
	static void InitMono();
	static void ShutdownMono();
};

