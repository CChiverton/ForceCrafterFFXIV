#pragma once

#include <iostream>

class Item {
public:
	Item(int maxProgress, int maxQuality, int maxDurability) {
		maxItemState.progress = maxProgress;
		currentItemState.progress = 0;
		maxItemState.quality = maxQuality;
		currentItemState.quality = 0;
		maxItemState.durability = maxDurability;
		currentItemState.durability = maxItemState.durability;
	}

	void UpdateDurability(int durability) {
		currentItemState.durability += durability;
		if (currentItemState.durability > maxItemState.durability)	currentItemState.durability = maxItemState.durability;
	}

	
	void AddProgress(int progress, int durabilityCost) {
		currentItemState.progress += progress;
		UpdateDurability(-durabilityCost);
	}

	void AddQuality(int quality, int durabilityCost) {
		currentItemState.quality += quality;
		UpdateDurability(-durabilityCost);
	}

	// Return false if item needs deleted and re-added
	// Return true if the item can still be worked on
	bool IsItemWorkable() const {
		if (IsItemCrafted() || IsItemBroken()) {
			//OutputStats();
			return false;
		}

		return true;
	}

	bool IsItemCrafted() const {
		if (currentItemState.progress < maxItemState.progress) {
			return false;
		}
		return true;
	}

	bool IsItemMaxQuality() const {
		if (currentItemState.quality < maxItemState.quality) {
			return false;
		}
		return true;
	}

	int GetDurability() const {
		return currentItemState.durability;
	}

	int GetCurrentProgress() const {
		return currentItemState.progress;
	}

	int GetMaxProgress() const {
		return maxItemState.progress;
	}

	int GetCurrentQuality() const {
		return currentItemState.quality;
	}


	void OutputStats() const {
		std::cout << "Progress: " << currentItemState.progress << "/" << maxItemState.progress << '\n';
		std::cout << "Quality: " << currentItemState.quality << "/" << maxItemState.quality << '\n';
		std::cout << "Durability: " << currentItemState.durability << "/" << maxItemState.durability << '\n';
	}

	

	struct ItemState {
		int progress{};
		int quality{};
		int durability{};
	};

	void LoadItemState(ItemState& itemState) {
		currentItemState = itemState;
	}

	ItemState GetItemState() const {
		//OutputStats();
		return currentItemState;
	}

	bool IsItemBroken() const {
		if (currentItemState.durability > 0) {
			return false;
		}
		//std::cout << "Item is broken\n";
		return true;
	}

private:
	ItemState currentItemState, maxItemState;

	

	

	bool IsItemStillCraftable() const {
		if (currentItemState.durability <= 0) {
			return false;
		}

		return true;
	}

};