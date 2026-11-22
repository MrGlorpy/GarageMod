#include <Geode/Geode.hpp>
#include <Geode/modify/GameManager.hpp>
#include <Geode/modify/GJGarageLayer.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/CharacterColorPage.hpp>

using namespace geode::prelude;

struct Loadout {
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
};

template <>
struct matjson::Serialize<Loadout> {
    static Result<Loadout> fromJson(matjson::Value const& val) {
        GEODE_UNWRAP_INTO(int cube, val["cube"].asInt());
        GEODE_UNWRAP_INTO(int ship, val["ship"].asInt());
        GEODE_UNWRAP_INTO(int ball, val["ball"].asInt());
        GEODE_UNWRAP_INTO(int ufo, val["ufo"].asInt());
        GEODE_UNWRAP_INTO(int wave, val["wave"].asInt());
        GEODE_UNWRAP_INTO(int robot, val["robot"].asInt());
        GEODE_UNWRAP_INTO(int spider, val["spider"].asInt());
        GEODE_UNWRAP_INTO(int swing, val["swing"].asInt());
        GEODE_UNWRAP_INTO(int jetpack, val["jetpack"].asInt());
        GEODE_UNWRAP_INTO(int color1, val["color1"].asInt());
        GEODE_UNWRAP_INTO(int color2, val["color2"].asInt());
        GEODE_UNWRAP_INTO(int glowColor, val["glowColor"].asInt());
        GEODE_UNWRAP_INTO(bool glow, val["glow"].asBool());

        return Ok(Loadout{
            cube, ship, ball, ufo, wave, robot, spider, swing, jetpack,
            color1, color2, glowColor, glow
        });
    }

    static matjson::Value toJson(Loadout const& l) {
        return matjson::makeObject({
            {"cube", l.cube}, {"ship", l.ship}, {"ball", l.ball},
            {"ufo", l.ufo}, {"wave", l.wave}, {"robot", l.robot},
            {"spider", l.spider}, {"swing", l.swing}, {"jetpack", l.jetpack},
            {"color1", l.color1}, {"color2", l.color2},
            {"glowColor", l.glowColor}, {"glow", l.glow}
        });
    }
};

static void saveLoadout() {
    auto gm = GameManager::get();
    Mod::get()->setSavedValue("loadout", Loadout{
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
    });
}

static void applyLoadout(Loadout const& l) {
    auto gm = GameManager::get();
    gm->setPlayerFrame(l.cube);
    gm->setPlayerShip(l.ship);
    gm->setPlayerBall(l.ball);
    gm->setPlayerBird(l.ufo);
    gm->setPlayerDart(l.wave);
    gm->setPlayerRobot(l.robot);
    gm->setPlayerSpider(l.spider);
    gm->setPlayerSwing(l.swing);
    gm->setPlayerJetpack(l.jetpack);
    gm->setPlayerColor(l.color1);
    gm->setPlayerColor2(l.color2);
    gm->m_playerGlowColor = l.glowColor;
    gm->setPlayerGlow(l.glow);
}

class $modify(GarageModGameManager, GameManager) {
    bool isIconUnlocked(int id, IconType type) {
        if (Mod::get()->getSettingValue<bool>("enabled")) return true;
        return GameManager::isIconUnlocked(id, type);
    }

    bool isColorUnlocked(int id, UnlockType type) {
        if (Mod::get()->getSettingValue<bool>("enabled")) return true;
        return GameManager::isColorUnlocked(id, type);
    }

    void dataLoaded(DS_Dictionary* dict) {
        GameManager::dataLoaded(dict);

        if (Mod::get()->getSettingValue<bool>("enabled") && Mod::get()->hasSavedValue("loadout")) {
            applyLoadout(Mod::get()->getSavedValue<Loadout>("loadout"));
        }
    }
};

class $modify(GarageModMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        static bool loaded = false;
        if (!loaded && Mod::get()->getSettingValue<bool>("enabled") && Mod::get()->hasSavedValue("loadout")) {
            applyLoadout(Mod::get()->getSavedValue<Loadout>("loadout"));
            loaded = true;
        }

        return true;
    }
};

class $modify(GarageModColorPage, CharacterColorPage) {
    void onClose(CCObject* sender) {
        if (Mod::get()->getSettingValue<bool>("enabled")) {
            saveLoadout();
        }
        CharacterColorPage::onClose(sender);
    }
};

class $modify(GarageModGarageLayer, GJGarageLayer) {
    bool init() {
        if (!GJGarageLayer::init()) return false;
        if (!Mod::get()->getSettingValue<bool>("enabled")) return true;

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

        if (auto menu = this->getChildByID("shards-menu")) {
            menu->addChild(randBtn);
            menu->addChild(resetBtn);
            menu->updateLayout();
        }

        return true;
    }

    void refreshPreview() {
        this->updatePlayerColors();
        if (m_playerObject) {
            auto gm = GameManager::get();
            m_playerObject->updatePlayerFrame(gm->activeIconForType(m_iconType), m_iconType);
        }
    }

    void onSelect(CCObject* sender) {
        GJGarageLayer::onSelect(sender);
        if (Mod::get()->getSettingValue<bool>("enabled")) {
            saveLoadout();
        }
    }

    void onBack(CCObject* sender) {
        if (Mod::get()->getSettingValue<bool>("enabled")) {
            saveLoadout();
        }
        GJGarageLayer::onBack(sender);
    }

    void onRandomize(CCObject*) {
        auto gm = GameManager::get();
        int maxColors = 107;

        applyLoadout({
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
        });

        saveLoadout();
        this->refreshPreview();
    }

    void onReset(CCObject*) {
        createQuickPopup(
            "Reset Icons",
            "Reset all icons and colors to default?",
            "Cancel", "Reset",
            [this](auto, bool btn2) {
                if (!btn2) return;
                applyLoadout(Loadout{});
                saveLoadout();
                this->refreshPreview();
            }
        );
    }
};
