#include <iostream>

#include "gameplay/inventory_system.hpp"
#include "gameplay/social_system.hpp"

namespace {
int g_failed = 0;

void expectTrue(bool cond, const char* msg) {
    if (!cond) {
        std::cerr << "FAIL: " << msg << "\n";
        ++g_failed;
    }
}

cozy::gameplay::NpcSocialRoute makeRoute() {
    cozy::gameplay::NpcSocialRoute route{};
    route.npcId = 7;
    route.gifts[0] = {.itemId = 501, .friendshipDelta = 90};
    route.giftCount = 1;
    route.dialogueIds = {10, 11, 12, 13, 14};
    return route;
}

void testGiftAdvancesFriendshipAndConsumesItem() {
    cozy::gameplay::Inventory<6> inventory{};
    inventory.add(501, 2, 20);

    cozy::gameplay::RelationshipComponent rel{.npcId = 7};
    cozy::gameplay::NpcSocialRoute route = makeRoute();

    const bool gifted = cozy::gameplay::GiveGiftToNpc(inventory, rel, route, 501, 10);

    expectTrue(gifted, "gift should succeed when matching item exists");
    expectTrue(rel.friendship == 90, "liked gift should increase friendship by configured amount");
    expectTrue(rel.heartLevel == 0, "heart level should stay zero below first threshold");
    expectTrue(inventory.slots[0].amount == 1, "gift should consume one item from inventory");
}

void testGiftCapsAtTwoPerWeek() {
    cozy::gameplay::Inventory<6> inventory{};
    inventory.add(501, 3, 20);

    cozy::gameplay::RelationshipComponent rel{.npcId = 7};
    cozy::gameplay::NpcSocialRoute route = makeRoute();

    const bool first = cozy::gameplay::GiveGiftToNpc(inventory, rel, route, 501, 1);
    const bool second = cozy::gameplay::GiveGiftToNpc(inventory, rel, route, 501, 3);
    const bool third = cozy::gameplay::GiveGiftToNpc(inventory, rel, route, 501, 4);

    expectTrue(first && second, "first two gifts should pass in same week window");
    expectTrue(!third, "third gift in same week should be blocked");
}

void testGiftWindowResetsAfterSevenDays() {
    cozy::gameplay::Inventory<6> inventory{};
    inventory.add(501, 3, 20);

    cozy::gameplay::RelationshipComponent rel{.npcId = 7};
    cozy::gameplay::NpcSocialRoute route = makeRoute();

    cozy::gameplay::GiveGiftToNpc(inventory, rel, route, 501, 1);
    cozy::gameplay::GiveGiftToNpc(inventory, rel, route, 501, 2);
    const bool afterReset = cozy::gameplay::GiveGiftToNpc(inventory, rel, route, 501, 8);

    expectTrue(afterReset, "gifting should resume after week window resets");
}

void testDialogueUsesHeartLevel() {
    cozy::gameplay::RelationshipComponent rel{.npcId = 7, .friendship = 760, .heartLevel = 3};
    cozy::gameplay::NpcSocialRoute route = makeRoute();

    const uint16_t dialogueId = cozy::gameplay::ActiveDialogueId(rel, route);

    expectTrue(dialogueId == 13, "dialogue id should map to active heart level");
}

void testNpcMismatchFails() {
    cozy::gameplay::Inventory<6> inventory{};
    inventory.add(501, 1, 20);

    cozy::gameplay::RelationshipComponent rel{.npcId = 99};
    cozy::gameplay::NpcSocialRoute route = makeRoute();

    const bool gifted = cozy::gameplay::GiveGiftToNpc(inventory, rel, route, 501, 1);
    expectTrue(!gifted, "gifting should fail when relationship NPC does not match route NPC");
}

} // namespace

int main() {
    testGiftAdvancesFriendshipAndConsumesItem();
    testGiftCapsAtTwoPerWeek();
    testGiftWindowResetsAfterSevenDays();
    testDialogueUsesHeartLevel();
    testNpcMismatchFails();

    if (g_failed != 0) {
        std::cerr << g_failed << " test(s) failed\n";
        return 1;
    }

    std::cout << "All social loop tests passed\n";
    return 0;
}
