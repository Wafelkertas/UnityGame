#pragma once
#include <algorithm>
#include <cstdint>

namespace cozy::sim {
struct SimClock {
    uint64_t tick = 0;
    uint32_t ticksPerDay = 86400;
    float secondsPerTick = 0.05f;

    [[nodiscard]] uint32_t minuteOfDay() const {
        const uint64_t ticksIntoDay = (ticksPerDay == 0) ? 0 : (tick % ticksPerDay);
        return static_cast<uint32_t>((ticksIntoDay * 1440ULL) / std::max(1U, ticksPerDay));
    }
};

class SimulationLoop {
public:
    explicit SimulationLoop(float fixedStepSeconds = 0.05f) : m_fixedStepSeconds(fixedStepSeconds) {
        clock.secondsPerTick = fixedStepSeconds;
    }

    template <typename TickFn>
    uint32_t advance(float frameDeltaSeconds, TickFn&& onTick) {
        m_accumulatorSeconds += frameDeltaSeconds;

        uint32_t ticksRun = 0;
        while (m_accumulatorSeconds >= m_fixedStepSeconds) {
            m_accumulatorSeconds -= m_fixedStepSeconds;
            fixedTick();
            onTick(clock);
            ++ticksRun;
        }

        return ticksRun;
    }

    void fixedTick() { ++clock.tick; }

    [[nodiscard]] float interpolationAlpha() const {
        return m_accumulatorSeconds / m_fixedStepSeconds;
    }

    SimClock clock;

private:
    float m_accumulatorSeconds = 0.0f;
    float m_fixedStepSeconds = 0.05f;
};
} // namespace cozy::sim
