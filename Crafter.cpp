#include "Crafter.hpp"

#define ProgressUpdate 1

Crafter::Crafter(std::vector<Skills::SkillTest> startingMoves, int maxCP, float progressPerHundred, float qualityPerHundred, int maxProgress, int maxQuality, int maxDurability, bool forceQuality, bool greaterByregot, int maximumTurnLimit)
	: forceMaxQuality(forceQuality), forceGreaterByregot(greaterByregot), maxTurnLimit(maximumTurnLimit),
	actionTracker(new ActionTracker()),
	Player(maxCP, progressPerHundred, qualityPerHundred) {
	AddItem(maxProgress, maxQuality, maxDurability);

	craftingRecord.player = playerState;
	craftingRecord.item = craftableItem->GetItemState();
	craftingHistory.reserve(maximumTurnLimit + 1);
	
	if (forceMaxQuality) {
		playerState.currentTurn = 2;
		FindMinQualityForMax();
		if (!successfulQualityCrafts.empty()) {
			minTouchSkills = successfulQualityCrafts[bestQualityTime][0].size();
			RemoveItem();
			AddItem(maxProgress, maxQuality, maxDurability);
			int innovationBuff{ 0 };
			int greatStridesBuff{ 0 };
			int durability = 0;
			int maxQuality = craftableItem->GetMaxQuality();
			for (int i{ 0 }; i < minTouchSkills; ++i) {
				SkillName move = successfulQualityCrafts[bestQualityTime][0][i];
				for (const auto& entry : skillTest) {		// find skill efficiency
					if (entry.skillName == move) {
						if (entry.efficiency == 0) {
							BuffSkills(move);
						}
						else {
							TouchSkills(move, 0, durability);
						}
						break;
					}

				}
				
				bestQuality.emplace(bestQuality.begin(), maxQuality - craftableItem->GetCurrentQuality());
			}
			craftingHistory.clear();
			std::cout << "The minimum number of touch skills required to achieve max quality is " << minTouchSkills << '\n';
		}
		else {
			std::cout << "There was no way to find max quality with your QP100 and the maximum number of steps.\n";
		}
		
		RemoveItem();
		AddItem(maxProgress, maxQuality, maxDurability);
		craftingRecord.player = playerState;
		craftingRecord.item = craftableItem->GetItemState();
	}

	SaveCraftingHistory(SkillName::NONE);		// An extra move is pruned off so it is necessary to keep the skills going
	
	if (!startingMoves.empty()) {
		std::cout << "For the starting moves:";
		for (const Skills::SkillTest& move : startingMoves) {
			if (!CastSkill(move)) {
				std::cout << "Invalid. The starting moves break/finish the item.\n";
				invalid = true;
				return;
			}
			std::cout << " " << Skills::GetSkillName(move.skillName) << " |";
			SaveCraftingHistory(move.skillName);
		}
		std::cout << '\n';
	}
	baseTurn = playerState.currentTurn;
}

Crafter::~Crafter() {
	if (successfulCrafts.empty()) {
		return;
	}

	std::cout << "The fastest time was " << bestTime << " seconds.\n";
	std::cout << "Ways to achieve this are:\n";
	int i{ 1 };
	for (const auto& entry : successfulCrafts[bestTime]) {
		std::cout << "Solution " << i << ": ";
		for (const auto& move : entry) {
			if (move == SkillName::NONE)	continue;
			std::cout << Skills::GetSkillName(move) << ", ";
		}
		std::cout << '\n';
		++i;
	}
}


