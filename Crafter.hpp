#pragma once


#include "Item.hpp"
#include "Player.hpp"
#include "Skills.hpp"
#include "ActionTracker.hpp"
#include <map>
#include <vector>


class Crafter {
public:
	Crafter(std::vector<Skills::SkillName> startingMoves, int maxCP, int maxProgress, int maxQuality, int maxDurability, bool forceQuality, bool greaterByregot, int maximumTurnLimit);

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

	void UpdateValidBuffCheck(int& appropriateActionTurn, bool& actionUsedThisTurn) {
		appropriateActionTurn <<= 1;
		appropriateActionTurn |= actionUsedThisTurn;
	}

	bool ActionUsedDuringBuff(int& timeLeft, int& actionHistory, int turns) {
		return (timeLeft == 1 && !(actionHistory & turns));
	}

	// For example, BasicSynthesis->CarefulSynthesis and CarefulSynthesis->BasicSynthesis are the same if both buffed or unbuffed
	//@TODO update to include muscle memory buff
		// Only cancel out one order pair, the other will be tried out
	bool SimilarTrees(SkillName skillName, bool& basic, bool& careful, bool& prudent, bool& ground, int& history, int& wasteNotHistory) {
		if (history != 0b11 && history != 0b00)	return false;	// only proceed if the veneration history is actually the same
		if (wasteNotHistory != 0b11 && wasteNotHistory != 0b00)	return false;	// only proceed if the veneration history is actually the same
		
		switch (skillName) {
		case SkillName::BASICSYNTHESIS:
			break;
		case SkillName::CAREFULSYNTHESIS:
			if (basic)		return true;
			break;
		case SkillName::PRUDENTSYNTHESIS:
			if (basic || careful)		return true;
			break;
		case SkillName::GROUNDWORK:
			if (basic || careful || prudent)	return true;
			break;
		default:
			break;
		}
		return false;
	}

	void ForceCraft() {
		CraftingHistory& previousStep = craftingRecord;		// stack allocation for faster loading
		bool lastMove = ((previousStep.currentTime + 3) >= bestTime || player->GetCurrentTurn() == maxTurnLimit - 1) ? true : false; // Only one move left to match the best time and turn limit
		bool secondToLastMove = ((previousStep.currentTime + 6) >= bestTime || player->GetCurrentTurn() == maxTurnLimit - 2) ? true : false;
		int innovationTimer = player->GetBuffDuration(SkillName::INNOVATION);

		int venerationTimer = player->GetBuffDuration(SkillName::VENERATION);
		actionTracker->ProgressBuffs(venerationTimer > 0, player->GetBuffDuration(SkillName::WASTENOTI) > 0);
		int finalAppraisalTimer = player->GetBuffDuration(SkillName::FINALAPPRAISAL);
		bool isMaxQuality = playerItem->IsItemMaxQuality();
		int itemDurability = playerItem->GetDurability();
		

		bool requireTouch = ActionUsedDuringBuff(innovationTimer, touchActionsUsedSuccessfully, 0b111);
		bool requireSynth = ActionUsedDuringBuff(venerationTimer, synthActionsUsedSuccessfully, 0b111);
		bool requireAppraisal = ActionUsedDuringBuff(finalAppraisalTimer, synthActionsUsedSuccessfully, 0b1111);

		bool basic = actionTracker->basicSynthesis & 0b1;
		bool careful = actionTracker->carefulSynthesis & 0b1;
		bool prudent = actionTracker->prudentSynthesis & 0b1;
		bool ground = actionTracker->groundwork & 0b1;
		int venerationHistory = actionTracker->venerationHistory & 0b11;
		int wasteNotHistory = actionTracker->wasteNotHistory & 0b11;

		for (const auto& move : fullSkillList) {
			
			/* BUFF TURN TRACKERS */
			touchActionUsed = false;
			

			switch (craftLogicType.at(move)) {
			case SkillType::SYNTHESIS:
				if (lastMove) {
					if (!SynthesisCheck(move))	continue;
				}
				if (secondToLastMove && forceMaxQuality && !isMaxQuality)	continue;
				if (requireTouch) continue;
				if (SimilarTrees(move, basic, careful, prudent, ground, venerationHistory, wasteNotHistory))	continue;
				synthActionUsed = true;
				break;
			case SkillType::TOUCH:
				if (!forceMaxQuality)	continue;
				if (isMaxQuality)	continue;
				if (lastMove)	continue;
				if (move == SkillName::DELICATESYNTHESIS) {
					synthActionUsed = true;
				}
				if (requireSynth && !synthActionUsed) continue;
				if (requireAppraisal && !synthActionUsed) continue;
				if (QualityCheck(move)) {
					continue;
				}
				break;
			case SkillType::BUFF:
				if (lastMove)	continue;
				if (secondToLastMove && forceMaxQuality && !isMaxQuality)	continue;
				if (requireTouch) continue;
				if (requireSynth) continue;
				if (requireAppraisal) continue;
				if (BuffCheck(move)) {
					continue;
				}
				break;
			case SkillType::REPAIR:
				if (lastMove)	continue;
				if (secondToLastMove && (itemDurability >= 20 || (forceMaxQuality && !isMaxQuality)))	continue;
				if (requireTouch) continue;
				if (requireAppraisal) continue;
				break;
			case SkillType::OTHER:
				if (requireTouch) continue;
				if (requireSynth) continue;
				if (requireAppraisal) continue;
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
					if (forceMaxQuality && !playerItem->IsItemMaxQuality()) {
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
				}
				else if (finalAppraisalTimer == 1 && (playerItem->GetMaxProgress() - playerItem->GetCurrentProgress()) != 1) {		// not appraised
					LoadLastCraftingRecord(previousStep);
					continue;
				} else if (!playerItem->IsItemBroken()) {
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
		actionTracker->ProgressSynthSkills(skillName);
	}

	inline void DeleteCraftingHistory() {
		craftingHistory.pop_back();
	}

private:
	Player* player;
	const int maxProgress{}, maxQuality{}, maxDurability{};
	std::vector<CraftingHistory> craftingHistory{};
	//std::vector<Skills::SkillName> currentCraft{};
	int bestTime{ 99 };
	std::map<int, std::vector<std::vector<Skills::SkillName>>> successfulCrafts{};
	const bool forceMaxQuality;
	const bool forceGreaterByregot;
	const int maxTurnLimit;
	Item* playerItem;
	bool touchActionUsed{ false }, synthActionUsed{ false };
	int touchActionsUsedSuccessfully = 0b0, synthActionsUsedSuccessfully = 0b0;
	ActionTracker* actionTracker;



	bool Craft(Skills::SkillName skillName) {
		if (!player->CastSkill(skillName)) {
			//std::cout << "Invalid move " << Skills::GetSkillName(skillName) << '\n';
			return false;
		}
		UpdateValidBuffCheck(touchActionsUsedSuccessfully, touchActionUsed);
		UpdateValidBuffCheck(synthActionsUsedSuccessfully, synthActionUsed);
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
		touchActionsUsedSuccessfully >>= 1;
		synthActionsUsedSuccessfully >>= 1;
		actionTracker->BacktrackBuffs();
		actionTracker->BacktrackSynthSkills();
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
			touchActionUsed = true;
			break;
		default:			// Should be touch action skills
			touchActionUsed = true;
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
			else if (!forceMaxQuality || playerItem->IsItemMaxQuality()) {
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
		{ SkillName::MASTERSMEND,		SkillType::REPAIR},
		{ SkillName::IMMACULATEMEND,	SkillType::REPAIR}
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