#include <iostream>
#include <iomanip>
#include <limits>
#include "CharGen.h"

int getValidInt(const std::string& prompt, int minVal, int maxVal) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value && value >= minVal && value <= maxVal) {
            return value;
        }
        std::cout << "Invalid input. Please enter a number between " << minVal << " and " << maxVal << ".\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

std::string getDispositionString(Disposition disp) {
    switch (disp) {
        case Disposition::Friendly: return "Friendly";
        case Disposition::Neutral:  return "Neutral";
        case Disposition::Hostile:  return "Hostile";
    }
    return "Unknown";
}

int main() {
    std::cout << "Initializing relational network...\n";
    auto characters = generate50Characters();
    std::cout << "Loaded " << characters.size() << " characters.\n\n";

    bool running = true;
    while (running) {
        std::cout << "====================================\n";
        std::cout << "       RELATIONAL GRAPH MENU        \n";
        std::cout << "====================================\n";
        std::cout << "1. View all Characters\n";
        std::cout << "2. View Character (0-49)\n";
        std::cout << "3. View Relationship between A(0-49) and B(0-49)\n";
        std::cout << "4. Simulate (Empty)\n";
        std::cout << "5. Exit\n";
        std::cout << "====================================\n";

        int choice = getValidInt("Enter your choice (1-5): ", 1, 5);
        std::cout << "\n";

        switch (choice) {
            case 1: {
                std::cout << std::left 
                          << std::setw(6)  << "Index"
                          << std::setw(8)  << "ID" 
                          << std::setw(15) << "Name" 
                          << std::setw(16) << "User Distance" 
                          << "Disposition\n";
                std::cout << std::string(58, '-') << "\n";

                for (int i = 0; i < 50; ++i) {
                    if (characters.find(i) == characters.end()) continue;

                    const auto& ch = characters[i];
                    std::cout << std::left 
                              << std::setw(6)  << i
                              << std::setw(8)  << ch.getId()
                              << std::setw(15) << ch.getName()
                              << std::setw(16) << ch.getUserRelation()
                              << getDispositionString(ch.getUserDisposition()) << "\n";
                }
                std::cout << "\n";
                break;
            }

            case 2: {
                int id = getValidInt("Enter Character ID (0-49): ", 0, 49);

                if (characters.find(id) == characters.end()) {
                    std::cout << "Character not found.\n\n";
                    break;
                }

                const auto& ch = characters[id];
                std::cout << "--- CHARACTER DETAILS ---\n";
                std::cout << "Index / ID:     " << ch.getId() << "\n";
                std::cout << "Name:           " << ch.getName() << "\n";
                std::cout << "User Distance:  " << ch.getUserRelation() << " (" 
                          << getDispositionString(ch.getUserDisposition()) << ")\n";
                std::cout << "User Dialogue:  \"" << ch.getDialogueForUser("User") << "\"\n";
                std::cout << "Memory Count:   " << ch.getMemories().size() << "\n\n";
                break;
            }

            case 3: {
                int idA = getValidInt("Enter Character A ID (0-49): ", 0, 49);
                int idB = getValidInt("Enter Character B ID (0-49): ", 0, 49);

                if (idA == idB) {
                    std::cout << "A character cannot evaluate a relationship with themselves.\n\n";
                    break;
                }

                if (characters.find(idA) == characters.end() || characters.find(idB) == characters.end()) {
                    std::cout << "One or both characters not found.\n\n";
                    break;
                }

                const auto& charA = characters[idA];
                const auto& charB = characters[idB];

                int distAB = charA.getRelationTo(idB);
                auto dispAB = charA.getDispositionTowards(idB);

                int distBA = charB.getRelationTo(idA);
                auto dispBA = charB.getDispositionTowards(idA);

                std::cout << "--- RELATIONSHIP DETAILS ---\n";
                std::cout << charA.getName() << " (ID " << idA << ") -> " << charB.getName() << " (ID " << idB << "):\n";
                std::cout << "  Distance:    " << distAB << "\n";
                std::cout << "  Disposition: " << getDispositionString(dispAB) << "\n";
                std::cout << "  Dialogue:    \"" << charA.getDialogueForCharacter(idB, charB.getName()) << "\"\n\n";

                std::cout << charB.getName() << " (ID " << idB << ") -> " << charA.getName() << " (ID " << idA << "):\n";
                std::cout << "  Distance:    " << distBA << "\n";
                std::cout << "  Disposition: " << getDispositionString(dispBA) << "\n";
                std::cout << "  Dialogue:    \"" << charB.getDialogueForCharacter(idA, charA.getName()) << "\"\n\n";
                break;
            }

            case 4: {
                std::cout << "Simulation engine not yet configured.\n\n";
                break;
            }

            case 5: {
                std::cout << "Exiting menu.\n";
                running = false;
                break;
            }
        }
    }

    return 0;
}