void Crafter::ForceCraft() {
	if (invalid) return;
	actionTracker->ProgressBuffs(craftingRecord.player.buffInfo.innovationActive, craftingRecord.player.buffInfo.wasteNotActive,
		craftingRecord.player.buffInfo.greatStridesActive, craftingRecord.player.buffInfo.innovationActive);

	if (playerState.currentTurn == 1) {
		StarterCraft();
	}
	else {
		bool requireQuality = forceMaxQuality && !craftableItem->IsItemMaxQuality();
		int remainingTime = bestTime - craftingRecord.currentTime;
		
		bool synthActionRequired = remainingTime < 5 || craftingRecord.player.currentTurn == maxTurnLimit - 1		// Only one move left to match the best time and turn limit
				|| actionTracker->ActionsUsedDuringBuff(4, craftingRecord.player.buffInfo.veneration, 3, actionTracker->synthActionUsed, 2)	// Requires a synth action
				|| actionTracker->ActionsUsedDuringBuff(5, craftingRecord.player.buffInfo.finalAppraisal, 4, actionTracker->synthActionUsed, 1);	// Requires a synth action
		

		if (requireQuality) {
			if (!synthActionRequired) {
				QualityCraft();
			}
			OtherCraft();
		}

		//bool secondToLastMove = (remainingTime < 7 || craftingRecord.player.currentTurn == maxTurnLimit - 2);
		if (!(actionTracker->ActionsUsedDuringBuff(4, craftingRecord.player.buffInfo.innovation, 3, actionTracker->touchActionUsed, 2)	// If there is only one buff use it may as well be great strides
			|| actionTracker->ActionsUsedDuringBuff(3, craftingRecord.player.buffInfo.greatStrides, 2, actionTracker->touchActionUsed, 1)
			|| ((remainingTime < 7 || craftingRecord.player.currentTurn == maxTurnLimit - 2) && requireQuality) ||
			((minTouchSkills - actionTracker->numTouchSkillsUsed) == ((maxTurnLimit - 1) - (playerState.currentTurn)))
			)) {
			SynthesisCraft();

			if (!synthActionRequired) {
				BuffCraft();
				int repairableDurability = craftableItem->GetMaxDurability() - craftableItem->GetDurability();
				if ((repairableDurability >= 25)) {
					RepairCraft(repairableDurability);
				}
			}
		}	
	}
	ContinueCraft();
}


/*----------------------PRIVATE-------------------------------------*/

void Crafter::CraftAndRecord(const SkillTest& move) {
	if (CastSkill(move)) {
		bool requireQuality = forceMaxQuality && !craftableItem->IsItemMaxQuality();
		int remainingCraftTurns = maxTurnLimit - 1 - playerState.currentTurn;

		if (craftableItem->IsItemCrafted()) {
			if (requireQuality) {
				LoadLastCraftingRecord();
				return;
			}
			AddSuccessfulCraft(move.skillName);
			return;
		}

		if (requireQuality && remainingCraftTurns < minTouchSkills) {
			if ((craftableItem->GetMaxQuality() - craftableItem->GetCurrentQuality()) > bestQuality[remainingCraftTurns]) {	
				LoadLastCraftingRecord();							// two quality turns and synth turn, strongest ending possible
				return;												// if worse than this then it is impossible
			}
		}

		if (playerState.currentTurn >= maxTurnLimit || (playerState.currentTime + 3) > bestTime) {		// can't use lastMove here, causes some form of memory leak
			LoadLastCraftingRecord();
			return;
		}
		else if (craftingRecord.player.buffInfo.finalAppraisal == 1 && (craftableItem->GetMaxProgress() - craftableItem->GetCurrentProgress()) != 1) {		// not appraised
			LoadLastCraftingRecord();
			return;
		}
		else if (!craftableItem->IsItemBroken()) {
			SaveCraftingHistory(move.skillName);
			ForceCraft();
		}
		else if (craftableItem->IsItemBroken()) {
			LoadLastCraftingRecord();
			return;
		}
#if ProgressUpdate
		if (playerState.currentTurn == baseTurn) {
			std::cout << Skills::GetSkillName(move.skillName) << " completed\n";
		}
#endif
	}
}


void Crafter::FindMinQualityForMax() {
	for (const SkillTest& move : qualitySkills) {
		craftableItem->UpdateDurability(1000);
		if (QualityCheck(move.skillName)) {
			continue;
		}
		QualityOnlyCrafts(move);
	}
	for (const SkillTest& move : otherSkills) {
		QualityOnlyCrafts(move);
	}
	ContinueCraft();
}

