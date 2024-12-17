#pragma once

#include <iostream>

class Item {
public:
	Item(int maxProgress, int maxQuality, int maxDurability) {
		this->maxProgress = maxProgress;
		currentProgress = 0;
		this->maxQuality = maxQuality;
		currentQuality = 0;
		this->maxDurability = maxDurability;
		currentDurability = this->maxDurability;
	}

	void UpdateDurability(int durability) {
		currentDurability += durability;
		if (currentDurability > maxDurability)	currentDurability = maxDurability;
	}

	
	void AddProgress(int progress, int durabilityCost) {
		currentProgress += progress;
		UpdateDurability(-durabilityCost);
	}

	void AddQuality(int quality, int durabilityCost) {
		currentQuality += quality;
		UpdateDurability(-durabilityCost);
	}

	// Return false if item needs deleted and re-added
	// Return true if the item can still be worked on
	bool IsItemWorkable() {
		if (IsItemCrafted() || IsItemBroken()) {
			//OutputStats();
			return false;
		}

		return true;
	}

	bool IsItemCrafted() {
		if (currentProgress < maxProgress) {
			return false;
		}
		return true;
	}

	bool IsItemMaxQuality() {
		if (currentQuality < maxQuality) {
			return false;
		}
		return true;
	}

	int GetDurability() {
		return currentDurability;
	}

	int GetCurrentProgress() {
		return currentProgress;
	}

	int GetMaxProgress() {
		return maxProgress;
	}

	int GetCurrentQuality() {
		return currentQuality;
	}

	

private:
	int currentProgress{}, maxProgress{};
	int currentQuality{}, maxQuality{};
	int currentDurability{}, maxDurability{};

	bool IsItemBroken() {
		if (currentDurability > 0) {
			return false;
		}
		//std::cout << "Item is broken\n";
		return true;
	}

	

	bool IsItemStillCraftable() {
		if (currentDurability <= 0) {
			return false;
		}

		return true;
	}

	void OutputStats() {
		std::cout << "Progress: " << currentProgress << "/" << maxProgress << '\n';
		std::cout << "Quality: " << currentQuality << "/" << maxQuality << '\n';
		std::cout << "Durability: " << currentDurability << "/" << maxDurability << '\n';
	}
};