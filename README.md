# Ripple_Graph

A multi-tiered, 3D relational graph simulation in C++ that models sociopolitical dynamics, loyalty chains, and butterfly-effect event propagation across macroscopic and microscopic entities.

Nodes represent discrete entities (Kingdoms, Factions, and Characters), while weighted directional edges represent normalized social affinity and fidelity. High-impact events ripple dynamically across lateral alliances, descend down hierarchical command structures, and bubble up to sovereign governing bodies.

---

## Architecture Overview

The simulation organizes entities across discrete structural tiers along a vertical Z-axis:

* **Layer 2 (State / Macro):** Sovereign nations, kingdoms, and planetary empires (IDs `1000–1004`).
* **Layer 1 (Faction / Meso):** Guilds, cartels, syndicates, and military orders operating within states (IDs `500–517`).
* **Layer 0 (Character / Micro):** Individual NPCs, citizens, and operatives (IDs `0–499`).
* **Layer 3 (User / Apex):** The player entity whose actions introduce external forces into the network.

### Propagation & Affinity Mechanics
* **Continuous Range:** Relational affinity is evaluated on a normalized floating-point spectrum `[-1.00, +1.00]`, removing integer quantization loss and enabling small secondary ripples to accumulate over time.
* **Asymmetric & Sparse Edges:** Interpersonal and inter-faction networks are generated with realistic sparsity rather than complete graphs, preserving the concept of strangers and unacquainted parties.
* **3D Ripple Cascades:**
  * **Lateral Propagation:** Transmits across peer alliances or rivalries on the same layer, decaying by factor `0.60`.
  * **Vertical Downward Cascade:** Sovereign and factional policies cascade down to member characters with high loyalty transmission (`weight = 0.80`, decay `0.80`).
  * **Vertical Upward Cascade:** Grassroots actions on individuals bubble notoriety up to their parent organizations at half-intensity (`decay * 0.50`).
  * **Cutoff Floor:** Traversal halts via breadth-first search when absolute affinity delta drops below `0.04`.

---

## Disposition Spectrum

Raw floating affinity maps directly into seven distinct behavioral and conversational tiers:

| Affinity Band | Disposition | Behavioral Profile & Dialogue Tone |
| :--- | :--- | :--- |
| `+0.75` to `+1.00` | **Devoted / Love** | Unconditional allegiance, soulbound protection, prioritized assistance |
| `+0.40` to `+0.75` | **Allied** | Active cooperation, mutual defense, reliable trade |
| `+0.08` to `+0.40` | **Friendly** | Receptive, warm greetings, open to requests |
| `-0.08` to `+0.08` | **Neutral** | Transactional, indifferent, detached |
| `-0.25` to `-0.08` | **Unfriendly** | Cold, dismissive, suspicious |
| `-0.65` to `-0.25` | **Hostile** | Aggressive, uncooperative, brandishing weapons |
| `-1.00` to `-0.65` | **Nemesis** | Active blood feud, kill-on-sight posture |
| *N/A* | **Stranger** | Default state for unlinked entities with no recorded interaction |

---

## Build & Run

### Prerequisites
* GCC/MinGW-w64 with C++20 support (`g++`)
* Windows PowerShell or compatible shell

### Compilation
From VS Code, press **`Ctrl + Shift + B`** to trigger the default build task, or run manually from the terminal:

```powershell
g++ -std=c++20 -g *.cpp -o app.exe
```

### Execution
```powershell
.\app.exe
```

---

## Project Status

### Completed
* [x] **Polymorphic Entity Hierarchy:** Base `EntityClass` model with specialized `CharacterClass` and `GroupClass` derivations.
* [x] **Normalized Math Engine:** Transitioned system from legacy 1–15 integer distances to continuous `[-1.00, +1.00]` floating-point affinities.
* [x] **Hierarchical 3D Graph Engine:** Bi-directional lateral ties, parent/child vertical containment, loyalty weights, and memory logging.
* [x] **Event Ripple Simulator:** BFS propagation cascade modeling multi-hop impact across Micro, Meso, and Macro tiers.
* [x] **Interactive CLI:** Terminal dashboard for entity inspection, pairwise relationship queries, and targeted Help/Hurt event injection.

### Roadmap
* [ ] **Temporal Decay System:** Simulation tick loop to relax minor memory deltas and drift unreinforced affinities back to baseline equilibriums over time.
* [ ] **Dynamic Procedural Dialogue:** Context-aware voice generation referencing hop distances and specific historical event summaries.
* [ ] **Visual Graph Renderer:** Real-time 2D/3D topological canvas (via SFML, Raylib, or WebAssembly) replacing raw CLI tables.
* [ ] **Multi-Agent Autonomy:** Independent NPC-to-NPC action cycles allowing factions and kingdoms to declare wars and forge treaties organically without player intervention.