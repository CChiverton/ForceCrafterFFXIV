#include "Crafter.hpp"

#define ProgressUpdate 1

Crafter::Crafter(std::vector<Skills::SkillTest> startingMoves, int maxCP, float progressPerHundred, float qualityPerHundred, int maxProgress, int maxQuality, int maxDurability, bool forceQuality, bool greaterByregot, int maximumTurnLimit)
	: forceMaxQuality(forceQuality), forceGreaterByregot(greaterByregot), maxTurnLimit(maximumTurnLimit),
	actionTracker(new ActionTracker()),
	Player(maxCP, progressPerHundred, qualityPerHundred) {
	AddItem(maxProgress, maxQuality, maxDurability);

	craftingHistory.reserve(maximumTurnLimit);

	if (!startingMoves.empty()) {
		std::cout << "For the starting moves:";
		for (const Skills::SkillTest& move : startingMoves) {
			if (!Craft(move)) {
				std::cout << "Invalid. The starting moves break/finish the item.\n";
				invalid = true;
				return;
			}
			std::cout << " " << Skills::GetSkillName(move.skillName) << " |";
			SaveCraftingHistory(move.skillName);
			//std::cout << "Player turn is " << player->GetCurrentTurn() << '\n';
		}
		std::cout << '\n';
	}
	baseTurn = playerState.currentTurn;
	//std::cout << "-----------------------------------------\n";
}

Crafter::~Crafter() {
	if (successfulCrafts.empty()) {
		return;
	}

	std::cout << "The fastest time was " << bestTime << " seconds.\n";
	std::cout << "Ways to achieve this are:\n";

	for (const auto& entry : successfulCrafts[bestTime]) {
		std::cout << "Solution: ";
		for (const auto& move : entry) {
			std::cout << Skills::GetSkillName(move) << ", ";
		}
		std::cout << '\n';
	}
}

void Crafter::CraftAndRecord(const SkillTest& move, const CraftingHistory& previousStep, int finalAppraisalTimer) {
	if (Craft(move)) {

		//std::cout << "Turn " << player->GetCurrentTurn() << ": " << Skills::GetSkillName(move) << '\n';

		if (craftableItem->IsItemCrafted()) {
			if (forceMaxQuality && !craftableItem->IsItemMaxQuality()) {
				//std::cout << "Not maximum quality when needed\n";
				LoadLastCraftingRecord(previousStep);
				return;
			}
			SaveCraftingHistory(move.skillName);
			AddSuccessfulCraft();
			ContinueCraft();
		}
		else if (playerState.currentTurn >= maxTurnLimit || (playerState.currentTime + 3) > bestTime) {		// can't use lastMove here, causes some form of memory leak
			//std::cout << "Run out of moves\n";
			LoadLastCraftingRecord(previousStep);
			return;
		}
		else if (finalAppraisalTimer == 1 && (craftableItem->GetMaxProgress() - craftableItem->GetCurrentProgress()) != 1) {		// not appraised
			LoadLastCraftingRecord(previousStep);
			return;
		}
		else if (!craftableItem->IsItemBroken()) {
			SaveCraftingHistory(move.skillName);
			ForceCraft();
		}
#if ProgressUpdate
		if (playerState.currentTurn == baseTurn) {
			std::cout << Skills::GetSkillName(move.skillName) << " completed\n";
		}
#endif
		//std::cout << "Finisheng Turn " << player->GetCurrentTurn() + 1 << ": " << Skills::GetSkillName(move) << '\n';
	}
}

