#include <cstdint>
#include <iostream>

#include "gameplay/crop_system.hpp"
#include "gameplay/economy_system.hpp"
#include "gameplay/inventory_system.hpp"

namespace {
int g_failed = 0;

void expectTrue(bool cond, const char* msg) {
    if (!cond) {
        std::cerr << "FAIL: " << msg << "\n";
        ++g_failed;
    }
}

void testHarvestFlow() {
    cozy::gameplay::CropDef def{};
    def.stageCount = 3;

    cozy::gameplay::CropComponent crop{};
    crop.stage = 2;
    crop.growth = 7;
    crop.hydration = 150;
    crop.fertilized = true;

    cozy::gameplay::Inventory<4> inventory{};
    const bool harvested = cozy::gameplay::HarvestCropToInventory(crop, def, inventory, 200, 3, 20);

    expectTrue(harvested, "harvest should succeed for mature crop");
    expectTrue(inventory.slots[0].itemId == 200, "harvest should add produce item to inventory");
    expectTrue(inventory.slots[0].amount == 3, "harvest should add expected produce amount");
    expectTrue(crop.stage == 0 && crop.growth == 0, "harvest should reset crop progression");
}

void testSellFlow() {
    cozy::gameplay::Inventory<4> inventory{};
    cozy::gameplay::Wallet wallet{};

    inventory.add(300, 9, 20);
    const bool sold = cozy::gameplay::SellItem(inventory, wallet, 300, 5, 12);

    expectTrue(sold, "sell should succeed when enough inventory exists");
    expectTrue(wallet.gold == 60, "gold should increase by amount * unit price");
    expectTrue(inventory.slots[0].amount == 4, "inventory amount should decrement by sold amount");
}
} // namespace

int main() {
    testHarvestFlow();
    testSellFlow();

    if (g_failed != 0) {
        std::cerr << g_failed << " test(s) failed\n";
        return 1;
    }

    std::cout << "All farming loop tests passed\n";
    return 0;
}
