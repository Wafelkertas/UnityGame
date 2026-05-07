#include <cmath>
#include <cstdint>
#include <iostream>

#include "sim/simulation_loop.hpp"

namespace {
int g_failed = 0;

void expectTrue(bool cond, const char* msg) {
    if (!cond) {
        std::cerr << "FAIL: " << msg << "\n";
        ++g_failed;
    }
}

void testAdvanceRunsExpectedTicks() {
    cozy::sim::SimulationLoop loop(0.05f);
    uint32_t callbacks = 0;
    const uint32_t ticks = loop.advance(0.12f, [&](const cozy::sim::SimClock&) { ++callbacks; });

    expectTrue(ticks == 2, "advance should run 2 ticks for 0.12s at 0.05s fixed step");
    expectTrue(callbacks == 2, "callback should run once per tick");
    expectTrue(loop.clock.tick == 2, "clock tick should increment per fixed tick");
    expectTrue(std::fabs(loop.interpolationAlpha() - 0.4f) < 0.0001f, "interpolation alpha should retain remainder");
}

void testMinuteOfDayMapping() {
    cozy::sim::SimulationLoop loop(0.05f);
    loop.clock.ticksPerDay = 2400;
    loop.clock.tick = 600;

    expectTrue(loop.clock.minuteOfDay() == 360, "600/2400 of a day should map to 06:00 (360 minutes)");
}

void testFrameDeltaClamp() {
    cozy::sim::SimulationLoop loop(0.05f);
    uint32_t callbacks = 0;
    const uint32_t ticks = loop.advance(3.0f, [&](const cozy::sim::SimClock&) { ++callbacks; });

    expectTrue(ticks == 5, "advance should clamp long frame dt to max catch-up ticks");
    expectTrue(callbacks == 5, "callback count should match clamped ticks");
}
} // namespace

int main() {
    testAdvanceRunsExpectedTicks();
    testMinuteOfDayMapping();
    testFrameDeltaClamp();

    if (g_failed != 0) {
        std::cerr << g_failed << " test(s) failed\n";
        return 1;
    }

    std::cout << "All simulation loop tests passed\n";
    return 0;
}
