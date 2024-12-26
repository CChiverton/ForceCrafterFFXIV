#include "Player.hpp"

#include <iostream>

/* PUBLIC */
Player::Player(int maximumCP, float progressPerHundred, float qualityPerHundred) : 
	progressPerOne(progressPerHundred/100.0f), qualityPerOne(qualityPerHundred/100.0f), maxCP(maximumCP) {
	ResetPlayerStats();
	
	PreComputeQualityEfficiency();
	ResetPlayerStats();
}

inline void Player::PreComputeQualityEfficiency() {
	const int basic = 100, standard = 125, advanced = 150, prep = 200, reflect = 300;
	for (int i{ 0 }; i < 11; i++) {
		/* Basic, Prudent, refined, delicate */
		preComputeQualityEfficiency[i][basic] = basic * qualityPerOne * InnerQuietEfficiencyMultiplier();
		preComputeQualityTouchEfficiency[i][basic] = preComputeQualityEfficiency[i][basic] + preComputeQualityEfficiency[i][basic] / 2;
		preComputeQualityStrideEfficiency[i][basic] = preComputeQualityEfficiency[i][basic] * 2;
		preComputeQualityTouchStrideEfficiency[i][basic] = preComputeQualityEfficiency[i][basic] + preComputeQualityTouchEfficiency[i][basic];
		
		/* Standard */
		preComputeQualityEfficiency[i][standard] = standard * qualityPerOne * InnerQuietEfficiencyMultiplier();
		preComputeQualityTouchEfficiency[i][standard] = preComputeQualityEfficiency[i][standard] + preComputeQualityEfficiency[i][standard] / 2;
		preComputeQualityStrideEfficiency[i][standard] = preComputeQualityEfficiency[i][standard] * 2;
		preComputeQualityTouchStrideEfficiency[i][standard] = preComputeQualityEfficiency[i][standard] + preComputeQualityTouchEfficiency[i][standard];
		/* Advanced */
		preComputeQualityEfficiency[i][advanced] = advanced * qualityPerOne * InnerQuietEfficiencyMultiplier();
		preComputeQualityTouchEfficiency[i][advanced] = preComputeQualityEfficiency[i][advanced] + preComputeQualityEfficiency[i][advanced] / 2;
		preComputeQualityStrideEfficiency[i][advanced] = preComputeQualityEfficiency[i][advanced] * 2;
		preComputeQualityTouchStrideEfficiency[i][advanced] = preComputeQualityEfficiency[i][advanced] + preComputeQualityTouchEfficiency[i][advanced];
		/* Preparatory */
		preComputeQualityEfficiency[i][prep] = prep * qualityPerOne * InnerQuietEfficiencyMultiplier();
		preComputeQualityTouchEfficiency[i][prep] = preComputeQualityEfficiency[i][prep] + preComputeQualityEfficiency[i][prep] / 2;
		preComputeQualityStrideEfficiency[i][prep] = preComputeQualityEfficiency[i][prep] * 2;
		preComputeQualityTouchStrideEfficiency[i][prep] = preComputeQualityEfficiency[i][prep] + preComputeQualityTouchEfficiency[i][prep];
		/* Reflect */
		preComputeQualityEfficiency[i][reflect] = reflect * qualityPerOne * InnerQuietEfficiencyMultiplier();
		preComputeQualityTouchEfficiency[i][reflect] = preComputeQualityEfficiency[i][reflect] + preComputeQualityEfficiency[i][reflect] / 2;
		preComputeQualityStrideEfficiency[i][reflect] = preComputeQualityEfficiency[i][reflect] * 2;
		preComputeQualityTouchStrideEfficiency[i][reflect] = preComputeQualityEfficiency[i][reflect] + preComputeQualityTouchEfficiency[i][reflect];
		/* Byregot */
		int efficiency = 100 + (20 * i);
		preComputeQualityEfficiency[i][efficiency] = efficiency * qualityPerOne * InnerQuietEfficiencyMultiplier();
		preComputeQualityTouchEfficiency[i][efficiency] = preComputeQualityEfficiency[i][efficiency] + preComputeQualityEfficiency[i][efficiency] / 2;
		preComputeQualityStrideEfficiency[i][efficiency] = preComputeQualityEfficiency[i][efficiency] * 2;
		preComputeQualityTouchStrideEfficiency[i][efficiency] = preComputeQualityEfficiency[i][efficiency] + preComputeQualityTouchEfficiency[i][efficiency];

		AddInnerQuiet(1);
	}
}

void Player::AddItem(const int& maxProgress, const int& maxQuality, const int& maxDurability) {
	RemoveItem();
	ResetPlayerStats();
	craftableItem.reset(new Item(maxProgress, maxQuality, maxDurability));
}

