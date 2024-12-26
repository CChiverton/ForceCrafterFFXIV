#pragma once


#include "Item.hpp"
#include "Player.hpp"
#include "Skills.hpp"
#include "ActionTracker.hpp"
#include <map>
#include <vector>
#include <array>


class Crafter : public Player {
public:
	Crafter(std::vector<Skills::SkillTest> startingMoves, int maxCP, float progressPerHundred, float qualityPerHundred, int maxProgress, int maxQuality, int maxDurability, bool forceQuality, bool greaterByregot, int maximumTurnLimit);

	~Crafter();
private:
	struct CraftingHistory {
		Player::PlayerState player;
		Item::ItemState item;
		int currentTime{ 0 };
		SkillName skillName{ SkillName::NONE };
	}craftingRecord;

public:

	void FindMinQualityForMax();
	void QualityOnlyCrafts(const SkillTest& move, const CraftingHistory& previousStep);
	void CraftAndRecord(const SkillTest& move, int finalAppraisalTimer);

	void ForceCraft();

private:

	std::vector<CraftingHistory> craftingHistory{};
	unsigned char bestTime{ 99 }, bestQualityTime{ 99 };
	std::map<int, std::vector<std::vector<Skills::SkillName>>> successfulCrafts{}, successfulQualityCrafts{};
	const bool forceMaxQuality, forceGreaterByregot;
	const unsigned char maxTurnLimit;
	std::unique_ptr<ActionTracker> actionTracker;
	bool invalid{ false };
	int16_t baseTurn{};
	int16_t minTouchSkills{ 0 };

	void StarterCraft(int finalAppraisalTimer);
	void SynthesisCraft(int finalAppraisalTimer);
	void QualityCraft(int finalAppraisalTimer);
	void BuffCraft(int finalAppraisalTimer);
	void RepairCraft(int finalAppraisalTimer, int remainingDurability);
	void OtherCraft(int finalAppraisalTimer);

	void UpdateValidBuffCheck(int appropriateActionTurn, bool actionUsedThisTurn);

	bool ActionUsedDuringBuff(int timeLeft, int actionHistory, int turns);

	// For example, BasicSynthesis->CarefulSynthesis and CarefulSynthesis->BasicSynthesis are the same if both buffed or unbuffed
	//@TODO update to include muscle memory buff
		// Only cancel out one order pair, the other will be tried out
	bool SimilarTrees(SkillName skillName);

	inline void SaveCraftingHistory(SkillName skillName);
	inline void DeleteCraftingHistory();

	bool Craft(Skills::SkillTest skillName);
	void AddSuccessfulQualityCraft();
	void AddSuccessfulCraft();
	inline void LoadLastCraftingRecord(const CraftingHistory& lastRecord);
	void ContinueCraft();

	bool QualityCheck(SkillName skillName);
	bool BuffCheck(SkillName skillName);

	void PrintCrafts();

	const std::array<SkillTest, 2> startingMoveList = {
		skillTest[(int)SkillName::MUSCLEMEMORY],
		skillTest[(int)SkillName::REFLECT]
	};

	const std::array<SkillTest, 5> synthesisSkills = {
		skillTest[(int)SkillName::MUSCLEMEMORY],
		skillTest[(int)SkillName::BASICSYNTHESIS],
		skillTest[(int)SkillName::CAREFULSYNTHESIS],
		skillTest[(int)SkillName::PRUDENTSYNTHESIS],
		skillTest[(int)SkillName::GROUNDWORK]
	};

		// All skills focused on touch regardless of category
	const std::array<SkillTest, 10> qualitySkills = {
		skillTest[(int)SkillName::REFLECT],
		skillTest[(int)SkillName::BYREGOTSBLESSING],
		skillTest[(int)SkillName::PREPARATORYTOUCH],
		skillTest[(int)SkillName::BASICTOUCH],
		skillTest[(int)SkillName::STANDARDTOUCH],
		skillTest[(int)SkillName::ADVANCEDTOUCH],
		skillTest[(int)SkillName::PRUDENTTOUCH],
		skillTest[(int)SkillName::REFINEDTOUCH],
		skillTest[(int)SkillName::GREATSTRIDES],
		skillTest[(int)SkillName::INNOVATION],
	};

	const std::array<SkillTest, 5> buffSkills = {
		skillTest[(int)SkillName::WASTENOTI],
		skillTest[(int)SkillName::WASTENOTII],
		skillTest[(int)SkillName::VENERATION],
		skillTest[(int)SkillName::MANIPULATION],
		skillTest[(int)SkillName::FINALAPPRAISAL],
	};

	const std::array<SkillTest, 2> repairSkills = {
		skillTest[(int)SkillName::MASTERSMEND],
		skillTest[(int)SkillName::IMMACULATEMEND]
	};
	const std::array <SkillTest, 1> otherSkills = {
		skillTest[(int)SkillName::DELICATESYNTHESIS]
	};
};