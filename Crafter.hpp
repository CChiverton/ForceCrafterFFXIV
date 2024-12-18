#pragma once

#include "Item.hpp"
#include "Player.hpp"
#include "Skills.hpp"
#include <map>
#include <vector>


class Crafter {
public:
	Crafter(std::vector<Skills::SkillName> startingMoves, int maxCP, int maxProgress, int maxQuality, int maxDurability, bool topQuality, bool greaterByregot, int maximumTurnLimit) {
		player = new Player(630);
		this->maxProgress = maxProgress;
		this->maxQuality = maxQuality;
		this->maxDurability = maxDurability;
		this->topQuality = topQuality;
		forceGreaterByregot = greaterByregot;
		this->maxTurnLimit = maximumTurnLimit;
		player->AddItem(maxProgress, maxQuality, maxDurability);
		

		if (!startingMoves.empty()) {
			for (const Skills::SkillName& move : startingMoves) {
				if (!Craft(move)) {
					std::cout << "Invalid. The starting moves break/finish the item.\n";
					return;
				}
			}
		}
	}

	~Crafter() {
		if (successfulCrafts.empty()) {
			return;
		}

		std::cout << "The fastest time was " << bestTime << " seconds.\n";
		std::cout << "Ways to achieve this are:\n";

		for (const auto& entry : successfulCrafts[bestTime]) {
			std::cout << "Solution: ";
			for (const auto& move : entry) {
				std::cout << Skills::GetSkillName(move) << ", ";
			}
			std::cout << '\n';
		}
	}

	void ForceCraft() {
		bool completedCraft{ false };
		for (const auto& move : fullSkillList) {
			if(player->GetCurrentTurn() >= maxTurnLimit) {
				break;
			}

			if ((currentTime + 3) > bestTime) {	// worse than best time, move back down a step
				//std::cout << "Time error, Best time is " << bestTime << " and the current time is " << currentTime << '\n';
				break;
			}

			if ((currentTime + 3) == bestTime || player->GetCurrentTurn() == maxTurnLimit -1) {		// Only one move left to match the best time
				if (!SynthesisCheck(move)) {
					//std::cout << "Only checking synth moves\n";
					continue;
				}
			}

			if (QualityCheck(move))	continue;
			if (BuffCheck(move)) continue;

			if (move == SkillName::BYREGOTSBLESSING && forceGreaterByregot) {
				if (player->GetBuffDuration(SkillName::GREATSTRIDES) == 0) {
					continue;
				}
			}

			bool validMove = Craft(move);
			//std::cout << Skills::GetSkillName(move) << '\n';
			if (validMove) {
				//std::cout << "Turn " << player->GetCurrentTurn() << ": " << Skills::GetSkillName(move) << '\n';
				//std::cout << "After crafting the durability is " << player->craftableItem->GetDurability() << '\n';
				if (IsItemWorkable()) {
					ForceCraft();			// After this path has finished

				}
				else {
					ContinueCraft();		// Remove the base and start a new one
				}
				//std::cout << "Finisheng Turn " << player->GetCurrentTurn() + 1 << ": " << Skills::GetSkillName(move) << '\n';
			}

		}
		ContinueCraft();
		//std::cout << player->GetCurrentTurn() << " TRIED ALL POSSIBLE MOVES AT THIS LEVEL\n";
	}

private:
	Player* player;
	int maxProgress{}, maxQuality{}, maxDurability{};
	std::vector<Skills::SkillName> currentCraft{};
	int currentTime{}, bestTime{99};
	std::map<int, std::vector<std::vector<Skills::SkillName>>> successfulCrafts{};
	bool topQuality{ false };
	bool forceGreaterByregot{ false };
	int maxTurnLimit;
	

	bool Craft(Skills::SkillName skillName) {
		if (!player->CastSkill(skillName)) {
			//std::cout << "Invalid move " << Skills::GetSkillName(skillName) << '\n';
			return false;
		}

		//std::cout << Skills::GetSkillName(skillName) << '\n';
		currentTime += player->GetSkillTime(skillName);
		currentCraft.push_back(skillName);

		return true;
	}

	bool IsItemWorkable() {
		if (!player->craftableItem->IsItemWorkable()) {		// Not workable
			if (player->craftableItem->IsItemCrafted()) {	// successful
				AddSuccessfulCraft();
			}

			return false;
		}
		return true;
	}

	void AddSuccessfulCraft() {
		if (topQuality && !player->craftableItem->IsItemMaxQuality()) {
			//std::cout << "Not maximum quality when needed\n";
			return;
		}
		//std::cout << "Craft successful\n";
		if (currentTime < bestTime)	bestTime = currentTime;
		successfulCrafts[currentTime].push_back(currentCraft);
	}

	void ContinueCraft() {
		player->RemoveItem();
		if (player->craftableItem == nullptr) {
			//std::cout << "Item has been deleted\n";
		}
		else {
			std::cout << "Item deletion broken. Exiting...\n";
			return;
		}
		player->AddItem(maxProgress, maxQuality, maxDurability);
		std::vector<Skills::SkillName> tempCraft{};
		currentTime = 0;
		currentCraft.swap(tempCraft);
		currentCraft.clear();
		//std::cout << "------------GOING BACK A STEP--------------------\n";
		for (int i{ 0 }; i < tempCraft.size() - 1; ++i) {
			Craft(tempCraft[i]);
			//std::cout << Skills::GetSkillName(tempCraft[i]) << '\n';
		}
	}

	bool SynthesisCheck(SkillName skillName) {
		for (const auto& synth : finalMoveList) {
			if (skillName == synth) {
				return true;
			}
		}
		return false;
	}

	bool QualityCheck(SkillName skillName) {
		bool maxQuality = player->craftableItem->IsItemMaxQuality();
		bool touchSkill{ false };
		

		if (maxQuality || !topQuality) {
			//std::cout << "Skipping Quality\n";
			for (const auto& touch : qualityList) {
				if (skillName == touch) {
					touchSkill = true;
					break;
				}
			}
			if (maxQuality) {
				//std::cout << "Maximum quality reached!\n";
			}
		}
		//std::cout << "Too high quality\n";
		return touchSkill;
	}

	bool BuffCheck(SkillName skillName) {
		bool buffSkip{ false };
		if (player->GetBuffDuration(skillName) > 0) {
			buffSkip = true;
		}
		if (!topQuality || player->craftableItem->IsItemMaxQuality()) {
			if (skillName == SkillName::FINALAPPRAISAL)		buffSkip = true;;
		}
		//std::cout << "Too high quality\n";
		return buffSkip;
	}

	const std::vector<SkillName> synthesisSkills = {
		SkillName::BASICSYNTHESIS,
		SkillName::CAREFULSYNTHESIS,
		SkillName::PRUDENTSYNTHESIS,
		SkillName::GROUNDWORK,
		SkillName::MUSCLEMEMORY
	};

};