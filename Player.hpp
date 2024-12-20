#pragma once

//@TODO create synth table lookup. May be more expensive to do so as there are generally fewer calculations with it

#include "Item.hpp"
#include "Skills.hpp"

#include <unordered_map>

using namespace Skills;

class Player {

public:
	Player(int maximumCP, float progressPerHundred, float qualityPerHundred);

	void AddItem(int maxProgress, int maxQuality, int maxDurability);
	void RemoveItem();

	bool CastSkill(Skills::SkillTest& skillName);

	int GetSkillTime(Skills::SkillName skillName) const;
	int GetCurrentTime() const {
		return playerState.currentTime;
	}
	int GetCurrentTurn() const {
		return (playerState.currentTurn + 1);
	}

	Item* craftableItem = nullptr;

	int GetBuffDuration(SkillName skillName) const {
		switch (skillName) {
		case SkillName::MUSCLEMEMORY:
			return playerState.muscleMemory;
		case SkillName::WASTENOTI:
		case SkillName::WASTENOTII:
			return playerState.wasteNot;
		case SkillName::GREATSTRIDES:
			return playerState.greatStrides;
		case SkillName::INNOVATION:
			return playerState.innovation;
		case SkillName::VENERATION:
			return playerState.veneration;
		case SkillName::FINALAPPRAISAL:
			return playerState.finalAppraisal;
		case SkillName::MANIPULATION:
			return playerState.manipulation;
		default:
			std::cout << "This is not a buff\n";
			std::cout << Skills::GetSkillName(skillName) << '\n';
			break;
		}
	
		return 0;
	}

	struct PlayerState {
		int currentCP{ 0 };
		int currentTurn{ 0 };
		int currentTime{ 0 };
		int innerQuiet{ 0 };
		int muscleMemory = 0;
		int wasteNot = 0;
		int greatStrides = 0;
		int innovation = 0;
		int veneration= 0;
		int finalAppraisal = 0;
		int manipulation = 0;
	}playerState;

	void LoadPlayerStats(PlayerState&);

	PlayerState GetPlayerState() const {
		return playerState;
	}

private:
	int maxCP{};
	const float progressPerOne{}, qualityPerOne{};
	bool successfulCast{ true };
	static const int maxInnerQuiet{ 10 };
	int preComputeQualityEfficiency[maxInnerQuiet + 1][300]{}, preComputeQualityTouchEfficiency[maxInnerQuiet + 1][300]{},
		preComputeQualityStrideEfficiency[maxInnerQuiet + 1][300]{}, preComputeQualityTouchStrideEfficiency[maxInnerQuiet + 1][300]{};
	
	Skills::SkillName lastSkillUsed{ Skills::SkillName::NONE };

	

	void ResetPlayerStats();
	
	bool CheckItem();

	int CalculateProgress(int efficiency);
	int CalculateQuality(int efficiency);

	void AddInnerQuiet(int stacks);
	inline float InnerQuietEfficiencyMultiplier();


	void SynthesisSkills(SkillName skillName, int& skillDurabilityCost, int& skillEfficiency);
	void TouchSkills(SkillName skillName, int& skillDurabilityCost, int& skillEfficiency, int& skillCPCost);
	void BuffSkills(SkillName skillName);
	void RepairSkills(SkillName skillName);
	void OtherSkills(SkillName skillName, int& skillDurabilityCost);
	void SynthesisBuffs(int& skillEfficiency);
	void TouchBuffs(int& skillEfficiency);
	void DecrementBuffs();

	const std::unordered_map<Skills::SkillName, Skill> SkillList =/*CP	Durability	Efficiency	Time*/
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