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
}