#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <algorithm>
#include <cmath>
#include <random>

enum class Disposition { Friendly, Neutral, Hostile };

struct Memory {
    int EventID;
    int SourceCharID;
    int TargetCharID;
    int DeltaImpact;
    int hops;
    std::string summary;
};

class CharacterClass {
private:
    int CharID;
    std::string CharName;
    int UserRelation;

    std::unordered_map<int, int> charRelationalNodes;
    std::vector<Memory> MemoryLog;
    std::vector<std::string> Dialogue;

public:
    CharacterClass(int id, std::string name, int initialUserRelation = 8);
    CharacterClass();

    int getId() const;
    const std::string& getName() const;
    int getUserRelation() const;
    int getRelationTo(int characterId) const;

    Disposition getUserDisposition() const;
    Disposition getDispositionTowards(int characterId) const;
    static Disposition evaluateDisposition(int distance);

    void setRelationship(int characterId, int distance);
    void setUserRelation(int distance);
    void adjustUserRelation(int deltaDistance);

    void addMemory(const Memory& mem);
    const std::vector<Memory>& getMemories() const;
    const std::unordered_map<int, int>& getAllRelations() const;

    void addDialogue(const std::string& line);
    std::string getDialogueFor(const std::string& targetName, Disposition disp) const;
    std::string getDialogueForCharacter(int characterId, const std::string& characterName) const;
    std::string getDialogueForUser(const std::string& userName = "User") const;
};