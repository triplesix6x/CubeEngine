#include "../includes/ScriptEngine.h"
#include <fstream>
#include <iostream>
#include "../core/includes/Log.h"
#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"

struct ScriptEngineData
{
	MonoDomain* rootDomain = nullptr;
	MonoDomain* appDomain = nullptr;

    MonoAssembly* coreAssembly = nullptr;
    MonoImage* coreAssemblyImage = nullptr;
};

static ScriptEngineData* m_Data;

void ScriptEngine::Init()
{
	m_Data = new ScriptEngineData();

	InitMono();
    LoadAssembly("Cube-ScriptCore.dll");
    mono_add_internal_call("Cube.Main::CppFunction", CppFunc);
}

void ScriptEngine::Shutdown()
{
    ShutdownMono();
	delete m_Data;
}

void ScriptEngine::LoadAssembly(const std::filesystem::path filepath)
{
    m_Data->appDomain = mono_domain_create_appdomain((char*)"CubeScriptRuntime", nullptr);
    mono_domain_set(m_Data->appDomain, true);

    m_Data->coreAssembly = LoadCSharpAssembly(filepath.string());
    m_Data->coreAssemblyImage = mono_assembly_get_image(m_Data->coreAssembly);
}

char* ReadBytes(const std::string& filepath, uint32_t* outSize)
{
    std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

    if (!stream)
    {
        CUBE_CORE_ERROR("Mono: failed to open file {}", filepath);
        return nullptr;
    }

    std::streampos end = stream.tellg();
    stream.seekg(0, std::ios::beg);
    uint32_t size = end - stream.tellg();

    if (size == 0)
    {
        CUBE_CORE_ERROR("Mono: file {} is empty", filepath);
        return nullptr;
    }

    char* buffer = new char[size];
    stream.read((char*)buffer, size);
    stream.close();

    *outSize = size;
    return buffer;
}

MonoAssembly* LoadCSharpAssembly(const std::string& assemblyPath)
{
    uint32_t fileSize = 0;
    char* fileData = ReadBytes(assemblyPath, &fileSize);

    MonoImageOpenStatus status;
    MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

    if (status != MONO_IMAGE_OK)
    {
        const char* errorMessage = mono_image_strerror(status);
        CUBE_CORE_ERROR("Mono: Failled to open image. \n{}", errorMessage);
        return nullptr;
    }

    MonoAssembly* assembly = mono_assembly_load_from_full(image, assemblyPath.c_str(), &status, 0);
    mono_image_close(image);

    delete[] fileData;

    return assembly;
}

static void CppFunc()
{
    std::cout << "This is CppFunc" << std::endl;
}

void ScriptEngine::InitMono()
{
	mono_set_assemblies_path("mono/lib");
	MonoDomain* rootDomain = mono_jit_init("CubeJITRuntime");
	if (rootDomain == nullptr)
	{
		CUBE_CORE_ERROR("Faild to init Mono");
		return;
	}

	m_Data->rootDomain = rootDomain;

    MonoClass* monoClass = mono_class_from_name(m_Data->coreAssemblyImage, "Cube", "Main");

    MonoObject* instance = mono_object_new(m_Data->appDomain, monoClass);
    mono_runtime_object_init(instance);

    MonoMethod* printCustomMessageFunc = mono_class_get_method_from_name(monoClass, "PrintMessage", 1);
    MonoString* monoString = mono_string_new(m_Data->appDomain, "Hello world from c++");
    void* stringParam = monoString;
    mono_runtime_invoke(printCustomMessageFunc, instance, &stringParam, nullptr);
}

void ScriptEngine::ShutdownMono()
{
   /* mono_domain_unload(m_Data->appDomain);
    mono_jit_cleanup(m_Data->rootDomain);*/

    m_Data->appDomain = nullptr;
    m_Data->rootDomain = nullptr;
}
