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

	~Crafter();
private:
	struct CraftingHistory {
		Player::PlayerState player;
		Item::ItemState item;
		int currentTime{ 0 };
		SkillName skillName{ SkillName::NONE };
	}craftingRecord;

public:

	void CraftAndRecord(SkillName move, CraftingHistory& previousStep, int& finalAppraisalTimer);

	void ForceCraft();

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
	bool invalid{ false };

	void SynthesisCraft(bool& lastMove, bool& secondToLastMove, bool& requireTouch, bool& isMaxQuality, CraftingHistory& previousStep, int& finalAppraisalTimer);
	void QualityCraft(bool& isMaxQuality, bool& skip, CraftingHistory& previousStep, int& finalAppraisalTimer);
	void BuffCraft(bool& skip, bool& secondToLastMove, bool& requireTouch, bool& isMaxQuality, CraftingHistory& previousStep, int& finalAppraisalTimer);
	void RepairCraft(bool& skip, bool& secondToLastMove, int& itemDurability, bool& requireTouch, bool& isMaxQuality, CraftingHistory& previousStep, int& finalAppraisalTimer);
	void OtherCraft(CraftingHistory& previousStep, int& finalAppraisalTimer);

	void UpdateValidBuffCheck(int& appropriateActionTurn, bool& actionUsedThisTurn);

	bool ActionUsedDuringBuff(int& timeLeft, int& actionHistory, int turns);

	// For example, BasicSynthesis->CarefulSynthesis and CarefulSynthesis->BasicSynthesis are the same if both buffed or unbuffed
	//@TODO update to include muscle memory buff
		// Only cancel out one order pair, the other will be tried out
	bool SimilarTrees(SkillName skillName);

	inline void SaveCraftingHistory(SkillName skillName);
	inline void DeleteCraftingHistory();

	bool Craft(Skills::SkillName skillName);
	void AddSuccessfulCraft();
	inline void LoadLastCraftingRecord(CraftingHistory& lastRecord);
	void ContinueCraft();

	bool SynthesisCheck(SkillName skillName);
	bool QualityCheck(SkillName skillName);
	bool BuffCheck(SkillName skillName);

	void PrintCrafts();

	const SkillName finalMoveList[4] = {
		SkillName::BASICSYNTHESIS,
		SkillName::CAREFULSYNTHESIS,
		SkillName::PRUDENTSYNTHESIS,
		SkillName::GROUNDWORK,
	};

	const SkillName synthesisSkills[5] = {
		SkillName::MUSCLEMEMORY,
		SkillName::BASICSYNTHESIS,
		SkillName::CAREFULSYNTHESIS,
		SkillName::PRUDENTSYNTHESIS,
		SkillName::GROUNDWORK		
	};

		// All skills focused on touch regardless of category
	const SkillName qualitySkills[10] = {
		SkillName::REFLECT,
		SkillName::BYREGOTSBLESSING,
		SkillName::PREPARATORYTOUCH,
		SkillName::BASICTOUCH,
		SkillName::STANDARDTOUCH,
		SkillName::ADVANCEDTOUCH,
		SkillName::PRUDENTTOUCH,
		SkillName::REFINEDTOUCH,
		SkillName::GREATSTRIDES,
		SkillName::INNOVATION,
	};

	const SkillName buffSkills[5] = {
		SkillName::WASTENOTI,
		SkillName::WASTENOTII,
		SkillName::VENERATION,
		SkillName::MANIPULATION,
		SkillName::FINALAPPRAISAL,
	};

	const SkillName repairSkills[2] = {
		SkillName::MASTERSMEND,
		SkillName::IMMACULATEMEND
	};
	const SkillName otherSkills[1] = {
		SkillName::DELICATESYNTHESIS
	};
};