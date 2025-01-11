#pragma once

//#include "Skills.hpp"
#include <bitset>

class ActionTracker {
public:
	ActionTracker() {};
	~ActionTracker() {};

	uint32_t venerationHistory{ 0b0 }, wasteNotHistory{ 0b0 }, strideHistory{ 0b0 }, innovationHistory{ 0b0 }, muscleHistory{ 0b0 };
	uint32_t basicSynthesis{ 0b0 }, carefulSynthesis{ 0b0 }, prudentSynthesis{ 0b0 }, groundwork{ 0b0 };
	uint32_t basicTouch{ 0b0 }, standardTouch{ 0b0 }, advancedTouch{ 0b0 }, byregots{ 0b0 }, prudentTouch{ 0b0 }, prepTouch{ 0b0 }, refinedTouch{ 0b0 };
	uint32_t synthActionUsed{ 0b0 }, touchActionUsed{ 0b0 }, durabilityActionUsed{ 0b0 };
	uint32_t numTouchSkillsUsed{ 0b0 }, numSynthSkillsUsed{ 0b0 }, numDurabilitySkillsUsed{ 0b0 };
	uint32_t touchTime{ 0 }, synthTime{ 0 };

	void PrintHistory() {
		std::cout << "Basic: " << BuffHistory(basicSynthesis, 0b1000) << BuffHistory(basicSynthesis, 0b100) << BuffHistory(basicSynthesis, 0b10) << BuffHistory(basicSynthesis, 0b1) << '\n';
		std::cout << "Careful: " << BuffHistory(carefulSynthesis, 0b1000) << BuffHistory(carefulSynthesis, 0b100) << BuffHistory(carefulSynthesis, 0b10) << BuffHistory(carefulSynthesis, 0b1) << '\n';
		std::cout << "prudent: " << BuffHistory(prudentSynthesis, 0b1000) << BuffHistory(prudentSynthesis, 0b100) << BuffHistory(prudentSynthesis, 0b10) << BuffHistory(prudentSynthesis, 0b1) << '\n';
		std::cout << "ground: " << BuffHistory(groundwork, 0b1000) << BuffHistory(groundwork, 0b100) << BuffHistory(groundwork, 0b10) << BuffHistory(groundwork, 0b1) << '\n';
	}

	inline static constexpr uint32_t BuffHistory(int history, int bitmask) {
		return history & bitmask;
	}

	bool ActionsUsedDuringBuff(uint8_t maxBuffDuration, uint8_t buffDurationRemaining, uint8_t checkUnderRemaining, uint8_t actionHistory, uint8_t numberOfActionsNeeded) {
		if (buffDurationRemaining == 0 || buffDurationRemaining >= checkUnderRemaining) return false;
		uint8_t bitmaskTurnsCount = 0;

		for (int i{ 0 }; i < maxBuffDuration - buffDurationRemaining; ++i) {
			bitmaskTurnsCount += BuffHistory(actionHistory, 0b1);
			actionHistory >>= 1;
		}

		return  bitmaskTurnsCount < numberOfActionsNeeded;
	}

	/* PROGRESS */
	// will always be 0b0 before the move is taken
	void ProgressSynthSkills(SkillName skillName) {
		basicSynthesis <<= 1;
		carefulSynthesis <<= 1;
		prudentSynthesis <<= 1;
		groundwork <<= 1;
		synthActionUsed <<= 1;
		switch (skillName) {
		case SkillName::BASICSYNTHESIS:
			//std::cout << "Basic synth used\n";
			basicSynthesis |= 0b1;
			synthActionUsed |= BuffHistory(basicSynthesis, 0b1);
			break;
		case SkillName::CAREFULSYNTHESIS:
			//std::cout << "Careful synth used\n";
			carefulSynthesis |= 0b1;
			synthActionUsed |= BuffHistory(carefulSynthesis, 0b1);
			break;
		case SkillName::PRUDENTSYNTHESIS:
			//std::cout << "Prudent synth used\n";
			prudentSynthesis |= 0b1;
			synthActionUsed |= BuffHistory(prudentSynthesis, 0b1);
			break;
		case SkillName::GROUNDWORK:
			//std::cout << "Groundword used\n";
			groundwork |= 0b1;
			synthActionUsed |= BuffHistory(groundwork, 0b1);
			break;
		case SkillName::MUSCLEMEMORY:
			synthActionUsed |= 0b1;
			break;
		default:
			break;
		}
		if (BuffHistory(synthActionUsed, 0b1)) {
			++numSynthSkillsUsed;
			synthTime += 3;
		}
	}

