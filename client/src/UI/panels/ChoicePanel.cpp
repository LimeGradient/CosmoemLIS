#include "UI/panels/ChoicePanel.hpp"

#include <imgui.h>

#include "network/Client.hpp"
#include "packets/Client.hpp"

void ChoicePanel::render() {
    if (_showPanel) {
        ImGui::SetNextWindowSize(ImVec2(300, 300));
        ImGui::Begin("LIS Multiplayer - Choice");

        ImGui::Text("Make your choice.");

        if (!this->selectedChoice) {
            for (auto choice : this->choices) {
                ImGui::PushID(choice.choiceID);
                if (ImGui::Button(choice.title.c_str())) {
                    this->choice = choice;
                    this->selectedChoice = true;
                    ClientManager::get()->send(SendChoicePacket::create(choice));
                }
                ImGui::PopID();
            }
        } else {
            ImGui::Text("You have selected: %s", this->choice.title.c_str());
        }

        ImGui::End();
    }
}
