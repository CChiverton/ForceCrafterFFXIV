#include "Crafter.hpp"

Crafter::Crafter(std::vector<Skills::SkillName> startingMoves, int maxCP, int maxProgress, int maxQuality, int maxDurability, bool forceQuality, bool greaterByregot, int maximumTurnLimit)
	: maxProgress(maxProgress), maxQuality(maxQuality), maxDurability(maxDurability), forceMaxQuality(forceQuality), forceGreaterByregot(greaterByregot), maxTurnLimit(maximumTurnLimit) {
	player = new Player(630);
	player->AddItem(maxProgress, maxQuality, maxDurability);
	playerItem = player->craftableItem;
	actionTracker = new ActionTracker();

	craftingHistory.reserve(maximumTurnLimit);

	if (!startingMoves.empty()) {
		std::cout << "For the starting moves:";
		for (const Skills::SkillName& move : startingMoves) {
			if (!Craft(move)) {
				std::cout << "Invalid. The starting moves break/finish the item.\n";
				invalid = true;
				return;
			}
			std::cout << " " << Skills::GetSkillName(move) << " |";
			SaveCraftingHistory(move);
			//std::cout << "Player turn is " << player->GetCurrentTurn() << '\n';
		}
		std::cout << '\n';
	}
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

void Crafter::CraftAndRecord(SkillName move, CraftingHistory& previousStep, int& finalAppraisalTimer) {
	if (Craft(move)) {

		//std::cout << "Turn " << player->GetCurrentTurn() << ": " << Skills::GetSkillName(move) << '\n';

		if (playerItem->IsItemCrafted()) {
			if (forceMaxQuality && !playerItem->IsItemMaxQuality()) {
				//std::cout << "Not maximum quality when needed\n";
				LoadLastCraftingRecord(previousStep);
				return;
			}
			SaveCraftingHistory(move);
			AddSuccessfulCraft();
			ContinueCraft();
		}
		else if (player->GetCurrentTurn() >= maxTurnLimit || (player->GetCurrentTime() + 3) > bestTime) {		// can't use lastMove here, causes some form of memory leak
			//std::cout << "Run out of moves\n";
			LoadLastCraftingRecord(previousStep);
			return;
		}
		else if (finalAppraisalTimer == 1 && (playerItem->GetMaxProgress() - playerItem->GetCurrentProgress()) != 1) {		// not appraised
			LoadLastCraftingRecord(previousStep);
			return;
		}
		else if (!playerItem->IsItemBroken()) {
			SaveCraftingHistory(move);
			ForceCraft();
		}
		//std::cout << "Finisheng Turn " << player->GetCurrentTurn() + 1 << ": " << Skills::GetSkillName(move) << '\n';
	}
}

void Crafter::ForceCraft() {
	if (invalid) return;
	CraftingHistory& previousStep = craftingRecord;		// stack allocation for faster loading
	bool lastMove = ((previousStep.currentTime + 3) >= bestTime || player->GetCurrentTurn() == maxTurnLimit - 1) ? true : false; // Only one move left to match the best time and turn limit
	bool secondToLastMove = ((previousStep.currentTime + 6) >= bestTime || player->GetCurrentTurn() == maxTurnLimit - 2) ? true : false;
	int innovationTimer = player->GetBuffDuration(SkillName::INNOVATION);

	int venerationTimer = player->GetBuffDuration(SkillName::VENERATION);
	actionTracker->ProgressBuffs(venerationTimer > 0, player->GetBuffDuration(SkillName::WASTENOTI) > 0);
	int finalAppraisalTimer = player->GetBuffDuration(SkillName::FINALAPPRAISAL);
	bool isMaxQuality = playerItem->IsItemMaxQuality();
	int itemDurability = playerItem->GetDurability();


	bool requireTouch = ActionUsedDuringBuff(innovationTimer, touchActionsUsedSuccessfully, 0b111);
	bool requireSynth = ActionUsedDuringBuff(venerationTimer, synthActionsUsedSuccessfully, 0b111);
	bool requireAppraisal = ActionUsedDuringBuff(finalAppraisalTimer, synthActionsUsedSuccessfully, 0b1111);

	bool skip = lastMove || requireSynth || requireAppraisal;
	bool skipForTouch = secondToLastMove && forceMaxQuality && !isMaxQuality;

	if (!(!forceMaxQuality || isMaxQuality || skip)) {
		QualityCraft(previousStep, finalAppraisalTimer);
	}
	touchActionUsed = false;
	if (!(skipForTouch || requireTouch)) {
		SynthesisCraft(previousStep, finalAppraisalTimer);
	}
	touchActionUsed = false;
	OtherCraft(previousStep, finalAppraisalTimer);
	touchActionUsed = false;
	if (!(skip || (secondToLastMove && forceMaxQuality && !isMaxQuality) || requireTouch)) {
		BuffCraft(previousStep, finalAppraisalTimer);
	}
	if (!(skip || (secondToLastMove && (itemDurability >= 20 || (forceMaxQuality && !isMaxQuality))) || requireTouch)) {
		RepairCraft(previousStep, finalAppraisalTimer);
	}

	ContinueCraft();
	//std::cout << player->GetCurrentTurn() << " TRIED ALL POSSIBLE MOVES AT THIS LEVEL\n";
}


/*----------------------PRIVATE-------------------------------------*/

void Crafter::SynthesisCraft(CraftingHistory& previousStep, int& finalAppraisalTimer) {
	
	for (const SkillName& move : synthesisSkills) {
		if (SimilarTrees(move))	continue;
		synthActionUsed = true;

		CraftAndRecord(move, previousStep, finalAppraisalTimer);
	}
}

void Crafter::QualityCraft(CraftingHistory& previousStep, int& finalAppraisalTimer) {
	for (const SkillName& move : qualitySkills) {
		touchActionUsed = false;
		
		if (QualityCheck(move)) {
			continue;
		}

		CraftAndRecord(move, previousStep, finalAppraisalTimer);
	}
}

void Crafter::BuffCraft(CraftingHistory& previousStep, int& finalAppraisalTimer) {
	for (const SkillName& move : buffSkills) {
		if (BuffCheck(move)) {
			continue;
		}

		CraftAndRecord(move, previousStep, finalAppraisalTimer);
	}
}

void Crafter::RepairCraft(CraftingHistory& previousStep, int& finalAppraisalTimer) {
	for (const SkillName& move : repairSkills) {
		CraftAndRecord(move, previousStep, finalAppraisalTimer);
	}
}

void Crafter::OtherCraft(CraftingHistory& previousStep, int& finalAppraisalTimer) {
	for (const SkillName& move : otherSkills) {
		synthActionUsed = true;
		touchActionUsed = true;

		CraftAndRecord(move, previousStep, finalAppraisalTimer);
	}
}

void Crafter::UpdateValidBuffCheck(int& appropriateActionTurn, bool& actionUsedThisTurn) {
	appropriateActionTurn <<= 1;
	appropriateActionTurn |= actionUsedThisTurn;
}

bool Crafter::ActionUsedDuringBuff(int& timeLeft, int& actionHistory, int turns) {
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
	craftingRecord.player = player->GetPlayerState();
	craftingRecord.item = playerItem->GetItemState();
	craftingRecord.currentTime = player->GetCurrentTime();
	craftingRecord.skillName = skillName;
	craftingHistory.emplace_back(craftingRecord);
	actionTracker->ProgressSynthSkills(skillName);
}

inline void Crafter::DeleteCraftingHistory() {
	craftingHistory.pop_back();
}


bool Crafter::Craft(Skills::SkillName skillName) {
	if (!player->CastSkill(skillName)) {
		//std::cout << "Invalid move " << Skills::GetSkillName(skillName) << '\n';
		return false;
	}
	UpdateValidBuffCheck(touchActionsUsedSuccessfully, touchActionUsed);
	UpdateValidBuffCheck(synthActionsUsedSuccessfully, synthActionUsed);
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

inline void Crafter::LoadLastCraftingRecord(CraftingHistory& lastRecord) {
	player->LoadPlayerStats(lastRecord.player);
	playerItem->LoadItemState(lastRecord.item);
	craftingRecord = lastRecord;
	/*std::cout << "After loading item stats are\n";
	player->craftableItem->OutputStats();*/
}

void Crafter::ContinueCraft() {
	DeleteCraftingHistory();
	CraftingHistory& last = craftingHistory.back();
	LoadLastCraftingRecord(last);
	touchActionsUsedSuccessfully >>= 1;
	synthActionsUsedSuccessfully >>= 1;
	actionTracker->BacktrackBuffs();
	actionTracker->BacktrackSynthSkills();
}

bool Crafter::QualityCheck(SkillName skillName) {
	bool skipTouchSkill{ false };
	switch (skillName) {
	case SkillName::GREATSTRIDES:
		skipTouchSkill = player->GetBuffDuration(skillName) != 0;
		break;
	case SkillName::INNOVATION:
		skipTouchSkill = player->GetBuffDuration(skillName) > 1;		// fringe cases you want to recast innovation with 1 turn left
		break;
	case SkillName::BYREGOTSBLESSING:
		if (forceGreaterByregot) {
			skipTouchSkill = player->GetBuffDuration(SkillName::GREATSTRIDES) == 0;
		}
		touchActionUsed = true;
		break;
	default:			// Should be touch action skills
		touchActionUsed = true;
		break;
	}
	//std::cout << "Too high quality\n";
	return skipTouchSkill;
}

bool Crafter::BuffCheck(SkillName skillName) {
	bool buffSkip{ false };
	if (player->GetBuffDuration(skillName) > 0) {			// potentially bad logic
		buffSkip = true;
	}
	switch (skillName) {
	case SkillName::WASTENOTI:
	case SkillName::WASTENOTII:
		break;
	case SkillName::VENERATION:
		break;
	case SkillName::FINALAPPRAISAL:
		if (player->GetCurrentTurn() + 6 >= maxTurnLimit) {
			buffSkip = true;
		}
		else if (!forceMaxQuality || playerItem->IsItemMaxQuality()) {
			buffSkip = true;
		}
		break;
	case SkillName::MANIPULATION:
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