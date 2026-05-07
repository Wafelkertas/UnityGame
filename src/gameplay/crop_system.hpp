#pragma once
#include <algorithm>
#include <cstdint>

namespace cozy::gameplay {
struct CropComponent {
    uint16_t cropId = 0;
    uint8_t stage = 0;
    uint8_t quality = 0;
    uint16_t hydration = 0; // 0..1000
    uint16_t growth = 0;
    bool fertilized = false;
    bool dead = false;
};

struct CropDef {
    uint16_t growthPerStage[8]{};
    uint8_t stageCount = 0;
    uint16_t hydrationUsePerTick = 4;
};

inline void UpdateCrop(CropComponent& c, const CropDef& d, bool inSeason) {
    if (c.dead) return;
    c.hydration = static_cast<uint16_t>(std::max(0, int(c.hydration) - int(d.hydrationUsePerTick)));
    if (!inSeason || c.hydration == 0) {
        if (c.growth > 0) c.growth -= 1;
        return;
    }

    const uint16_t bonus = c.fertilized ? 2 : 1;
    c.growth = static_cast<uint16_t>(c.growth + bonus);
    const uint16_t threshold = d.growthPerStage[c.stage];
    if (c.growth >= threshold && c.stage + 1 < d.stageCount) {
        c.growth = 0;
        ++c.stage;
    }
}
} // namespace cozy::gameplay
