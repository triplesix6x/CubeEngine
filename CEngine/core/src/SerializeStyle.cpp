#include "imgui.h"
#include <string.h>
#include <stdio.h> 
#define IM_ARRAYSIZE(_ARR)          ((int)(sizeof(_ARR)/sizeof(*_ARR)))



static size_t ImFormatString(char* buf, size_t buf_size, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int w = vsnprintf(buf, buf_size, fmt, args);
    va_end(args);
    buf[buf_size - 1] = 0;
    return (w == -1) ? buf_size : (size_t)w;
}


bool ImGuiSaveStyle(const char* filename, const ImGuiStyle& style)
{

    FILE* f = fopen(filename, "wt");
    if (!f)  return false;

    fprintf(f, "[Alpha]\n%1.3f\n", style.Alpha);
    fprintf(f, "[WindowPadding]\n%1.3f %1.3f\n", style.WindowPadding.x, style.WindowPadding.y);
    fprintf(f, "[WindowMinSize]\n%1.3f %1.3f\n", style.WindowMinSize.x, style.WindowMinSize.y);
    fprintf(f, "[FramePadding]\n%1.3f %1.3f\n", style.FramePadding.x, style.FramePadding.y);
    fprintf(f, "[CellPadding]\n%1.3f %1.3f\n", style.CellPadding.x, style.CellPadding.y);
    fprintf(f, "[FrameRounding]\n%1.3f\n", style.FrameRounding);
    fprintf(f, "[ItemSpacing]\n%1.3f %1.3f\n", style.ItemSpacing.x, style.ItemSpacing.y);
    fprintf(f, "[IndentSpacing]\n%1.3f\n", style.IndentSpacing);
    fprintf(f, "[ItemInnerSpacing]\n%1.3f %1.3f\n", style.ItemInnerSpacing.x, style.ItemInnerSpacing.y);
    fprintf(f, "[TouchExtraPadding]\n%1.3f %1.3f\n", style.TouchExtraPadding.x, style.TouchExtraPadding.y);
    fprintf(f, "[WindowRounding]\n%1.3f\n", style.WindowRounding);
    fprintf(f, "[ColumnsMinSpacing]\n%1.3f\n", style.ColumnsMinSpacing);
    fprintf(f, "[ScrollbarSize]\n%1.3f\n", style.ScrollbarSize);
    fprintf(f, "[GrabMinSize]\n%1.3f\n", style.GrabMinSize);
    fprintf(f, "[WindowBorderSize]\n%1.3f\n", style.WindowBorderSize);
    fprintf(f, "[ChildBorderSize]\n%1.3f\n", style.ChildBorderSize);
    fprintf(f, "[PopupBorderSize]\n%1.3f\n", style.PopupBorderSize);
    fprintf(f, "[FrameBorderSize]\n%1.3f\n", style.FrameBorderSize);
    fprintf(f, "[TabBorderSize]\n%1.3f\n", style.TabBorderSize);
    fprintf(f, "[ChildRounding]\n%1.3f\n", style.ChildRounding);
    fprintf(f, "[PopupRounding]\n%1.3f\n", style.PopupRounding);
    fprintf(f, "[ScrollbarRounding]\n%1.3f\n", style.ScrollbarRounding);
    fprintf(f, "[GrabRounding]\n%1.3f\n", style.GrabRounding);
    fprintf(f, "[LogSliderDeadzone]\n%1.3f\n", style.LogSliderDeadzone);
    fprintf(f, "[TabRounding]\n%1.3f\n", style.TabRounding);

    for (size_t i = 0; i != ImGuiCol_COUNT; i++)
    {
        const ImVec4& c = style.Colors[i];
        fprintf(f, "[%s]\n", ImGui::GetStyleColorName(i));
        fprintf(f, "%1.3f %1.3f %1.3f %1.3f\n", c.x, c.y, c.z, c.w);
    }
    fprintf(f, "\n");
    fclose(f);

    return true;
}


