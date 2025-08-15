
#include <_main.hpp>

#include <Geode/modify/MenuGameLayer.hpp>
class $modify(MenuGameLayerRuinifyExt, MenuGameLayer) {
    $override bool init() {
        auto init_result = MenuGameLayer::init();

        //move
        auto moveTo1 = CCEaseSineInOut::create(CCMoveTo::create(10, CCPoint(0, 3)));//          ↑
        auto moveTo2 = CCEaseSineInOut::create(CCMoveTo::create(10, CCPoint(3, 3)));//          ↗
        auto moveTo3 = CCEaseSineInOut::create(CCMoveTo::create(10, CCPoint(3, 0)));//          →
        auto moveTo4 = CCEaseSineInOut::create(CCMoveTo::create(10, CCPoint(-3, -3)));//        ↘
        auto moveTo5 = CCEaseSineInOut::create(CCMoveTo::create(10, CCPoint(0, -3)));//         ↓
        auto moveTo6 = CCEaseSineInOut::create(CCMoveTo::create(10, CCPoint(-3, 0)));//         ←
        auto moveTo7 = CCEaseSineInOut::create(CCMoveTo::create(10, CCPoint(-3, 3)));//         ↖
        this->runAction(CCRepeatForever::create(CCSequence::create(
            moveTo1, moveTo2, moveTo3, moveTo4, moveTo5, moveTo6, moveTo7, nullptr
        )));
        //rotation
        this->runAction(CCRepeatForever::create(CCSequence::create(
            CCEaseSineInOut::create(CCRotateTo::create(5, 1.05)),// >
            CCEaseSineInOut::create(CCRotateTo::create(5, -1.05)),// <
            nullptr
        )));
        //scale
        this->runAction(CCRepeatForever::create(CCSequence::create(
            CCEaseSineInOut::create(CCScaleTo::create(10, 1.005)),// o
            CCEaseSineInOut::create(CCScaleTo::create(10, 1.010)),// 0
            nullptr
        )));

        auto pulsebg = CCSprite::create("game_bg_13_001.png");
        pulsebg->setID("pulsebg");
        pulsebg->setScale(1.250f);
        pulsebg->setAnchorPoint({ 0.0f, 1.f });
        pulsebg->setBlendFunc({ GL_ONE, GL_ONE });
        pulsebg->setColor({ 0,0,0 });
        m_groundLayer->addChild(pulsebg, 10);

        findFirstChildRecursive<CCNodeRGBA>(this,
            [](CCNodeRGBA* node) {
                auto parent_id = node->getParent()->getID();
                auto __this_id = node->getID();
                if (string::contains(parent_id, "ground-sprites")) {
                    node->setColor({ 0, 0, 0 });
                }
                if (string::contains(__this_id, "background")) {
                    node->setColor({ 0, 0, 0 });
                }

                return false;
            }
        );

        updateColorCustom(0.f);
        this->schedule(schedule_selector(MenuGameLayerRuinifyExt::updateColorCustom), 5.0f);

        return init_result;
    }
    $override void update(float p0) {
        MenuGameLayer::update(p0);

        //pulse
        auto fmod = FMODAudioEngine::sharedEngine();
        if (not fmod->m_metering) fmod->enableMetering();
        auto pulse = (fmod->m_pulse1 + fmod->m_pulse2 + fmod->m_pulse3) / 3;
        this->m_backgroundSprite->setOpacity(255.f - (pulse * 80.f));
        if (auto pulsebg = typeinfo_cast<CCSprite*>(m_groundLayer->getChildByID("pulsebg"))) {
            pulsebg->setColor(darken3B(this->m_backgroundSprite->getColor(), 255 - pulse * 60.f));
        }

        m_backgroundSpeed = pulse;
        m_groundLayer->setZOrder(9999);
        findFirstChildRecursive<CCNode>(m_groundLayer,
            [pulse](CCNode* node) {
                auto parent_id = node->getParent()->getID();
                auto __this_id = node->getID();
                if (string::contains(parent_id, "ground-sprites")) {
                    //node->stopAllActions();
                    node->setPosition(CCPointZero + CCPoint(pulse, 0));
                }

                return false;
            }
        );
    }
    $override void updateColor(float p0) {
        //log::debug("{}({})", __func__, p0);
        return p0 != 1337.f ? void() : MenuGameLayer::updateColor(p0);
    }
    void updateColorCustom(float) {
        return this->updateColor(1337.f);
    }
    $override cocos2d::ccColor3B getBGColor(int colorID) {
        srand(time(0));
        auto cid = rand() % 23;
        auto color = reinterpret_cast<LevelSelectLayer*>(this)->colorForPage(cid);
        return color;
    }
};

