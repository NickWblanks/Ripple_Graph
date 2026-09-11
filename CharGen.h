#pragma once

#include <unordered_map>
#include <vector>
#include <random>
#include "Classes.h"
#include "Graph.h"

// Generates 500 characters and wires sparse interpersonal peer relationships
std::unordered_map<int, CharacterClass> generate500Characters();

// Populates Kingdoms (L2), Groups (L1), and associates 500 Characters (L0) into the Graph
void populateHierarchicalWorld(
    Graph& graph,
    std::unordered_map<int, CharacterClass>& characters,
    std::vector<GroupClass>& kingdoms,
    std::vector<GroupClass>& groups
);