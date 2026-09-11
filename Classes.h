#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

enum class Disposition {
    Nemesis,   // [-1.00, -0.65)  -> Active blood feud / Kill-on-sight
    Hostile,   // [-0.65, -0.25)  -> Distrustful, aggressive, uncooperative
    Unfriendly,// [-0.25, -0.08)  -> Cold, dismissive, resentful
    Neutral,   // [-0.08, +0.08]  -> Indifferent, transactional, unbiased
    Friendly,  // (+0.08, +0.40]  -> Warm, cooperative, willing to trade
    Allied,    // (+0.40, +0.75]  -> Loyal, supportive in conflicts, trusted
    Devoted,   // (+0.75, +1.00]  -> Love, sworn allegiance, soulbound
    Stranger   // No graph connection exists (Characters only)
};

struct Memory {
    int EventID;
    int SourceCharID;
    int TargetCharID;
    float DeltaImpact; // Now tracks floating delta directly
    int hops;
    std::string summary;
};

class EntityClass {
protected:
    int id;
    std::string name;
    float userAffinity; // -1.0 (Nemesis) to +1.0 (Ally), default 0.0 (Neutral)
    std::vector<Memory> memoryLog;

public:
    EntityClass(int id, std::string name, float initialUserAffinity = 0.0f);
    virtual ~EntityClass() = default;

    int getId() const;
    const std::string& getName() const;
    float getUserAffinity() const;

    void setUserAffinity(float val);
    void adjustUserAffinity(float delta);

    Disposition getUserDisposition() const;
    static Disposition evaluateDisposition(float affinity);
    

    void addMemory(const Memory& mem);
    const std::vector<Memory>& getMemories() const;
};

class CharacterClass : public EntityClass {
private:
    //std::vector<std::string> dialogue;
    std::unordered_map<int, float> personalAffinity; // Maps peerId -> affinity [-1.0, 1.0]

public:
    CharacterClass(int id, std::string name, float initialUserAffinity = 0.0f);
    CharacterClass();

    //void addDialogue(const std::string& line);
    std::string getDialogueFor(const std::string& targetName, Disposition disp) const;
    std::string getDialogueForCharacter(int characterId, const std::string& characterName) const;
    std::string getDialogueForUser(const std::string& userName = "User") const;

    void setPersonalAffinity(int targetId, float affinity);
    float getPersonalAffinity(int targetId) const;
    bool hasPersonalRelation(int targetId) const;
    Disposition getDispositionTowards(int targetId) const;
    const std::unordered_map<int, float>& getAllPersonalAffinity() const;
};

class GroupClass : public EntityClass {
private:
    int militaryPower;
    std::string doctrine;

public:
    GroupClass(int id, std::string name, std::string doctrine, int power = 100, float initialUserAffinity = 0.0f);

    int getMilitaryPower() const;
    void setMilitaryPower(int power);
    const std::string& getDoctrine() const;
};