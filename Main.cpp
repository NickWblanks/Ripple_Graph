#include <iostream>
#include <iomanip>
#include <limits>
#include <vector>
#include <unordered_map>
#include "CharGen.h"
#include "Graph.h"

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
        case Disposition::Devoted:    return "Devoted / Love";
        case Disposition::Allied:     return "Allied";
        case Disposition::Friendly:   return "Friendly";
        case Disposition::Neutral:    return "Neutral";
        case Disposition::Unfriendly: return "Unfriendly";
        case Disposition::Hostile:    return "Hostile";
        case Disposition::Nemesis:    return "Nemesis";
        case Disposition::Stranger:   return "Stranger (No direct bond)";
    }
    return "Unknown";
}

int main() {
    std::cout << "Initializing 3D Hierarchical Social Network...\n";

    Graph graph;
    std::vector<GroupClass> kingdoms;
    std::vector<GroupClass> groups;
    auto characters = generate500Characters();

    populateHierarchicalWorld(graph, characters, kingdoms, groups);

    std::cout << "Loaded 5 Kingdoms, 18 Factions, and 500 Characters.\n\n";

    bool running = true;
    int eventCounter = 1;

    while (running) {
        std::cout << "====================================\n";
        std::cout << "       RELATIONAL GRAPH MENU        \n";
        std::cout << "====================================\n";
        std::cout << "1. View all Characters (0-499)\n";
        std::cout << "2. View Character (0-499)\n";
        std::cout << "3. View Relationship between A and B\n";
        std::cout << "4. View Kingdoms (State Layer 2)\n";
        std::cout << "5. View Groups / Factions (Layer 1)\n";
        std::cout << "6. Simulate Ripple (Help / Hurt)\n";
        std::cout << "7. Exit\n";
        std::cout << "====================================\n";

        int choice = getValidInt("Enter your choice (1-7): ", 1, 7);
        std::cout << "\n";

        switch (choice) {
            case 1: {
                std::cout << std::left 
                          << std::setw(8)  << "ID" 
                          << std::setw(20) << "Name" 
                          << std::setw(16) << "User Affinity" 
                          << "Disposition\n";
                std::cout << std::string(56, '-') << "\n";

                for (int i = 0; i < 500; ++i) {
                    if (characters.find(i) == characters.end()) continue;
                    const auto& ch = characters[i];
                    std::cout << std::left 
                              << std::setw(8)  << ch.getId()
                              << std::setw(20) << ch.getName()
                              << std::showpos << std::fixed << std::setprecision(2)
                              << std::setw(16) << ch.getUserAffinity()
                              << std::noshowpos
                              << getDispositionString(ch.getUserDisposition()) << "\n";
                }
                std::cout << "\n";
                break;
            }

            case 2: {
                int id = getValidInt("Enter Character ID (0-499): ", 0, 499);
                if (characters.find(id) == characters.end()) {
                    std::cout << "Character not found.\n\n";
                    break;
                }

                const auto& ch = characters[id];
                std::cout << "\n========================================\n";
                std::cout << "           CHARACTER PROFILE            \n";
                std::cout << "========================================\n";
                std::cout << "ID:               " << ch.getId() << "\n";
                std::cout << "Name:             " << ch.getName() << "\n";
                std::cout << "User Affinity:    " << std::showpos << std::fixed << std::setprecision(2)
                          << ch.getUserAffinity() << std::noshowpos << " (" 
                          << getDispositionString(ch.getUserDisposition()) << ")\n";
                
                // NPC speaking directly to the Player
                std::cout << "Voice to You:     \"" << ch.getDialogueForUser("User") << "\"\n";
                std::cout << "----------------------------------------\n";
                std::cout << "Peer Connections: " << ch.getAllPersonalAffinity().size() << " direct acquaintances\n";
                
                std::cout << "Known Acquaintances:\n";
                for (const auto& [peerId, aff] : ch.getAllPersonalAffinity()) {
                    std::cout << "  - [" << peerId << "] " << characters[peerId].getName() 
                              << " | Affinity: " << std::showpos << std::fixed << std::setprecision(2) 
                              << aff << std::noshowpos << " (" 
                              << getDispositionString(ch.getDispositionTowards(peerId)) << ")\n";
                }

                if (!ch.getMemories().empty()) {
                    std::cout << "\nRecent Event Memory:\n";
                    for (const auto& mem : ch.getMemories()) {
                        std::cout << "  - [Hops: " << mem.hops << "] " << mem.summary 
                                  << " (Delta: " << std::showpos << std::fixed << std::setprecision(2)
                                  << mem.DeltaImpact << std::noshowpos << ")\n";
                    }
                }
                std::cout << "========================================\n\n";
                break;
            }

            case 3: {
                int idA = getValidInt("Enter Character A ID (0-499): ", 0, 499);
                int idB = getValidInt("Enter Character B ID (0-499): ", 0, 499);

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

                std::cout << "\n========================================\n";
                std::cout << "         INTERPERSONAL DYNAMICS         \n";
                std::cout << "========================================\n";

                // Character A -> Character B
                std::cout << charA.getName() << " (ID " << idA << ") -> " << charB.getName() << " (ID " << idB << "):\n";
                if (!charA.hasPersonalRelation(idB)) {
                    std::cout << "  Status:   Stranger (No direct bond)\n";
                    std::cout << "  Dialogue: \"" << charA.getDialogueForCharacter(idB, charB.getName()) << "\"\n";
                } else {
                    float affAB = charA.getPersonalAffinity(idB);
                    std::cout << "  Affinity: " << std::showpos << std::fixed << std::setprecision(2) 
                              << affAB << std::noshowpos << " (" << getDispositionString(charA.getDispositionTowards(idB)) << ")\n";
                    std::cout << "  Dialogue: \"" << charA.getDialogueForCharacter(idB, charB.getName()) << "\"\n";
                }

                std::cout << "----------------------------------------\n";

                // Character B -> Character A
                std::cout << charB.getName() << " (ID " << idB << ") -> " << charA.getName() << " (ID " << idA << "):\n";
                if (!charB.hasPersonalRelation(idA)) {
                    std::cout << "  Status:   Stranger (No direct bond)\n";
                    std::cout << "  Dialogue: \"" << charB.getDialogueForCharacter(idA, charA.getName()) << "\"\n";
                } else {
                    float affBA = charB.getPersonalAffinity(idA);
                    std::cout << "  Affinity: " << std::showpos << std::fixed << std::setprecision(2) 
                              << affBA << std::noshowpos << " (" << getDispositionString(charB.getDispositionTowards(idA)) << ")\n";
                    std::cout << "  Dialogue: \"" << charB.getDialogueForCharacter(idA, charA.getName()) << "\"\n";
                }
                std::cout << "========================================\n\n";
                break;
            }

            case 4: {
                std::cout << "--- KINGDOMS (STATE LAYER 2) ---\n";
                std::cout << std::left 
                          << std::setw(8)  << "ID" 
                          << std::setw(15) << "Name" 
                          << std::setw(12) << "Power" 
                          << std::setw(16) << "User Affinity" 
                          << std::setw(15) << "Disposition"
                          << "Memories\n";
                std::cout << std::string(75, '-') << "\n";

                for (const auto& k : kingdoms) {
                    std::cout << std::left 
                              << std::setw(8)  << k.getId()
                              << std::setw(15) << k.getName()
                              << std::setw(12) << k.getMilitaryPower()
                              << std::showpos << std::fixed << std::setprecision(2)
                              << std::setw(16) << k.getUserAffinity()
                              << std::noshowpos
                              << std::setw(15) << getDispositionString(k.getUserDisposition())
                              << k.getMemories().size() << "\n";
                }
                std::cout << "\n";

                int viewId = getValidInt("Enter Kingdom ID to view details (1000-1004) or 0 to return: ", 0, 1004);
                if (viewId >= 1000 && viewId <= 1004) {
                    const auto& k = kingdoms[viewId - 1000];
                    const auto* node = graph.getNode(viewId);

                    std::cout << "\n--- " << k.getName() << " Details ---\n";
                    std::cout << "Doctrine:      " << k.getDoctrine() << "\n";
                    std::cout << "User Standing: " << std::showpos << std::fixed << std::setprecision(2)
                              << k.getUserAffinity() << std::noshowpos << " (" 
                              << getDispositionString(k.getUserDisposition()) << ")\n";

                    if (node != nullptr) {
                        std::cout << "Member Factions (Children):\n";
                        for (const auto& [childId, edge] : node->children) {
                            const auto* childNode = graph.getNode(childId);
                            if (childNode != nullptr) {
                                std::cout << "  - [" << childId << "] " << childNode->getName() 
                                          << " (Loyalty Weight: " << std::fixed << std::setprecision(2) 
                                          << edge.weight << ")\n";
                            }
                        }
                    }

                    if (!k.getMemories().empty()) {
                        std::cout << "Recent Memory Log:\n";
                        for (const auto& mem : k.getMemories()) {
                            std::cout << "  - [Hops: " << mem.hops << "] " << mem.summary 
                                      << " (Delta: " << std::showpos << std::fixed << std::setprecision(2)
                                      << mem.DeltaImpact << std::noshowpos << ")\n";
                        }
                    }
                }
                std::cout << "\n";
                break;
            }

            case 5: {
                std::cout << "--- GROUPS / FACTIONS (MESO LAYER 1) ---\n";
                std::cout << std::left 
                          << std::setw(8)  << "ID" 
                          << std::setw(26) << "Name" 
                          << std::setw(16) << "User Affinity" 
                          << std::setw(15) << "Disposition"
                          << "Memories\n";
                std::cout << std::string(75, '-') << "\n";

                for (const auto& g : groups) {
                    std::cout << std::left 
                              << std::setw(8)  << g.getId()
                              << std::setw(26) << g.getName()
                              << std::showpos << std::fixed << std::setprecision(2)
                              << std::setw(16) << g.getUserAffinity()
                              << std::noshowpos
                              << std::setw(15) << getDispositionString(g.getUserDisposition())
                              << g.getMemories().size() << "\n";
                }
                std::cout << "\n";

                int viewId = getValidInt("Enter Faction ID to view details (500-517) or 0 to return: ", 0, 517);
                if (viewId >= 500 && viewId <= 517) {
                    const auto& g = groups[viewId - 500];
                    const auto* node = graph.getNode(viewId);

                    std::cout << "\n--- " << g.getName() << " Details ---\n";
                    std::cout << "Doctrine:      " << g.getDoctrine() << "\n";
                    std::cout << "Power:         " << g.getMilitaryPower() << "\n";
                    std::cout << "User Standing: " << std::showpos << std::fixed << std::setprecision(2)
                              << g.getUserAffinity() << std::noshowpos << " (" 
                              << getDispositionString(g.getUserDisposition()) << ")\n";

                    if (node != nullptr) {
                        std::cout << "Affiliated Sovereign Kingdom (Parent):\n";
                        for (const auto& [parentId, edge] : node->parents) {
                            const auto* pNode = graph.getNode(parentId);
                            if (pNode != nullptr) {
                                std::cout << "  - [" << parentId << "] " << pNode->getName() 
                                          << " (Loyalty Weight: " << std::fixed << std::setprecision(2) 
                                          << edge.weight << ")\n";
                            }
                        }
                        std::cout << "Total Character Members: " << node->children.size() << "\n";
                    }

                    if (!g.getMemories().empty()) {
                        std::cout << "Recent Memory Log:\n";
                        for (const auto& mem : g.getMemories()) {
                            std::cout << "  - [Hops: " << mem.hops << "] " << mem.summary 
                                      << " (Delta: " << std::showpos << std::fixed << std::setprecision(2)
                                      << mem.DeltaImpact << std::noshowpos << ")\n";
                        }
                    }
                }
                std::cout << "\n";
                break;
            }

            case 6: {
                std::cout << "--- SIMULATE EVENT RIPPLE ---\n";
                std::cout << "1. Target Kingdom (State Layer 2)\n";
                std::cout << "2. Target Group / Faction (Meso Layer 1)\n";
                std::cout << "3. Target Individual Character (Micro Layer 0)\n";
                int targetLayerChoice = getValidInt("Select tier (1-3): ", 1, 3);

                std::cout << "\nAction Type:\n";
                std::cout << "1. Help (+ Affinity)\n";
                std::cout << "2. Hurt (- Affinity)\n";
                int actionType = getValidInt("Select action (1-2): ", 1, 2);

                float impact = (actionType == 1) ? 0.60f : -0.60f;
                int targetId = -1;
                std::string targetName = "";

                if (targetLayerChoice == 1) {
                    std::cout << "\nAvailable Kingdoms:\n";
                    for (size_t i = 0; i < kingdoms.size(); ++i) {
                        std::cout << "  " << (1000 + i) << ": " << kingdoms[i].getName() 
                                  << " (Affinity: " << std::showpos << std::fixed << std::setprecision(2) 
                                  << kingdoms[i].getUserAffinity() << std::noshowpos << ")\n";
                    }
                    targetId = getValidInt("Choose Kingdom ID (1000-1004): ", 1000, 1004);
                    targetName = kingdoms[targetId - 1000].getName();

                } else if (targetLayerChoice == 2) {
                    std::cout << "\nAvailable Factions:\n";
                    for (size_t i = 0; i < groups.size(); ++i) {
                        std::cout << "  " << (500 + i) << ": " << groups[i].getName() 
                            << " (Affinity: " << std::showpos << std::fixed << std::setprecision(2) 
                            << groups[i].getUserAffinity() << std::noshowpos << ")\n";
                        }
                    targetId = getValidInt("Choose Faction ID (500-517): ", 500, 517);
                    targetName = groups[targetId - 500].getName();

                } else {
                    targetId = getValidInt("Choose Character ID (0-499): ", 0, 499);
                    targetName = characters[targetId].getName();
                }

                std::string reason = (actionType == 1) 
                    ? ("Player aided " + targetName) 
                    : ("Player attacked " + targetName);

                std::cout << "\n>> Triggering 3D ripple on [" << targetName << "] with impact " 
                          << std::showpos << std::fixed << std::setprecision(2) << impact << std::noshowpos << "...\n";
                graph.triggerEvent(targetId, impact, reason, eventCounter++);
                std::cout << ">> Propagation complete! Check character/group memories and affinities to observe the ripple.\n\n";
                break;
            }

            case 7: {
                std::cout << "Exiting menu.\n";
                running = false;
                break;
            }
        }
    }

    return 0;
}