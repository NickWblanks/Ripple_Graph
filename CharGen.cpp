#include "CharGen.h"

namespace {
    static std::mt19937 rng(1337);
}

const std::vector<std::string> FirstNames = {
    "Aria", "Boran", "Caelen", "Darius", "Elena", "Fenn", "Gideon", "Halia",
    "Ignis", "Jarek", "Kael", "Lyra", "Marek", "Nadia", "Orin", "Phaedra",
    "Quinn", "Rowan", "Soren", "Talia", "Urien", "Valen", "Wren", "Xander",
    "Yara", "Zephyr", "Alden", "Briony", "Corin", "Daphne", "Eamon", "Freya",
    "Garrick", "Harlan", "Isolde", "Joss", "Keira", "Lucan", "Mira", "Niall",
    "Ophelia", "Perrin", "Rhys", "Silas", "Thorne", "Una", "Vesper", "Warrick",
    "Yvette", "Zane"
};

std::unordered_map<int, CharacterClass> generate500Characters() {
    std::unordered_map<int, CharacterClass> characters;
    // Initial user affinity distributed across neutral / slight leanings [-0.3, 0.3]
    std::uniform_real_distribution<float> userAffinityGen(-0.3f, 0.3f);
    // Peer-to-peer personal relations across wide spectrum [-0.8, 0.8]
    std::uniform_real_distribution<float> peerAffinityGen(-0.8f, 0.8f);

    characters.reserve(500);

    for (int i = 0; i < 500; ++i) {
        std::string name = FirstNames[i % FirstNames.size()] + "_" + std::to_string(i);
        float initialAffinity = userAffinityGen(rng);

        CharacterClass character(i, name, initialAffinity);
        //character.addDialogue("I feel positively towards <character>.");
        //character.addDialogue("I feel neutral towards <character>.");
        //character.addDialogue("I feel negative towards <character>.");

        characters.emplace(i, character);
    }

    // Sparse local connections (avg 6 peer connections per NPC)
    std::uniform_int_distribution<int> peerCountGen(4, 8);
    std::uniform_int_distribution<int> targetGen(0, 499);

    for (int i = 0; i < 500; ++i) {
        int count = peerCountGen(rng);
        for (int k = 0; k < count; ++k) {
            int peerId = targetGen(rng);
            if (peerId != i) {
                float affinity = peerAffinityGen(rng);
                characters[i].setPersonalAffinity(peerId, affinity);
                characters[peerId].setPersonalAffinity(i, affinity);
            }
        }
    }

    return characters;
}

void populateHierarchicalWorld(
    Graph& graph,
    std::unordered_map<int, CharacterClass>& characters,
    std::vector<GroupClass>& kingdoms,
    std::vector<GroupClass>& groups
) {
    std::uniform_real_distribution<float> affinityGen(-1.0f, 1.0f);

    // 1. Register Characters in Graph & connect direct lateral edges
    for (auto& [id, ch] : characters) {
        graph.registerCharacter(ch);
    }

    for (const auto& [id, ch] : characters) {
        for (const auto& [peerId, affinity] : ch.getAllPersonalAffinity()) {
            if (id < peerId) {
                // Direct interpersonal connection in the graph
                graph.setLateralEdge(id, peerId, affinity, 0.7f, true);
            }
        }
    }

    // 2. Kingdoms (Layer 2)
    std::vector<std::string> kNames = {"Avalon", "Ryker", "Bulva", "Astar", "Murmund"};
    kingdoms.reserve(5);
    for (int i = 0; i < 5; ++i) {
        kingdoms.emplace_back(1000 + i, kNames[i], "Sovereign State", 500, 0.0f);
        graph.registerEntity(kingdoms.back(), LayerDepth::State);
    }

    // Connect ALL Kingdoms to each other
    for (int i = 0; i < 5; ++i) {
        for (int j = i + 1; j < 5; ++j) {
            float affinity = affinityGen(rng);
            graph.setLateralEdge(1000 + i, 1000 + j, affinity, 1.0f, true);
        }
    }

    // 3. Groups / Factions (Layer 1) - 18 Groups: IDs 500 to 517
    std::vector<std::string> gNames = {
        "Ironclad Vanguard", "Shadow Syndicate", "Silvercrest Merchants", "Obsidian Order",
        "Crimson Corsairs", "Verdant Wardens", "Gilded Hand", "Silent Fang Cult",
        "Northern Hunters", "Steamworks Union", "Brotherhood of Steel", "Dune Stalkers",
        "Crown Inquisitors", "Sovereign Fleet", "Rust Walkers", "Storm Watch",
        "Moonlit Blades", "Black Sun Circle"
    };

    groups.reserve(18);
    for (int i = 0; i < 18; ++i) {
        int groupId = 500 + i; // <--- NO LONGER OVERLAPS WITH CHARACTERS (0-499)
        groups.emplace_back(groupId, gNames[i], "Faction Doctrine", 150, 0.0f);
        graph.registerEntity(groups.back(), LayerDepth::Faction);
    }

    // Connect ALL Factions to each other
    for (int i = 0; i < 18; ++i) {
        for (int j = i + 1; j < 18; ++j) {
            float affinity = affinityGen(rng);
            graph.setLateralEdge(500 + i, 500 + j, affinity, 1.0f, true);
        }
    }

    // 4. Assign Groups to Operating Kingdoms (Vertical L2 -> L1)
    for (int g = 0; g < 18; ++g) {
        int primaryKingdom = 1000 + (g % 5);
        graph.assignMembership(primaryKingdom, 500 + g, 0.85f);

        if (g == 1 || g == 2 || g == 4) {
            int secondaryKingdom = 1000 + ((g + 2) % 5);
            graph.assignMembership(secondaryKingdom, 500 + g, 0.5f);
        }
    }

    // 5. Assign Characters to Groups (Vertical L1 -> L0)
    for (int c = 0; c < 500; ++c) {
        int assignedGroup = 500 + (c % 18);
        graph.assignMembership(assignedGroup, c, 0.8f);
    }
}