// ласс реализации скриптового движка, позвол€ющий взаимодействовать C# и C++

#pragma once
#include "ScriptGlue.h"
#include <filesystem>

class ScriptEngine
{
public:
	static void Init();
	static void Shutdown();

	static void LoadAssembly(const std::filesystem::path filepath);
private:
	static void InitMono();
	static void ShutdownMono();
};

class ScriptClass
{
public:
    ScriptClass() = default;
    ScriptClass(const std::string& classNamespace, const std::string& className);

    MonoMethod* GetMethod(const std::string& name, int parameterCount);
    MonoObject* Instanciate();
private:
    std::string m_ClassNamespace;
    std::string m_ClassName;
    MonoClass* m_MonoClass = nullptr;
};