void Player::RemoveItem() {
	if (craftableItem != nullptr)	craftableItem.reset(nullptr);
	craftableItem = nullptr;
}

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
		//SynthesisBuffs(skillEfficiency);
		SynthesisSkills(skill.skillName, skillDurabilityCost, skillEfficiency);
		break;
	case SkillType::TOUCH:
		//TouchBuffs(skillEfficiency);
		TouchSkills(skill.skillName, skillDurabilityCost, skillEfficiency, skillCPCost);
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
	}

	if (successfulCast) {
		if (playerState.buffInfo.finalAppraisalActive && craftableItem->IsItemCrafted()) {
			craftableItem->AddProgress(-(craftableItem->GetCurrentProgress() - craftableItem->GetMaxProgress() + 1), 0);
		}

		if (playerState.buffInfo.manipulation < 9 && playerState.buffInfo.manipulationActive) {
			craftableItem->UpdateDurability(5);
		}
		//std::cout << craftableItem->GetDurability() << '\n';
		playerState.lastSkillUsed = skill.skillName;
		playerState.currentCP -= skillCPCost;
		++playerState.currentTurn;
		playerState.currentTime += skill.castTime;
		if (skill.skillName != SkillName::FINALAPPRAISAL) {
			DecrementBuffs();
		}
	}

	return successfulCast;
	//CheckItem();
}

const int& Player::GetSkillTime(SkillName skillName) const {
	return SkillList.at(skillName).castTime;
}


/* PRIVATE */
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

bool Player::CheckItem() {
	//std::cout << "Checking item\n";
	if (!craftableItem->IsItemWorkable()) {
		RemoveItem();
		if (craftableItem == nullptr) {
			std::cout << "Item has been deleted\n";
		}
		return false;
	}
	return true;
}

const int Player::CalculateProgress(const int16_t efficiency) {
	int result = progressPerOne * efficiency;
	/*std::cout << "Progress per one is " << progressPerOne << '\n';
	std::cout << "Efficiency is " << efficiency << '\n';
	std::cout << "Progress is " << result << '\n';*/
	SynthesisBuffs(result);
	return result;
}

const int& Player::CalculateQuality(const int16_t efficiency) {
	if (playerState.buffInfo.innovationActive) {
		if (playerState.buffInfo.greatStridesActive) {
			playerState.buffInfo.greatStrides = 0;
			playerState.buffInfo.greatStridesActive = false;
			//std::cout << preComputeQualityTouchStrideEfficiency[playerState.innerQuiet][efficiency] << "\n\n";
			return preComputeQualityTouchStrideEfficiency[playerState.innerQuiet][efficiency];
		}
		return preComputeQualityTouchEfficiency[playerState.innerQuiet][efficiency];
	}
	if (playerState.buffInfo.greatStridesActive) {
		playerState.buffInfo.greatStrides = 0;
		playerState.buffInfo.greatStridesActive = false;
		return preComputeQualityStrideEfficiency[playerState.innerQuiet][efficiency];
	}
	return preComputeQualityEfficiency[playerState.innerQuiet][efficiency];
}

void Player::AddInnerQuiet(unsigned char stacks) {
	playerState.innerQuiet += stacks;
	if (playerState.innerQuiet > 10) {
		playerState.innerQuiet = 10;
	}
}

inline const float Player::InnerQuietEfficiencyMultiplier() const {
	//std::cout << "Inner quiet multiplier " << (1 + (playerState.innerQuiet / 10.0f)) << '\n';
	return (1 + (playerState.innerQuiet / 10.0f));
}

void Player::SynthesisSkills(const SkillName skillName, const int& skillDurabilityCost, int& skillEfficiency) {
	switch (skillName) {
	case Skills::SkillName::PRUDENTSYNTHESIS:
		if (!playerState.buffInfo.wasteNotActive) {
			craftableItem->AddProgress(CalculateProgress(skillEfficiency), skillDurabilityCost);
		}
		else {
			successfulCast = false;
		}
		break;
	case Skills::SkillName::GROUNDWORK:
		if (craftableItem->GetDurability() < skillDurabilityCost) {
			craftableItem->AddProgress(CalculateProgress(skillEfficiency/2), skillDurabilityCost);
		}
		else {
			craftableItem->AddProgress(CalculateProgress(skillEfficiency), skillDurabilityCost);
		}
		
		break;
	case Skills::SkillName::MUSCLEMEMORY:
		if (playerState.currentTurn == 1) {
			craftableItem->AddProgress(CalculateProgress(skillEfficiency), skillDurabilityCost);
			playerState.buffInfo.muscleMemory = 6;
			playerState.buffInfo.muscleMemoryActive = true;
		}
		else {
			successfulCast = false;
		}
		break;
	default:
		craftableItem->AddProgress(CalculateProgress(skillEfficiency), skillDurabilityCost);
		break;
	}
}

