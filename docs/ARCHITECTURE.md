# CozyFarmRPG Technical Blueprint

## 1) Full engine architecture
- **Core**: ECS world, memory allocators, job system, event bus, deterministic RNG.
- **Simulation** (fixed tick): farming, NPC AI, weather, ecology, inventory, crafting.
- **Runtime services**: save/load, resource manager, chunk streamer, localization.
- **Render** (variable frame): Vulkan 2D orthographic pipeline, sprite batcher, tilemap instancing, lighting pass.
- **Platform**: input, window, audio backend, filesystem abstraction (desktop/mobile).

## 2) Folder structure
- `src/core`: ECS, jobs, events, math, memory
- `src/sim`: fixed-step loop, world clock, deterministic layer
- `src/gameplay`: systems (farming, npc, inventory, weather)
- `src/render`: Vulkan renderer, atlas, batching, tilemap
- `src/platform`: Android/Windows platform glue
- `src/net`: snapshot + rollback prep types
- `data/`: JSON/YAML gameplay definitions
- `docs/`: design docs and roadmap

## 3) ECS design
- Entity ID: 32-bit generational handles.
- Components: POD-like structs, SoA pools for hot components.
- Systems iterate contiguous arrays and avoid virtual dispatch.
- Message bus for cross-system signals (`CropHarvested`, `GiftGiven`, `WeatherChanged`).

## 4) Rendering pipeline (Vulkan)
1. Gather visible chunks (camera culling).
2. Build tile/sprite instance buffers by material/atlas.
3. Single orthographic render pass:
   - pass A: tile layers
   - pass B: entities/sprites
   - pass C: lighting overlay
4. Submit minimal pipelines and descriptor sets.

## 5) Save/load design
- Binary snapshot for world state sections:
  - `meta.bin` (version, seed, day/time)
  - `chunks/<x>_<y>.bin` (tile + entities)
  - `player/<id>.bin`
- Delta/incremental saves for dirty chunks.
- Schema versioning with migration steps.

## 6) Chunk streaming strategy
- Chunk size: `32x32` or `64x64` tiles (profile both).
- 3 rings around player:
  - hot (sim + render)
  - warm (render only)
  - cold (unloaded)
- Async load/decompress on job threads.
- Deterministic sim only runs on loaded hot chunks.

## 7) Simulation loop
- Fixed tick (`20-60 Hz`) independent from framerate.
- Accumulator pattern for frame updates.
- Order: input commands -> time/weather -> crops -> AI -> path -> inventory/crafting -> events -> persistence queue.

## 8) Data schema examples
See `data/schemas/*.json`.

## 9) Example systems
See headers in `src/gameplay` and `src/render`.

## 10) Roadmap
1. **Vertical slice foundation (4-6 weeks)**: ECS, loop, tilemap render, player movement.
2. **Farming loop (4 weeks)**: crops/hydration/seasons/harvest.
3. **NPC social loop (6 weeks)**: schedules, pathing, dialogue, relationships.
4. **Persistence + world sim (4 weeks)**.
5. **Content tooling + mods (4 weeks)**.
6. **Optimization + Android port (6 weeks)**.

## 11) Solo-dev milestones
- M1: Walk in world, day/night, save/load.
- M2: Plant-grow-harvest sell loop.
- M3: One full NPC social route.
- M4: One season complete with weather + festivals.
- M5: Performance target (50k entities desktop, 20k mobile).

## 12) Optimization strategies
- SoA component layout + archetype chunks for hot loops.
- Multithread crop/AI by chunk partition.
- Batch pathfinding queries; use nav chunk caches.
- Texture atlas + sprite sorting by material.
- Dirty-region updates for tile mesh/instances.

## 13) Android Vulkan considerations
- Prefer ASTC textures, avoid runtime texture conversion.
- Limit descriptor churn, use ring buffers and push constants.
- Profile on Adreno + Mali separately.
- Keep memory budgets explicit (chunks, atlas, audio banks).

## 14) Multiplayer scaling path
- Deterministic sim core with command stream inputs.
- Server-authoritative state with periodic snapshots.
- Client interpolation for visuals only.
- Rollback-friendly event log with deterministic RNG seeds.

## 15) Modding architecture
- Data-first mods in `mods/<modid>/`:
  - `manifest.json`
  - `data/*.json` patches
  - optional scripts (later, sandboxed)
- Content registry loads base + mod layers with conflict rules.
- Hash/signature checks for multiplayer compatibility.
