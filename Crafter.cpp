#include "Crafter.hpp"

#define ProgressUpdate 1

Crafter::Crafter(std::vector<Skills::SkillTest> startingMoves, int maxCP, float progressPerHundred, float qualityPerHundred, int maxProgress, int maxQuality, int maxDurability, bool forceQuality, bool greaterByregot, int maximumTurnLimit)
	: forceMaxQuality(forceQuality), forceGreaterByregot(greaterByregot), maxTurnLimit(maximumTurnLimit),
	Player(maxCP, progressPerHundred, qualityPerHundred) {
	AddItem(maxProgress, maxQuality, maxDurability);

	int durabilityCosts{ 0 };
	int twentyCosts{ 0 };
	SaveCraftingHistory(SkillName::NONE);
	if (forceMaxQuality) {
		FindFastestQuality(durabilityCosts, twentyCosts);
	}

	FindFastestSynth(durabilityCosts, twentyCosts);
	FindDurabilityCost(durabilityCosts, twentyCosts);
	
	playerState.currentTurn = 2;
	LoadLastCraftingRecord();

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

	PrintSuccessfulCrafts();
}

void Crafter::FindFastestQuality(int& durabilityCosts, int& twentyCosts) {
	playerState.currentTurn = 2;
	FindMinQualityForMax();
	if (successfulQualityCrafts.empty()) {
		std::cout << "There was no way to find max quality with your QP100 and the maximum number of steps.\n";
		return;
	}
	minTouchSkills = successfulQualityCrafts[bestQualityTime][0].size();
	AddItem(craftableItem->GetMaxProgress(), craftableItem->GetMaxQuality(), craftableItem->GetMaxDurability());
	int durability = 0;
	for (const auto& skill : successfulQualityCrafts[bestQualityTime][0]) {
		if (skill == SkillName::NONE)	continue;
		for (const auto& entry : skillTest) {		// find skill efficiency
			if (entry.skillName == skill) {
				if (entry.efficiency == 0) {
					BuffSkills(skill);
				}
				else {
					TouchSkills(skill, 0, durability);
					durabilityCosts += entry.costDurability;
					if (entry.costDurability == 20) ++twentyCosts;
				}
				break;
			}

		}

		bestQuality.emplace(bestQuality.begin(), craftableItem->GetRemainingQuality());
	}
	int difference = bestQuality[0];
	for (auto& entry : bestQuality) {
		entry -= difference;
	}
	--minTouchSkills;	// Deletes the number from SkillName::NONE
		std::cout << "The minimum number of touch skills required to achieve max quality is " << minTouchSkills << '\n';
}

void Crafter::FindFastestSynth(int& durabilityCosts, int& twentyCosts) {
	playerState.currentTurn = 3;
	LoadLastCraftingRecord();
	FindMinSynthForMax();
	if (successfulSynthCrafts.empty()) {
		std::cout << "There was no way to craft this item.\n";
		return;
	}
	minSynthSkills = successfulSynthCrafts[bestSynthTime][0].size();
	AddItem(craftableItem->GetMaxProgress(), craftableItem->GetMaxQuality(), craftableItem->GetMaxDurability());
	int durability = 0;
	for (const auto& skill : successfulSynthCrafts[bestSynthTime][0]) {
		if (skill == SkillName::NONE)	continue;
		for (const auto& entry : skillTest) {		// find skill efficiency
			if (entry.efficiency == 0) {
				BuffSkills(skill);
			}
			else if (entry.skillName == skill) {
				SynthesisSkills(entry.skillName, durability, entry.efficiency);
				durabilityCosts += entry.costDurability;
				if (entry.costDurability == 20) ++twentyCosts;
			}

		}
		//std::cout << Skills::GetSkillName(skill) << '\n';
		bestSynth.emplace(bestSynth.begin(), craftableItem->GetRemainingProgress());
		//std::cout << craftableItem->GetRemainingProgress() << '\n';
	}
	int difference = bestSynth[0];
	for (auto& entry : bestSynth) {
		entry -= difference;
	}
	std::cout << "The minimum number of synth skills required to craft the item is " << minSynthSkills << '\n';
}

