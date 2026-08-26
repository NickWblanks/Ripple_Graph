#include "characterClass.h"

CharacterClass::CharacterClass(int id, std::string name, int initialUserRelation) {
    this->CharID = id;
    this->CharName = name;
    this->UserRelation = initialUserRelation;
}

CharacterClass::CharacterClass() {
    this->CharID = 0;
    this->CharName = "Unnamed";
    this->UserRelation = 8;
}

int CharacterClass::getId() const { return this->CharID; }
const std::string& CharacterClass::getName() const { return this->CharName; }
int CharacterClass::getUserRelation() const { return this->UserRelation; }

int CharacterClass::getRelationTo(int characterId) const {
    auto it = this->charRelationalNodes.find(characterId);
    return (it != this->charRelationalNodes.end()) ? it->second : 8;
}

Disposition CharacterClass::getUserDisposition() const {
    return evaluateDisposition(this->UserRelation);
}

Disposition CharacterClass::getDispositionTowards(int characterId) const {
    return evaluateDisposition(this->getRelationTo(characterId));
}

Disposition CharacterClass::evaluateDisposition(int distance) {
    if (distance <= 5) return Disposition::Friendly;
    if (distance <= 10) return Disposition::Neutral;
    return Disposition::Hostile;
}

void CharacterClass::setRelationship(int characterId, int distance) {
    this->charRelationalNodes[characterId] = std::clamp(distance, 1, 15);
}

void CharacterClass::setUserRelation(int distance) {
    this->UserRelation = std::clamp(distance, 1, 15);
}

void CharacterClass::adjustUserRelation(int deltaDistance) {
    this->UserRelation = std::clamp(this->UserRelation + deltaDistance, 1, 15);
}

void CharacterClass::addMemory(const Memory& mem) {
    this->MemoryLog.push_back(mem);
}

const std::vector<Memory>& CharacterClass::getMemories() const {
    return this->MemoryLog;
}

const std::unordered_map<int, int>& CharacterClass::getAllRelations() const {
    return this->charRelationalNodes;
}

void CharacterClass::addDialogue(const std::string& line) {
    this->Dialogue.push_back(line);
}

std::string CharacterClass::getDialogueFor(const std::string& targetName, Disposition disp) const {
    if (this->Dialogue.size() < 3) return "...";

    std::string line;
    switch (disp) {
        case Disposition::Friendly: line = this->Dialogue[0]; break;
        case Disposition::Neutral:  line = this->Dialogue[1]; break;
        case Disposition::Hostile:  line = this->Dialogue[2]; break;
    }

    std::string placeholder = "<character>";
    size_t pos = line.find(placeholder);
    if (pos != std::string::npos) {
        line.replace(pos, placeholder.length(), targetName);
    }
    return line;
}

std::string CharacterClass::getDialogueForCharacter(int characterId, const std::string& characterName) const {
    Disposition disp = this->getDispositionTowards(characterId);
    return this->getDialogueFor(characterName, disp);
}

std::string CharacterClass::getDialogueForUser(const std::string& userName) const {
    Disposition disp = this->getUserDisposition();
    return this->getDialogueFor(userName, disp);
}