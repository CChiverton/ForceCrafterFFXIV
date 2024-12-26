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

	ResetPlayerStats();
	
	if (forceMaxQuality) {
		playerState.currentTurn = 2;
		FindMinQualityForMax();
		if (!successfulQualityCrafts.empty()) {
			for (const auto& entry : successfulQualityCrafts[bestQualityTime]) {
				for (const auto& move : entry) {
					++minTouchSkills;
				}
				break;
				std::cout << '\n';
			}
			craftingHistory.clear();
			std::cout << "The minimum number of touch skills required to achieve max quality is " << minTouchSkills << '\n';
		}
		else {
			std::cout << "There was no way to find max quality with your QP100 and the maximum number of steps.\n";
		}
		ResetPlayerStats();
		RemoveItem();
		AddItem(maxProgress, maxQuality, maxDurability);
	}

	SaveCraftingHistory(SkillName::NONE);
	
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

void Crafter::FindMinQualityForMax() {
	//std::cout << playerState.currentCP << '\n';
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
			LoadLastCraftingRecord(craftingRecord);
		}
		else if (!craftableItem->IsItemBroken()) {
			SaveCraftingHistory(move.skillName);
			FindMinQualityForMax();
		}
		else if (craftableItem->IsItemBroken()) {
			LoadLastCraftingRecord(craftingRecord);
		}
	}
}

void Crafter::CraftAndRecord(const SkillTest& move) {
	if (Craft(move)) {

		if (craftableItem->IsItemCrafted()) {
			if (forceMaxQuality && !craftableItem->IsItemMaxQuality()) {
				//std::cout << "Not maximum quality when needed\n";
				LoadLastCraftingRecord(craftingRecord);
				return;
			}
			SaveCraftingHistory(move.skillName);
			AddSuccessfulCraft();
			ContinueCraft();
		}
		else if (playerState.currentTurn >= maxTurnLimit || (playerState.currentTime + 3) > bestTime) {		// can't use lastMove here, causes some form of memory leak
			//std::cout << "Run out of moves\n";
			LoadLastCraftingRecord(craftingRecord);
			return;
		}
		else if (craftingRecord.player.buffInfo.finalAppraisal == 1 && (craftableItem->GetMaxProgress() - craftableItem->GetCurrentProgress()) != 1) {		// not appraised
			LoadLastCraftingRecord(craftingRecord);
			return;
		}
		else if (!craftableItem->IsItemBroken()) {
			SaveCraftingHistory(move.skillName);
			ForceCraft();
		}
		else if (craftableItem->IsItemBroken()) {
			LoadLastCraftingRecord(craftingRecord);
			return;
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
	int remainingTime = bestTime - craftingRecord.currentTime;
	bool lastMove = ((remainingTime < 5) || craftingRecord.player.currentTurn == maxTurnLimit - 1) ? true : false; // Only one move left to match the best time and turn limit
	bool secondToLastMove = (remainingTime < 7 || craftingRecord.player.currentTurn == maxTurnLimit - 2) ? true : false;
	actionTracker->ProgressBuffs(craftingRecord.player.buffInfo.innovationActive, craftingRecord.player.buffInfo.wasteNotActive,
								craftingRecord.player.buffInfo.greatStridesActive, craftingRecord.player.buffInfo.innovationActive);
	bool isMaxQuality = craftableItem->IsItemMaxQuality();
	int itemDurability = craftableItem->GetMaxDurability() - craftableItem->GetDurability();

	bool requireTouch = actionTracker->ActionsUsedDuringBuff(4, craftingRecord.player.buffInfo.innovation, 3, actionTracker->touchActionUsed, 2)	// If there is only one buff use it may as well be great strides
						|| actionTracker->ActionsUsedDuringBuff(3, craftingRecord.player.buffInfo.greatStrides, 2, actionTracker->touchActionUsed, 1) //ActionUsedDuringBuff(craftingRecord.player.buffInfo.greatStrides, actionTracker->touchActionUsed, 0b11)
						|| (secondToLastMove && forceMaxQuality && !isMaxQuality) || 
							((minTouchSkills - actionTracker->numTouchSkillsUsed) == ((maxTurnLimit - 1) - (playerState.currentTurn)));
	bool requireSynth = ActionUsedDuringBuff(craftingRecord.player.buffInfo.veneration, actionTracker->synthActionUsed, 0b111);
	bool requireAppraisal = ActionUsedDuringBuff(craftingRecord.player.buffInfo.finalAppraisal, actionTracker->synthActionUsed, 0b1111);

	bool synthActionRequired = lastMove || requireSynth || requireAppraisal;
	//bool skipForTouch = secondToLastMove && forceMaxQuality && !isMaxQuality;

	if (playerState.currentTurn == 1) {
		StarterCraft();
	}
	else {
		if (!(!forceMaxQuality || isMaxQuality || synthActionRequired)) {
			QualityCraft();
		}
		if (!isMaxQuality && forceMaxQuality) {		// Better to use a different synthesis skill if quality isn't required
			OtherCraft();
		}
		if (!requireTouch) {
			SynthesisCraft();
		}

		if (!(synthActionRequired || requireTouch)) {
			BuffCraft();
			if (!(secondToLastMove && itemDurability >= 20)) {
				RepairCraft(itemDurability);
			}
		}
	}
	/*if (!(synthActionRequired || (secondToLastMove && itemDurability >= 20) || requireTouch)) {
		RepairCraft(previousStep, finalAppraisalTimer);
	}*/

	ContinueCraft();
	//std::cout << player->GetCurrentTurn() << " TRIED ALL POSSIBLE MOVES AT THIS LEVEL\n";
}


/*----------------------PRIVATE-------------------------------------*/

void Crafter::StarterCraft() {
	for (const SkillTest& move : startingMoveList) {
		std::cout << "Scanned moves\n";
		CraftAndRecord(move);
	}
}

void Crafter::SynthesisCraft() {
	bool checkForCarefulGroundWork = (((actionTracker->prudentSynthesis & 0b11) | (actionTracker->groundwork & 0b11)) == 0b11);	// either have been used for the past two turns	
	for (const SkillTest& move : synthesisSkills) {
		if (SimilarTrees(move.skillName))	continue;

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

void Crafter::RepairCraft(int remainingDurability) {
	for (const SkillTest& move : repairSkills) {
		if (playerState.lastSkillUsed == SkillName::MASTERSMEND)	continue;		//If previously used this skill, it would have been more effective to use Immaculate Mend
		if (move.skillName == SkillName::IMMACULATEMEND && remainingDurability <= 30)	continue;	// better to use masters mend here
		if (remainingDurability > 15)	continue;		// Arbritrary number, more of a logical "Why repair at this stage"
		CraftAndRecord(move);
	}
}

void Crafter::OtherCraft() {
	for (const SkillTest& move : otherSkills) {
		CraftAndRecord(move);
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

void Crafter::AddSuccessfulQualityCraft() {
	bestQualityTime = craftingRecord.currentTime;		// Time restraints already managed by force craft
	std::vector<SkillName> success{};
	//success.reserve(craftingHistory.size());
	for (const auto& entry : craftingHistory) {
		success.emplace_back(entry.skillName);
	}
	successfulQualityCrafts[craftingRecord.currentTime].emplace_back(success);
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