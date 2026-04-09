#include "panel/VotesPanel.hpp"

#include <algorithm>
#include <random>
#include <thread>
#include <imgui.h>

#include "server/GameManager.hpp"
#include "server/Server.hpp"

void VotesPanel::render() {
    if (_showPanel) {
        ImGui::SetNextWindowSize(ImVec2(450, 300));
        ImGui::Begin("LIS Multiplayer - Votes");

        auto server = Server::get();
        auto choices = server->getChoices();
        auto totalVotes = server->getTotalVotes();
        
        if (totalVotes == server->getClients().size() || this->timerComplete) {
            auto isSplitChoice = [choices](const std::vector<std::pair<Choice, float>>& vec) -> std::pair<Choice, float>* {
                auto it = std::find_if(
                    vec.begin(), 
                    vec.end(),
                [](const auto& p) {
                    return p.second > 0.0f; 
                });

                if (it == vec.end()) return nullptr;

                float reference = it->second;

                bool isEven = std::all_of(
                vec.begin(), 
                vec.end(),
                [reference](const auto& p) {
                    return p.second <= 0.0f || p.second == reference;
                });

                if (isEven) {
                    std::vector<std::pair<Choice, float>*> candidates;
                    for (auto p : vec) {
                        if (p.second > 0.0f) {
                            candidates.push_back(&p);
                        }
                    }

                    std::mt19937 rng(std::random_device{}());
                    std::uniform_int_distribution<size_t> dist(0, candidates.size() - 1);
                    return candidates[dist(rng)];
                } else {
                    return nullptr;
                }
            };

            if (auto choice = isSplitChoice(choices)) {
                auto it = std::find_if(choices.begin(), choices.end(), [choice](const std::pair<Choice, float>& p) {
                    return p.first.choiceID == choice->first.choiceID;
                });
                int index = std::distance(choices.begin(), it);
            }

            auto it = std::max_element(choices.begin(), choices.end(), [](const auto& lhs, const auto& rhs) {
                return lhs.second < rhs.second;
            });

            if (it != choices.end()) {
                auto choice = it->first;
                // choice auto positioning guide
                // left -> right -> up -> down
                int index = std::distance(choices.begin(), it);
                switch (index) {
                    case 0:
                        this->makeChoice(eInteractMenu::kLeft);
                        break;
                    case 1:
                        this->makeChoice(eInteractMenu::kRight);
                        break;
                    case 2:
                        this->makeChoice(eInteractMenu::kUp);
                        break;
                    case 3:
                        this->makeChoice(eInteractMenu::kDown);
                        break;
                }
                this->choiceMade = true;
            }
        }

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

    this->timerComplete = false;

    std::thread thread([this, seconds] {
        for (int i = seconds; i >= 0; i--) {
            if (this->choiceMade) {
                break;
            }

            this->timerStr = std::to_string(i);

            if (i > 0) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
        this->timerComplete = true;
        this->choiceMade = false;
        this->timerStr = std::to_string(GameManager::get()->choiceTime);

        this->timerThreadSpawned = false;
    });

    thread.detach();
}

void VotesPanel::makeChoice(eInteractMenu button) {
    this->oChoiceMade.oChoiceMade(
        this->oChoiceMade.instance,
        button,
        this->oChoiceMade.methodInfo
    );
}