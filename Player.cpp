#include "Player.hpp"

#include <iostream>

/* PUBLIC */
Player::Player(int maximumCP, float progressPerHundred, float qualityPerHundred) : 
	progressPerOne(progressPerHundred/100.0f), qualityPerOne(qualityPerHundred/100.0f), maxCP(maximumCP) {
	ResetPlayerStats();
	PreComputeQualityEfficiency();
	ResetPlayerStats();
}

/* PLAYER CONTROL */
void Player::ResetPlayerStats() {
	playerState.currentCP = maxCP;
	playerState.innerQuiet = 0;
	playerState.currentTurn = 1;
	playerState.currentTime = 0;
	playerState.lastSkillUsed = SkillName::NONE;
	successfulCast = true; // Only turned false on failure. True by default
	playerState.buffInfo.muscleMemory = 0;
	playerState.buffInfo.muscleMemoryActive = false;
	playerState.buffInfo.wasteNot = 0;
	playerState.buffInfo.wasteNotActive = false;
	playerState.buffInfo.greatStrides = 0;
	playerState.buffInfo.greatStridesActive = false;
	playerState.buffInfo.innovation = 0;
	playerState.buffInfo.innovationActive = false;
	playerState.buffInfo.veneration = 0;
	playerState.buffInfo.venerationActive = false;
	playerState.buffInfo.finalAppraisal = 0;
	playerState.buffInfo.finalAppraisalActive = false;
	playerState.buffInfo.manipulation = 0;
	playerState.buffInfo.manipulationActive = false;
}

void Player::LoadPlayerStats(const PlayerState& state) {
	playerState = state;
}

const Player::PlayerState& Player::GetPlayerState() const {
	return playerState;
}

inline const unsigned char Player::GetCurrentTurn() const {
	return playerState.currentTurn;
}

inline const unsigned char Player::GetCurrentTime() const {
	return playerState.currentTime;
}

const unsigned char Player::GetBuffDuration(SkillName skillName) const {
	switch (skillName) {
	case SkillName::MUSCLEMEMORY:
		return playerState.buffInfo.muscleMemory;
	case SkillName::WASTENOTI:
	case SkillName::WASTENOTII:
		return playerState.buffInfo.wasteNot;
	case SkillName::GREATSTRIDES:
		return playerState.buffInfo.greatStrides;
	case SkillName::INNOVATION:
		return playerState.buffInfo.innovation;
	case SkillName::VENERATION:
		return playerState.buffInfo.veneration;
	case SkillName::FINALAPPRAISAL:
		return playerState.buffInfo.finalAppraisal;
	case SkillName::MANIPULATION:
		return playerState.buffInfo.manipulation;
	default:
		std::cout << "This is not a buff\n";
		std::cout << Skills::GetSkillName(skillName) << '\n';
		break;
	}
	return 0;
}


/* ITEM CONTROL */
void Player::AddItem(const int& maxProgress, const int& maxQuality, const int& maxDurability) {
	ResetPlayerStats();
	craftableItem = Item(maxProgress, maxQuality, maxDurability);
}

void Player::RemoveItem() {
	craftableItem = Item();
}


/* SKILL APPLICATION */
bool Player::CastSkill(const Skills::SkillTest& skill) {
	if (skill.costCP > playerState.currentCP) {
		//std::cout << "Not enough CP\n";
		return false;
	}
	successfulCast = true;

	int skillCPCost = skill.costCP;
	int skillDurabilityCost = (playerState.buffInfo.wasteNotActive) ? skill.costDurability/2 : skill.costDurability;
	int skillEfficiency = skill.efficiency;
	switch (skill.type) {
	case SkillType::SYNTHESIS:
		SynthesisSkills(skill.skillName, skillDurabilityCost, skillEfficiency);
		break;
	case SkillType::TOUCH:
		TouchSkills(skill.skillName, skillDurabilityCost, skillCPCost);
		break;
	case SkillType::BUFF:
		BuffSkills(skill.skillName);
		break;
	case SkillType::REPAIR:
		RepairSkills(skill.skillName);
		break;
	case SkillType::OTHER:
		OtherSkills(skill.skillName, skillDurabilityCost);
		break;
	default:
		std::cout << "A serious error has occured\n";
		break;
	}

	if (successfulCast) {
		/* Apply appraisal buff */
		if (playerState.buffInfo.finalAppraisalActive && craftableItem.IsItemCrafted()) {
			craftableItem.AddProgress(craftableItem.GetRemainingProgress() - 1, 0);
		}

		/* Apply manipulation buff */
		if (playerState.buffInfo.manipulationActive && playerState.buffInfo.manipulation < 9 && !craftableItem.IsItemBroken()) {
			craftableItem.UpdateDurability(5);		// Item needs to not be broken for the buff to be applied
		}

		/* Update player state */
		playerState.lastSkillUsed = skill.skillName;
		playerState.currentCP -= skillCPCost;
		++playerState.currentTurn;
		playerState.currentTime += skill.castTime;
		if (skill.skillName != SkillName::FINALAPPRAISAL) {
			DecrementBuffs();
		}
	}

	return successfulCast;
}

