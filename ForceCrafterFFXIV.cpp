// ForceCrafterFFXIV.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Crafter.hpp"
#include "Player.hpp"
#include "Setup.hpp"
#include "Skills.hpp"

#define NDEBUG 0

int main()
{
    std::vector<SkillTest> skills = { skillTest[4], skillTest[(int)SkillName::MANIPULATION], skillTest[(int)SkillName::WASTENOTII]};

#if NDEBUG
    int maxCP = SelectCP();
    int progressPerHundred = SelectProgress();
    int qualityPerHundred = SelectQuality();
    int itemMaxProgress = SelectMaxProgress();
    int itemMaxQuality = SelectMaxQuality();
    int itemMaxDurability = SelectMaxDurability();

    bool startCrafting = false;
    while (!startCrafting) {
        std::cout << "Current skills are: ";
        for (const auto& entry : skills) {
            std::cout << Skills::GetSkillName(entry.skillName) << ",";
        }
        int move;
        std::cout << "\nPlease select your starting moves:\n";
        std::cout << "1:Muscle Memory\n2:Reflect\n3:Manipulation\n4:Waste Not II\n5:Start Crafting\n6:Remove last skill\n";
        std::cin >> move;
        switch (move) {
        case 1:
            skills.emplace_back(skillTest[(int)SkillName::MUSCLEMEMORY]);
            break;
        case 2:
            skills.emplace_back(skillTest[(int)SkillName::REFLECT]);
            break;
        case 3:
            skills.emplace_back(skillTest[(int)SkillName::MANIPULATION]);
            break;
        case 4:
            skills.emplace_back(skillTest[(int)SkillName::WASTENOTII]);
            break;
        case 5:
            startCrafting = true;
            break;
        case 6:
            skills.pop_back();
            break;
        default:
            std::cout << "Not a valid input\n";
        }
        std::cout << '\n';

}
#endif

#if NDEBUG
    Crafter crafter(skills, maxCP, progressPerHundred, qualityPerHundred, itemMaxProgress, itemMaxQuality, itemMaxDurability, true, true, 15);
#else
    Crafter crafter(skills, 630, 331, 397, 3000, 11000, 40, true, true, 15);
#endif

    
    crafter.ForceCraft();

    std::cin;

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
