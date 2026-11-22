#include <Geode/Geode.hpp>
#include <Geode/modify/GameManager.hpp>
#include <Geode/modify/GJGarageLayer.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;

static void saveIcons() {
    auto gm = GameManager::get();
    Mod::get()->setSavedValue("icons", std::vector<int>{
        gm->getPlayerFrame(), gm->getPlayerShip(), gm->getPlayerBall(),
        gm->getPlayerBird(), gm->getPlayerDart(), gm->getPlayerRobot(),
        gm->getPlayerSpider(), gm->getPlayerSwing(), gm->getPlayerJetpack(),
        gm->getPlayerColor(), gm->getPlayerColor2(), gm->m_playerGlowColor,
        gm->getPlayerGlow() ? 1 : 0
    });
}

class $modify(GarageModGameManager, GameManager) {
    bool isIconUnlocked(int id, IconType type) {
        if (Mod::get()->getSettingValue<bool>("enabled")) {
            return true;
        }
        return GameManager::isIconUnlocked(id, type);
    }

    bool isColorUnlocked(int id, UnlockType type) {
        if (Mod::get()->getSettingValue<bool>("enabled")) {
            return true;
        }
        return GameManager::isColorUnlocked(id, type);
    }
};

class $modify(GarageModGarageLayer, GJGarageLayer) {
    bool init() {
        if (!GJGarageLayer::init()) return false;

        auto menu = CCMenu::create();
        menu->setID("lockpick-menu"_spr);
        menu->setPosition({28.f, 85.f});
        menu->setLayout(ColumnLayout::create()->setGap(6.f));

        auto randBtn = CCMenuItemSpriteExtra::create(
            CircleButtonSprite::createWithSpriteFrameName(
                "GJ_updateBtn_001.png", 1.f, CircleBaseColor::Green, CircleBaseSize::Small
            ),
            this,
            menu_selector(GarageModGarageLayer::onRandomize)
        );
        randBtn->setID("randomize-btn"_spr);

        auto resetBtn = CCMenuItemSpriteExtra::create(
            CircleButtonSprite::createWithSpriteFrameName(
                "GJ_trashBtn_001.png", 1.f, CircleBaseColor::Pink, CircleBaseSize::Small
            ),
            this,
            menu_selector(GarageModGarageLayer::onReset)
        );
        resetBtn->setID("reset-btn"_spr);

        menu->addChild(randBtn);
        menu->addChild(resetBtn);
        menu->updateLayout();
        this->addChild(menu);

        return true;
    }

    void onSelect(CCObject* sender) {
        GJGarageLayer::onSelect(sender);
        saveIcons();
    }

    void onBack(CCObject* sender) {
        saveIcons();
        GJGarageLayer::onBack(sender);
    }

    void onRandomize(CCObject*) {
        auto gm = GameManager::get();

        gm->setPlayerFrame(utils::random::generate(1, gm->countForType(IconType::Cube) + 1));
        gm->setPlayerShip(utils::random::generate(1, gm->countForType(IconType::Ship) + 1));
        gm->setPlayerBall(utils::random::generate(1, gm->countForType(IconType::Ball) + 1));
        gm->setPlayerBird(utils::random::generate(1, gm->countForType(IconType::Ufo) + 1));
        gm->setPlayerDart(utils::random::generate(1, gm->countForType(IconType::Wave) + 1));
        gm->setPlayerRobot(utils::random::generate(1, gm->countForType(IconType::Robot) + 1));
        gm->setPlayerSpider(utils::random::generate(1, gm->countForType(IconType::Spider) + 1));
        gm->setPlayerSwing(utils::random::generate(1, gm->countForType(IconType::Swing) + 1));
        gm->setPlayerJetpack(utils::random::generate(1, gm->countForType(IconType::Jetpack) + 1));

        gm->setPlayerColor(utils::random::generate(0, 107));
        gm->setPlayerColor2(utils::random::generate(0, 107));
        gm->m_playerGlowColor = utils::random::generate(0, 107);
        gm->setPlayerGlow(utils::random::generate(0, 2) == 1);

        saveIcons();
        this->updatePlayerColors();
    }

    void onReset(CCObject*) {
        createQuickPopup(
            "Reset Icons",
            "Reset all icons and colors to default?",
            "Cancel", "Reset",
            [this](auto, bool btn2) {
                if (!btn2) return;
                auto gm = GameManager::get();

                gm->setPlayerFrame(1);
                gm->setPlayerShip(1);
                gm->setPlayerBall(1);
                gm->setPlayerBird(1);
                gm->setPlayerDart(1);
                gm->setPlayerRobot(1);
                gm->setPlayerSpider(1);
                gm->setPlayerSwing(1);
                gm->setPlayerJetpack(1);
                gm->setPlayerColor(0);
                gm->setPlayerColor2(3);
                gm->m_playerGlowColor = 0;
                gm->setPlayerGlow(false);

                saveIcons();
                this->updatePlayerColors();
            }
        );
    }
};

class $modify(GarageModMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        if (Mod::get()->getSettingValue<bool>("enabled") && Mod::get()->hasSavedValue("icons")) {
            auto v = Mod::get()->getSavedValue<std::vector<int>>("icons");
            if (v.size() >= 13) {
                auto gm = GameManager::get();
                gm->setPlayerFrame(v.at(0));
                gm->setPlayerShip(v.at(1));
                gm->setPlayerBall(v.at(2));
                gm->setPlayerBird(v.at(3));
                gm->setPlayerDart(v.at(4));
                gm->setPlayerRobot(v.at(5));
                gm->setPlayerSpider(v.at(6));
                gm->setPlayerSwing(v.at(7));
                gm->setPlayerJetpack(v.at(8));
                gm->setPlayerColor(v.at(9));
                gm->setPlayerColor2(v.at(10));
                gm->m_playerGlowColor = v.at(11);
                gm->setPlayerGlow(v.at(12) == 1);
            }
        }

        return true;
    }
};
