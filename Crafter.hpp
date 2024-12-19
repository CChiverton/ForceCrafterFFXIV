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
		playerItem = player->craftableItem;

		//craftingHistory.reserve(maximumTurnLimit);

		if (!startingMoves.empty()) {
			for (const Skills::SkillName& move : startingMoves) {
				if (!Craft(move)) {
					std::cout << "Invalid. The starting moves break/finish the item.\n";
					return;
				}
				SaveCraftingHistory(move);
				//std::cout << "Player turn is " << player->GetCurrentTurn() << '\n';
			}
		}
		//std::cout << "-----------------------------------------\n";
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

	void PrintCrafts() {
		std::cout << "Current: ";
		for (const auto& entry : craftingHistory) {
			std::cout << Skills::GetSkillName(entry.skillName) << ", ";
		}
		std::cout << '\n';
	}

	void ValidateCraft(SkillName skillName) {

	}

	void ForceCraft() {
		CraftingHistory& previousStep = craftingRecord;		// stack allocation for faster loading
		bool lastMove = ((previousStep.currentTime + 3) == bestTime || player->GetCurrentTurn() == maxTurnLimit - 1) ? true : false; // Only one move left to match the best time and turn limit
		for (const auto& move : fullSkillList) {
			if (lastMove) {		
				if (!SynthesisCheck(move)) {
					//std::cout << "Only checking synth moves\n";
					continue;
				}
			}

			if (QualityCheck(move)) {
				continue;
			}
			else if (BuffCheck(move)) {
				continue;
			}

			if (Craft(move)) {


				//std::cout << "Turn " << player->GetCurrentTurn() << ": " << Skills::GetSkillName(move) << '\n';

				if (playerItem->IsItemCrafted()) {
					if (topQuality && !playerItem->IsItemMaxQuality()) {
						//std::cout << "Not maximum quality when needed\n";
						LoadLastCraftingRecord(previousStep);
						continue;
					}
					SaveCraftingHistory(move);
					AddSuccessfulCraft();
					ContinueCraft();
				} else if (player->GetCurrentTurn() >= maxTurnLimit || (player->GetCurrentTime() + 3) > bestTime) {		// can't use lastMove here, causes some form of memory leak
					//std::cout << "Run out of moves\n";
					LoadLastCraftingRecord(previousStep);
					continue;
				}  else if (!playerItem->IsItemBroken()) {
					SaveCraftingHistory(move);
					ForceCraft();
				}
				//std::cout << "Finisheng Turn " << player->GetCurrentTurn() + 1 << ": " << Skills::GetSkillName(move) << '\n';
			}

		}
		ContinueCraft();
		//std::cout << player->GetCurrentTurn() << " TRIED ALL POSSIBLE MOVES AT THIS LEVEL\n";
	}

	struct CraftingHistory {
		Player::PlayerState player;
		Item::ItemState item;
		int currentTime{ 0 };
		SkillName skillName{ SkillName::NONE };
	}craftingRecord;

	inline void SaveCraftingHistory(SkillName skillName) {
		craftingRecord.player = player->GetPlayerState();
		craftingRecord.item = playerItem->GetItemState();
		craftingRecord.currentTime = player->GetCurrentTime();
		craftingRecord.skillName = skillName;
		craftingHistory.emplace_back(craftingRecord);
	}

	inline void DeleteCraftingHistory() {
		craftingHistory.pop_back();
	}

