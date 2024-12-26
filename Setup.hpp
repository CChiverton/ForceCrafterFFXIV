#pragma once

int SelectCP() {
    int maxCP{};
    bool confirmed = false;
    std::cout << "Hello! To begin, please input your stats.\n";
    while (!confirmed) {
        int input{};
        std::cout << "Please choose your available CP : \n";
        std::cin >> maxCP;
        std::cout << "Is this your desired CP? " << maxCP << "\n1)Yes\n2)No\n";
        std::cin >> input;
        if (input == 1) confirmed = true;
    }
    return maxCP;
}

int SelectProgress() {
    int progressPerHundred{};
    bool confirmed = false;
    std::cout << "Please input your progress per 100% efficiency. You can find this on FFXIV Teamcraft and putting in your desired Craftsmanship\n";
    while (!confirmed) {
        int input{};
        std::cout << "Please choose your Progress per 100% efficiency : \n";
        std::cin >> progressPerHundred;
        std::cout << "Is this your desired PP100? " << progressPerHundred << "\n1)Yes\n2)No\n";
        std::cin >> input;
        if (input == 1) confirmed = true;
    }
    return progressPerHundred;
}

int SelectQuality() {
    int qualityPerHundred{};
    bool confirmed = false;
    std::cout << "Please input your quality per 100% efficiency. You can find this on FFXIV Teamcraft and putting in your desired Control\n";
    while (!confirmed) {
        int input{};
        std::cout << "Please choose your Quality per 100% efficiency : \n";
        std::cin >> qualityPerHundred;
        std::cout << "Is this your desired QP100? " << qualityPerHundred << "\n1)Yes\n2)No\n";
        std::cin >> input;
        if (input == 1) confirmed = true;
    }
    return qualityPerHundred;
}

int SelectMaxProgress() {
    int maxProgress{};
    bool confirmed = false;
    std::cout << "Now for the item stats.\n";
    while (!confirmed) {
        int input{};
        std::cout << "Please input the required progress to complete the item.\n";
        std::cin >> maxProgress;
        std::cout << "Is this the required progress to craft the item? " << maxProgress << "\n1)Yes\n2)No\n";
        std::cin >> input;
        if (input == 1) confirmed = true;
    }
    return maxProgress;
}

int SelectMaxQuality() {
    int maxQuality{};
    bool confirmed = false;
    while (!confirmed) {
        int input{};
        std::cout << "Please input the required quality for 100% HQ.\n";
        std::cin >> maxQuality;
        std::cout << "Is this the required max quality? " << maxQuality << "\n1)Yes\n2)No\n";
        std::cin >> input;
        if (input == 1) confirmed = true;
    }
    return maxQuality;
}

int SelectMaxDurability() {
    int maxDurability{};
    bool confirmed = false;
    while (!confirmed) {
        int input{};
        std::cout << "What is the max durability of the item?.\n";
        std::cin >> maxDurability;
        std::cout << "Is this the maximum durability? " << maxDurability << "\n1)Yes\n2)No\n";
        std::cin >> input;
        if (input == 1) confirmed = true;
    }
    return maxDurability;
}