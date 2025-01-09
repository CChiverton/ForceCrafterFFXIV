#pragma once

//@TODO create synth table lookup. May be more expensive to do so as there are generally fewer calculations with it

#include "Item.hpp"
#include "Skills.hpp"
#include <array>

using namespace Skills;

class Player {

public:
	Player(int maximumCP, float progressPerHundred, float qualityPerHundred);

	struct BuffInfo {
		unsigned char muscleMemory = 0;
		bool muscleMemoryActive = false;
		unsigned char wasteNot = 0;
		bool wasteNotActive = false;
		unsigned char greatStrides = 0;
		bool greatStridesActive = false;
		unsigned char innovation = 0;
		bool innovationActive = false;
		unsigned char veneration = 0;
		bool venerationActive = false;
		unsigned char finalAppraisal = 0;
		bool finalAppraisalActive = false;
		unsigned char manipulation = 0;
		bool manipulationActive = false;
	};

	struct PlayerState {
		int currentCP{ 0 };
		unsigned char currentTurn{ 1 };
		unsigned char currentTime{ 0 };
		unsigned char innerQuiet{ 0 };;
		BuffInfo buffInfo{};
		Skills::SkillName lastSkillUsed{ Skills::SkillName::NONE };
	}playerState;

	void ResetPlayerStats();
	void LoadPlayerStats(const PlayerState&);
	
	/* Getters */
	const PlayerState& GetPlayerState() const;
	inline const unsigned char GetCurrentTime() const;
	inline const unsigned char GetCurrentTurn() const;
	const unsigned char GetBuffDuration(SkillName skillName) const;

protected:
	/* Item control */
	std::unique_ptr<Item> craftableItem = nullptr;
	void AddItem(const int& maxProgress, const int& maxQuality, const int& maxDurability);
	void RemoveItem();

	/* Skills application */
	bool CastSkill(const Skills::SkillTest& skillName);
	void SynthesisSkills(const SkillName skillName, const int& skillDurabilityCost, int skillEfficiency);
	void TouchSkills(const SkillName skillName, const int skillDurabilityCost, int& skillCPCost);
	void BuffSkills(const SkillName skillName);
	void RepairSkills(const SkillName skillName);
	void OtherSkills(const SkillName skillName, const int& skillDurabilityCost);
private:
	/* Calculations */
	const int CalculateProgress(const int16_t efficiency);
	const int CalculateQuality(SkillName skillName);

	/* Inner Quiet*/
	void AddInnerQuiet(unsigned char stacks);
	inline const float InnerQuietEfficiencyMultiplier() const;

	/* Buffs */
	void SynthesisBuffs(int& skillEfficiency);
	void TouchBuffs(int& skillEfficiency);
	void DecrementBuffs();

	inline void PreComputeQualityEfficiency();

	static constexpr unsigned char maxInnerQuiet{ 10 };
	std::array<std::array<int16_t, SkillName::REFINEDTOUCH + 1>, maxInnerQuiet + 1> preComputeQualityEfficiency, preComputeQualityTouchEfficiency,
		preComputeQualityStrideEfficiency, preComputeQualityTouchStrideEfficiency;
	const int maxCP{};
	const float progressPerOne{}, qualityPerOne{};
	bool successfulCast{ true };
};