#pragma once

#include <iostream>

class Item {
public:
	Item(uint16_t maxProgress, uint16_t maxQuality, int16_t maxDurability) {
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
		uint16_t progress{};
		uint16_t quality{};
		int16_t durability{};
	};
	inline ItemState GetItemState() const {
		return currentItemState;
	}
	void LoadItemState(const ItemState& itemState) {
		currentItemState = itemState;
	}

	/* CHANGE ITEM STATS */
	void UpdateDurability(int16_t durability) {
		currentItemState.durability += durability;
		if (currentItemState.durability > maxItemState.durability)	currentItemState.durability = maxItemState.durability;
	}
	void AddProgress(uint16_t progress, int16_t durabilityCost) {
		currentItemState.progress += progress;
		UpdateDurability(-durabilityCost);
	}
	void AddQuality(uint16_t quality, int16_t durabilityCost) {
		currentItemState.quality += quality;
		UpdateDurability(-durabilityCost);
	}

	/* PROGRESS */
	inline uint16_t GetCurrentProgress() const {
		return currentItemState.progress;
	}
	inline uint16_t GetMaxProgress() const {
		return maxItemState.progress;
	}
	inline int32_t GetRemainingProgress() const {
		return maxItemState.progress - currentItemState.progress;
	}
	bool IsItemCrafted() const {
		return currentItemState.progress >= maxItemState.progress;
	}

	/* QUALITY */
	inline uint16_t GetCurrentQuality() const {
		return currentItemState.quality;
	}
	inline uint16_t GetMaxQuality() const {
		return maxItemState.quality;
	}
	inline int32_t GetRemainingQuality() const {
		return maxItemState.quality - currentItemState.quality;
	}
	bool IsItemMaxQuality() const {
		return currentItemState.quality >= maxItemState.quality;
	}

	/* DURABILITY*/
	inline int16_t GetCurrentDurability() const {
		return currentItemState.durability;
	}
	inline int16_t GetMaxDurability() const {
		return maxItemState.durability;
	}
	inline int16_t GetRemainingDurability() const {
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