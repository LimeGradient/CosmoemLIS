#include "panel/VotesPanel.hpp"

#include <thread>
#include <imgui.h>

#include "server/Server.hpp"

void VotesPanel::render() {
    if (_showPanel) {
        ImGui::SetNextWindowSize(ImVec2(450, 300));
        ImGui::Begin("LIS Multiplayer - Votes");

        auto server = Server::get();
        auto choices = server->getChoices();
        auto totalVotes = server->getTotalVotes();

        if (!choices.empty()) {
            ImGui::Text("Remaining Time: %ss", this->timerStr.c_str());
            ImGui::Text("Total Votes: %d", totalVotes);
            if (ImGui::BeginTable("choices_table", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                ImGui::TableSetupColumn("Choice");
                ImGui::TableSetupColumn("Votes");
                ImGui::TableHeadersRow();

                for (auto choice : choices) {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("%s", choice.first.title.c_str());
                    ImGui::TableNextColumn();
                    ImGui::ProgressBar((totalVotes == 0) ? 0.f : (choice.second / totalVotes));
                }

                ImGui::EndTable();
            }
        }

        ImGui::End();
    }
}

void VotesPanel::startTimer(int seconds) {
    if (this->timerThreadSpawned.exchange(true)) {
        return;
    }

    std::thread thread([this, seconds] {
        for (int i = seconds; i >= 0; i--) {
            this->timerStr = std::to_string(i);
            printf("There is %ss left\n", this->timerStr.c_str());

            if (i > 0) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
        this->timerComplete = true;
        this->timerThreadSpawned = false;
    });

    thread.detach();
}