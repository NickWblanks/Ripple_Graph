#include "Graph.h"
#include <queue>
#include <unordered_set>
#include <cmath>
#include <algorithm>

// ==========================================
// GraphNode Implementation
// ==========================================

GraphNode::GraphNode() 
    : id(0), layer(LayerDepth::Character), entity(nullptr) {}

GraphNode::GraphNode(EntityClass* ent, LayerDepth depth) {
    if (ent != nullptr) {
        this->id = ent->getId();
        this->entity = ent;
    } else {
        this->id = 0;
        this->entity = nullptr;
    }
    this->layer = depth;
}

bool GraphNode::isCharacter() const {
    return this->layer == LayerDepth::Character && this->entity != nullptr;
}

const std::string& GraphNode::getName() const {
    static const std::string fallback = "Unnamed";
    return (this->entity != nullptr) ? this->entity->getName() : fallback;
}

// ==========================================
// Graph Implementation
// ==========================================

Graph::Graph() = default;

void Graph::registerCharacter(CharacterClass& character) {
    this->m_nodes.emplace(
        character.getId(),
        GraphNode(&character, LayerDepth::Character)
    );
}

void Graph::registerEntity(EntityClass& entity, LayerDepth layer) {
    this->m_nodes.emplace(
        entity.getId(),
        GraphNode(&entity, layer)
    );
}

void Graph::setLateralEdge(int nodeA, int nodeB, float affinity, float weight, bool bidirectional) {
    if (this->m_nodes.find(nodeA) == this->m_nodes.end() || 
        this->m_nodes.find(nodeB) == this->m_nodes.end()) {
        return;
    }

    this->m_nodes[nodeA].lateralEdges[nodeB] = GraphEdge{affinity, weight};
    if (bidirectional) {
        this->m_nodes[nodeB].lateralEdges[nodeA] = GraphEdge{affinity, weight};
    }
}

void Graph::assignMembership(int parentId, int childId, float loyalty) {
    if (this->m_nodes.find(parentId) == this->m_nodes.end() || 
        this->m_nodes.find(childId) == this->m_nodes.end()) {
        return;
    }

    // Downward edge: Parent contains Child
    this->m_nodes[parentId].children[childId] = GraphEdge{1.0f, loyalty};

    // Upward edge: Child reports/belongs to Parent
    this->m_nodes[childId].parents[parentId] = GraphEdge{1.0f, loyalty};
}

void Graph::triggerEvent(int targetId, float affinityImpact, const std::string& reason, int eventId) {
    if (this->m_nodes.find(targetId) == this->m_nodes.end()) return;

    struct RippleStep {
        int nodeId;
        float delta;
        int hops;
    };

    std::queue<RippleStep> queue;
    std::unordered_set<int> visited;

    queue.push({targetId, affinityImpact, 0});
    visited.insert(targetId);

    while (!queue.empty()) {
        RippleStep current = queue.front();
        queue.pop();

        int currId = current.nodeId;
        float currDelta = current.delta;
        int hops = current.hops;

        auto& currNode = this->m_nodes[currId];

        // 1. Update user relation & memory across ANY entity type polymorphically
        // Inside triggerEvent loop in Graph.cpp:
        if (currNode.entity != nullptr) {
            // Apply floating delta directly to user affinity
            currNode.entity->adjustUserAffinity(currDelta);

            Memory mem;
            mem.EventID = eventId;
            mem.SourceCharID = -1; // -1 represents the User
            mem.TargetCharID = targetId;
            mem.DeltaImpact = currDelta;
            mem.hops = hops;
            mem.summary = reason;

            currNode.entity->addMemory(mem);
        }
        // 2. Cascade DOWN to child members (Faction -> Characters, Kingdom -> Factions)
        for (const auto& pair : currNode.children) {
            int childId = pair.first;
            const GraphEdge& edge = pair.second;

            if (visited.find(childId) == visited.end()) {
                float downDelta = currDelta * edge.weight * this->verticalDecay;
                if (std::abs(downDelta) >= this->minCutoff) {
                    visited.insert(childId);
                    queue.push({childId, downDelta, hops + 1});
                }
            }
        }

        // 3. Cascade UP to parent groups (Member notoriety rolls up to Faction)
        for (const auto& pair : currNode.parents) {
            int parentId = pair.first;
            const GraphEdge& edge = pair.second;

            if (visited.find(parentId) == visited.end()) {
                float upDelta = currDelta * edge.weight * (this->verticalDecay * 0.5f);
                if (std::abs(upDelta) >= this->minCutoff) {
                    visited.insert(parentId);
                    queue.push({parentId, upDelta, hops + 1});
                }
            }
        }

        // 4. Cascade LATERALLY to peer nodes (Allies, rivals, neutral neighbors)
        for (const auto& pair : currNode.lateralEdges) {
            int peerId = pair.first;
            const GraphEdge& edge = pair.second;

            if (visited.find(peerId) == visited.end()) {
                float peerDelta = currDelta * edge.affinity * this->lateralDecay;
                if (std::abs(peerDelta) >= this->minCutoff) {
                    visited.insert(peerId);
                    queue.push({peerId, peerDelta, hops + 1});
                }
            }
        }
    }
}

const GraphNode* Graph::getNode(int id) const {
    auto it = this->m_nodes.find(id);
    return (it != this->m_nodes.end()) ? &it->second : nullptr;
}

const std::unordered_map<int, GraphNode>& Graph::getAllNodes() const {
    return this->m_nodes;
}