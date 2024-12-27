#pragma once

//@TODO create synth table lookup. May be more expensive to do so as there are generally fewer calculations with it

#include "Item.hpp"
#include "Skills.hpp"

#include <array>
#include <unordered_map>

using namespace Skills;

class Player {

public:
	Player(int maximumCP, float progressPerHundred, float qualityPerHundred);

	inline void PreComputeQualityEfficiency();
	void AddItem(const int& maxProgress, const int& maxQuality, const int& maxDurability);
	void RemoveItem();

	bool CastSkill(const Skills::SkillTest& skillName);

	inline const int& GetSkillTime(Skills::SkillName skillName) const;
	inline const unsigned char& GetCurrentTime() const {
		return playerState.currentTime;
	}
	inline const unsigned char& GetCurrentTurn() const {
		return playerState.currentTurn;
	}

	std::unique_ptr<Item> craftableItem = nullptr;

	inline const int& GetBuffDuration(SkillName skillName) const {
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

	void LoadPlayerStats(const PlayerState&);

	const PlayerState& GetPlayerState() const {
		return playerState;
	}

	void ResetPlayerStats();

	static constexpr unsigned char maxInnerQuiet{ 10 };
	std::array<std::array<int16_t, 300 + 1>, maxInnerQuiet + 1> preComputeQualityEfficiency, preComputeQualityTouchEfficiency,
		preComputeQualityStrideEfficiency, preComputeQualityTouchStrideEfficiency;
private:
	const int maxCP{};
	const float progressPerOne{}, qualityPerOne{};
	bool successfulCast{ true };
	
	

	

	
	
	bool CheckItem();

	const int CalculateProgress(const int16_t efficiency);
	const int CalculateQuality(const int16_t efficiency);

	void AddInnerQuiet(unsigned char stacks);
	inline const float InnerQuietEfficiencyMultiplier() const;


	void SynthesisSkills(const SkillName skillName, const int& skillDurabilityCost, int& skillEfficiency);
	void TouchSkills(const SkillName skillName, const int skillDurabilityCost, const int skillEfficiency, int& skillCPCost);
	void BuffSkills(const SkillName skillName);
	void RepairSkills(const SkillName skillName);
	void OtherSkills(const SkillName skillName, const int& skillDurabilityCost);
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