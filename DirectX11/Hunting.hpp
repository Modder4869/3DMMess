#pragma once

#include "HackerDevice.hpp"

// Hunting
//  This code is to implement the Hunting mechanism as a separate compilation from the main wrapper code.
//  It implements all the shader management based on user input via key presses from Input.

// Custom #include handler used to track which shaders need to be reloaded after an included file is modified
class MigotoIncludeHandler : public ID3DInclude
{
    std::vector<std::string> dir_stack;

    void push_dir(const char *path);
public:
    MigotoIncludeHandler(const char *path);

    STDMETHOD(Open)(D3D_INCLUDE_TYPE include_type, LPCSTR file_name, LPCVOID parent_data, LPCVOID *data, UINT *bytes);
    STDMETHOD(Close)(LPCVOID data);
};

void TimeoutHuntingBuffers();
void ParseHuntingSection();
void DumpUsage(wchar_t *dir);