#include <Geode/modify/MenuLayer.hpp>
class $modify(MenuLayerRuinifyExt, MenuLayer) {
    $override bool init() {
        srand(time(0)); //bool(rand() % 2)

        //rand bg
        auto background = rand() % 60;
        auto ground = rand() % 23;
        if (background <= 7) ground = background;
        else ground = 7 + (rand() % (23 - 7));
        GameManager::get()->loadBackground(background);
        GameManager::get()->loadGround(ground);

        auto rtn = MenuLayer::init();

        if (rndb()) {
            if (auto game = this->getChildByType<MenuGameLayer>(0)) game->setZOrder(-10);
            this->addChild(geode::createLayerBG(), -1);
        }

        //remove unused links
        if (auto node = this->getChildByIDRecursive("facebook-button")) node->setVisible(0);
        if (auto node = this->getChildByIDRecursive("twitter-button")) node->setVisible(0);
        if (auto node = this->getChildByIDRecursive("youtube-button")) node->setVisible(0);
        if (auto node = this->getChildByIDRecursive("twitch-button")) node->setVisible(0);
        if (auto node = this->getChildByIDRecursive("discord-button")) node->setVisible(0);
        if (auto node = this->getChildByIDRecursive("more-games-button")) {
            node->setContentSize(CCPointZero);
            node->setVisible(0);
        }

        if (auto node = this->getChildByIDRecursive("more-games-menu")) {

            auto top = SimplePlayer::create(218);

            auto image = CircleButtonSprite::create(
                top, CircleBaseColor::Green, CircleBaseSize::Small
            );

            top->setScale(0.7f);

            auto item = CCMenuItemExt::createSpriteExtra(image,
                [](auto) {
                    auto total_kills = GameStatsManager::sharedState()->getStat("9");
                    auto level = GJGameLevel::create();
                    level->m_levelName = "Kills Count";
                    level->m_levelID = 1107;
                    level->m_levelLength = 5;
                    level->m_levelType = GJLevelType::Saved;
                    level->m_bestTime = (total_kills);
                    level->m_bestPoints = (total_kills);
                    level->m_k111 = (total_kills);
                    level->m_unkString3 = GameToolbox::pointsToString(total_kills);
                    level->m_unkString4 = GameToolbox::pointsToString(total_kills);
                    level->savePercentage(13, 0, 22, 22, 1);
                    auto popup = LevelLeaderboard::create(
                        level, LevelLeaderboardType::Global, LevelLeaderboardMode::Points
                    );
                    popup->show();
                }
            );

            node->addChild(item);
            node->updateLayout();

        }

        if (auto title = this->getChildByIDRecursive("main-title")) {

            auto pos = title->getPosition();
            auto parent = title->getParent();

            title->removeFromParentAndCleanup(0);

            auto menu = CCMenu::createWithItem(CCMenuItemExt::createSpriteExtra(
                title, [this](auto) {
                    MDPopup* mdpop = nullptr;
                    mdpop = MDPopup::create(
                        "Gemetry Trash", //reinterpret_cast<LoadingLayer*>(CCNode::create())->getLoadingString(),
                        "yo",
                        "Settings", "Credits",
                        [this, mdpop](bool btn2) {
                            if (not btn2) openSettingsPopup(getMod());
                            else openSupportPopup(getMod());
                        }
                    );
                    mdpop->show();
                }
            ));

            auto btn = getChild<CCMenuItemSpriteExtra>(menu, 0);
            btn->setPosition(CCPointZero);
            btn->m_colorEnabled = 1;
            btn->m_animationEnabled = 0;

            menu->setPosition(pos);
            menu->setID(title->getID() + "-menu");
            parent->addChild(menu);

            auto verLabel = CCLabelBMFont::create(
                fmt::format(
                    "Platform: {}" "\n"
                    "Version: {}" "\n"
                    "Geode: {}",
                    GEODE_PLATFORM_NAME,
                    Mod::get()->getVersion().toVString(),
                    Mod::get()->getMetadataRef().getGeodeVersion().toVString()
                ).c_str(),
                fmt::format("gjFont{:02d}.fnt", rand() % 60).c_str()
            );
            verLabel->limitLabelWidth(76.f, 0.525f, 0.1f);
            verLabel->setPosition(CCPoint(-218.f, -36.f));
            verLabel->setAnchorPoint(CCPoint(0.f, 1.f));
            verLabel->setColor(ccColor3B(77, 77, 77));
            verLabel->setBlendFunc({ GL_ONE, GL_ONE });
            menu->addChild(verLabel);
        }

        //centerNode
        CCNode* centerNode = CCNode::create();
        centerNode->setID("centerNode");
        centerNode->setPosition(this->getContentSize() / 2);
        this->addChild(centerNode);

        //flyinamopgus!!!
        CCSprite* flyinAmogus_001 = CCSprite::create("flyinAmogus_001.png");
        if (flyinAmogus_001) {
            flyinAmogus_001->setID("flyinamopgus");
            flyinAmogus_001->setScale(2.f);
            flyinAmogus_001->setPosition({
                CCDirector::sharedDirector()->getScreenRight(),
                CCDirector::sharedDirector()->getScreenTop() }
                );
            flyinAmogus_001->runAction(CCRepeatForever::create(CCRotateBy::create(0.1, 1)));
            flyinAmogus_001->setColor(reinterpret_cast<LevelSelectLayer*>(this)->colorForPage(rand() % 23));
            this->addChild(flyinAmogus_001);
            CCSprite* flyinAmogus_002 = CCSprite::create("flyinAmogus_002.png");
            flyinAmogus_001->setID("flyinAmogus_002");
            flyinAmogus_002->setAnchorPoint(CCPointZero);
            flyinAmogus_001->addChild(flyinAmogus_002);
        };

        static bool seenWarn;
        if (!seenWarn) {
            seenWarn = true;
            Notification::create(
                R"(Этот мод может содержать нецензурный, 
сексуальный и весьма поражающий контент. 
Ничего тут не строит воспринимать всерьёз. 
Вас предупредили, наслаждайтесь.)"
, NotificationIcon::None, 6.66f)->show();

            auto logo = "../../../user95401.gemetry-trash/logo.png";
            CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFrame(
                CCSprite::create(logo)->displayFrame(), logo
            );
            AchievementNotifier::sharedState()->notifyAchievement(
                "Gemetry Trash Запущен!", "Вау, не у каждого это\nудаётся... ведь так?.", logo, 1
            );
        }

