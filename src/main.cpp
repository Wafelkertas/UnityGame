#include <array>
#include <iostream>

#include "gameplay/crop_system.hpp"
#include "gameplay/economy_system.hpp"
#include "gameplay/inventory_system.hpp"
#include "gameplay/npc_schedule_system.hpp"
#include "gameplay/social_system.hpp"
#include "sim/simulation_loop.hpp"

int main() {
    using namespace cozy;

    sim::SimulationLoop loop(0.05f);

    gameplay::CropComponent crop{.cropId = 1, .hydration = 200, .fertilized = true};
    gameplay::CropDef cropDef{};
    cropDef.stageCount = 4;
    cropDef.growthPerStage[0] = 4;
    cropDef.growthPerStage[1] = 4;
    cropDef.growthPerStage[2] = 6;

    gameplay::NpcScheduleComponent npc{
        .entries = {
            {360, 10, 1}, // 06:00
            {720, 22, 2}, // 12:00
            {1080, 5, 3}  // 18:00
        }
    };

    gameplay::Inventory<8> inventory;
    gameplay::Wallet wallet;

    gameplay::NpcSocialRoute socialRoute{};
    socialRoute.npcId = 7;
    socialRoute.gifts[0] = {.itemId = 200, .friendshipDelta = 100};
    socialRoute.giftCount = 1;
    socialRoute.dialogueIds = {1000, 1001, 1002, 1003, 1004};
    gameplay::RelationshipComponent relationship{.npcId = 7};

    const std::array<float, 3> frameDeltas{0.016f, 0.016f, 0.022f};
    for (float dt : frameDeltas) {
        loop.advance(dt, [&](const sim::SimClock& clock) {
            gameplay::UpdateCrop(crop, cropDef, true);
            gameplay::EvaluateNpcSchedule(npc, clock.minuteOfDay());
        });
    }

    const bool pickedUpSeeds = inventory.add(100, 12, 20);

    crop.stage = static_cast<uint8_t>(cropDef.stageCount - 1);
    const bool harvested = gameplay::HarvestCropToInventory(crop, cropDef, inventory, 200, 5, 20);
    const bool gifted = gameplay::GiveGiftToNpc(inventory, relationship, socialRoute, 200, 1);
    const bool sold = gameplay::SellItem(inventory, wallet, 200, 4, 15);

    std::cout << "CozyFarmRPG prototype bootstrap\n";
    std::cout << "Tick: " << loop.clock.tick << " minute: " << loop.clock.minuteOfDay() << "\n";
    std::cout << "Crop stage: " << static_cast<int>(crop.stage) << " growth: " << crop.growth
              << " hydration: " << crop.hydration << "\n";
    std::cout << "NPC active schedule index: " << npc.activeIndex << "\n";
    std::cout << "Inventory add seeds: " << (pickedUpSeeds ? "ok" : "full") << "\n";
    std::cout << "Harvested produce: " << (harvested ? "yes" : "no") << " sold produce: " << (sold ? "yes" : "no") << "\n";
    std::cout << "Gifted villager: " << (gifted ? "yes" : "no")
              << " dialogueId: " << gameplay::ActiveDialogueId(relationship, socialRoute) << "\n";
    std::cout << "Wallet gold: " << wallet.gold << "\n";

    return 0;
}
