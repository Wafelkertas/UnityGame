#pragma once

#include <array>
#include <cstdint>

#include "gameplay/inventory_system.hpp"

namespace cozy::gameplay {
constexpr uint8_t kMaxGiftPreferences = 8;
constexpr uint8_t kHeartLevelCount = 5;

struct RelationshipComponent {
    uint16_t npcId = 0;
    int32_t friendship = 0;
    uint8_t heartLevel = 0;
    uint16_t lastGiftDay = 0;
    uint16_t weeklyGiftCount = 0;
    uint16_t weeklyGiftWindowStartDay = 0;
};

struct GiftPreference {
    uint16_t itemId = 0;
    int16_t friendshipDelta = 0;
};

struct NpcSocialRoute {
    uint16_t npcId = 0;
    std::array<GiftPreference, kMaxGiftPreferences> gifts{};
    uint8_t giftCount = 0;
    std::array<int32_t, kHeartLevelCount - 1> heartThresholds{150, 400, 700, 1000};
    std::array<uint16_t, kHeartLevelCount> dialogueIds{};
    int16_t defaultGiftDelta = -10;
    uint8_t maxGiftsPerWeek = 2;
};

inline int16_t GiftDeltaForItem(const NpcSocialRoute& route, uint16_t itemId) {
    const uint8_t count = route.giftCount > kMaxGiftPreferences ? kMaxGiftPreferences : route.giftCount;
    for (uint8_t i = 0; i < count; ++i) {
        if (route.gifts[i].itemId == itemId) return route.gifts[i].friendshipDelta;
    }
    return route.defaultGiftDelta;
}

inline uint8_t HeartLevelFromFriendship(const NpcSocialRoute& route, int32_t friendship) {
    if (friendship >= route.heartThresholds[3]) return 4;
    if (friendship >= route.heartThresholds[2]) return 3;
    if (friendship >= route.heartThresholds[1]) return 2;
    if (friendship >= route.heartThresholds[0]) return 1;
    return 0;
}

inline bool CanGiveGift(const RelationshipComponent& relationship, const NpcSocialRoute& route, uint16_t currentDay) {
    if (relationship.lastGiftDay == currentDay) return false;

    const uint16_t daysIntoWindow = static_cast<uint16_t>(currentDay - relationship.weeklyGiftWindowStartDay);
    if (daysIntoWindow < 7 && relationship.weeklyGiftCount >= route.maxGiftsPerWeek) return false;

    return true;
}

inline void AdvanceGiftWindow(RelationshipComponent& relationship, uint16_t currentDay) {
    const uint16_t daysIntoWindow = static_cast<uint16_t>(currentDay - relationship.weeklyGiftWindowStartDay);
    if (daysIntoWindow >= 7) {
        relationship.weeklyGiftWindowStartDay = currentDay;
        relationship.weeklyGiftCount = 0;
    }
}

template <size_t Slots>
inline bool GiveGiftToNpc(Inventory<Slots>& inventory,
                          RelationshipComponent& relationship,
                          const NpcSocialRoute& route,
                          uint16_t itemId,
                          uint16_t currentDay) {
    if (relationship.npcId != route.npcId) return false;

    AdvanceGiftWindow(relationship, currentDay);
    if (!CanGiveGift(relationship, route, currentDay)) return false;

    for (auto& slot : inventory.slots) {
        if (slot.itemId != itemId || slot.amount == 0) continue;

        slot.amount = static_cast<uint16_t>(slot.amount - 1);
        if (slot.amount == 0) {
            slot.itemId = 0;
            slot.durability = 0;
        }

        relationship.friendship += GiftDeltaForItem(route, itemId);
        if (relationship.friendship < 0) relationship.friendship = 0;
        relationship.heartLevel = HeartLevelFromFriendship(route, relationship.friendship);
        relationship.lastGiftDay = currentDay;
        relationship.weeklyGiftCount = static_cast<uint16_t>(relationship.weeklyGiftCount + 1);
        return true;
    }

    return false;
}

inline uint16_t ActiveDialogueId(const RelationshipComponent& relationship, const NpcSocialRoute& route) {
    const uint8_t clampedHeart = relationship.heartLevel >= kHeartLevelCount ? kHeartLevelCount - 1 : relationship.heartLevel;
    return route.dialogueIds[clampedHeart];
}

} // namespace cozy::gameplay
