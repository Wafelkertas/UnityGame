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
    static constexpr uint32_t kDefaultMaxCatchUpTicks = 5;

    explicit SimulationLoop(float fixedStepSeconds = 0.05f) {
        setFixedStepSeconds(fixedStepSeconds);
    }

    template <typename TickFn>
    uint32_t advance(float frameDeltaSeconds, TickFn&& onTick) {
        m_accumulatorSeconds += std::max(0.0f, frameDeltaSeconds);

        const float maxCatchUpSeconds = m_fixedStepSeconds * static_cast<float>(m_maxCatchUpTicks);
        if (m_accumulatorSeconds > maxCatchUpSeconds) {
            m_accumulatorSeconds = maxCatchUpSeconds;
        }

        uint32_t ticksRun = 0;
        while (m_accumulatorSeconds >= m_fixedStepSeconds && ticksRun < m_maxCatchUpTicks) {
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

    void setFixedStepSeconds(float fixedStepSeconds) {
        m_fixedStepSeconds = std::max(0.001f, fixedStepSeconds);
        clock.secondsPerTick = m_fixedStepSeconds;
    }

    [[nodiscard]] float fixedStepSeconds() const { return m_fixedStepSeconds; }
    void setMaxCatchUpTicks(uint32_t maxCatchUpTicks) { m_maxCatchUpTicks = std::max(1U, maxCatchUpTicks); }

    SimClock clock;

private:
    float m_accumulatorSeconds = 0.0f;
    float m_fixedStepSeconds = 0.05f;
    uint32_t m_maxCatchUpTicks = kDefaultMaxCatchUpTicks;
};
} // namespace cozy::sim