void Crafter::ForceCraft() {
	if (invalid) return;
	CraftingHistory& previousStep = craftingRecord;		// stack allocation for faster loading
	int remainingTime = bestTime - previousStep.currentTime;
	bool lastMove = ((remainingTime < 5) || previousStep.player.currentTurn == maxTurnLimit - 1) ? true : false; // Only one move left to match the best time and turn limit
	bool secondToLastMove = (remainingTime < 7 || previousStep.player.currentTurn == maxTurnLimit - 2) ? true : false;
	//int innovationTimer = previousStep.player.buffInfo.innovation;

	//int venerationTimer = previousStep.player.buffInfo.veneration;
	//int strideTimer = previousStep.player.buffInfo.greatStrides;
	actionTracker->ProgressBuffs(previousStep.player.buffInfo.innovationActive, previousStep.player.buffInfo.wasteNotActive,
								previousStep.player.buffInfo.greatStridesActive, previousStep.player.buffInfo.innovationActive);
	//int finalAppraisalTimer = previousStep.player.buffInfo.finalAppraisal;
	bool isMaxQuality = craftableItem->IsItemMaxQuality();
	int itemDurability = craftableItem->GetMaxDurability() - craftableItem->GetDurability();

	bool requireTouch = actionTracker->ActionsUsedDuringBuff(4, previousStep.player.buffInfo.innovation, 3, actionTracker->touchActionUsed, 2)	// If there is only one buff use it may as well be great strides
						|| actionTracker->ActionsUsedDuringBuff(3, previousStep.player.buffInfo.greatStrides, 2, actionTracker->touchActionUsed, 1) //ActionUsedDuringBuff(previousStep.player.buffInfo.greatStrides, actionTracker->touchActionUsed, 0b11)
						|| (secondToLastMove && forceMaxQuality && !isMaxQuality);
	bool requireSynth = ActionUsedDuringBuff(previousStep.player.buffInfo.veneration, actionTracker->synthActionUsed, 0b111);
	bool requireAppraisal = ActionUsedDuringBuff(previousStep.player.buffInfo.finalAppraisal, actionTracker->synthActionUsed, 0b1111);

	bool synthActionRequired = lastMove || requireSynth || requireAppraisal;
	//bool skipForTouch = secondToLastMove && forceMaxQuality && !isMaxQuality;

	if (!(!forceMaxQuality || isMaxQuality || synthActionRequired)) {
		QualityCraft(previousStep, previousStep.player.buffInfo.finalAppraisal);
	}
	if (!requireTouch) {
		SynthesisCraft(previousStep, previousStep.player.buffInfo.finalAppraisal);
	}
	if (!isMaxQuality && forceMaxQuality) {		// Better to use a different synthesis skill if quality isn't required
		OtherCraft(previousStep, previousStep.player.buffInfo.finalAppraisal);
	}
	if (!(synthActionRequired || requireTouch)) {
		BuffCraft(previousStep, previousStep.player.buffInfo.finalAppraisal);
		if (!(secondToLastMove && itemDurability >= 20)) {
			RepairCraft(previousStep, previousStep.player.buffInfo.finalAppraisal, itemDurability);
		}
	}
	/*if (!(synthActionRequired || (secondToLastMove && itemDurability >= 20) || requireTouch)) {
		RepairCraft(previousStep, finalAppraisalTimer);
	}*/

	ContinueCraft();
	//std::cout << player->GetCurrentTurn() << " TRIED ALL POSSIBLE MOVES AT THIS LEVEL\n";
}


/*----------------------PRIVATE-------------------------------------*/

void Crafter::SynthesisCraft(const CraftingHistory& previousStep, int finalAppraisalTimer) {
	bool checkForCarefulGroundWork = (((actionTracker->prudentSynthesis & 0b11) | (actionTracker->groundwork & 0b11)) == 0b11);	// either have been used for the past two turns	
	for (const SkillTest& move : synthesisSkills) {
		if (SimilarTrees(move.skillName))	continue;

		if (checkForCarefulGroundWork && (move.skillName == SkillName::CAREFULSYNTHESIS 
										|| move.skillName == SkillName::GROUNDWORK))	continue;	// will be faster and same with veneration buff

		CraftAndRecord(move, previousStep, finalAppraisalTimer);
	}
}

void Crafter::QualityCraft(const CraftingHistory& previousStep, int finalAppraisalTimer) {
	for (const SkillTest& move : qualitySkills) {
		
		if (QualityCheck(move.skillName)) {
			continue;
		}

		CraftAndRecord(move, previousStep, finalAppraisalTimer);
	}
}

void Crafter::BuffCraft(const CraftingHistory& previousStep, int finalAppraisalTimer) {
	for (const SkillTest& move : buffSkills) {
		if (BuffCheck(move.skillName)) {
			continue;
		}

		CraftAndRecord(move, previousStep, finalAppraisalTimer);
	}
}

