#pragma once
#include <cstdint>

#include "gameplay/crop_system.hpp"
#include "gameplay/inventory_system.hpp"

namespace cozy::gameplay {
struct Wallet {
    uint32_t gold = 0;
};

inline bool IsCropHarvestReady(const CropComponent& crop, const CropDef& def) {
    return !crop.dead && def.stageCount > 0 && crop.stage + 1 >= def.stageCount;
}

template <size_t Slots>
inline bool HarvestCropToInventory(CropComponent& crop,
                                   const CropDef& def,
                                   Inventory<Slots>& inventory,
                                   uint16_t produceItemId,
                                   uint16_t yieldAmount,
                                   uint16_t maxStack) {
    if (!IsCropHarvestReady(crop, def)) return false;
    if (!inventory.add(produceItemId, yieldAmount, maxStack)) return false;

    crop.stage = 0;
    crop.growth = 0;
    crop.hydration = 0;
    crop.fertilized = false;
    return true;
}

template <size_t Slots>
inline bool SellItem(Inventory<Slots>& inventory, Wallet& wallet, uint16_t itemId, uint16_t amount, uint16_t pricePerUnit) {
    uint16_t remaining = amount;
    for (auto& slot : inventory.slots) {
        if (slot.itemId != itemId || slot.amount == 0) continue;

        const uint16_t sold = (slot.amount < remaining) ? slot.amount : remaining;
        slot.amount = static_cast<uint16_t>(slot.amount - sold);
        remaining = static_cast<uint16_t>(remaining - sold);
        wallet.gold += static_cast<uint32_t>(sold) * pricePerUnit;

        if (slot.amount == 0) {
            slot.itemId = 0;
            slot.durability = 0;
        }
        if (remaining == 0) return true;
    }

    return false;
}
} // namespace cozy::gameplay
