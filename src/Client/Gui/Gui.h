#pragma once

#include <string>
#include <raylib.h>
#include <imgui.h>
#include <rlImGui.h>
#include <imgui_stdlib.h>
#include <regex>

#include "../Api/Api.h"
#include "Theme.h"

class Gui
{
    Gui();

public:
    ~Gui();
    static Gui &GetInstance();
    void Init();
    void HandleInput();
    void Draw();
    void Destroy();
    void DisplayChatHistory();
    void Run();

private:
    int m_width;
    int m_height;
    std::string m_title;

    bool m_ipSet = false;
    bool m_portSet = false;
    bool m_usernameSet = false;

    std::string m_ip = "";
    std::string m_port = "";
    std::string m_username = "";

    std::string m_chatInput = "";

    std::vector<std::shared_ptr<Message>> m_chatHistory;

    Api &m_api = Api::GetInstance();
    Theme &m_theme = Theme::GetInstance();

    std::shared_ptr<std::thread> m_receiveThread;
};