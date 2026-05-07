#pragma once
#include <cstdint>
#include <vector>

namespace cozy::gameplay {
struct NpcScheduleEntry {
    uint32_t minuteOfDay;
    uint32_t targetNode;
    uint8_t behaviorId;
};

struct NpcScheduleComponent {
    std::vector<NpcScheduleEntry> entries;
    uint32_t activeIndex = 0;
};

inline void EvaluateNpcSchedule(NpcScheduleComponent& s, uint32_t minuteOfDay) {
    for (uint32_t i = 0; i < s.entries.size(); ++i) {
        if (minuteOfDay >= s.entries[i].minuteOfDay) s.activeIndex = i;
    }
}
} // namespace cozy::gameplay