private:
	Player* player;
	int maxProgress{}, maxQuality{}, maxDurability{};
	std::vector<CraftingHistory> craftingHistory{};
	//std::vector<Skills::SkillName> currentCraft{};
	int bestTime{ 99 };
	std::map<int, std::vector<std::vector<Skills::SkillName>>> successfulCrafts{};
	bool topQuality{ false };
	bool forceGreaterByregot{ false };
	int maxTurnLimit;
	Item* playerItem;



	bool Craft(Skills::SkillName skillName) {
		if (!player->CastSkill(skillName)) {
			//std::cout << "Invalid move " << Skills::GetSkillName(skillName) << '\n';
			return false;
		}
		//std::cout << Skills::GetSkillName(skillName) << '\n';

		return true;
	}

	void AddSuccessfulCraft() {
		//std::cout << "Craft successful\n";

		bestTime = craftingRecord.currentTime;		// Time restraints already managed by force craft
		std::vector<SkillName> success{};
		//success.reserve(craftingHistory.size());
		for (const auto& entry : craftingHistory) {
			success.emplace_back(entry.skillName);
		}
		successfulCrafts[craftingRecord.currentTime].emplace_back(success);
	}

	inline void LoadLastCraftingRecord(CraftingHistory& lastRecord) {
		player->LoadPlayerStats(lastRecord.player);
		playerItem->LoadItemState(lastRecord.item);
		craftingRecord = lastRecord;
		/*std::cout << "After loading item stats are\n";
		player->craftableItem->OutputStats();*/
	}

	void ContinueCraft() {
		/*std::cout << "Previous item stats were\n";
		player->craftableItem->OutputStats();*/
		/*if (player->GetBuffDuration(SkillName::GREATSTRIDES) > 0)
		std::cout << "Greater strides buff before is " << player->GetBuffDuration(SkillName::GREATSTRIDES) << '\n';*/
		DeleteCraftingHistory();
		CraftingHistory& last = craftingHistory.back();
		LoadLastCraftingRecord(last);
		/*if (player->GetBuffDuration(SkillName::GREATSTRIDES) > 0)
		std::cout << "Greater strides buff after is " << player->GetBuffDuration(SkillName::GREATSTRIDES) << '\n';*/
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
		bool maxQuality = playerItem->IsItemMaxQuality();
		bool touchSkill{ false };

		if (skillName == SkillName::BYREGOTSBLESSING && forceGreaterByregot) {
			if (player->GetBuffDuration(SkillName::GREATSTRIDES) == 0) {
				return true;
			}
		}

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
		if (player->GetBuffDuration(skillName) > 0) {			// potentially bad logic
			buffSkip = true;
		}
		if (skillName == SkillName::FINALAPPRAISAL) {
			if (player->GetCurrentTurn() + 6 >= maxTurnLimit) {
				buffSkip = true;
			}
			else if (!topQuality || playerItem->IsItemMaxQuality()) {
				buffSkip = true;
			}
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

	const SkillName fullSkillList[23] = {
		SkillName::MUSCLEMEMORY,
		SkillName::REFLECT,

		SkillName::BYREGOTSBLESSING,
		SkillName::PREPARATORYTOUCH,
		SkillName::BASICTOUCH,
		SkillName::STANDARDTOUCH,
		SkillName::ADVANCEDTOUCH,
		SkillName::PRUDENTTOUCH,
		SkillName::REFINEDTOUCH,

		SkillName::BASICSYNTHESIS,
		SkillName::CAREFULSYNTHESIS,
		SkillName::PRUDENTSYNTHESIS,
		SkillName::GROUNDWORK,

		SkillName::DELICATESYNTHESIS,

		SkillName::WASTENOTI,
		SkillName::WASTENOTII,
		SkillName::GREATSTRIDES,
		SkillName::INNOVATION,
		SkillName::VENERATION,
		SkillName::MASTERSMEND,
		SkillName::MANIPULATION,
		SkillName::IMMACULATEMEND,


		SkillName::FINALAPPRAISAL
	};

	const SkillName finalMoveList[4] = {
		SkillName::BASICSYNTHESIS,
		SkillName::CAREFULSYNTHESIS,
		SkillName::PRUDENTSYNTHESIS,
		SkillName::GROUNDWORK,
	};

	// All skills focused on touch regardless of category
	const SkillName qualityList[10] = {
		SkillName::BASICTOUCH,
		SkillName::STANDARDTOUCH,
		SkillName::ADVANCEDTOUCH,
		SkillName::BYREGOTSBLESSING,
		SkillName::PRUDENTTOUCH,
		SkillName::PREPARATORYTOUCH,
		SkillName::REFLECT,
		SkillName::REFINEDTOUCH,
		SkillName::GREATSTRIDES,
		SkillName::INNOVATION
	};

	const SkillName buffList[7] = {
		SkillName::WASTENOTI,
		SkillName::WASTENOTII,
		SkillName::VENERATION,
		SkillName::FINALAPPRAISAL,
		SkillName::MASTERSMEND,
		SkillName::MANIPULATION,
		SkillName::IMMACULATEMEND
	};

};