void Crafter::FindDurabilityCost(int& durabilityCosts, int& twentyCosts) {
	std::cout << "The total durability lost to attain crafted item is: " << durabilityCosts << '\n';
	durabilityCosts -= 10;		// emulating starter move
	while (durabilityCosts > craftableItem->GetMaxDurability()) {
		if (twentyCosts >= 6) {
			durabilityCosts -= 60;		// emulating the strongest casts durability savings
			twentyCosts -= 6;
			++minDurabilitySkills;
		}
		if (craftableItem->GetMaxDurability() < durabilityCosts) {
			durabilityCosts -= 40;	// emulating manipulation
			++minDurabilitySkills;
		}
		if (durabilityCosts > craftableItem->GetMaxDurability()) {
			++minDurabilitySkills;		// emulating a repair action
		}
	}
	std::cout << "The estimated durability after finishing crafting for " << minDurabilitySkills << " durability-saving skills is " << craftableItem->GetMaxDurability() - durabilityCosts << '\n';

}


void Crafter::ForceCraft() {
	if (invalid) return;

	if (playerState.currentTurn == 1) {
		StarterCraft();
	}
	else {
		bool requireQuality = forceMaxQuality && !craftableItem->IsItemMaxQuality();
		int remainingTime = bestTime - craftingRecord.currentTime;
		
		bool synthActionRequired = remainingTime < 5
				|| actionTracker.ActionsUsedDuringBuff(4, craftingRecord.player.buffInfo.veneration, 3, actionTracker.synthActionUsed, 2)				// Requires a synth action
				|| (GetBuffDuration(SkillName::FINALAPPRAISAL) == 1 && craftableItem->GetRemainingProgress() != 1);										// Requires a synth action
		

		if (requireQuality) {
			if (!synthActionRequired) {
				QualityCraft();
			}
			OtherCraft();
		}

		if (!actionTracker.ActionsUsedDuringBuff(4, craftingRecord.player.buffInfo.innovation, 3, actionTracker.touchActionUsed, 2)	// If there is only one buff use it may as well be great strides
			&& !GetBuffDuration(SkillName::GREATSTRIDES) == 1							// Great strides will be wasted if anything other than a touch skill is used
			&& !(remainingTime < 7 && requireQuality)									// In the last two steps and quality isn't required
			&& !(remainingTime == (bestQualityTime - actionTracker.touchTime))) {		// There aren't quality skills needed to match the fastest found quality level
			SynthesisCraft();

			if (!synthActionRequired) {
				BuffCraft();
				int repairableDurability = craftableItem->GetRemainingDurability();
				if ((repairableDurability >= 25)) {
					RepairCraft(repairableDurability);
				}
			}
		}	
		ContinueCraft();
	}
}


/*----------------------PRIVATE-------------------------------------*/