bool ImGuiLoadStyle(const char* filename, ImGuiStyle& style)
{

    if (!filename)  return false;

    FILE* f;
    if ((f = fopen(filename, "rt")) == NULL) return false;
    if (fseek(f, 0, SEEK_END)) {
        fclose(f);
        return false;
    }
    const long f_size_signed = ftell(f);
    if (f_size_signed == -1) {
        fclose(f);
        return false;
    }
    size_t f_size = (size_t)f_size_signed;
    if (fseek(f, 0, SEEK_SET)) {
        fclose(f);
        return false;
    }
    char* f_data = (char*)ImGui::MemAlloc(f_size + 1);
    f_size = fread(f_data, 1, f_size, f); 
    fclose(f);
    if (f_size == 0) {
        ImGui::MemFree(f_data);
        return false;
    }
    f_data[f_size] = 0;


    char name[128]; name[0] = '\0';
    const char* buf_end = f_data + f_size;
    for (const char* line_start = f_data; line_start < buf_end; )
    {
        const char* line_end = line_start;
        while (line_end < buf_end && *line_end != '\n' && *line_end != '\r')
            line_end++;

        if (name[0] == '\0' && line_start[0] == '[' && line_end > line_start && line_end[-1] == ']')
        {
            ImFormatString(name, IM_ARRAYSIZE(name), "%.*s", line_end - line_start - 2, line_start + 1);

        }
        else if (name[0] != '\0')
        {

            float* pf[4] = { 0,0,0,0 };
            int npf = 0;


            {
                if (strcmp(name, "Alpha") == 0) { npf = 1; pf[0] = &style.Alpha; }
                else if (strcmp(name, "WindowPadding") == 0) { npf = 2; pf[0] = &style.WindowPadding.x; pf[1] = &style.WindowPadding.y; }
                else if (strcmp(name, "WindowMinSize") == 0) { npf = 2; pf[0] = &style.WindowMinSize.x; pf[1] = &style.WindowMinSize.y; }
                else if (strcmp(name, "FramePadding") == 0) { npf = 2; pf[0] = &style.FramePadding.x; pf[1] = &style.FramePadding.y; }
                else if (strcmp(name, "CellPadding") == 0) { npf = 2; pf[0] = &style.CellPadding.x; pf[1] = &style.CellPadding.y; }
                else if (strcmp(name, "FrameRounding") == 0) { npf = 1; pf[0] = &style.FrameRounding; }
                else if (strcmp(name, "IndentSpacing") == 0) { npf = 1; pf[0] = &style.IndentSpacing; }
                else if (strcmp(name, "ItemSpacing") == 0) { npf = 2; pf[0] = &style.ItemSpacing.x; pf[1] = &style.ItemSpacing.y; }
                else if (strcmp(name, "ItemInnerSpacing") == 0) { npf = 2; pf[0] = &style.ItemInnerSpacing.x; pf[1] = &style.ItemInnerSpacing.y; }
                else if (strcmp(name, "TouchExtraPadding") == 0) { npf = 2; pf[0] = &style.TouchExtraPadding.x; pf[1] = &style.TouchExtraPadding.y; }
                else if (strcmp(name, "WindowRounding") == 0) { npf = 1; pf[0] = &style.WindowRounding; }
                else if (strcmp(name, "ColumnsMinSpacing") == 0) { npf = 1; pf[0] = &style.ColumnsMinSpacing; }
                else if (strcmp(name, "ScrollbarSize") == 0) { npf = 1; pf[0] = &style.ScrollbarSize; }
                else if (strcmp(name, "GrabMinSize") == 0) { npf = 1; pf[0] = &style.GrabMinSize; }
                else if (strcmp(name, "WindowBorderSize") == 0) { npf = 1; pf[0] = &style.WindowBorderSize; }
                else if (strcmp(name, "ChildBorderSize") == 0) { npf = 1; pf[0] = &style.ChildBorderSize; }
                else if (strcmp(name, "PopupBorderSize") == 0) { npf = 1; pf[0] = &style.PopupBorderSize; }
                else if (strcmp(name, "FrameBorderSize") == 0) { npf = 1; pf[0] = &style.FrameBorderSize; }
                else if (strcmp(name, "TabBorderSize") == 0) { npf = 1; pf[0] = &style.TabBorderSize; }
                else if (strcmp(name, "ChildRounding") == 0) { npf = 1; pf[0] = &style.ChildRounding; }
                else if (strcmp(name, "PopupRounding") == 0) { npf = 1; pf[0] = &style.PopupRounding; }
                else if (strcmp(name, "ScrollbarRounding") == 0) { npf = 1; pf[0] = &style.ScrollbarRounding; }
                else if (strcmp(name, "GrabRounding") == 0) { npf = 1; pf[0] = &style.GrabRounding; }
                else if (strcmp(name, "LogSliderDeadzone") == 0) { npf = 1; pf[0] = &style.LogSliderDeadzone; }
                else if (strcmp(name, "TabRounding") == 0) { npf = 1; pf[0] = &style.TabRounding; }

                else {
                    for (int j = 0; j < ImGuiCol_COUNT; j++) {
                        if (strcmp(name, ImGui::GetStyleColorName(j)) == 0) {
                            npf = 4;
                            ImVec4& color = style.Colors[j];
                            pf[0] = &color.x; pf[1] = &color.y; pf[2] = &color.z; pf[3] = &color.w;
                            break;
                        }
                    }
                }
            }
            float x, y, z, w;
            switch (npf) {
            case 1:
                if (sscanf(line_start, "%f", &x) == npf) {
                    *pf[0] = x;
                }
                else fprintf(stderr, "Warning in ImGui::LoadStyle(\"%s\"): skipped [%s] (parsing error).\n", filename, name);
                break;
            case 2:
                if (sscanf(line_start, "%f %f", &x, &y) == npf) {
                    *pf[0] = x; *pf[1] = y;
                }
                else fprintf(stderr, "Warning in ImGui::LoadStyle(\"%s\"): skipped [%s] (parsing error).\n", filename, name);
                break;
            case 3:
                if (sscanf(line_start, "%f %f %f", &x, &y, &z) == npf) {
                    *pf[0] = x; *pf[1] = y; *pf[2] = z;
                }
                else fprintf(stderr, "Warning in ImGui::LoadStyle(\"%s\"): skipped [%s] (parsing error).\n", filename, name);
                break;
            case 4:
                if (sscanf(line_start, "%f %f %f %f", &x, &y, &z, &w) == npf) {
                    *pf[0] = x; *pf[1] = y; *pf[2] = z; *pf[3] = w;
                }
                else fprintf(stderr, "Warning in ImGui::LoadStyle(\"%s\"): skipped [%s] (parsing error).\n", filename, name);
                break;
            default:
                fprintf(stderr, "Warning in ImGui::LoadStyle(\"%s\"): skipped [%s] (unknown field).\n", filename, name);
                break;
            }
            name[0] = '\0'; 
        }

        line_start = line_end + 1;
    }
    ImGui::MemFree(f_data);
    return true;
}