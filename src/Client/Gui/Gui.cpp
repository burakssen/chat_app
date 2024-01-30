#include "Gui.h"

Gui::Gui()
{
}

Gui::~Gui()
{
    this->Destroy();
}

Gui &Gui::GetInstance()
{
    static Gui instance;
    return instance;
}

void Gui::Init()
{
    this->m_width = 1080;
    this->m_height = 720;
    this->m_title = "Chat";

    InitWindow(this->m_width, this->m_height, this->m_title.c_str());
    SetTargetFPS(60);
    rlImGuiSetup(true);

    this->m_theme.SetDarkTheme();
}

void Gui::Run()
{
    while (!WindowShouldClose())
    {
        this->HandleInput();
        this->Draw();
    }
}

void Gui::HandleInput()
{
}

void Gui::Draw()
{
    BeginDrawing();
    ClearBackground(BLACK);
    rlImGuiBegin();

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(this->m_width, this->m_height));
    ImGui::Begin("Main Panel", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

    if (!this->m_ipSet || !this->m_portSet || !this->m_usernameSet)
    {
        int windowWidth = 500;
        int windowHeight = 200;
        ImGui::SetNextWindowPos(ImVec2(this->m_width / 2 - windowWidth / 2, this->m_height / 2 - windowHeight / 2));
        ImGui::BeginChild("Connect Panel", ImVec2(windowWidth, windowHeight), true);

        ImGui::Text("Please enter the following information to connect to the server:");
        ImGui::SetNextItemWidth(200);
        ImGui::InputTextWithHint("##IP", "IP", &this->m_ip);
        ImGui::SameLine();
        ImGui::Text(":");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(50);
        ImGui::InputTextWithHint("##Port", "PORT", &this->m_port, ImGuiInputTextFlags_CharsDecimal);
        ImGui::SetNextItemWidth(200);
        ImGui::InputTextWithHint("##Username", "Username", &this->m_username);

        if (ImGui::Button("Connect"))
        {
            if (this->m_ip.empty() || this->m_port.empty() || this->m_username.empty())
            {
                this->m_ipSet = false;
                this->m_portSet = false;
                this->m_usernameSet = false;
            }
            else
            {
                try
                {
                    this->m_api.SetServerIp(this->m_ip);
                    this->m_api.SetServerPort(std::atoi(this->m_port.c_str()));
                    this->m_api.Init();
                    this->m_api.Send(Message{MessageType::CONNECT, "", this->m_username});

                    this->m_receiveThread = std::make_shared<std::thread>([this]()
                                                                          {
                        while (true)
                        {
                            Message msg;
                            try
                            {
                                this->m_api.Receive(msg);

                                if (msg.type == MessageType::CONNECT_AKN)
                                {
                                    this->m_ipSet = true;
                                    this->m_portSet = true;
                                    this->m_usernameSet = true;
                                }

                                if (msg.type == MessageType::DISCONNECT_AKN)
                                {
                                    std::cout << "Disconnected from server." << std::endl;
                                    this->m_ipSet = false;
                                    this->m_portSet = false;
                                    this->m_usernameSet = false;
                                    break;
                                }
                            }
                            catch (const std::exception &e)
                            {
                                std::cerr << e.what() << '\n';
                                break;
                            }

                            this->m_chatHistory.push_back(std::make_shared<Message>(msg));
                        } });

                    this->m_receiveThread->detach();
                }
                catch (const std::exception &e)
                {
                    std::cerr << e.what() << '\n';
                }
            }
        }

        ImGui::EndChild();
    }
    else
    {
        ImGui::Text("Connected to %s:%s as %s", this->m_ip.c_str(), this->m_port.c_str(), this->m_username.c_str());
        ImGui::SameLine();
        if (ImGui::Button("Disconnect"))
        {
            this->m_api.Send(Message{MessageType::DISCONNECT, "", this->m_username});
        }
        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
        ImGui::BeginChild("Chat Panel", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y * 0.7f), true, ImGuiWindowFlags_NoDecoration);
        ImGui::PopStyleVar();
        ImGui::SetNextWindowScroll(ImVec2(0.0f, FLT_MAX));

        ImGui::BeginChild("Chat History", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), true, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_NoDecoration);
        this->DisplayChatHistory();
        ImGui::EndChild();
        ImGui::EndChild();
        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
        ImGui::BeginChild("Input Panel", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), true);
        ImGui::PopStyleVar();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::InputTextMultiline("##ChatInput", &this->m_chatInput, ImVec2(ImGui::GetContentRegionAvail().x - 50, ImGui::GetContentRegionAvail().y));
        ImGui::SameLine();
        if (ImGui::Button("Send", ImVec2(ImGui::GetContentRegionAvail().x, 20)))
        {
            if (this->m_chatInput.empty())
            {
                return;
            }

            Message msg;
            msg.type = MessageType::MESSAGE;
            msg.content = this->m_chatInput;
            msg.username = this->m_username;
            this->m_api.Send(msg);

            this->m_chatHistory.push_back(std::make_shared<Message>(msg));

            this->m_chatInput = "";
        }
        ImGui::EndChild();
    }

    ImGui::End();

    rlImGuiEnd();
    EndDrawing();
}

void Gui::Destroy()
{
    rlImGuiShutdown();
    CloseWindow();
}

void Gui::DisplayChatHistory()
{
    for (auto msg : this->m_chatHistory)
    {
        if (msg->type == MessageType::CONNECT_AKN)
        {
            if (msg->username == this->m_username)
            {
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "You have connected to the server.");
                continue;
            }

            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "%s has connected to the server.", msg->username.c_str());
            continue;
        }

        if (msg->type == MessageType::DISCONNECT_AKN)
        {
            if (msg->username == this->m_username)
            {
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "You have disconnected from the server.");
                continue;
            }

            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "%s has disconnected from the server.", msg->username.c_str());
            continue;
        }

        if (msg->type == MessageType::MESSAGE)
        {
            if (msg->username == this->m_username)
            {
                ImGui::TextColored(ImVec4(0.0f, 0.5f, 1.0f, 1.0f), "%s: %s", msg->username.c_str(), msg->content.c_str());
                continue;
            }

            ImGui::Text("%s: %s", msg->username.c_str(), msg->content.c_str());
            continue;
        }

        if (msg->type == MessageType::COMMAND)
        {
            if (msg->username == this->m_username)
            {
                ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "%s: %s", msg->username.c_str(), msg->content.c_str());
                continue;
            }

            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "%s: %s", msg->username.c_str(), msg->content.c_str());
            continue;
        }

        ImGui::Text("Unknown message type.");
    }
}