        return rtn;
    }
};

#include <Geode/modify/PlayerObject.hpp>
class $modify(PlayerObjectMenuGameExt, PlayerObject) {
    void updatePlatMovements(float) {
        auto plr = this;

        plr->m_isPlatformer = 1;

        //random l/r going
        if (rndb()) plr->m_holdingLeft = rndb();
        else plr->m_holdingRight = rndb();

        //dont go left a lot
        plr->m_holdingLeft = plr->getPosition().x < 5 ? false : plr->m_holdingLeft;
        plr->m_holdingRight = plr->getPosition().x < 5 ? true : plr->m_holdingRight;//go back to screen!
    }
    void runaway(float) {
        auto plr = this;
        //runaway from mouse
        auto mousePos = getMousePos();
        auto playrPos = plr->getPosition();
        auto mousezone = CCSize(1, 1) * 42.f;
        auto mouseRect = CCRect(mousePos - mousezone, mousePos + mousezone);
        auto playrRect = plr->getObjectRect();
        if (playrRect.intersectsRect(mouseRect)) {
            //log::warn("{} intersectsRect {}", playrRect, mouseRect);
            plr->m_holdingLeft = playrPos.x < mousePos.x;
            plr->m_holdingRight = playrPos.x > mousePos.x;
            if (!plr->m_isSpider && !plr->m_isBall) plr->m_jumpBuffered = playrPos.y + (mousezone.height / 2) > mousePos.y;
        }
    }
    $override bool init(int p0, int p1, GJBaseGameLayer * p2, cocos2d::CCLayer * p3, bool p4) {
        if (!PlayerObject::init(p0, p1, p2, p3, p4)) return false;

        if (typeinfo_cast<MenuGameLayer*>(p3)) {

            this->schedule(schedule_selector(PlayerObjectMenuGameExt::runaway), 0.001);
            this->schedule(schedule_selector(PlayerObjectMenuGameExt::updatePlatMovements), 0.35f);
            this->m_isPlatformer = 1;

        }

        return true;
    }
};