void Player::SynthesisSkills(const SkillName skillName, const int& skillDurabilityCost, int skillEfficiency) {
	switch (skillName) {
	case Skills::SkillName::PRUDENTSYNTHESIS:
		if (!playerState.buffInfo.wasteNotActive) {
			craftableItem.AddProgress(CalculateProgress(skillEfficiency), skillDurabilityCost);
		}
		else {
			successfulCast = false;
		}
		break;
	case Skills::SkillName::GROUNDWORK:
		if (craftableItem.GetCurrentDurability() < skillDurabilityCost) {
			craftableItem.AddProgress(CalculateProgress(skillEfficiency / 2), skillDurabilityCost);
		}
		else {
			craftableItem.AddProgress(CalculateProgress(skillEfficiency), skillDurabilityCost);
		}

		break;
	case Skills::SkillName::MUSCLEMEMORY:
		if (playerState.currentTurn == 1) {
			craftableItem.AddProgress(CalculateProgress(skillEfficiency), skillDurabilityCost);
			playerState.buffInfo.muscleMemory = 6;
			playerState.buffInfo.muscleMemoryActive = true;
		}
		else {
			successfulCast = false;
		}
		break;
	default:
		craftableItem.AddProgress(CalculateProgress(skillEfficiency), skillDurabilityCost);
		break;
	}
}

//@TODO Change the way cp cost is checked so combo works
void Player::TouchSkills(const SkillName skillName, const int skillDurabilityCost, int& skillCPCost) {
	switch (skillName) {
	case Skills::SkillName::BASICTOUCH:
		craftableItem.AddQuality(CalculateQuality(skillName), skillDurabilityCost);
		AddInnerQuiet(1);
		break;
	case Skills::SkillName::STANDARDTOUCH:
		if (playerState.lastSkillUsed == SkillName::BASICTOUCH) {
			skillCPCost = 18;
		}
		craftableItem.AddQuality(CalculateQuality(skillName), skillDurabilityCost);
		AddInnerQuiet(1);
		break;
	case Skills::SkillName::ADVANCEDTOUCH:
		if (playerState.lastSkillUsed == SkillName::STANDARDTOUCH) {
			skillCPCost = 18;
		}
		craftableItem.AddQuality(CalculateQuality(skillName), skillDurabilityCost);
		AddInnerQuiet(1);
		break;
	case Skills::SkillName::BYREGOTSBLESSING:
		if (playerState.innerQuiet > 0) {
			craftableItem.AddQuality(CalculateQuality(skillName), skillDurabilityCost);
			playerState.innerQuiet = 0;
		}
		else {
			successfulCast = false;
		}
		break;
	case Skills::SkillName::PRUDENTTOUCH:
		if (!playerState.buffInfo.wasteNotActive) {
			craftableItem.AddQuality(CalculateQuality(SkillName::BASICTOUCH), skillDurabilityCost);
		}
		else {
			successfulCast = false;
		}
		break;
	case Skills::SkillName::PREPARATORYTOUCH:
		craftableItem.AddQuality(CalculateQuality(skillName), skillDurabilityCost);
		AddInnerQuiet(2);
		break;
	case Skills::SkillName::REFLECT:
		if (playerState.currentTurn == 1) {
			craftableItem.AddQuality(CalculateQuality(skillName), skillDurabilityCost);
			AddInnerQuiet(2);
		}
		else {
			successfulCast = false;
		}
		break;
	case Skills::SkillName::REFINEDTOUCH:
		craftableItem.AddQuality(CalculateQuality(SkillName::BASICTOUCH), skillDurabilityCost);
		if (playerState.lastSkillUsed == Skills::SkillName::BASICTOUCH) {
			AddInnerQuiet(1);
		}
		AddInnerQuiet(1);
		break;
	default:
		break;
	}
}

