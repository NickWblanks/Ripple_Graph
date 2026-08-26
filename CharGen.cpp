#include "CharGen.h"

const std::vector<std::string> FirstNames = {
    "Aria", "Boran", "Caelen", "Darius", "Elena", "Fenn", "Gideon", "Halia",
    "Ignis", "Jarek", "Kael", "Lyra", "Marek", "Nadia", "Orin", "Phaedra",
    "Quinn", "Rowan", "Soren", "Talia", "Urien", "Valen", "Wren", "Xander",
    "Yara", "Zephyr", "Alden", "Briony", "Corin", "Daphne", "Eamon", "Freya",
    "Garrick", "Harlan", "Isolde", "Joss", "Keira", "Lucan", "Mira", "Niall",
    "Ophelia", "Perrin", "Rhys", "Silas", "Thorne", "Una", "Vesper", "Warrick",
    "Yvette", "Zane"
};

std::unordered_map<int, CharacterClass> generate50Characters() {
    std::unordered_map<int, CharacterClass> characters;

    std::mt19937 rng(1337);
    std::uniform_int_distribution<int> distGen(1, 15);

    for (int i = 0; i < 50; ++i) {
        int charId = i;
        std::string name = FirstNames[i % FirstNames.size()];
        int userDist = distGen(rng);

        CharacterClass character(charId, name, userDist);
        character.addDialogue("I feel positively towards <character>.");
        character.addDialogue("I feel neutral towards <character>.");
        character.addDialogue("I feel negative towards <character>.");

        characters.emplace(charId, character);
    }

    for (int i = 1; i <= 50; ++i) {
        for (int j = i + 1; j <= 50; ++j) {
            int relDistance = distGen(rng);
            characters[i].setRelationship(j, relDistance);
            characters[j].setRelationship(i, relDistance);
        }
    }

    return characters;
}