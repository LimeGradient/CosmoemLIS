#include "panel/ServerPanel.hpp"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include "server/Server.hpp"

void ServerPanel::renderHostPanel() {
    auto server = Server::get();
    auto clients = server->getClients();

    ImGui::SetNextWindowSize(ImVec2(375, 400));
    ImGui::Begin("LIS Multiplayer - Host");

    ImGui::InputInt("Port", &this->port);

    if (ImGui::Button("Start")) {
        server->start(this->port);
    }
    ImGui::SameLine();
    if (server->isOnline()) {
        if (ImGui::Button("Stop")) {
            server->stop();
        }
    }

    ImGui::Separator();
    
    ImGui::Text("Lobby");
    if (!clients.empty()) {
        if (ImGui::BeginTable("users_table", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
            ImGui::TableSetupColumn("Name");
            ImGui::TableSetupColumn("User ID");
            ImGui::TableSetupColumn("");
            ImGui::TableHeadersRow();

            for (auto client : clients) {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("%s", client.name.c_str());
                ImGui::TableNextColumn();
                ImGui::Text("%s", client.userID.c_str());
                ImGui::TableNextColumn();
                ImGui::PushID(client.userID.c_str());
                if (ImGui::Button("Kick")) {
                    
                }
                ImGui::PopID();
            }

            ImGui::EndTable();
        }
    }

    ImGui::End();
}

void ServerPanel::startServer() {
    auto server = Server::get();
    server->start(this->port);
}