void Crafter::CraftAndRecord(const SkillTest& move) {
	if (CastSkill(move)) {
		const bool requireQuality = forceMaxQuality && !craftableItem->IsItemMaxQuality();
		
		if (craftableItem->IsItemCrafted()) {
			if (requireQuality) {
				LoadLastCraftingRecord();
				return;
			}
			AddSuccessfulCraft(move.skillName);
			return;
		}

		const int remainingCraftTurns = maxTurnLimit - playerState.currentTurn;
		const int remainingCraftTime = bestTime - playerState.currentTime;
		int minQualityTurnsLeft = 0;
		int maxQualityTime = 0;

		if (requireQuality && remainingCraftTurns < minTouchSkills) {
			CalculateRemainingQualityTime(minQualityTurnsLeft, maxQualityTime);
		}

		int minSynthTurnsLeft = 1;
		int maxSynthTime = 0;
		CalculateRemainingSynthTime(minSynthTurnsLeft, maxSynthTime);
		int minDurabilityTurnsLeft = minDurabilitySkills - actionTracker.numDurabilitySkillsUsed;
		if (minDurabilityTurnsLeft < 0)	minDurabilityTurnsLeft = 0;

		/* Prediction based crafting */
		if (remainingCraftTime <= maxQualityTime + maxSynthTime
			|| (minQualityTurnsLeft + minSynthTurnsLeft + minDurabilityTurnsLeft) > maxTurnLimit - playerState.currentTurn
			|| actionTracker.touchTime > bestQualityTime				// Quality crafting should not exceed this limit
			|| actionTracker.synthTime == bestSynthTime) {				// As the final synth should craft the object it should never reach here
			LoadLastCraftingRecord();
			return;
		} 
		/* Current state based crafting */
		else if (playerState.currentTurn >= maxTurnLimit || (playerState.currentTime + 3) > bestTime
			|| (craftingRecord.player.buffInfo.finalAppraisal == 1 && craftableItem->GetRemainingProgress() != 1)	// not appraised
			|| craftableItem->IsItemBroken()) {
			LoadLastCraftingRecord();
			return;
		}
		else {
			SaveCraftingHistory(move.skillName);
			ForceCraft();
		}
#if ProgressUpdate
		if (playerState.currentTurn == baseTurn) {
			std::cout << Skills::GetSkillName(move.skillName) << " completed\n";
		}
		else if (playerState.currentTurn == baseTurn + 1) {
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

void Crafter::FindMinSynthForMax() {
	for (const SkillTest& move : synthesisSkills) {
		craftableItem->UpdateDurability(1000);
		SynthOnlyCrafts(move);
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
		else if (playerState.currentTurn >= maxTurnLimit || (playerState.currentTime + 3) > bestQualityTime
			|| craftableItem->IsItemBroken()) {
			LoadLastCraftingRecord();
		}
		else {
			SaveCraftingHistory(move.skillName);
			FindMinQualityForMax();
		}
	}
}

void Crafter::SynthOnlyCrafts(const SkillTest& move) {
	if (CastSkill(move)) {
		if (craftableItem->IsItemCrafted()) {
			SaveCraftingHistory(move.skillName);
			AddSuccessfulSynthCraft();
			ContinueCraft();
		}
		else if (playerState.currentTurn >= maxTurnLimit || (playerState.currentTime + 3) > bestSynthTime
			|| craftableItem->IsItemBroken()) {
			LoadLastCraftingRecord();
		}
		else {
			SaveCraftingHistory(move.skillName);
			FindMinSynthForMax();
		}
	}
}

void Crafter::CalculateRemainingQualityTime(int& minQualityTurnsLeft, int& maxQualityTime) {
	int remainingQuality = craftableItem->GetRemainingQuality();
	minQualityTurnsLeft = 1;
	//std::cout << maxQualityTime << '\n';
	for (minQualityTurnsLeft; remainingQuality > bestQuality[minQualityTurnsLeft]; ++minQualityTurnsLeft) {
		if (bestQuality[minQualityTurnsLeft] == bestQuality[minQualityTurnsLeft - 1]) {
			maxQualityTime += 2;
		}
		else {
			maxQualityTime += 3;
		}
	}
}

void Crafter::CalculateRemainingSynthTime(int& minSynthTurnsLeft, int& maxSynthTime) {
	int remainingProgress = craftableItem->GetRemainingProgress();
	for (minSynthTurnsLeft; remainingProgress > bestSynth[minSynthTurnsLeft]; ++minSynthTurnsLeft) {
		if (bestSynth[minSynthTurnsLeft] == bestSynth[minSynthTurnsLeft - 1]) {
			maxSynthTime += 2;
		}
		else {
			maxSynthTime += 3;
		}
	}
}

void Crafter::StarterCraft() {
	for (const SkillTest& move : startingMoveList) {
		CraftAndRecord(move);
	}
}

void Crafter::SynthesisCraft() {
	bool checkForCarefulGroundWork = (((actionTracker.prudentSynthesis & 0b11) | (actionTracker.groundwork & 0b11)) == 0b11);	// either have been used for the past two turns	
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
	int itemDurability = playerState.buffInfo.wasteNotActive ? craftableItem->GetCurrentDurability() * 2 : craftableItem->GetCurrentDurability();	// double it to act as if the skill is half cost
	for (const SkillTest& move : qualitySkills) {
		if (itemDurability <= move.costDurability) continue;	// won't catch prudent touch but that is still caught by the craft process
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
	bool basic = actionTracker.basicSynthesis & 0b1;
	bool careful = actionTracker.carefulSynthesis & 0b1;
	bool prudent = actionTracker.prudentSynthesis & 0b1;
	bool ground = actionTracker.groundwork & 0b1;
	int history = actionTracker.venerationHistory & 0b11;
	int wasteNotHistory = actionTracker.wasteNotHistory & 0b11;
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
	actionTracker.Backtrack();
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
inline void Crafter::SaveCraftingRecord(SkillName skillName) {
	craftingRecord.player = playerState;
	craftingRecord.item = craftableItem->GetItemState();
	craftingRecord.currentTime = playerState.currentTime;
	craftingRecord.skillName = skillName;
}
inline void Crafter::SaveCraftingHistory(SkillName skillName) {
	SaveCraftingRecord(skillName);
	craftingHistory.at(playerState.currentTurn) = craftingRecord;
	actionTracker.Progress(skillName, craftingRecord.player.buffInfo.innovationActive, craftingRecord.player.buffInfo.wasteNotActive,
		craftingRecord.player.buffInfo.greatStridesActive, craftingRecord.player.buffInfo.innovationActive);
}

inline void Crafter::DeleteCraftingHistory() {
	//craftingHistory.pop_back();
}

inline std::vector<SkillName> Crafter::CompileSuccessfulCraft() {
	std::vector<SkillName> success{};
	for (int i{ 2 }; i <= playerState.currentTurn; ++i) {
		success.emplace_back(craftingHistory[i].skillName);
	}
	return success;
}

void Crafter::AddSuccessfulQualityCraft() {
	if (craftingRecord.currentTime > bestQualityTime)	return;
	bestQualityTime = craftingRecord.currentTime;
	successfulQualityCrafts[craftingRecord.currentTime].emplace_back(CompileSuccessfulCraft());
}

void Crafter::AddSuccessfulSynthCraft() {
	if (craftingRecord.currentTime > bestSynthTime)	return;
	bestSynthTime = craftingRecord.currentTime;
	successfulSynthCrafts[craftingRecord.currentTime].emplace_back(CompileSuccessfulCraft());
}

void Crafter::AddSuccessfulCraft(SkillName skillName) {
	SaveCraftingRecord(skillName);
	if (bestTime > craftingRecord.currentTime) {
		std::cout << "New best time found!: " << craftingRecord.currentTime << "s\n";
		//std::cout << actionTracker.numDurabilitySkillsUsed << " durability actions used\n";
	}
	bestTime = craftingRecord.currentTime;		// Time restraints already managed by force craft

	std::vector<SkillName> success{};
	for (int i{ 1 }; i < playerState.currentTurn; ++i) {
		success.emplace_back(craftingHistory[i].skillName);
	}
	success.emplace_back(craftingRecord.skillName);
	successfulCrafts[craftingRecord.currentTime].emplace_back(success);
	LoadLastCraftingRecord();
}

inline void Crafter::LoadLastCraftingRecord() {
	LoadPlayerStats(craftingHistory[playerState.currentTurn-1].player);
	craftableItem->LoadItemState(craftingHistory[playerState.currentTurn].item);
	craftingRecord = craftingHistory[playerState.currentTurn];
}

void Crafter::PrintCrafts() {
	std::cout << "Current: ";
	for (const auto& entry : craftingHistory) {
		std::cout << Skills::GetSkillName(entry.skillName) << ", ";
	}
	std::cout << '\n';
}

void Crafter::PrintSuccessfulCrafts() {
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