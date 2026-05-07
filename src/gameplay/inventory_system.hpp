#pragma once
#include <array>
#include <cstdint>

namespace cozy::gameplay {
struct ItemStack {
    uint16_t itemId = 0;
    uint16_t amount = 0;
    uint16_t durability = 0;
};

template <size_t Slots>
struct Inventory {
    std::array<ItemStack, Slots> slots{};

    bool add(uint16_t itemId, uint16_t amount, uint16_t maxStack) {
        for (auto& s : slots) {
            if (s.itemId == itemId && s.amount < maxStack) {
                const uint16_t room = maxStack - s.amount;
                const uint16_t move = (amount < room) ? amount : room;
                s.amount += move;
                amount -= move;
                if (amount == 0) return true;
            }
        }
        for (auto& s : slots) {
            if (s.amount == 0) {
                s.itemId = itemId;
                const uint16_t move = (amount < maxStack) ? amount : maxStack;
                s.amount = move;
                amount -= move;
                if (amount == 0) return true;
            }
        }
        return false;
    }
};
} // namespace cozy::gameplay
