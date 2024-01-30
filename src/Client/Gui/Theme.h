#pragma once

#include <imgui.h>

class Theme
{
    Theme();

public:
    static Theme &GetInstance();

    void SetDarkTheme();

private:
    ImGuiStyle *m_style;
    ImGuiIO *m_io;
};