//@TODO Change the way cp cost is checked so combo works
void Player::TouchSkills(const SkillName skillName, const int& skillDurabilityCost, const int& skillEfficiency, int& skillCPCost) {
	//std::cout << "Touch skill efficiency is " << skillEfficiency << '\n';
	
	//std::cout << "After inner quiet is " << skillEfficiency << '\n';
	switch (skillName) {
	case Skills::SkillName::BASICTOUCH:
		craftableItem->AddQuality(CalculateQuality(skillEfficiency), skillDurabilityCost);
		AddInnerQuiet(1);
		break;
	case Skills::SkillName::STANDARDTOUCH:
		if (playerState.lastSkillUsed == SkillName::BASICTOUCH) {
			skillCPCost = 18;
		}
		craftableItem->AddQuality(CalculateQuality(skillEfficiency), skillDurabilityCost);
		AddInnerQuiet(1);
		break;
	case Skills::SkillName::ADVANCEDTOUCH:
		if (playerState.lastSkillUsed == SkillName::STANDARDTOUCH) {
			skillCPCost = 18;
		}
		craftableItem->AddQuality(CalculateQuality(skillEfficiency), skillDurabilityCost);
		AddInnerQuiet(1);
		break;
	case Skills::SkillName::BYREGOTSBLESSING:
		if (playerState.innerQuiet > 0) {
			craftableItem->AddQuality(CalculateQuality(100 + (20 * playerState.innerQuiet)), skillDurabilityCost);
			playerState.innerQuiet = 0;
		}
		else {
			successfulCast = false;
		}
		break;
	case Skills::SkillName::PRUDENTTOUCH:
		if (!playerState.buffInfo.wasteNotActive) {
			craftableItem->AddQuality(CalculateQuality(skillEfficiency), skillDurabilityCost);
		}
		else {
			successfulCast = false;
		}
		break;
	case Skills::SkillName::PREPARATORYTOUCH:
		craftableItem->AddQuality(CalculateQuality(skillEfficiency), skillDurabilityCost);
		AddInnerQuiet(2);
		break;
	case Skills::SkillName::REFLECT:
		if (playerState.currentTurn == 1) {
			craftableItem->AddQuality(CalculateQuality(skillEfficiency), skillDurabilityCost);
			AddInnerQuiet(2);
		}
		else {
			successfulCast = false;
		}
		break;
	case Skills::SkillName::REFINEDTOUCH:
		craftableItem->AddQuality(CalculateQuality(skillEfficiency), skillDurabilityCost);
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
		craftableItem->UpdateDurability(30);
		break;
	case SkillName::MANIPULATION:
		playerState.buffInfo.manipulation = 9;
		playerState.buffInfo.manipulationActive = true;
		break;
	case SkillName::IMMACULATEMEND:
		craftableItem->UpdateDurability(1000);
		break;
	default:
		break;
	}
}

void Player::OtherSkills(const SkillName skillName, const int& skillDurabilityCost) {
	switch (skillName) {
	case SkillName::DELICATESYNTHESIS:
		craftableItem->AddQuality(CalculateQuality(100), 0);
		AddInnerQuiet(1);
		craftableItem->AddProgress(CalculateProgress(150), skillDurabilityCost);
		break;
	}
}

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
	if (playerState.buffInfo.muscleMemoryActive) {
		playerState.buffInfo.muscleMemoryActive = --playerState.buffInfo.muscleMemory;
	}
	if (playerState.buffInfo.wasteNotActive) {
		playerState.buffInfo.wasteNotActive = --playerState.buffInfo.wasteNot;
	}
	if (playerState.buffInfo.greatStridesActive) {
		playerState.buffInfo.greatStridesActive = --playerState.buffInfo.greatStrides;
	}
	if (playerState.buffInfo.innovationActive) {
		playerState.buffInfo.innovationActive = --playerState.buffInfo.innovation;
	}
	if (playerState.buffInfo.venerationActive) {
		playerState.buffInfo.venerationActive = --playerState.buffInfo.veneration;
	}
	if (playerState.buffInfo.finalAppraisalActive) {
		playerState.buffInfo.finalAppraisalActive = --playerState.buffInfo.finalAppraisal;
	}
	if (playerState.buffInfo.manipulationActive) {
		playerState.buffInfo.manipulationActive = --playerState.buffInfo.manipulation;
	}
}