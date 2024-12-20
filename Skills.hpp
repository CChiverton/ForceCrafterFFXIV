#pragma once

namespace Skills {
	enum class SkillName
	{
		BASICSYNTHESIS = 0,
		CAREFULSYNTHESIS,
		PRUDENTSYNTHESIS,
		GROUNDWORK,
		MUSCLEMEMORY,

		BASICTOUCH,
		STANDARDTOUCH,
		ADVANCEDTOUCH,
		BYREGOTSBLESSING,
		PRUDENTTOUCH,
		PREPARATORYTOUCH,
		REFLECT,
		REFINEDTOUCH,

		WASTENOTI,
		WASTENOTII,
		GREATSTRIDES,
		INNOVATION,
		VENERATION,
		FINALAPPRAISAL,

		MASTERSMEND,
		MANIPULATION,
		IMMACULATEMEND,

		DELICATESYNTHESIS,

		NONE
	};

	extern inline std::string GetSkillName(SkillName skillName) {
		switch (skillName) {
		case SkillName::BASICSYNTHESIS:
			return "BASIC SYNTHESIS";
		case SkillName::CAREFULSYNTHESIS:
			return "CAREFUL SYNTHESIS";
		case SkillName::PRUDENTSYNTHESIS:
			return "PRUDENT SYNTHESIS";
		case SkillName::GROUNDWORK:
			return "GROUNDWORK";
		case SkillName::MUSCLEMEMORY:
			return "MUSCLEMEMORY";
		case SkillName::BASICTOUCH:
			return "BASIC TOUCH";
		case SkillName::STANDARDTOUCH:
			return "STANDARD TOUCH";
		case SkillName::ADVANCEDTOUCH:
			return "ADVANCED TOUCH";
		case SkillName::BYREGOTSBLESSING:
			return "BYREGOT'S BLESSING";
		case SkillName::PRUDENTTOUCH:
			return "PRUDENT TOUCH";
		case SkillName::PREPARATORYTOUCH:
			return "PREPARATORY TOUCH";
		case SkillName::REFLECT:
			return "REFLECT";
		case SkillName::REFINEDTOUCH:
			return "REFINED TOUCH";
		case SkillName::WASTENOTI:
			return "WASTE NOT I";
		case SkillName::WASTENOTII:
			return "WASTE NOT II";
		case SkillName::GREATSTRIDES:
			return "GREAT STRIDES";
		case SkillName::INNOVATION:
			return "INNOVATION";
		case SkillName::VENERATION:
			return "VENERATION";
		case SkillName::FINALAPPRAISAL:
			return "FINAL APPRAISAL";
		case SkillName::MASTERSMEND:
			return "MASTERS MEND";
		case SkillName::MANIPULATION:
			return "MANIPULATION";
		case SkillName::IMMACULATEMEND:
			return "IMMACULATE MEND";
		case SkillName::DELICATESYNTHESIS:
			return "DELICATE SYNTHESIS";
		default:
			return "Skill not found.\n";
			break;
		}
		return "";
	}

	enum class SkillType
	{
		SYNTHESIS = 0,
		TOUCH,
		BUFF,
		REPAIR,
		OTHER
	};

	const struct Skill {
		SkillType type;
		int costCP;
		int costDurability;
		int efficiency;
		int castTime;
	};

	const struct SkillTest {
		SkillName skillName;
		SkillType type;
		int costCP;
		int costDurability;
		int efficiency;
		int castTime;
	};

	const SkillTest skillTest[23] =
	{ { SkillName::BASICSYNTHESIS, SkillType::SYNTHESIS,		0,		10,		120,		3 } ,
		{ SkillName::CAREFULSYNTHESIS, SkillType::SYNTHESIS,		7,		10,		180,		3 } ,
		{ SkillName::PRUDENTSYNTHESIS,	 SkillType::SYNTHESIS,		18,		5,		180,		3 } ,
		{ SkillName::GROUNDWORK,		 SkillType::SYNTHESIS,		18,		20,		360,		3 } ,
		{ SkillName::MUSCLEMEMORY,		 SkillType::SYNTHESIS,		6,		10,		300,		3} ,
		{ SkillName::BASICTOUCH,		 SkillType::TOUCH,			18,		10,		100,		3 } ,
		{ SkillName::STANDARDTOUCH,		 SkillType::TOUCH,			32,		10,		125,		3 } ,
		{ SkillName::ADVANCEDTOUCH,		 SkillType::TOUCH,			46,		10,		150,		3 } ,
		{ SkillName::BYREGOTSBLESSING,	 SkillType::TOUCH,			24,		10,		100,		3 } ,
		{ SkillName::PRUDENTTOUCH,		 SkillType::TOUCH,			25,		5,		100,		3 } ,
		{ SkillName::PREPARATORYTOUCH,	 SkillType::TOUCH,			40,		20,		200,		3 } ,
		{ SkillName::REFLECT,			 SkillType::TOUCH,			6,		10,		300,		3 } ,
		{ SkillName::REFINEDTOUCH,		 SkillType::TOUCH,			24,		10,		100,		3 } ,
		{ SkillName::WASTENOTI,			 SkillType::BUFF,			56,		0,		0,			2 } ,
		{ SkillName::WASTENOTII,		 SkillType::BUFF,			98,		0,		0,			2 } ,
		{ SkillName::GREATSTRIDES,		 SkillType::BUFF,			32,		0,		0,			2 } ,
		{ SkillName::INNOVATION,		 SkillType::BUFF,			18,		0,		0,			2 } ,
		{ SkillName::VENERATION,		 SkillType::BUFF,			18,		0,		0,			2 } ,
		{ SkillName::FINALAPPRAISAL,	 SkillType::BUFF,			1,		0,		0,			2 } ,
		{ SkillName::MASTERSMEND,		 SkillType::REPAIR,		88,		0,		0,			3 } ,
		{ SkillName::MANIPULATION,		 SkillType::REPAIR,		96,		0,		0,			2 } ,
		{ SkillName::IMMACULATEMEND,	 SkillType::REPAIR,		112,	0,		0,			3 } ,
		{ SkillName::DELICATESYNTHESIS,	 SkillType::OTHER,			32,		10,		100,		3 } 
	};
}