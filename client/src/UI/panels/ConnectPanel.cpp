#include "UI/panels/ConnectPanel.hpp"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include "network/Client.hpp"
#include "packets/Client.hpp"

void ConnectPanel::render() {
    auto cm = ClientManager::get();

    ImGui::SetNextWindowSize(ImVec2(375, 400));
    ImGui::Begin("LIS Multiplayer - Connect");

    ImGui::InputText("Host", &this->host);
    ImGui::InputInt("Port", &this->port);
    ImGui::InputText("Name", &this->name);
    if (ImGui::Button("Connect")) {
        cm->init(this->host, this->port);
        cm->send(UserJoinPacket::create(this->name, cm->getUserID()));
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

    ImGui::End();
}