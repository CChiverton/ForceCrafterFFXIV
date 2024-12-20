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