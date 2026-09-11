#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "Classes.h"

class EntityClass;
class CharacterClass;

enum class LayerDepth {
    Character = 0, // Layer 0: Individual NPCs
    Faction   = 1, // Layer 1: Gangs, Guilds, Syndicates
    State     = 2, // Layer 2: Kingdoms, Empires, Regions
    User      = 3  // Apex: The player
};

struct GraphEdge {
    float affinity = 0.0f; // -1.0 (Hostile) to +1.0 (Allied)
    float weight = 1.0f;   // Transmission/Loyalty factor (0.0 to 1.0)
};

class GraphNode {
public:
    int id;
    LayerDepth layer;
    EntityClass* entity; // Points to polymorphic CharacterClass or GroupClass

    // Horizontal edges (peers on same Z level)
    std::unordered_map<int, GraphEdge> lateralEdges;

    // Vertical edges DOWN (entities contained inside this one)
    std::unordered_map<int, GraphEdge> children;

    // Vertical edges UP (entities this one reports to / operates in)
    std::unordered_map<int, GraphEdge> parents;

    GraphNode();
    GraphNode(EntityClass* ent, LayerDepth depth);

    bool isCharacter() const;
    const std::string& getName() const;
};

class Graph {
private:
    std::unordered_map<int, GraphNode> m_nodes;

    float lateralDecay = 0.6f;
    float verticalDecay = 0.8f;
    float minCutoff = 0.04f;

public:
    Graph();

    // Node registration
    void registerCharacter(CharacterClass& character);
    void registerEntity(EntityClass& entity, LayerDepth layer);

    // Edge management
    void setLateralEdge(int nodeA, int nodeB, float affinity, float weight = 1.0f, bool bidirectional = true);
    void assignMembership(int parentId, int childId, float loyalty = 0.8f);

    // Event ripple propagation across 3D graph
    void triggerEvent(int targetId, float affinityImpact, const std::string& reason, int eventId = 1);

    // Lookups
    const GraphNode* getNode(int id) const;
    const std::unordered_map<int, GraphNode>& getAllNodes() const;
};