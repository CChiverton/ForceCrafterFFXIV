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
	Item() {
		maxItemState.progress = 0;
		currentItemState.progress = 0;
		maxItemState.quality = 0;
		currentItemState.quality = 0;
		maxItemState.durability = 0;
		currentItemState.durability = 0;
	}
	struct ItemState {
		int progress{};
		int quality{};
		int durability{};
	};
	inline ItemState GetItemState() const {
		return currentItemState;
	}
	void LoadItemState(const ItemState& itemState) {
		currentItemState = itemState;
	}

	/* CHANGE ITEM STATS */
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

	/* PROGRESS */
	inline int GetCurrentProgress() const {
		return currentItemState.progress;
	}
	inline int GetMaxProgress() const {
		return maxItemState.progress;
	}
	inline int GetRemainingProgress() const {
		return maxItemState.progress - currentItemState.progress;
	}
	bool IsItemCrafted() const {
		return currentItemState.progress >= maxItemState.progress;
	}

	/* QUALITY */
	inline int GetCurrentQuality() const {
		return currentItemState.quality;
	}
	inline int GetMaxQuality() const {
		return maxItemState.quality;
	}
	inline int GetRemainingQuality() const {
		return maxItemState.quality - currentItemState.quality;
	}
	bool IsItemMaxQuality() const {
		return currentItemState.quality >= maxItemState.quality;
	}

	/* DURABILITY*/
	inline int GetCurrentDurability() const {
		return currentItemState.durability;
	}
	inline int GetMaxDurability() const {
		return maxItemState.durability;
	}
	inline int GetRemainingDurability() const {
		return maxItemState.durability - currentItemState.durability;
	}


	inline bool IsItemBroken() const {
		return currentItemState.durability <= 0;
	}
	inline bool IsItemCraftable() const {
		return currentItemState.durability > 0;
	}

	void OutputStats() const {
		std::cout << "Progress: " << currentItemState.progress << "/" << maxItemState.progress << '\n';
		std::cout << "Quality: " << currentItemState.quality << "/" << maxItemState.quality << '\n';
		std::cout << "Durability: " << currentItemState.durability << "/" << maxItemState.durability << '\n';
	}
private:
	ItemState currentItemState, maxItemState;
};