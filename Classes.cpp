#include "Classes.h"

// ==========================================
// EntityClass Implementation
// ==========================================

EntityClass::EntityClass(int id, std::string name, float initialUserAffinity) {
    this->id = id;
    this->name = std::move(name);
    this->userAffinity = std::clamp(initialUserAffinity, -1.0f, 1.0f);
}

int EntityClass::getId() const { return this->id; }
const std::string& EntityClass::getName() const { return this->name; }
float EntityClass::getUserAffinity() const { return this->userAffinity; }

void EntityClass::setUserAffinity(float val) {
    this->userAffinity = std::clamp(val, -1.0f, 1.0f);
}

void EntityClass::adjustUserAffinity(float delta) {
    this->userAffinity = std::clamp(this->userAffinity + delta, -1.0f, 1.0f);
}

Disposition EntityClass::getUserDisposition() const {
    return evaluateDisposition(this->userAffinity);
}

Disposition EntityClass::evaluateDisposition(float affinity) {
    if (affinity > 0.75f)   return Disposition::Devoted;
    if (affinity > 0.40f)   return Disposition::Allied;
    if (affinity > 0.08f)   return Disposition::Friendly;
    if (affinity >= -0.08f) return Disposition::Neutral;
    if (affinity >= -0.25f) return Disposition::Unfriendly;
    if (affinity >= -0.65f) return Disposition::Hostile;
    return Disposition::Nemesis;
}

void EntityClass::addMemory(const Memory& mem) {
    this->memoryLog.push_back(mem);
}

const std::vector<Memory>& EntityClass::getMemories() const {
    return this->memoryLog;
}

// ==========================================
// CharacterClass Implementation
// ==========================================

CharacterClass::CharacterClass(int id, std::string name, float initialUserAffinity)
    : EntityClass(id, std::move(name), initialUserAffinity) {}

CharacterClass::CharacterClass()
    : EntityClass(0, "Unnamed", 0.0f) {}
/*
void CharacterClass::addDialogue(const std::string& line) {
    this->dialogue.push_back(line);
}
*/



std::string CharacterClass::getDialogueFor(const std::string& targetName, Disposition disp) const {
    switch (disp) {
        case Disposition::Devoted:
            return "My heart and blade are yours, " + targetName + ".";
        case Disposition::Allied:
            return "Stand firm, " + targetName + ". We fight together.";
        case Disposition::Friendly:
            return "Good to see you, " + targetName + ". How can I help?";
        case Disposition::Neutral:
            return "State your business, " + targetName + ".";
        case Disposition::Unfriendly:
            return "I don't trust you, " + targetName + ". Make it brief.";
        case Disposition::Hostile:
            return "Draw steel or walk away, " + targetName + ".";
        case Disposition::Nemesis:
            return "I'll carve your name into the dirt, " + targetName + "!";
        case Disposition::Stranger:
            return "I don't know who " + targetName + " is.";
    }
    return "...";
}

std::string CharacterClass::getDialogueForCharacter(int characterId, const std::string& characterName) const {
    Disposition disp = this->getDispositionTowards(characterId);
    return this->getDialogueFor(characterName, disp);
}

std::string CharacterClass::getDialogueForUser(const std::string& userName) const {
    return this->getDialogueFor(userName, this->getUserDisposition());
}

void CharacterClass::setPersonalAffinity(int targetId, float affinity) {
    this->personalAffinity[targetId] = std::clamp(affinity, -1.0f, 1.0f);
}

float CharacterClass::getPersonalAffinity(int targetId) const {
    auto it = this->personalAffinity.find(targetId);
    return (it != this->personalAffinity.end()) ? it->second : 0.0f;
}

bool CharacterClass::hasPersonalRelation(int targetId) const {
    return this->personalAffinity.find(targetId) != this->personalAffinity.end();
}

Disposition CharacterClass::getDispositionTowards(int targetId) const {
    if (!hasPersonalRelation(targetId)) {
        return Disposition::Stranger;
    }
    return evaluateDisposition(this->getPersonalAffinity(targetId));
}

const std::unordered_map<int, float>& CharacterClass::getAllPersonalAffinity() const {
    return this->personalAffinity;
}

// ==========================================
// GroupClass Implementation
// ==========================================

GroupClass::GroupClass(int id, std::string name, std::string doctrine, int power, float initialUserAffinity)
    : EntityClass(id, std::move(name), initialUserAffinity),
      militaryPower(power),
      doctrine(std::move(doctrine)) {}

int GroupClass::getMilitaryPower() const { return this->militaryPower; }
void GroupClass::setMilitaryPower(int power) { this->militaryPower = power; }
const std::string& GroupClass::getDoctrine() const { return this->doctrine; }