#pragma once
#include <functional>

namespace cozy::core {
class JobSystem {
public:
    using Job = std::function<void()>;
    void schedule(Job&& job) { job(); } // Replace with thread-pool implementation.
};
} // namespace cozy::core
