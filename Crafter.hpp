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
	Crafter(std::vector<Skills::SkillTest> startingMoves, int32_t maxCP, float progressPerHundred, float qualityPerHundred, uint16_t maxProgress,
		uint16_t maxQuality, int16_t maxDurability, bool forceQuality, bool greaterByregot, uint8_t maximumTurnLimit);
	~Crafter();

	void ForceCraft();

private:
	/*******************CRAFTING**********************/
	void FindFastestQuality(int16_t& durabilityCosts, uint8_t& twentyCosts);
	void FindFastestSynth(int16_t& durabilityCosts, uint8_t& twentyCosts);
	void FindDurabilityCost(int16_t& durabilityCosts, uint8_t& twentyCosts);
	void CraftAndRecord(const SkillTest& move);
	void FindMinQualityForMax();
	void FindMinSynthForMax();
	void QualityOnlyCrafts(const SkillTest& move);
	void SynthOnlyCrafts(const SkillTest& move);

	void CalculateRemainingQualityTime(int16_t& minQualityTurnsLeft, int16_t& maxQualityTime);
	void CalculateRemainingSynthTime(int16_t& minSynthTurnsLeft, int16_t& maxSynthTime);

	void StarterCraft();
	void SynthesisCraft();
	void QualityCraft();
	void BuffCraft();
	void RepairCraft(uint8_t remainingDurability);
	void OtherCraft();

	// For example, BasicSynthesis->CarefulSynthesis and CarefulSynthesis->BasicSynthesis are the same if both buffed or unbuffed
	// Only cancel out one order pair, the other will be tried out
	bool SimilarTrees(SkillName skillName);
	void ContinueCraft();

	bool QualityCheck(SkillName skillName);
	bool BuffCheck(SkillName skillName);


	/*********************** CRAFTING RECORDS ************************/
	inline void SaveCraftingRecord(SkillName skillName);
	inline void SaveCraftingHistory(SkillName skillName);
	inline void DeleteCraftingHistory();
	inline std::vector<SkillName> CompileSuccessfulCraft();
	void AddSuccessfulQualityCraft();
	void AddSuccessfulSynthCraft();
	void AddSuccessfulCraft(SkillName skillName);
	inline void LoadLastCraftingRecord();

	void PrintCrafts();
	void PrintSuccessfulCrafts();


	struct CraftingHistory {
		Player::PlayerState player;
		Item::ItemState item;
		uint8_t currentTime{ 0 };
		SkillName skillName{ SkillName::NONE };
	}craftingRecord;

	ActionTracker actionTracker;
	std::array<CraftingHistory, 60> craftingHistory;
	std::vector<uint16_t> bestQuality{}, bestSynth{};
	uint16_t bestTime{ 99 }, bestQualityTime{ 99 }, bestSynthTime{ 99 };
	std::map<uint8_t, std::vector<std::vector<Skills::SkillName>>> successfulCrafts{}, successfulQualityCrafts{}, successfulSynthCrafts{};
	const uint8_t maxTurnLimit;
	int16_t minTouchSkills{ 0 }, minSynthSkills{ 0 }, minDurabilitySkills{ 0 };

	/* Constructor controlled */
	bool invalid{ false };
	uint8_t baseTurn{};
	const bool forceMaxQuality, forceGreaterByregot;


	/************************** MOVES LIST ****************************/

	static inline const std::array<SkillTest, 2> startingMoveList = {
		skillTest[(int)SkillName::MUSCLEMEMORY],
		skillTest[(int)SkillName::REFLECT]
	};

	static inline const std::array<SkillTest, 6> synthesisSkills = {
		skillTest[(int)SkillName::MUSCLEMEMORY],
		skillTest[(int)SkillName::BASICSYNTHESIS],
		skillTest[(int)SkillName::CAREFULSYNTHESIS],
		skillTest[(int)SkillName::PRUDENTSYNTHESIS],
		skillTest[(int)SkillName::GROUNDWORK],
		skillTest[(int)SkillName::VENERATION]
	};

		// All skills focused on touch regardless of category
	static inline const std::array<SkillTest, 10> qualitySkills = {
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

	static inline const std::array<SkillTest, 4> buffSkills = {
		skillTest[(int)SkillName::WASTENOTI],
		skillTest[(int)SkillName::WASTENOTII],
		skillTest[(int)SkillName::MANIPULATION],
		skillTest[(int)SkillName::FINALAPPRAISAL],
	};

	static inline const std::array<SkillTest, 2> repairSkills = {
		skillTest[(int)SkillName::MASTERSMEND],
		skillTest[(int)SkillName::IMMACULATEMEND]
	};
	static inline const std::array <SkillTest, 1> otherSkills = {
		skillTest[(int)SkillName::DELICATESYNTHESIS]
	};
};