void Crafter::RepairCraft(const CraftingHistory& previousStep, int finalAppraisalTimer, int remainingDurability) {
	for (const SkillTest& move : repairSkills) {
		if (playerState.lastSkillUsed == SkillName::MASTERSMEND)	continue;		//If previously used this skill, it would have been more effective to use Immaculate Mend
		if (move.skillName == SkillName::IMMACULATEMEND && remainingDurability <= 30)	continue;	// better to use masters mend here
		if (remainingDurability > 15)	continue;		// Arbritrary number, more of a logical "Why repair at this stage"
		CraftAndRecord(move, previousStep, finalAppraisalTimer);
	}
}

void Crafter::OtherCraft(const CraftingHistory& previousStep, int finalAppraisalTimer) {
	for (const SkillTest& move : otherSkills) {
		CraftAndRecord(move, previousStep, finalAppraisalTimer);
	}
}

void Crafter::UpdateValidBuffCheck(int appropriateActionTurn, bool actionUsedThisTurn) {
	appropriateActionTurn <<= 1;
	appropriateActionTurn |= actionUsedThisTurn;
}

bool Crafter::ActionUsedDuringBuff(int timeLeft, int actionHistory, int turns) {
	//if (actionHistory & turns)	std::cout << "I have some history\n";
	//if ((timeLeft == 1 && !(actionHistory & turns))) std::cout << "More history than others\n";
	return (timeLeft == 1 && !(actionHistory & turns));
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


bool Crafter::Craft(Skills::SkillTest skillName) {
	if (!CastSkill(skillName)) {
		//std::cout << "Invalid move " << Skills::GetSkillName(skillName) << '\n';
		return false;
	}
	//std::cout << Skills::GetSkillName(skillName) << '\n';

	return true;
}

void Crafter::AddSuccessfulCraft() {
	//std::cout << "Craft successful\n";

	bestTime = craftingRecord.currentTime;		// Time restraints already managed by force craft
	std::vector<SkillName> success{};
	//success.reserve(craftingHistory.size());
	for (const auto& entry : craftingHistory) {
		success.emplace_back(entry.skillName);
	}
	successfulCrafts[craftingRecord.currentTime].emplace_back(success);
}

inline void Crafter::LoadLastCraftingRecord(const CraftingHistory& lastRecord) {
	LoadPlayerStats(lastRecord.player);
	craftableItem->LoadItemState(lastRecord.item);
	craftingRecord = lastRecord;
	/*std::cout << "After loading item stats are\n";
	player->craftableItem->OutputStats();*/
}

void Crafter::ContinueCraft() {
	DeleteCraftingHistory();
	CraftingHistory& last = craftingHistory.back();
	LoadLastCraftingRecord(last);
	actionTracker->Backtrack();
}

bool Crafter::QualityCheck(SkillName skillName) {
	bool skipTouchSkill{ false };
	switch (skillName) {
	case SkillName::GREATSTRIDES:
		skipTouchSkill = playerState.buffInfo.greatStridesActive;
		break;
	case SkillName::INNOVATION:
		skipTouchSkill = playerState.buffInfo.innovation > 1;		// fringe cases you want to recast innovation with 1 turn left
		break;
	case SkillName::BYREGOTSBLESSING:
		if ((playerState.lastSkillUsed == SkillName::BASICTOUCH && playerState.innerQuiet < 2) ||			// possible logic flaw with combo implementation
			(playerState.lastSkillUsed == SkillName::STANDARDTOUCH && playerState.innerQuiet < 3)) {
			skipTouchSkill = false;
		} else if (forceGreaterByregot) {
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
	default:			// Should be touch action skills
		break;
	}
	//std::cout << "Too high quality\n";
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
	case SkillName::VENERATION:
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

	//std::cout << "Too high quality\n";
	return buffSkip;
}

void Crafter::PrintCrafts() {
	std::cout << "Current: ";
	for (const auto& entry : craftingHistory) {
		std::cout << Skills::GetSkillName(entry.skillName) << ", ";
	}
	std::cout << '\n';
}