// buffs are 1 turn higher than they should be due to immediately losing a turn to buffs upon cast
void Player::BuffSkills(const SkillName skillName) {
	switch (skillName) {
	case SkillName::WASTENOTI:
		playerState.buffInfo.wasteNot = 5;
		playerState.buffInfo.wasteNotActive = true;
		break;
	case SkillName::WASTENOTII:
		playerState.buffInfo.wasteNot = 9;
		playerState.buffInfo.wasteNotActive = true;
		break;
	case SkillName::GREATSTRIDES:
		playerState.buffInfo.greatStrides = 4;
		playerState.buffInfo.greatStridesActive = true;
		break;
	case SkillName::INNOVATION:
		playerState.buffInfo.innovation = 5;
		playerState.buffInfo.innovationActive = true;
		break;
	case SkillName::VENERATION:
		playerState.buffInfo.veneration = 5;
		playerState.buffInfo.venerationActive = true;
		break;
	case SkillName::FINALAPPRAISAL:
		playerState.buffInfo.finalAppraisal = 5;		// not incremented by 1 like the others as buff decrements are paused for the cast
		playerState.buffInfo.finalAppraisalActive = true;
		break;
	default:
		break;
	}
}

void Player::RepairSkills(const SkillName skillName) {
	switch (skillName) {
	case SkillName::MASTERSMEND:
		craftableItem.UpdateDurability(30);
		break;
	case SkillName::MANIPULATION:
		playerState.buffInfo.manipulation = 9;
		playerState.buffInfo.manipulationActive = true;
		break;
	case SkillName::IMMACULATEMEND:
		craftableItem.UpdateDurability(1000);
		break;
	default:
		break;
	}
}

void Player::OtherSkills(const SkillName skillName, const int& skillDurabilityCost) {
	switch (skillName) {
	case SkillName::DELICATESYNTHESIS:
		craftableItem.AddQuality(CalculateQuality(SkillName::BASICTOUCH), 0);
		AddInnerQuiet(1);
		craftableItem.AddProgress(CalculateProgress(150), skillDurabilityCost);
		break;
	default:
		break;
	}
}


/* PRIVATE */

const int Player::CalculateProgress(const int16_t efficiency) {
	int result = progressPerOne * efficiency;
	SynthesisBuffs(result);
	return result;
}

const int Player::CalculateQuality(SkillName skillName) {
	if (playerState.buffInfo.innovationActive) {
		if (playerState.buffInfo.greatStridesActive) {
			playerState.buffInfo.greatStrides = 0;
			playerState.buffInfo.greatStridesActive = false;
			return preComputeQualityTouchStrideEfficiency[playerState.innerQuiet][skillName];
		}
		return preComputeQualityTouchEfficiency[playerState.innerQuiet][skillName];
	}
	if (playerState.buffInfo.greatStridesActive) {
		playerState.buffInfo.greatStrides = 0;
		playerState.buffInfo.greatStridesActive = false;
		return preComputeQualityStrideEfficiency[playerState.innerQuiet][skillName];
	}
	return preComputeQualityEfficiency[playerState.innerQuiet][skillName];
}


void Player::AddInnerQuiet(unsigned char stacks) {
	playerState.innerQuiet += stacks;
	if (playerState.innerQuiet > 10) {
		playerState.innerQuiet = 10;
	}
}

inline const float Player::InnerQuietEfficiencyMultiplier() const {
	return (1 + (playerState.innerQuiet / 10.0f));
}


/* BUFFS */
void Player::SynthesisBuffs(int& skillEfficiency) {
	int baseSkillEfficiency = skillEfficiency;
	if (playerState.buffInfo.muscleMemory) {
		skillEfficiency += baseSkillEfficiency;
		playerState.buffInfo.muscleMemory = 0;
		playerState.buffInfo.muscleMemoryActive = false;
	}
	if (playerState.buffInfo.venerationActive) {
		skillEfficiency += baseSkillEfficiency / 2;
	}
}

void Player::TouchBuffs(int& skillEfficiency) {
	int baseSkillEfficiency = skillEfficiency;
	if (playerState.buffInfo.innovationActive) {
		skillEfficiency += baseSkillEfficiency / 2;
	}
	if (playerState.buffInfo.greatStridesActive) {
		skillEfficiency += baseSkillEfficiency;
		playerState.buffInfo.greatStrides = 0;
		playerState.buffInfo.greatStridesActive = false;
	}
}

void Player::DecrementBuffs() {	
	playerState.buffInfo.muscleMemoryActive = (playerState.buffInfo.muscleMemory -= playerState.buffInfo.muscleMemoryActive);
	playerState.buffInfo.wasteNotActive = (playerState.buffInfo.wasteNot -= playerState.buffInfo.wasteNotActive);
	playerState.buffInfo.greatStridesActive = (playerState.buffInfo.greatStrides -= playerState.buffInfo.greatStridesActive);
	playerState.buffInfo.innovationActive = (playerState.buffInfo.innovation -= playerState.buffInfo.innovationActive);
	playerState.buffInfo.venerationActive = (playerState.buffInfo.veneration -= playerState.buffInfo.venerationActive);
	playerState.buffInfo.finalAppraisalActive = (playerState.buffInfo.finalAppraisal -= playerState.buffInfo.finalAppraisalActive);
	playerState.buffInfo.manipulationActive = (playerState.buffInfo.manipulation -= playerState.buffInfo.manipulationActive);
}


