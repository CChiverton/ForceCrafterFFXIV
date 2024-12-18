#include "Player.hpp"

#include <iostream>

/* PUBLIC */
Player::Player(int maximumCP) {
	maxCP = maximumCP;
	ResetPlayerStats();
	//AddItem(3000, 11000, 40);
}

void Player::AddItem(int maxProgress, int maxQuality, int maxDurability) {
	RemoveItem();
	ResetPlayerStats();
	craftableItem = new Item(maxProgress, maxQuality, maxDurability);
}

void Player::RemoveItem() {
	if (craftableItem != nullptr)	delete craftableItem;
	craftableItem = nullptr;
}

bool Player::CastSkill(Skills::SkillName skillName) {
	Skill skill = SkillList.at(skillName);
	if (skill.costCP > playerState.currentCP) {
		//std::cout << "Not enough CP\n";
		successfulCast = false;
		return false;
	}
	successfulCast = true;

	int skillCPCost = skill.costCP;
	int skillDurabilityCost = skill.costDurability;
	int skillEfficiency = skill.efficiency;
	if (playerState.buffs[SkillName::WASTENOTI] > 0) {
		skillDurabilityCost /= 2;
	}
	switch (skill.type) {
	case SkillType::SYNTHESIS:
		//SynthesisBuffs(skillEfficiency);
		SynthesisSkills(skillName, skillDurabilityCost, skillEfficiency);
		break;
	case SkillType::TOUCH:
		//TouchBuffs(skillEfficiency);
		TouchSkills(skillName, skillDurabilityCost, skillEfficiency, skillCPCost);
		break;
	case SkillType::BUFF:
		BuffSkills(skillName);
		break;
	case SkillType::REPAIR:
		RepairSkills(skillName);
		break;
	case SkillType::OTHER:
		OtherSkills(skillName, skillDurabilityCost);
		break;
	default:
		std::cout << "A serious error has occured\n";
	}

	if (successfulCast) {
		if (playerState.buffs[SkillName::FINALAPPRAISAL] > 0 && craftableItem->IsItemCrafted()) {
			craftableItem->AddProgress(-(craftableItem->GetCurrentProgress() - craftableItem->GetMaxProgress() + 1), 0);
		}

		if (playerState.buffs[SkillName::MANIPULATION] < 9 && playerState.buffs[SkillName::MANIPULATION] > 0) {
			craftableItem->UpdateDurability(5);
		}
		//std::cout << craftableItem->GetDurability() << '\n';
		lastSkillUsed = skillName;
		playerState.currentCP -= skillCPCost;
		playerState.currentTurn++;
		DecrementBuffs();
	}

	return successfulCast;
	//CheckItem();
}

int Player::GetSkillTime(SkillName skillName) {
	return SkillList[skillName].castTime;
}


/* PRIVATE */
void Player::ResetPlayerStats() {
	playerState.currentCP = maxCP;
	playerState.innerQuiet = 0;
	playerState.currentTurn = 0;
	lastSkillUsed = SkillName::NONE;
	successfulCast = true; // Only turned false on failure. True by default
	playerState.buffs[SkillName::MUSCLEMEMORY] = 0;
	playerState.buffs[SkillName::WASTENOTI] = 0;
	playerState.buffs[SkillName::WASTENOTII] = 0;
	playerState.buffs[SkillName::GREATSTRIDES] = 0;
	playerState.buffs[SkillName::INNOVATION] = 0;
	playerState.buffs[SkillName::VENERATION] = 0;
	playerState.buffs[SkillName::FINALAPPRAISAL] = 0;
	playerState.buffs[SkillName::MANIPULATION] = 0;
}

