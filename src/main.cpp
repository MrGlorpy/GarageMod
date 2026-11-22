#include <Geode/Geode.hpp>
#include <Geode/modify/GameManager.hpp>
#include <Geode/modify/GJGarageLayer.hpp>

using namespace geode::prelude;

struct SavedLoadout {
    int cube = 1;
    int ship = 1;
    int ball = 1;
    int ufo = 1;
    int wave = 1;
    int robot = 1;
    int spider = 1;
    int swing = 1;
    int jetpack = 1;
    int color1 = 0;
    int color2 = 3;
    int glowColor = 0;
    bool glow = false;

    static SavedLoadout fromGameManager(GameManager* gm) {
        return {
            gm->getPlayerFrame(),
            gm->getPlayerShip(),
            gm->getPlayerBall(),
            gm->getPlayerBird(),
            gm->getPlayerDart(),
            gm->getPlayerRobot(),
            gm->getPlayerSpider(),
            gm->getPlayerSwing(),
            gm->getPlayerJetpack(),
            gm->getPlayerColor(),
            gm->getPlayerColor2(),
            gm->m_playerGlowColor,
            gm->getPlayerGlow()
        };
    }

    void applyToGameManager(GameManager* gm) const {
        gm->setPlayerFrame(cube);
        gm->setPlayerShip(ship);
        gm->setPlayerBall(ball);
        gm->setPlayerBird(ufo);
        gm->setPlayerDart(wave);
        gm->setPlayerRobot(robot);
        gm->setPlayerSpider(spider);
        gm->setPlayerSwing(swing);
        gm->setPlayerJetpack(jetpack);
        gm->setPlayerColor(color1);
        gm->setPlayerColor2(color2);
        gm->m_playerGlowColor = glowColor;
        gm->setPlayerGlow(glow);
    }
};

template <>
struct matjson::Serialize<SavedLoadout> {
    static Result<SavedLoadout> fromJson(matjson::Value const& value) {
        GEODE_UNWRAP_INTO(int cube, value["cube"].asInt());
        GEODE_UNWRAP_INTO(int ship, value["ship"].asInt());
        GEODE_UNWRAP_INTO(int ball, value["ball"].asInt());
        GEODE_UNWRAP_INTO(int ufo, value["ufo"].asInt());
        GEODE_UNWRAP_INTO(int wave, value["wave"].asInt());
        GEODE_UNWRAP_INTO(int robot, value["robot"].asInt());
        GEODE_UNWRAP_INTO(int spider, value["spider"].asInt());
        GEODE_UNWRAP_INTO(int swing, value["swing"].asInt());
        GEODE_UNWRAP_INTO(int jetpack, value["jetpack"].asInt());
        GEODE_UNWRAP_INTO(int color1, value["color1"].asInt());
        GEODE_UNWRAP_INTO(int color2, value["color2"].asInt());
        GEODE_UNWRAP_INTO(int glowColor, value["glowColor"].asInt());
        GEODE_UNWRAP_INTO(bool glow, value["glow"].asBool());

        return Ok(SavedLoadout{
            cube, ship, ball, ufo, wave, robot, spider, swing, jetpack,
            color1, color2, glowColor, glow
        });
    }

    static matjson::Value toJson(SavedLoadout const& loadout) {
        return matjson::makeObject({
            {"cube", loadout.cube},
            {"ship", loadout.ship},
            {"ball", loadout.ball},
            {"ufo", loadout.ufo},
            {"wave", loadout.wave},
            {"robot", loadout.robot},
            {"spider", loadout.spider},
            {"swing", loadout.swing},
            {"jetpack", loadout.jetpack},
            {"color1", loadout.color1},
            {"color2", loadout.color2},
            {"glowColor", loadout.glowColor},
            {"glow", loadout.glow}
        });
    }
};

static void saveCurrentLoadout() {
    auto gm = GameManager::get();
    Mod::get()->setSavedValue("loadout", SavedLoadout::fromGameManager(gm));
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

    void dataLoaded(DS_Dictionary* dict) {
        GameManager::dataLoaded(dict);

        if (Mod::get()->getSettingValue<bool>("enabled") && Mod::get()->hasSavedValue("loadout")) {
            auto loadout = Mod::get()->getSavedValue<SavedLoadout>("loadout");
            loadout.applyToGameManager(this);
        }
    }
};

