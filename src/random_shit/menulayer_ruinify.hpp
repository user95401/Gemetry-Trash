#pragma once
#include <_main.hpp>

#include <Geode/modify/MenuGameLayer.hpp>
class $modify(MenuGameLayerExt, MenuGameLayer) {
    void runaway(float) {
        auto plr = this->m_playerObject;
        //runaway from mouse
        auto mousePos = getMousePos();
        auto playrPos = plr->getPosition();
        auto mousezone = CCSize(1, 1) * 42;
        auto mouseRect = CCRect(mousePos - mousezone, mousePos + mousezone);
        auto playrRect = plr->getObjectRect();
        if (playrRect.intersectsRect(mouseRect)) {
            //log::warn("{} intersectsRect {}", playrRect, mouseRect);
            plr->m_holdingLeft = playrPos.x < mousePos.x;
            plr->m_holdingRight = playrPos.x > mousePos.x;
            if (!plr->m_isSpider && !plr->m_isBall) plr->m_jumpBuffered = playrPos.y + (mousezone.height / 2) > mousePos.y;
        }
    }
    $override bool init() {
        auto init_result = MenuGameLayer::init();

        this->schedule(schedule_selector(MenuGameLayerExt::runaway), 0.001);

        if (SETTING(bool, "Animate Menu Game")) {
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
        }

        auto pulsebg = CCSprite::create("game_bg_13_001.png");
        pulsebg->setID("pulsebg");
        pulsebg->setScale(1.250f);
        pulsebg->setAnchorPoint({ 0.0f, 1.f });
        pulsebg->setBlendFunc({ GL_ONE, GL_ONE });
        pulsebg->setColor({0,0,0});
        m_groundLayer->addChild(pulsebg, 10);

        findFirstChildRecursive<CCNodeRGBA>(this,
            [](CCNodeRGBA* node) {
                auto parent_id = node->getParent()->getID();
                auto __this_id = node->getID();
                if (string::contains(parent_id, "ground-sprites")) {
                    node->setColor({ 0, 0,0 });
                }
                if (string::contains(__this_id, "background")) {
                    node->setColor({ 0, 0, 0 });
                }

                return false;
            }
        );

        updateColorCustom(0.f);
        this->schedule(schedule_selector(MenuGameLayerExt::updateColorCustom), 5.0f);

        return init_result;
    }
    $override void update(float p0) {
        MenuGameLayer::update(p0);

        //pulse
        if (SETTING(bool, "No Menu Game BG Pulse")) void();
        else {
            auto fmod = FMODAudioEngine::sharedEngine();
            if (not fmod->m_metering) fmod->enableMetering();
            auto pulse = (fmod->m_pulse1 + fmod->m_pulse2 + fmod->m_pulse3) / 3;
            this->m_backgroundSprite->setOpacity(255.f - (pulse * 80.f));
            if (auto pulsebg = typeinfo_cast<CCSprite*>(m_groundLayer->getChildByID("pulsebg"))) {
                pulsebg->setColor(darken3B(this->m_backgroundSprite->getColor(), 255 - pulse * 60.f));
            }
        }

        if (SETTING(bool, "Add Objects in Menu Game")) {
            auto plr = this->m_playerObject;
            auto spike1 = typeinfo_cast<GameObject*>(this->getChildByID("spike1"));
            auto spike2 = typeinfo_cast<GameObject*>(this->getChildByID("spike2"));
            auto bump = typeinfo_cast<EffectGameObject*>(this->getChildByID("bump"));
            if (spike1 and spike2 and bump) {
                auto plrRect = plr->getObjectRect();
                auto spike1Rect = spike1->getObjectRect();
                auto spike2Rect = spike2->getObjectRect();
                auto bumpRect = bump->getObjectRect();
                if (spike1Rect.intersectsRect(plrRect) or spike2Rect.intersectsRect(plrRect)) {
                    int total = GameStatsManager::sharedState()->getStat("9");
                    GameStatsManager::sharedState()->setStat("9", total - 1);
                    auto counter_ignore_next = CCNode::create();
                    counter_ignore_next->setID("counter_ignore_next");
                    this->addChild(counter_ignore_next);
                    this->destroyPlayer();
                    this->removeChildByID("counter_ignore_next");
                }
                if (bumpRect.intersectsRect(plrRect)) {
                    plr->bumpPlayer(1.0, 1, 1, bump);
                    bump->playShineEffect();
                }
            };
        };

        if (SETTING(bool, "No Platformer in Menu Game")) void();
        else if (SETTING(bool, "Freese Menu Game BG when Platformered")) {
            m_backgroundSpeed = 0.1;
            findFirstChildRecursive<CCNode>(m_groundLayer,
                [](CCNode* node) {
                    auto parent_id = node->getParent()->getID();
                    auto __this_id = node->getID();
                    if (string::contains(parent_id, "ground-sprites")) {
                        //node->stopAllActions();
                        node->setPosition(CCPointZero);
                    }

                    return false;
                }
            );
        };
    }
    $override void tryJump(float p0) {
        MenuGameLayer::tryJump(p0);
        auto plr = this->m_playerObject;
        if (SETTING(bool, "No Platformer in Menu Game")) void();
        else {
            plr->m_isPlatformer = 1;
            //random l/r going
            if (rndb()) plr->m_holdingLeft = rndb();
            else plr->m_holdingRight = rndb();
            //dont go left a lot
            plr->m_holdingLeft = plr->getPosition().x < 5 ? false : plr->m_holdingLeft;
            plr->m_holdingRight = plr->getPosition().x < 5 ? true : plr->m_holdingRight;//go back to screen!
        }
        //penis
        cocos::findFirstChildRecursive<CCSprite>(plr,
            [this](CCSprite* node) {
                node->setOpacity(255);
                return false;
            }
        );
    }
    $override void destroyPlayer() {
        if (SETTING(bool, "No New Explode in Menu Game")) void();
        GEODE_WINDOWS(else this->m_playerObject->playDeathEffect());
        MenuGameLayer::destroyPlayer();
        if (SETTING(bool, "No Players Destroyed Counter")) void();
        else {
            if (not this->getChildByID("counter_ignore_next")) {
                auto counter_layer = typeinfo_cast<CCLayer*>(this->getChildByIDRecursive(
                    "counter_layer"
                ));
                auto counter_label = typeinfo_cast<CCLabelBMFont*>(this->getChildByIDRecursive(
                    "counter_label"
                ));
                if (not counter_layer) {
                    counter_layer = CCLayer::create();
                    counter_layer->setAnchorPoint(CCPointZero);
                    counter_layer->setID("counter_layer");
                    this->addChild(counter_layer);
                    counter_label = CCLabelBMFont::create("", "gjFont51.fnt");
                    counter_label->setID("counter_label");
                    counter_label->setAlignment(kCCTextAlignmentRight);
                    counter_label->setAnchorPoint({ 1.f,0.f });
                    counter_label->setScale(0.3f);
                    counter_label->setTag(0);
                    counter_layer->addChildAtPosition(
                        counter_label, Anchor::BottomRight, { -10.f, 100.f }
                    );
                }
                counter_label->setTag(1 + counter_label->getTag());
                int total = GameStatsManager::sharedState()->getStat("9");
                int now = counter_label->getTag();
                counter_label->setString(fmt::format(
                    "Players Destroyed:\nTotally {}\nNow {}",
                    total, now
                ).data());
                counter_label->stopAllActions();
                counter_label->runAction(CCEaseExponentialIn::create(
                    CCFadeOut::create(15.f)
                ));
                counter_label->setScale(0.36f);
                counter_label->runAction(CCEaseBackOut::create(
                    CCScaleTo::create(0.25f, 0.3f)
                ));
            }
        }
    }
    $override void updateColor(float p0) {
        log::debug("{}({})", __func__, p0);
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
class $modify(MenuLayerExt, MenuLayer) {
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

        //remove unused links
        if(auto node = this->getChildByIDRecursive("facebook-button"))
            node->setVisible(0);
        if (auto node = this->getChildByIDRecursive("twitter-button"))
            node->setVisible(0);
        if (auto node = this->getChildByIDRecursive("youtube-button"))
            node->setVisible(0);
        if (auto node = this->getChildByIDRecursive("twitch-button"))
            node->setVisible(0);
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
                        "The project by a mod maker. The server is anarchic - easy mod, easy rate.""\n"
                        "\n"
                        "Probably the first GDPS, that is almost entirely a geode mod, which includes both textures and modifications. Modifications are also completely random - maybe something cool, or maybe some shit.""\n"
                        "\n"
                        "The things:"   "\n"
                        "- easy rate"   "\n"
                        "- easy mod"    "\n"
                        "- random explode sound, menu music"    "\n"
                        "- some changes in textures"    "\n"
                        "- kill counter in the main menu"   "\n"
                        "- the main levels have been changed"   "\n"
                        "- automatic updates"   "\n"
                        "- many more little things" "\n"
                        "\n"
                        "https://discord.gg/UyQytJsrGZ" "\n"
                        "https://gamejolt.com/games/GEOMETRY_TRASH_GDPS/776479/"    "\n"
                        ,
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

            auto verLabel = CCLabelBMFontAnimated::createWithBMFont(
                fmt::format(
                    "Platform: {}" "\n"
                    "Version: {}" "\n"
                    "Geode: {}", 
                    GEODE_PLATFORM_NAME,
                    Mod::get()->getVersion().toVString(),
                    Mod::get()->getMetadata().getGeodeVersion().toVString()
                ), 
                fmt::format("gjFont{:02d}.fnt", rand() % 60).c_str(),
                kCCTextAlignmentLeft
            );
            verLabel->limitLabelWidth(76.f, 0.525f, 0.1f);
            verLabel->setPosition(CCPoint(-218.f, -36.f));
            verLabel->setAnchorPoint(CCPoint(0.f, 1.f));
            verLabel->setColor(ccColor3B(77, 77, 77));
            verLabel->setBlendFunc({ GL_ONE, GL_ONE });
            verLabel->animateInRevealFromLeft(1.0f);
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

        auto get_rand_meme_button_menu_long_var_here_for_ever = CCMenu::createWithItem(CCMenuItemExt::createSpriteExtra(
            CCLabelTTF::create("get random programming meme", "Consolas", 6.f),
            [](auto) {
                PopupRandomMeme::loadAndShow();
            }
        ));
        get_rand_meme_button_menu_long_var_here_for_ever->setPosition({ 230.f, 6.f });
        this->addChild(get_rand_meme_button_menu_long_var_here_for_ever);

        return rtn;
    }
    inline static bool seenWarn = false;
};
