#include "../includes/ScriptEngine.h"
#include <fstream>
#include "../core/includes/Log.h"
#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"

struct ScriptEngineData
{
	MonoDomain* rootDomain = nullptr;
	MonoDomain* appDomain = nullptr;

    MonoAssembly* coreAssembly = nullptr;
};

static ScriptEngineData* m_Data;

void ScriptEngine::Init()
{
	m_Data = new ScriptEngineData();

	InitMono();
}

void ScriptEngine::Shutdown()
{
	delete m_Data;

	ShutdownMono();
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

void PrintAssemblyTypes(MonoAssembly* assembly)
{
    MonoImage* image = mono_assembly_get_image(assembly);
    const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
    int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

    for (int32_t i = 0; i < numTypes; i++)
    {
        uint32_t cols[MONO_TYPEDEF_SIZE];
        mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

        const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
        const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

        printf("%s.%s\n", nameSpace, name);
    }
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

	m_Data->appDomain = mono_domain_create_appdomain((char*)"CubeScriptRuntime", nullptr);
	mono_domain_set(m_Data->appDomain, true);

    m_Data->coreAssembly = LoadCSharpAssembly("Cube-ScriptCore.dll");
    PrintAssemblyTypes(m_Data->coreAssembly);
}

void ScriptEngine::ShutdownMono()
{

}
