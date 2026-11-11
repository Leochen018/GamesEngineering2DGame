#pragma once
struct GameStatistics
{
    int score = 0;
    int kills = 0;
    int shotsFired = 0;
    int shotsTaken = 0;
    int damageTaken = 0;
    int healthPotionsUsed = 0;
    int manaPotionsUsed = 0;
    float timeSurvived = 0.0f;
    

    void Print() const
    {
        std::cout << "Score:               " << score << "\n";
        std::cout << "Kills:               " << kills << "\n";
        std::cout << "Shots fired:         " << shotsFired << "\n";
        std::cout << "Shots taken:         " << shotsTaken << "\n";
        std::cout << "Damage taken:        " << damageTaken << "\n";
        std::cout << "Health potions used: " << healthPotionsUsed << "\n";
        std::cout << "Mana potions used:   " << manaPotionsUsed << "\n";
        std::cout << "Time survived:       " << timeSurvived << "\n";

    }
};