	void ProgressTouchActions(SkillName skillName) {
		basicTouch <<= 1;
		standardTouch <<= 1;
		advancedTouch <<= 1;
		byregots	  <<= 1;
		prudentTouch  <<= 1;
		prepTouch	  <<= 1;
		refinedTouch  <<= 1;
		touchActionUsed <<= 1;
		switch (skillName) {
		case SkillName::BASICTOUCH:
			basicTouch |= 0b1;
			touchActionUsed |= BuffHistory(basicTouch, 0b1);
			break;
		case SkillName::STANDARDTOUCH:
			standardTouch |= 0b1;
			touchActionUsed |= BuffHistory(standardTouch, 0b1);
			break;
		case SkillName::ADVANCEDTOUCH:
			advancedTouch |= 0b1;
			touchActionUsed |= BuffHistory(advancedTouch, 0b1);
			break;
		case SkillName::BYREGOTSBLESSING:
			byregots |= 0b1;
			touchActionUsed |= BuffHistory(byregots, 0b1);
			break;
		case SkillName::PRUDENTTOUCH:
			prudentTouch |= 0b1;
			touchActionUsed |= BuffHistory(prudentTouch, 0b1);
			break;
		case SkillName::PREPARATORYTOUCH:
			prepTouch |= 0b1;
			touchActionUsed |= BuffHistory(prepTouch, 0b1);
			break;
		case SkillName::REFINEDTOUCH:
			refinedTouch |= 0b1;
			touchActionUsed |= BuffHistory(refinedTouch, 0b1);
			break;
		case SkillName::REFLECT:
			touchActionUsed |= 0b1;
			break;
		default:
			break;
		}
		// If adding delicate synthesis to this, condition for X number of touch skills used must be reviewed
		if (BuffHistory(touchActionUsed, 0b1)) {
			++numTouchSkillsUsed;
			touchTime += 3;
		}
	}

	void ProgressDurabilityActions(SkillName skillName) {
		durabilityActionUsed <<= 1;
		switch (skillName) {
		case SkillName::WASTENOTI:
		case SkillName::WASTENOTII:
		case SkillName::MANIPULATION:
		case SkillName::MASTERSMEND:
		case SkillName::IMMACULATEMEND:
			durabilityActionUsed |= 0b1;
		}
		if (BuffHistory(durabilityActionUsed, 0b1)) ++numDurabilitySkillsUsed;
	}

	void ProgressBuffs(bool venerationBuff, bool wasteNotBuff, bool strideBuff, bool innoBuff, bool muscleBuff) {
		venerationHistory <<= 1;
		venerationHistory |= venerationBuff;
		if (venerationBuff)	synthTime += 2;
		wasteNotHistory <<= 1;
		wasteNotHistory |= wasteNotBuff;
		strideHistory <<= 1;
		strideHistory |= strideBuff;
		innovationHistory <<= 1;
		innovationHistory |= innoBuff;
		if (innoBuff)	touchTime += 2;
		muscleHistory <<= 1;
		muscleHistory |= muscleBuff;
	}

	inline void Progress(SkillName skillName, bool venerationBuff, bool wasteNotBuff, bool strideBuff, bool innoBuff, bool muscleBuff) {
		ProgressSynthSkills(skillName);
		ProgressTouchActions(skillName);
		ProgressDurabilityActions(skillName);
		ProgressBuffs(venerationBuff, wasteNotBuff, strideBuff, innoBuff, muscleBuff);
	}

	/* BACKTRACK */
	inline void BacktrackSynthSkills() {
		basicSynthesis >>= 1;
		carefulSynthesis >>= 1;
		prudentSynthesis >>= 1;
		groundwork >>= 1;
		if (BuffHistory(synthActionUsed, 0b1)) {
			--numSynthSkillsUsed;
			synthTime -= 3;
		}
		synthActionUsed >>= 1;

	}

	inline void BacktrackTouchActions() {
		basicTouch >>= 1;
		standardTouch >>= 1;
		advancedTouch >>= 1;
		byregots >>= 1;
		prudentTouch >>= 1;
		prepTouch >>= 1;
		refinedTouch >>= 1;
		if (BuffHistory(touchActionUsed, 0b1)) {
			--numTouchSkillsUsed;
			touchTime -= 3;
		}
		touchActionUsed >>= 1;
	}

	inline void BacktrackDurabilityActions() {
		if (BuffHistory(durabilityActionUsed, 0b1))	--numDurabilitySkillsUsed;
		durabilityActionUsed >>= 1;
	}

	inline void BacktrackBuffs() {
		if (BuffHistory(venerationHistory, 0b1))	synthTime -= 2;
		venerationHistory >>= 1;
		wasteNotHistory >>= 1;
		strideHistory >>= 1;
		if (BuffHistory(innovationHistory, 0b1))	touchTime -= 2;
		innovationHistory >>= 1;
		muscleHistory >>= 1;
	}

	inline void Backtrack() {
		BacktrackSynthSkills();
		BacktrackTouchActions();
		BacktrackDurabilityActions();
		BacktrackBuffs();
	}
};