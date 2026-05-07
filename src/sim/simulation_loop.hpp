#pragma once
#include <cstdint>

namespace cozy::sim {
struct SimClock {
    uint64_t tick = 0;
    uint32_t ticksPerDay = 86400;
    float secondsPerTick = 0.05f;
};

class SimulationLoop {
public:
    void fixedTick() { ++clock.tick; }
    SimClock clock;
};
} // namespace cozy::sim
