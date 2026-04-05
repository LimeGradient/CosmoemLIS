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

    ImGui::End();
}