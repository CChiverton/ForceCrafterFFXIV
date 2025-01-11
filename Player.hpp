#pragma once

//@TODO create synth table lookup. May be more expensive to do so as there are generally fewer calculations with it

#include "Item.hpp"
#include "Skills.hpp"
#include <array>

using namespace Skills;

class Player {

public:
	Player(int32_t maximumCP, float progressPerHundred, float qualityPerHundred);

	struct BuffInfo {
		uint8_t muscleMemory = 0;
		bool muscleMemoryActive = false;
		uint8_t wasteNot = 0;
		bool wasteNotActive = false;
		uint8_t greatStrides = 0;
		bool greatStridesActive = false;
		uint8_t innovation = 0;
		bool innovationActive = false;
		uint8_t veneration = 0;
		bool venerationActive = false;
		uint8_t finalAppraisal = 0;
		bool finalAppraisalActive = false;
		uint8_t manipulation = 0;
		bool manipulationActive = false;
	};

	struct PlayerState {
		int32_t currentCP{ 0 };
		uint8_t currentTurn{ 1 };
		uint8_t currentTime{ 0 };
		uint8_t innerQuiet{ 0 };;
		BuffInfo buffInfo{};
		Skills::SkillName lastSkillUsed{ Skills::SkillName::NONE };
	}playerState;

	void ResetPlayerStats();
	void LoadPlayerStats(const PlayerState&);
	
	/* Getters */
	const PlayerState& GetPlayerState() const;
	inline const uint8_t GetCurrentTime() const;
	inline const uint8_t GetCurrentTurn() const;
	const uint8_t GetBuffDuration(SkillName skillName) const;

	void OutputStats() {
		std::cout << "Current CP: " << playerState.currentCP << '\n';
		std::cout << "Current Turn: " << (int)playerState.currentTurn << '\n';
		std::cout << "Current Time: " << (int)playerState.currentTime << '\n';
		std::cout << "Current Quiet " << (int)playerState.innerQuiet << '\n';
	}

protected:
	/* Item control */
	Item craftableItem;
	void AddItem(const uint16_t& maxProgress, const uint16_t& maxQuality, const int16_t& maxDurability);
	void RemoveItem();

	/* Skills application */
	bool CastSkill(const Skills::SkillTest& skillName);
	void SynthesisSkills(const SkillName skillName, const int16_t& skillDurabilityCost, uint16_t skillEfficiency);
	void TouchSkills(const SkillName skillName, const int16_t skillDurabilityCost, int32_t& skillCPCost);
	void BuffSkills(const SkillName skillName);
	void RepairSkills(const SkillName skillName);
	void OtherSkills(const SkillName skillName, const int16_t& skillDurabilityCost);
private:
	/* Calculations */
	const uint16_t CalculateProgress(const int16_t efficiency);
	const uint16_t CalculateQuality(SkillName skillName);

	/* Inner Quiet*/
	void AddInnerQuiet(uint8_t stacks);
	inline const float InnerQuietEfficiencyMultiplier() const;

	/* Buffs */
	void SynthesisBuffs(uint16_t& skillEfficiency);
	void TouchBuffs(uint16_t& skillEfficiency);
	void DecrementBuffs();

	inline void PreComputeQualityEfficiency();

	static constexpr uint8_t maxInnerQuiet{ 10 };
	std::array<std::array<int16_t, SkillName::REFINEDTOUCH + 1>, maxInnerQuiet + 1> preComputeQualityEfficiency, preComputeQualityTouchEfficiency,
		preComputeQualityStrideEfficiency, preComputeQualityTouchStrideEfficiency;
	const int32_t maxCP{};
	bool successfulCast{ true };
};