#include "UI/panels/ConnectPanel.hpp"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include "network/Client.hpp"
#include "UI/Window.hpp"
#include "UI/UIHelper.hpp"

void ConnectPanel::render() {
    auto cm = ClientManager::get();
    auto window = Window::get();

    ImGui::BeginChild("Title", ImVec2(CONNECT_PANEL_WIDTH - 25, CONNECT_PANEL_HEIGHT - 25), true, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse );

    ImGui::InputText("Host", &this->host);
    ImGui::InputInt("Port", &this->port);
    ImGui::InputText("Name", &this->name);
    if (ImGui::Button("Connect")) {
        cm->init(this->host, this->port, this->name);
    }

    ImGui::Separator();

    if (cm->connected()) {
        auto clients = cm->getClients();
        ImGui::Text("Lobby");
        if (ImGui::BeginTable("users_table", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
            ImGui::TableSetupColumn("Name");
            ImGui::TableSetupColumn("User ID");
            ImGui::TableHeadersRow();

            for (auto client : clients) {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("%s", client.name.c_str());
                ImGui::TableNextColumn();
                ImGui::Text("%s", client.userID.c_str());
            }

            ImGui::EndTable();
        }
    }

    ImGui::Separator();

    ImGui::Text("Client Options");
    ImGui::Checkbox("Transparent Window", &window->isTransparent);

    ImGui::EndChild();
}

void ConnectPanel::init(float x, float y) {
    UIHelper::createPanel("ConnectPanel", ImVec2(CONNECT_PANEL_WIDTH, CONNECT_PANEL_HEIGHT), ImVec2(x, y), [this]() {
        this->render();
    }, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);
}