inline void Player::PreComputeQualityEfficiency() {
	const int basic = SkillName::BASICTOUCH, standard = SkillName::STANDARDTOUCH, advanced = SkillName::ADVANCEDTOUCH,
		prep = SkillName::PREPARATORYTOUCH, reflect = SkillName::REFLECT, byregot = SkillName::BYREGOTSBLESSING;
	for (int i{ 0 }; i < 11; i++) {
		/* Basic, Prudent, refined, delicate */
		preComputeQualityEfficiency[i][basic] = 100 * qualityPerOne * InnerQuietEfficiencyMultiplier();
		preComputeQualityTouchEfficiency[i][basic] = preComputeQualityEfficiency[i][basic] + preComputeQualityEfficiency[i][basic] / 2;
		preComputeQualityStrideEfficiency[i][basic] = preComputeQualityEfficiency[i][basic] * 2;
		preComputeQualityTouchStrideEfficiency[i][basic] = preComputeQualityEfficiency[i][basic] + preComputeQualityTouchEfficiency[i][basic];

		/* Standard */
		preComputeQualityEfficiency[i][standard] = 125 * qualityPerOne * InnerQuietEfficiencyMultiplier();
		preComputeQualityTouchEfficiency[i][standard] = preComputeQualityEfficiency[i][standard] + preComputeQualityEfficiency[i][standard] / 2;
		preComputeQualityStrideEfficiency[i][standard] = preComputeQualityEfficiency[i][standard] * 2;
		preComputeQualityTouchStrideEfficiency[i][standard] = preComputeQualityEfficiency[i][standard] + preComputeQualityTouchEfficiency[i][standard];
		
		/* Advanced */
		preComputeQualityEfficiency[i][advanced] = 150 * qualityPerOne * InnerQuietEfficiencyMultiplier();
		preComputeQualityTouchEfficiency[i][advanced] = preComputeQualityEfficiency[i][advanced] + preComputeQualityEfficiency[i][advanced] / 2;
		preComputeQualityStrideEfficiency[i][advanced] = preComputeQualityEfficiency[i][advanced] * 2;
		preComputeQualityTouchStrideEfficiency[i][advanced] = preComputeQualityEfficiency[i][advanced] + preComputeQualityTouchEfficiency[i][advanced];
		
		/* Preparatory */
		preComputeQualityEfficiency[i][prep] = 200 * qualityPerOne * InnerQuietEfficiencyMultiplier();
		preComputeQualityTouchEfficiency[i][prep] = preComputeQualityEfficiency[i][prep] + preComputeQualityEfficiency[i][prep] / 2;
		preComputeQualityStrideEfficiency[i][prep] = preComputeQualityEfficiency[i][prep] * 2;
		preComputeQualityTouchStrideEfficiency[i][prep] = preComputeQualityEfficiency[i][prep] + preComputeQualityTouchEfficiency[i][prep];
		
		/* Reflect */
		preComputeQualityEfficiency[i][reflect] = 300 * qualityPerOne * InnerQuietEfficiencyMultiplier();
		preComputeQualityTouchEfficiency[i][reflect] = preComputeQualityEfficiency[i][reflect] + preComputeQualityEfficiency[i][reflect] / 2;
		preComputeQualityStrideEfficiency[i][reflect] = preComputeQualityEfficiency[i][reflect] * 2;
		preComputeQualityTouchStrideEfficiency[i][reflect] = preComputeQualityEfficiency[i][reflect] + preComputeQualityTouchEfficiency[i][reflect];
		
		/* Byregot */
		int efficiency = 100 + (20 * i);
		preComputeQualityEfficiency[i][byregot] = efficiency * qualityPerOne * InnerQuietEfficiencyMultiplier();
		preComputeQualityTouchEfficiency[i][byregot] = preComputeQualityEfficiency[i][byregot] + preComputeQualityEfficiency[i][byregot] / 2;
		preComputeQualityStrideEfficiency[i][byregot] = preComputeQualityEfficiency[i][byregot] * 2;
		preComputeQualityTouchStrideEfficiency[i][byregot] = preComputeQualityEfficiency[i][byregot] + preComputeQualityTouchEfficiency[i][byregot];

		AddInnerQuiet(1);
	}
}