class $modify(GarageModLayer, GJGarageLayer) {
    bool init() {
        if (!GJGarageLayer::init()) return false;

        auto randBtn = CCMenuItemSpriteExtra::create(
            CircleButtonSprite::createWithSpriteFrameName(
                "GJ_updateBtn_001.png", 1.f, CircleBaseColor::Green, CircleBaseSize::Small
            ),
            this,
            menu_selector(GarageModLayer::onRandomize)
        );
        randBtn->setID("randomize-btn"_spr);

        auto resetBtn = CCMenuItemSpriteExtra::create(
            CircleButtonSprite::createWithSpriteFrameName(
                "GJ_trashBtn_001.png", 1.f, CircleBaseColor::Pink, CircleBaseSize::Small
            ),
            this,
            menu_selector(GarageModLayer::onReset)
        );
        resetBtn->setID("reset-btn"_spr);

        if (auto shardsMenu = this->getChildByID("shards-menu")) {
            shardsMenu->addChild(randBtn);
            shardsMenu->addChild(resetBtn);
            shardsMenu->updateLayout();
        } else {
            auto menu = CCMenu::create();
            menu->setID("garage-buttons-menu"_spr);
            menu->setPosition({28.f, 85.f});
            menu->setLayout(ColumnLayout::create()->setGap(6.f));
            menu->addChild(randBtn);
            menu->addChild(resetBtn);
            menu->updateLayout();
            this->addChild(menu);
        }

        return true;
    }

    void refreshGaragePreview() {
        auto gm = GameManager::get();
        this->updatePlayerColors();

        if (m_playerObject) {
            int frame = gm->getPlayerFrame();
            switch (m_iconType) {
                case IconType::Ship: frame = gm->getPlayerShip(); break;
                case IconType::Ball: frame = gm->getPlayerBall(); break;
                case IconType::Ufo: frame = gm->getPlayerBird(); break;
                case IconType::Wave: frame = gm->getPlayerDart(); break;
                case IconType::Robot: frame = gm->getPlayerRobot(); break;
                case IconType::Spider: frame = gm->getPlayerSpider(); break;
                case IconType::Swing: frame = gm->getPlayerSwing(); break;
                case IconType::Jetpack: frame = gm->getPlayerJetpack(); break;
                default: break;
            }
            m_playerObject->updatePlayerFrame(frame, m_iconType);
        }
    }

    void onSelect(CCObject* sender) {
        GJGarageLayer::onSelect(sender);
        saveCurrentLoadout();
    }

    void onBack(CCObject* sender) {
        saveCurrentLoadout();
        GJGarageLayer::onBack(sender);
    }

    void onRandomize(CCObject*) {
        auto gm = GameManager::get();
        int maxColors = std::max(1, gm->countForType(IconType::Special));

        SavedLoadout randomLoadout{
            utils::random::generate(1, gm->countForType(IconType::Cube) + 1),
            utils::random::generate(1, gm->countForType(IconType::Ship) + 1),
            utils::random::generate(1, gm->countForType(IconType::Ball) + 1),
            utils::random::generate(1, gm->countForType(IconType::Ufo) + 1),
            utils::random::generate(1, gm->countForType(IconType::Wave) + 1),
            utils::random::generate(1, gm->countForType(IconType::Robot) + 1),
            utils::random::generate(1, gm->countForType(IconType::Spider) + 1),
            utils::random::generate(1, gm->countForType(IconType::Swing) + 1),
            utils::random::generate(1, gm->countForType(IconType::Jetpack) + 1),
            utils::random::generate(0, maxColors),
            utils::random::generate(0, maxColors),
            utils::random::generate(0, maxColors),
            utils::random::generate(0, 2) == 1
        };

        randomLoadout.applyToGameManager(gm);
        saveCurrentLoadout();
        this->refreshGaragePreview();
    }

    void onReset(CCObject*) {
        createQuickPopup(
            "Reset Icons",
            "Reset all icons and colors to default?",
            "Cancel", "Reset",
            [this](auto, bool btn2) {
                if (!btn2) return;
                SavedLoadout defaultLoadout{};
                defaultLoadout.applyToGameManager(GameManager::get());
                saveCurrentLoadout();
                this->refreshGaragePreview();
            }
        );
    }
};
// hello i see you:)!