void Player::LoadPlayerStats(PlayerState& state) {
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

int Player::CalculateProgress(int efficiency) {
	const float perOneHundred = 331;
	int result = perOneHundred * efficiency / 100;
	SynthesisBuffs(result);
	return result;
}

int Player::CalculateQuality(int efficiency) {
	const float perOneHundred = 397;
	int result = perOneHundred * efficiency / 100 * InnerQuietEfficiencyMultiplier();
	//std::cout << "Quality addition is " << result << '\n';
	TouchBuffs(result);
	return result;
}

void Player::AddInnerQuiet(int stacks) {
	playerState.innerQuiet += stacks;
	if (stacks > 10) {
		stacks = 10;
	}
}

float Player::InnerQuietEfficiencyMultiplier() {
	//std::cout << "Inner quiet multiplier " << (1 + (playerState.innerQuiet / 10.0f)) << '\n';
	return (1 + (playerState.innerQuiet / 10.0f));
}

void Player::SynthesisSkills(SkillName skillName, int& skillDurabilityCost, int& skillEfficiency) {
	switch (skillName) {
	case Skills::SkillName::PRUDENTSYNTHESIS:
		if (playerState.buffs[SkillName::WASTENOTI] == 0) {
			craftableItem->AddProgress(CalculateProgress(skillEfficiency), skillDurabilityCost);
		}
		else {
			successfulCast = false;
		}
		break;
	case Skills::SkillName::GROUNDWORK:
		if (craftableItem->GetDurability() < skillDurabilityCost) {
			skillEfficiency /= 2;
		}
		craftableItem->AddProgress(CalculateProgress(skillEfficiency), skillDurabilityCost);
		break;
	case Skills::SkillName::MUSCLEMEMORY:
		if (playerState.currentTurn == 0) {
			craftableItem->AddProgress(CalculateProgress(skillEfficiency), skillDurabilityCost);
			playerState.buffs[SkillName::MUSCLEMEMORY] = 6;
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
void Player::TouchSkills(SkillName skillName, int& skillDurabilityCost, int& skillEfficiency, int& skillCPCost) {
	//std::cout << "Touch skill efficiency is " << skillEfficiency << '\n';
	
	//std::cout << "After inner quiet is " << skillEfficiency << '\n';
	switch (skillName) {
	case Skills::SkillName::BASICTOUCH:
		craftableItem->AddQuality(CalculateQuality(skillEfficiency), skillDurabilityCost);
		AddInnerQuiet(1);
		break;
	case Skills::SkillName::STANDARDTOUCH:
		if (lastSkillUsed == SkillName::BASICTOUCH) {
			skillCPCost = 18;
		}
		craftableItem->AddQuality(CalculateQuality(skillEfficiency), skillDurabilityCost);
		AddInnerQuiet(1);
		break;
	case Skills::SkillName::ADVANCEDTOUCH:
		if (lastSkillUsed == SkillName::STANDARDTOUCH) {
			skillCPCost = 18;
		}
		craftableItem->AddQuality(CalculateQuality(skillEfficiency), skillDurabilityCost);
		AddInnerQuiet(1);
		break;
	case Skills::SkillName::BYREGOTSBLESSING:
		if (playerState.innerQuiet > 0) {
			skillEfficiency = 100 + (20 * playerState.innerQuiet);
			craftableItem->AddQuality(CalculateQuality(skillEfficiency), skillDurabilityCost);
			playerState.innerQuiet = 0;
		}
		else {
			successfulCast = false;
		}
		break;
	case Skills::SkillName::PRUDENTTOUCH:
		if (playerState.buffs[SkillName::WASTENOTI] == 0) {
			craftableItem->AddQuality(CalculateQuality(skillEfficiency), skillDurabilityCost);
		}
		else {
			successfulCast = false;
		}
		break;
	case Skills::SkillName::PREPARATORYTOUCH:
		if (craftableItem->GetDurability() < skillDurabilityCost) {
			skillEfficiency /= 2;
		}
		craftableItem->AddQuality(CalculateQuality(skillEfficiency), skillDurabilityCost);
		AddInnerQuiet(2);
		break;
	case Skills::SkillName::REFLECT:
		if (playerState.currentTurn == 0) {
			craftableItem->AddQuality(CalculateQuality(skillEfficiency), skillDurabilityCost);
			AddInnerQuiet(2);
		}
		else {
			successfulCast = false;
		}
		break;
	case Skills::SkillName::REFINEDTOUCH:
		craftableItem->AddQuality(CalculateQuality(skillEfficiency), skillDurabilityCost);
		if (lastSkillUsed == Skills::SkillName::BASICTOUCH) {
			AddInnerQuiet(1);
		}
		AddInnerQuiet(1);
		break;
	default:
		break;
	}
}

// buffs are 1 turn higher than they should be due to immediately losing a turn to buffs upon cast
void Player::BuffSkills(SkillName skillName) {
	switch (skillName) {
	case SkillName::WASTENOTI:
		playerState.buffs[SkillName::WASTENOTI] = 5;
		break;
	case SkillName::WASTENOTII:
		playerState.buffs[SkillName::WASTENOTI] = 9;
		break;
	case SkillName::GREATSTRIDES:
		playerState.buffs[SkillName::GREATSTRIDES] = 4;
		break;
	case SkillName::INNOVATION:
		playerState.buffs[SkillName::INNOVATION] = 5;
		break;
	case SkillName::VENERATION:
		playerState.buffs[SkillName::VENERATION] = 5;
		break;
	case SkillName::FINALAPPRAISAL:
		playerState.buffs[SkillName::FINALAPPRAISAL] = 6;
		break;
	default:
		break;
	}
}

void Player::RepairSkills(SkillName skillName) {
	switch (skillName) {
	case SkillName::MASTERSMEND:
		craftableItem->UpdateDurability(30);
		break;
	case SkillName::MANIPULATION:
		playerState.buffs[SkillName::MANIPULATION] = 9;
		break;
	case SkillName::IMMACULATEMEND:
		craftableItem->UpdateDurability(1000);
		break;
	default:
		break;
	}
}

void Player::OtherSkills(SkillName skillName, int& skillDurabilityCost) {
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
	if (playerState.buffs[SkillName::MUSCLEMEMORY]) {
		skillEfficiency += baseSkillEfficiency;
		playerState.buffs[SkillName::MUSCLEMEMORY] = 0;
	}
	if (playerState.buffs[SkillName::VENERATION] > 0) {
		skillEfficiency += baseSkillEfficiency / 2;
	}
}

void Player::TouchBuffs(int& skillEfficiency) {
	int baseSkillEfficiency = skillEfficiency;
	if (playerState.buffs[SkillName::INNOVATION] > 0) {
		skillEfficiency += baseSkillEfficiency / 2;
	}
	if (playerState.buffs[SkillName::GREATSTRIDES] > 0) {
		skillEfficiency += baseSkillEfficiency;
		playerState.buffs[SkillName::GREATSTRIDES] = 0;
	}
}

void Player::DecrementBuffs() {
	if (playerState.buffs[SkillName::MUSCLEMEMORY] > 0)	playerState.buffs[SkillName::MUSCLEMEMORY]--;
	if (playerState.buffs[SkillName::WASTENOTI] > 0)		playerState.buffs[SkillName::WASTENOTI]--;
	if (playerState.buffs[SkillName::GREATSTRIDES] > 0)	playerState.buffs[SkillName::GREATSTRIDES]--;
	if (playerState.buffs[SkillName::INNOVATION] > 0)		playerState.buffs[SkillName::INNOVATION]--;
	if (playerState.buffs[SkillName::VENERATION] > 0)		playerState.buffs[SkillName::VENERATION]--;
	if (playerState.buffs[SkillName::FINALAPPRAISAL] > 0)	playerState.buffs[SkillName::FINALAPPRAISAL]--;
	if (playerState.buffs[SkillName::MANIPULATION] > 0)	playerState.buffs[SkillName::MANIPULATION]--;
}