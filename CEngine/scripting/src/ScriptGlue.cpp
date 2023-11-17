#include "../includes/ScriptGlue.h"
#include "../core/includes/Log.h"

#define CUBE_ADD_INTERNAL_CALL(Name) mono_add_internal_call("Cube.InternalCalls::" #Name, Name)

static void NativeLog(MonoString* string)
{
    char* cStr = mono_string_to_utf8(string);
    std::string str(cStr);
    mono_free(cStr);
    CUBE_CORE_TRACE(str);
}

void ScriptGlue::RegisterFunctions()
{
    CUBE_ADD_INTERNAL_CALL(NativeLog);
}