void Crafter::QualityOnlyCrafts(const SkillTest& move) {
	if (CastSkill(move)) {
		if (craftableItem->IsItemMaxQuality()) {
			SaveCraftingHistory(move.skillName);
			AddSuccessfulQualityCraft();
			ContinueCraft();
		}
		else if (playerState.currentTurn >= maxTurnLimit || (playerState.currentTime + 3) > bestQualityTime) {
			LoadLastCraftingRecord();
		}
		else if (!craftableItem->IsItemBroken()) {
			SaveCraftingHistory(move.skillName);
			FindMinQualityForMax();
		}
		else if (craftableItem->IsItemBroken()) {
			LoadLastCraftingRecord();
		}
	}
}

void Crafter::StarterCraft() {
	for (const SkillTest& move : startingMoveList) {
		CraftAndRecord(move);
	}
}

void Crafter::SynthesisCraft() {
	bool checkForCarefulGroundWork = (((actionTracker->prudentSynthesis & 0b11) | (actionTracker->groundwork & 0b11)) == 0b11);	// either have been used for the past two turns	
	bool venerationBuff = playerState.buffInfo.veneration > 0;
	for (const SkillTest& move : synthesisSkills) {
		if (SimilarTrees(move.skillName))	continue;

		if (move.skillName == SkillName::VENERATION  && venerationBuff)	continue;
		if (checkForCarefulGroundWork && (move.skillName == SkillName::CAREFULSYNTHESIS 
										|| move.skillName == SkillName::GROUNDWORK))	continue;	// will be faster and same with veneration buff

		CraftAndRecord(move);
	}
}

void Crafter::QualityCraft() {
	for (const SkillTest& move : qualitySkills) {
		if (QualityCheck(move.skillName)) {
			continue;
		}
		CraftAndRecord(move);
	}
}

void Crafter::BuffCraft() {
	for (const SkillTest& move : buffSkills) {
		if (BuffCheck(move.skillName)) {
			continue;
		}
		CraftAndRecord(move);
	}
}

void Crafter::RepairCraft(int repairableDurability) {
	for (const SkillTest& move : repairSkills) {
		if (playerState.lastSkillUsed == SkillName::MASTERSMEND)	continue;		//If previously used this skill, it would have been more effective to use Immaculate Mend
		if (move.skillName == SkillName::IMMACULATEMEND && repairableDurability <= 30)	continue;	// better to use masters mend here
		CraftAndRecord(move);
	}
}

void Crafter::OtherCraft() {
	for (const SkillTest& move : otherSkills) {
		CraftAndRecord(move);
	}
}

bool Crafter::SimilarTrees(SkillName skillName) {
	bool basic = actionTracker->basicSynthesis & 0b1;
	bool careful = actionTracker->carefulSynthesis & 0b1;
	bool prudent = actionTracker->prudentSynthesis & 0b1;
	bool ground = actionTracker->groundwork & 0b1;
	int history = actionTracker->venerationHistory & 0b11;
	int wasteNotHistory = actionTracker->wasteNotHistory & 0b11;
	if (history != 0b11 && history != 0b00)	return false;	// only proceed if the veneration history is actually the same
	if (wasteNotHistory != 0b11 && wasteNotHistory != 0b00)	return false;	// only proceed if the veneration history is actually the same

	switch (skillName) {
	case SkillName::BASICSYNTHESIS:
		break;
	case SkillName::CAREFULSYNTHESIS:
		if (basic)		return true;
		break;
	case SkillName::PRUDENTSYNTHESIS:
		if (basic || careful)		return true;
		break;
	case SkillName::GROUNDWORK:
		if (basic || careful || prudent)	return true;
		break;
	default:
		break;
	}
	return false;
}


void Crafter::ContinueCraft() {
	DeleteCraftingHistory();
	LoadLastCraftingRecord();
	actionTracker->Backtrack();
}



