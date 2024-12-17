#pragma once

#include "Item.hpp"
#include "Skills.hpp"

#include <map>

using namespace Skills;

class Player {

public:
	Player(int maximumCP);

	void AddItem(int maxProgress, int maxQuality, int maxDurability);
	void RemoveItem();

	bool CastSkill(Skills::SkillName skillName);

	int GetSkillTime(Skills::SkillName skillName);
	int GetCurrentTurn() {
		return (currentTurn + 1);
	}

	Item* craftableItem = nullptr;

	SkillName fullSkillList[23] = {
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

	SkillName finalMoveList[4] = {
		SkillName::BASICSYNTHESIS,
		SkillName::CAREFULSYNTHESIS,
		SkillName::PRUDENTSYNTHESIS,
		SkillName::GROUNDWORK,
	};

	// All skills focused on touch regardless of category
	SkillName qualityList[10] = {
		SkillName::BASICTOUCH,
		SkillName::STANDARDTOUCH,
		SkillName::ADVANCEDTOUCH,
		SkillName::BYREGOTSBLESSING,
		SkillName::PRUDENTTOUCH,
		SkillName::PREPARATORYTOUCH,
		SkillName::REFLECT,
		SkillName::REFINEDTOUCH,
		SkillName::GREATSTRIDES,
		SkillName::INNOVATION
	};

	SkillName buffList[7] = {
		SkillName::WASTENOTI,
		SkillName::WASTENOTII,
		SkillName::VENERATION,
		SkillName::FINALAPPRAISAL,
		SkillName::MASTERSMEND,
		SkillName::MANIPULATION,
		SkillName::IMMACULATEMEND
	};

	int GetBuffDuration(SkillName skillName) {
		return  buffs[skillName];
	}

private:
	int maxCP{}, currentCP{};
	int currentCraftTime{ 0 };
	int currentTurn{ 0 };
	int innerQuiet{ 0 };
	bool successfulCast{ true };
	// Buffs
	std::map<SkillName, int> buffs = {
		{SkillName::MUSCLEMEMORY, 0},
		{SkillName::WASTENOTI, 0},
		{SkillName::GREATSTRIDES, 0},
		{SkillName::INNOVATION, 0},
		{SkillName::VENERATION, 0},
		{SkillName::FINALAPPRAISAL, 0},
		{SkillName::MANIPULATION, 0}
	};
	

	Skills::SkillName lastSkillUsed{ Skills::SkillName::NONE };

	

	void ResetPlayerStats();
	bool CheckItem();

	int CalculateProgress(int efficiency);
	int CalculateQuality(int efficiency);

	void AddInnerQuiet(int stacks);
	float InnerQuietEfficiencyMultiplier();


	void SynthesisSkills(SkillName skillName, int& skillDurabilityCost, int& skillEfficiency);
	void TouchSkills(SkillName skillName, int& skillDurabilityCost, int& skillEfficiency, int& skillCPCost);
	void BuffSkills(SkillName skillName);
	void RepairSkills(SkillName skillName);
	void OtherSkills(SkillName skillName, int& skillDurabilityCost);
	void SynthesisBuffs(int& skillEfficiency);
	void TouchBuffs(int& skillEfficiency);
	void DecrementBuffs();

	std::map<Skills::SkillName, Skill> SkillList =				/* CP	Durability	Efficiency	Time*/
	{ {	SkillName::BASICSYNTHESIS,		{ SkillType::SYNTHESIS,		0,		10,		120,		3}},
	{	SkillName::CAREFULSYNTHESIS,	{ SkillType::SYNTHESIS,		7,		10,		180,		3 }},
	{	SkillName::PRUDENTSYNTHESIS,	{ SkillType::SYNTHESIS,		18,		5,		180,		3 }},
	{	SkillName::GROUNDWORK,			{ SkillType::SYNTHESIS,		18,		20,		360,		3 }},
	{	SkillName::MUSCLEMEMORY,		{ SkillType::SYNTHESIS,		6,		10,		300,		3} },
	{	SkillName::BASICTOUCH,			{ SkillType::TOUCH,			18,		10,		100,		3 }},
	{	SkillName::STANDARDTOUCH,		{ SkillType::TOUCH,			32,		10,		125,		3 }},
	{	SkillName::ADVANCEDTOUCH,		{ SkillType::TOUCH,			46,		10,		150,		3 }},
	{	SkillName::BYREGOTSBLESSING,	{ SkillType::TOUCH,			24,		10,		100,		3 }},
	{	SkillName::PRUDENTTOUCH,		{ SkillType::TOUCH,			25,		5,		100,		3 }},
	{	SkillName::PREPARATORYTOUCH,	{ SkillType::TOUCH,			40,		20,		200,		3 }},
	{	SkillName::REFLECT,				{ SkillType::TOUCH,			6,		10,		300,		3 }},
	{	SkillName::REFINEDTOUCH,		{ SkillType::TOUCH,			24,		10,		100,		3 } },
	{	SkillName::WASTENOTI,			{ SkillType::BUFF,			56,		0,		0,			2 }},
	{	SkillName::WASTENOTII,			{ SkillType::BUFF,			98,		0,		0,			2 }},
	{	SkillName::GREATSTRIDES,		{ SkillType::BUFF,			32,		0,		0,			2 }},
	{	SkillName::INNOVATION,			{ SkillType::BUFF,			18,		0,		0,			2 }},
	{	SkillName::VENERATION,			{ SkillType::BUFF,			18,		0,		0,			2 }},
	{	SkillName::FINALAPPRAISAL,		{ SkillType::BUFF,			1,		0,		0,			2 }},
	{	SkillName::MASTERSMEND,			{ SkillType::REPAIR,		88,		0,		0,			3 }},
	{	SkillName::MANIPULATION,		{ SkillType::REPAIR,		96,		0,		0,			2 }}, 
	{	SkillName::IMMACULATEMEND,		{ SkillType::REPAIR,		112,	0,		0,			3 }},
	{	SkillName::DELICATESYNTHESIS,	{ SkillType::OTHER,			32,		10,		100,		3 }} };
};