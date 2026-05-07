#pragma once
#include <cstdint>
#include <vector>

namespace cozy::render {
struct TileInstance {
    uint32_t packedPos; // 16 bits x, 16 bits y
    uint16_t uvIndex;
    uint16_t layer;
};

struct ChunkDrawPacket {
    uint32_t materialId;
    std::vector<TileInstance> instances;
};

class TilemapRenderer {
public:
    void buildChunkPacket(ChunkDrawPacket& packet, const std::vector<uint16_t>& tileIds, uint16_t width) {
        packet.instances.clear();
        for (uint32_t i = 0; i < tileIds.size(); ++i) {
            if (tileIds[i] == 0) continue;
            const uint16_t x = static_cast<uint16_t>(i % width);
            const uint16_t y = static_cast<uint16_t>(i / width);
            packet.instances.push_back({static_cast<uint32_t>(x) | (static_cast<uint32_t>(y) << 16), tileIds[i], 0});
        }
    }
};
} // namespace cozy::render