bool Crafter::QualityCheck(SkillName skillName) {
	bool skipTouchSkill{ false };
	switch (skillName) {
	case SkillName::BYREGOTSBLESSING:
		if (playerState.innerQuiet < minTouchSkills/2) {
			skipTouchSkill = true;
		}
		else if (forceGreaterByregot) {
			skipTouchSkill = !playerState.buffInfo.greatStridesActive;
		}
		break;
	case SkillName::BASICTOUCH:
		if (playerState.lastSkillUsed == SkillName::BASICTOUCH || playerState.lastSkillUsed == SkillName::STANDARDTOUCH)	return true;
		break;
	case SkillName::STANDARDTOUCH:
		if (playerState.lastSkillUsed == SkillName::STANDARDTOUCH)	return true;
		break;
	case SkillName::REFINEDTOUCH:
		skipTouchSkill = playerState.lastSkillUsed != SkillName::BASICTOUCH;
		break;
	case SkillName::GREATSTRIDES:
		skipTouchSkill = playerState.buffInfo.greatStridesActive;
		break;
	case SkillName::INNOVATION:
		skipTouchSkill = playerState.buffInfo.innovation > 1;		// fringe cases you want to recast innovation with 1 turn left
		break;
	default:			// Should be touch action skills
		break;
	}
	return skipTouchSkill;
}

bool Crafter::BuffCheck(SkillName skillName) {
	if (GetBuffDuration(skillName) > 0) {			// potentially bad logic
		return true;
	}
	bool buffSkip{ false };
	
	switch (skillName) {
	case SkillName::WASTENOTI:
	case SkillName::WASTENOTII:
		break;
	case SkillName::FINALAPPRAISAL:
		if (playerState.currentTurn + 6 >= maxTurnLimit) {
			buffSkip = true;
		}
		else if (!forceMaxQuality || craftableItem->IsItemMaxQuality()) {
			buffSkip = true;
		}
		break;
	case SkillName::MANIPULATION:
		//if (player->GetCurrentTurn() + 3 >= maxTurnLimit) buffSkip = true;			// logical limiter, this would be able to get 1 move extra in
		break;
	default:
		std::cout << "A serious error has occured for " << Skills::GetSkillName(skillName) << '\n';
		break;
	}
	return buffSkip;
}

/********************************* CRAFTING RECORDS *********************************/
inline void Crafter::SaveCraftingHistory(SkillName skillName) {
	craftingRecord.player = playerState;
	craftingRecord.item = craftableItem->GetItemState();
	craftingRecord.currentTime = playerState.currentTime;
	craftingRecord.skillName = skillName;
	craftingHistory.emplace_back(craftingRecord);
	actionTracker->ProgressSynthSkills(skillName);
	actionTracker->ProgressTouchActions(skillName);
}

inline void Crafter::DeleteCraftingHistory() {
	craftingHistory.pop_back();
}

void Crafter::AddSuccessfulQualityCraft() {
	bestQualityTime = craftingRecord.currentTime;		// Time restraints already managed by force craft
	std::vector<SkillName> success{};
	//success.reserve(craftingHistory.size());
	for (const auto& entry : craftingHistory) {
		success.emplace_back(entry.skillName);
	}
	successfulQualityCrafts[craftingRecord.currentTime].emplace_back(success);
}

void Crafter::AddSuccessfulCraft(SkillName skillName) {
	craftingRecord.player = playerState;
	craftingRecord.item = craftableItem->GetItemState();
	craftingRecord.currentTime = playerState.currentTime;
	craftingRecord.skillName = skillName;
	bestTime = craftingRecord.currentTime;		// Time restraints already managed by force craft

	std::vector<SkillName> success{};
	success.reserve(craftingHistory.size());
	for (const auto& entry : craftingHistory) {
		success.emplace_back(entry.skillName);
	}
	success.emplace_back(craftingRecord.skillName);
	successfulCrafts[craftingRecord.currentTime].emplace_back(success);
	LoadLastCraftingRecord();
}

inline void Crafter::LoadLastCraftingRecord() {
	LoadPlayerStats(craftingHistory.back().player);
	craftableItem->LoadItemState(craftingHistory.back().item);
	craftingRecord = craftingHistory.back();
}

void Crafter::PrintCrafts() {
	std::cout << "Current: ";
	for (const auto& entry : craftingHistory) {
		std::cout << Skills::GetSkillName(entry.skillName) << ", ";
	}
	std::cout << '\n';
}