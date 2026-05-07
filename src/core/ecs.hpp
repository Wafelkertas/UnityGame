#pragma once
#include <cstdint>
#include <vector>

namespace cozy::core {
using Entity = uint32_t;

struct Position { float x, y; };
struct Velocity { float x, y; };

// Minimal sparse-set style storage placeholder.
template <typename T>
struct ComponentPool {
    std::vector<Entity> entities;
    std::vector<T> data;
};
} // namespace cozy::core
