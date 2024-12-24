#pragma once

//#include "Skills.hpp"

class ActionTracker {
public:
	ActionTracker() { std::cout << "Hello\n"; };
	~ActionTracker() {};

	int venerationHistory{ 0b0 }, wasteNotHistory{ 0b0 }, strideHistory{ 0b0 };
	int basicSynthesis{ 0b0 }, carefulSynthesis{ 0b0 }, prudentSynthesis{ 0b0 }, groundwork{ 0b0 };
	int basicTouch{ 0b0 }, standardTouch{ 0b0 }, advancedTouch{ 0b0 }, byregots{ 0b0 }, prudentTouch{ 0b0 }, prepTouch{ 0b0 }, refinedTouch{ 0b0 };

	void PrintHistory() {
		std::cout << "Basic: " << (basicSynthesis & 0b1000) << (basicSynthesis & 0b100) << (basicSynthesis & 0b10) << (basicSynthesis & 0b1) << '\n';
		std::cout << "Careful: " << (carefulSynthesis & 0b1000) << (carefulSynthesis & 0b100) << (carefulSynthesis & 0b10) << (carefulSynthesis & 0b1) << '\n';
		std::cout << "prudent: " << (prudentSynthesis & 0b1000) << (prudentSynthesis & 0b100) << (prudentSynthesis & 0b10) << (prudentSynthesis & 0b1) << '\n';
		std::cout << "ground: " << (groundwork & 0b1000) << (groundwork & 0b100) << (groundwork & 0b10) << (groundwork & 0b1) << '\n';
	}

	//@TODO update to include groundwork
	// will always be 0b0 before the move is taken
	void ProgressSynthSkills(SkillName skillName) {
		basicSynthesis <<= 1;
		carefulSynthesis <<= 1;
		prudentSynthesis <<= 1;
		groundwork <<= 1;
		switch (skillName) {
		case SkillName::BASICSYNTHESIS:
			//std::cout << "Basic synth used\n";
			basicSynthesis |= 0b1;
			break;
		case SkillName::CAREFULSYNTHESIS:
			//std::cout << "Careful synth used\n";
			carefulSynthesis |= 0b1;
			break;
		case SkillName::PRUDENTSYNTHESIS:
			//std::cout << "Prudent synth used\n";
			prudentSynthesis |= 0b1;
			break;
		case SkillName::GROUNDWORK:
			//std::cout << "Groundword used\n";
			groundwork |= 0b1;
			break;
		default:
			break;
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
		switch (skillName) {
		case SkillName::BASICTOUCH:
			basicTouch |= 0b1;
			break;
		case SkillName::STANDARDTOUCH:
			standardTouch |= 0b1;
			break;
		case SkillName::ADVANCEDTOUCH:
			advancedTouch |= 0b1;
			break;
		case SkillName::BYREGOTSBLESSING:
			byregots |= 0b1;
			break;
		case SkillName::PRUDENTTOUCH:
			prudentTouch |= 0b1;
			break;
		case SkillName::PREPARATORYTOUCH:
			prepTouch |= 0b1;
			break;
		case SkillName::REFINEDTOUCH:
			refinedTouch |= 0b1;
			break;
		default:
			break;
		}
	}

	inline void BacktrackSynthSkills() {
		basicSynthesis >>= 1;
		carefulSynthesis >>= 1;
		prudentSynthesis >>= 1;
		groundwork >>= 1;
	}

	inline void BacktrackTouchActions() {
		basicTouch >>= 1;
		standardTouch >>= 1;
		advancedTouch >>= 1;
		byregots >>= 1;
		prudentTouch >>= 1;
		prepTouch >>= 1;
		refinedTouch >>= 1;
	}

	static constexpr int BuffHistory(int history, int bitmask) {
		return history & bitmask;
	}

	void ProgressBuffs(bool venerationBuff, bool wasteNotBuff, bool strideBuff) {
		venerationHistory <<= 1;
		venerationHistory |= venerationBuff;
		wasteNotHistory <<= 1;
		wasteNotHistory |= wasteNotBuff;
		strideHistory <<= 1;
		strideHistory |= strideBuff;
	}

	inline void BacktrackBuffs() {
		venerationHistory >>= 1;
		wasteNotHistory >>= 1;
		strideHistory >>= 1;
	}

	void Backtrack() {
		BacktrackBuffs();
		BacktrackSynthSkills();
		BacktrackTouchActions();
	}
};