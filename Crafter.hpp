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
			switch (craftLogicType.at(move)) {
			case SkillType::SYNTHESIS:
				if (lastMove) {
					if (!SynthesisCheck(move))	continue;
				}
				break;
			case SkillType::TOUCH:
				if (lastMove)	continue;
				if (QualityCheck(move)) {
					continue;
				}
				break;
			case SkillType::BUFF:
				if (lastMove)	continue;
				if (BuffCheck(move)) {
					continue;
				}
				break;
			case SkillType::REPAIR:
				if (lastMove)	continue;
				
				break;
			case SkillType::OTHER:
				
				break;
			default:
				std::cout << "A serious error has occured\n";
			}
			
				
			/*if (SynthesisCheck(move)) {

			}
			else if (lastMove) {
				continue;
			}
			else if (IsQualitySkill(move)) {
				if (QualityCheck(move)) {
					continue;
				}
			}
			else if (IsBuffSkill(move)) {
				if (BuffCheck(move)) {
					continue;
				}
			}*/

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
		DeleteCraftingHistory();
		CraftingHistory& last = craftingHistory.back();
		LoadLastCraftingRecord(last);
	}

	/*bool IsSynthesisSkill(SkillName skillName) {
		for (const auto& synth : finalMoveList) {
			if (skillName == synth) {
				return true;
			}
		}
		return false;
	}*/

	bool SynthesisCheck(SkillName skillName) {
		for (const auto& synth : finalMoveList) {
			if (skillName == synth) {
				return true;
			}
		}
		return false;
	}

	/*bool IsQualitySkill(SkillName skillName) {
		for (const auto& touch : qualityList) {
			if (skillName == touch) {
				return true;
			}
		}
		return false;
	}*/

	bool QualityCheck(SkillName skillName) {
		bool maxQuality = playerItem->IsItemMaxQuality();
		if (maxQuality || !topQuality) {		// no need for touch skills
			//std::cout << "Skipping Quality\n";
			return true;
		}
		bool skipTouchSkill{ false };
		switch (skillName) {
		case SkillName::GREATSTRIDES:
			skipTouchSkill = player->GetBuffDuration(skillName) != 0;
			break;
		case SkillName::INNOVATION:
			skipTouchSkill = player->GetBuffDuration(skillName) > 1;		// fringe cases you want to recast innovation with 1 turn left
			break;
		case SkillName::BYREGOTSBLESSING:
			if (forceGreaterByregot) {
				skipTouchSkill = player->GetBuffDuration(SkillName::GREATSTRIDES) == 0;
			}
			break;
		default:
			break;
		}
		//std::cout << "Too high quality\n";
		return skipTouchSkill;
	}

	/*bool IsBuffSkill(SkillName skillName) {
		for (const auto& buff : buffList) {
			if (skillName == buff) {
				return true;
			}
		}
		return false;
	}*/

	bool BuffCheck(SkillName skillName) {
		bool buffSkip{ false };
		if (player->GetBuffDuration(skillName) > 0) {			// potentially bad logic
			buffSkip = true;
		}
		switch (skillName) {
		case SkillName::WASTENOTI:
		case SkillName::WASTENOTII:
			break;
		case SkillName::VENERATION:
			break;
		case SkillName::FINALAPPRAISAL:
			if (player->GetCurrentTurn() + 6 >= maxTurnLimit) {
				buffSkip = true;
			}
			else if (!topQuality || playerItem->IsItemMaxQuality()) {
				buffSkip = true;
			}
			break;
		case SkillName::MANIPULATION:
			break;
		default:
			std::cout << "A serious error has occured for " << Skills::GetSkillName(skillName) << '\n';
			break;
		}
		
		//std::cout << "Too high quality\n";
		return buffSkip;
	}

	const std::unordered_map<SkillName, SkillType> craftLogicType = {
		{ SkillName::BASICSYNTHESIS,	SkillType::SYNTHESIS},
		{ SkillName::CAREFULSYNTHESIS,	SkillType::SYNTHESIS},
		{ SkillName::PRUDENTSYNTHESIS,	SkillType::SYNTHESIS},
		{ SkillName::GROUNDWORK,		SkillType::SYNTHESIS},
		{ SkillName::MUSCLEMEMORY,		SkillType::SYNTHESIS},
		{ SkillName::BASICTOUCH,		SkillType::TOUCH},
		{ SkillName::STANDARDTOUCH,		SkillType::TOUCH},
		{ SkillName::ADVANCEDTOUCH,		SkillType::TOUCH},
		{ SkillName::BYREGOTSBLESSING,	SkillType::TOUCH},
		{ SkillName::PRUDENTTOUCH,		SkillType::TOUCH},
		{ SkillName::PREPARATORYTOUCH,	SkillType::TOUCH},
		{ SkillName::DELICATESYNTHESIS,	SkillType::TOUCH},
		{ SkillName::REFLECT,			SkillType::TOUCH},
		{ SkillName::REFINEDTOUCH,		SkillType::TOUCH},
		{ SkillName::GREATSTRIDES,		SkillType::TOUCH},
		{ SkillName::INNOVATION,		SkillType::TOUCH},
		{ SkillName::WASTENOTI,			SkillType::BUFF},
		{ SkillName::WASTENOTII,		SkillType::BUFF},
		{ SkillName::VENERATION,		SkillType::BUFF},
		{ SkillName::FINALAPPRAISAL,	SkillType::BUFF},
		{ SkillName::MANIPULATION,		SkillType::BUFF},
		{ SkillName::MASTERSMEND,		SkillType::OTHER},
		{ SkillName::IMMACULATEMEND,	SkillType::OTHER}
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

	/*const std::vector<SkillName> synthesisSkills = {
		SkillName::BASICSYNTHESIS,
		SkillName::CAREFULSYNTHESIS,
		SkillName::PRUDENTSYNTHESIS,
		SkillName::GROUNDWORK,
		SkillName::MUSCLEMEMORY
	};*/

	//// All skills focused on touch regardless of category
	//const SkillName qualityList[11] = {
	//	SkillName::BASICTOUCH,
	//	SkillName::STANDARDTOUCH,
	//	SkillName::ADVANCEDTOUCH,
	//	SkillName::BYREGOTSBLESSING,
	//	SkillName::PRUDENTTOUCH,
	//	SkillName::PREPARATORYTOUCH,
	//	SkillName::DELICATESYNTHESIS,
	//	SkillName::REFLECT,
	//	SkillName::REFINEDTOUCH,
	//	SkillName::GREATSTRIDES,
	//	SkillName::INNOVATION
	//};

	//const SkillName buffList[5] = {
	//	SkillName::WASTENOTI,
	//	SkillName::WASTENOTII,
	//	SkillName::VENERATION,
	//	SkillName::FINALAPPRAISAL,
	//	SkillName::MANIPULATION,
	//	
	//};

	//const SkillName otherList[2] = {
	//	SkillName::MASTERSMEND,
	//	SkillName::IMMACULATEMEND
	//};

};