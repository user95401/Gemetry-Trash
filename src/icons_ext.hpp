#pragma once
#include <_main.hpp>

std::string keyForType(IconType type = IconType::Cube) {
    switch ((int)type) {
    case 1: return ("ship");
    case 2: return ("player_ball");
    case 3: return ("bird");
    case 4: return ("dart");
    case 5: return ("robot");
    case 6: return ("spider");
    case 7: return ("swing");
    case 8: return ("jetpack");
    default: return ("player");
    };
}

IconType typeForKey(std::string key) {
    auto rtn = 0;//player
    if (key == "ship") rtn = 1;
    if (key == "player_ball") rtn = 2;
    if (key == "bird") rtn = 3;
    if (key == "dart") rtn = 4;
    if (key == "robot") rtn = 5;
    if (key == "spider") rtn = 6;
    if (key == "swing") rtn = 7;
    if (key == "jetpack") rtn = 8;
    //log::debug("{}({}) = {}({})", __FUNCTION__, key, rtn, keyForType((IconType)rtn));
    return (IconType)rtn;
}

std::vector<const char*> frameNamesInVec(int index, IconType type) {
    //naming
    auto lay1_name = CCString::createWithFormat("%s_%02d_001.png", keyForType(type).c_str(), index)->getCString();
    auto lay2_name = CCString::createWithFormat("%s_%02d_2_001.png", keyForType(type).c_str(), index)->getCString();
    auto lay3_name = CCString::createWithFormat("%s_%02d_3_001.png", keyForType(type).c_str(), index)->getCString();
    auto glow_name = CCString::createWithFormat("%s_%02d_glow_001.png", keyForType(type).c_str(), index)->getCString();
    auto extr_name = CCString::createWithFormat("%s_%02d_extra_001.png", keyForType(type).c_str(), index)->getCString();
    //load frames
    //GameManager::get()->icon(index, type);
    //test
    auto placeholder = "emptyGlow.png";
    auto frameCache = CCSpriteFrameCache::sharedSpriteFrameCache();
    if (nullptr == frameCache->spriteFrameByName(lay1_name)) lay1_name = placeholder;
    if (nullptr == frameCache->spriteFrameByName(lay2_name)) lay2_name = placeholder;
    if (nullptr == frameCache->spriteFrameByName(lay3_name)) lay3_name = placeholder;
    if (nullptr == frameCache->spriteFrameByName(glow_name)) glow_name = placeholder;
    if (nullptr == frameCache->spriteFrameByName(extr_name)) extr_name = placeholder;
    //rtn
    return { lay1_name, lay2_name, lay3_name, glow_name, extr_name };
}

#include <Geode/modify/GameManager.hpp>
class $modify(GameManagerIconsExt, GameManager) {
    inline static Ref<CCNode> return_original_count_for_type = CCNode::create();
    $override bool init() {
        return_original_count_for_type->setID("return_original_count_for_type");
        return_original_count_for_type->setVisible(0);
        return GameManager::init();
    };
    $override int countForType(IconType p0) {
        auto rtn = GameManager::countForType(p0);
        //log::debug("{}({}).rtn (original) = {}", __FUNCTION__, keyForType(p0), rtn);

        if (return_original_count_for_type->isVisible()) return rtn;

        for (auto id = (rtn + 1); id <= (rtn + 255); id++) {

            auto simple_frame_names = frameNamesInVec(id, p0);
            auto animated_part1_name = CCString::createWithFormat("%s_%02d_01_001.png", keyForType(p0).c_str(), id)->getCString();

            auto incr = false;

            incr = simple_frame_names[0] != "emptyGlow.png" ? true : incr;
            incr = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(animated_part1_name) ? true : incr;
            
            if (incr) rtn += 1;
            else break;

        }

        //log::debug("{}({}).rtn = {}", __FUNCTION__, keyForType(p0), rtn);
        return rtn;
    };
};

#include <Geode/modify/PlayerObject.hpp>
class $modify(PlayerObjectIconsExt, PlayerObject) {
    static auto frame(const char* name) {
        return CCSpriteFrameCache::sharedSpriteFrameCache()
            ->spriteFrameByName(name);
    }
    int customFramesUpateFor(int index, IconType type, bool forVehicle = false) {

        if (this == nullptr) return index;

        GameManagerIconsExt::return_original_count_for_type->setVisible(1);
        auto original_count = GameManager::get()->countForType(type);
        GameManagerIconsExt::return_original_count_for_type->setVisible(0);

        if ((index != original_count) or (index != (original_count + 1))) {//fucking checks
            index = GameManager::get()->activeIconForType(type);
        }

        auto names = frameNamesInVec(index, type);

        if (not forVehicle) {
            if (m_iconSprite) m_iconSprite->setDisplayFrame(frame(names[0]));
            if (m_iconSpriteSecondary) m_iconSpriteSecondary->setDisplayFrame(frame(names[1]));
            if (m_iconSpriteWhitener) m_iconSpriteWhitener->setDisplayFrame(frame(names[4]));
            if (m_iconGlow) m_iconGlow->setDisplayFrame(frame(names[3]));
            //set pos
            if (m_iconSprite) this->m_iconSprite->setPosition({
                this->m_iconSprite->displayFrame()->getOriginalSize().width / this->m_iconSprite->getContentSize().width,
                this->m_iconSprite->displayFrame()->getOriginalSize().height / this->m_iconSprite->getContentSize().height,
                });
            if (m_iconSpriteSecondary) this->m_iconSpriteSecondary->setPosition(this->m_iconSprite->getContentSize() / 2);// ({ 16.600f, 16.200f });
            if (m_iconSpriteWhitener) this->m_iconSpriteWhitener->setPosition(this->m_iconSprite->getContentSize() / 2);// ({ 16.800f, 16.200f });
            if (m_iconGlow) this->m_iconGlow->setPosition({
                this->m_iconGlow->displayFrame()->getOriginalSize().width / this->m_iconGlow->getContentSize().width,
                this->m_iconGlow->displayFrame()->getOriginalSize().height / this->m_iconGlow->getContentSize().height,
                });
        }
        else {
            if (m_vehicleSprite) m_vehicleSprite->setDisplayFrame(frame(names[0]));
            if (m_vehicleSpriteSecondary) m_vehicleSpriteSecondary->setDisplayFrame(frame(names[1]));
            if (m_vehicleSpriteWhitener) m_vehicleSpriteWhitener->setDisplayFrame(frame(names[4]));
            if (m_vehicleGlow) m_vehicleGlow->setDisplayFrame(frame(names[3]));
            if (m_birdVehicle) m_birdVehicle->setDisplayFrame(frame(names[2]));
            //set pos
            if (m_vehicleSprite) this->m_vehicleSprite->setPosition({
                this->m_vehicleSprite->displayFrame()->getOriginalSize().width / this->m_vehicleSprite->getContentSize().width,
                this->m_vehicleSprite->displayFrame()->getOriginalSize().height / this->m_vehicleSprite->getContentSize().height,
                });
            if (m_vehicleSpriteSecondary and m_vehicleSprite) this->m_vehicleSpriteSecondary->setPosition(this->m_vehicleSprite->getContentSize() / 2);// ({ 16.600f, 16.200f });
            if (m_vehicleSpriteWhitener and m_vehicleSprite) this->m_vehicleSpriteWhitener->setPosition(this->m_vehicleSprite->getContentSize() / 2);// ({ 16.800f, 16.200f });
            if (m_birdVehicle and m_vehicleSprite) this->m_birdVehicle->setPosition(this->m_vehicleSprite->getContentSize() / 2);// ({ 16.800f, 16.200f });
            if (m_vehicleGlow) this->m_vehicleGlow->setPosition({
                this->m_vehicleGlow->displayFrame()->getOriginalSize().width / this->m_vehicleGlow->getContentSize().width,
                this->m_vehicleGlow->displayFrame()->getOriginalSize().height / this->m_vehicleGlow->getContentSize().height,
                });
            if (m_vehicleSprite and type == IconType::Ufo) m_vehicleSprite->setPositionY(-8.f);
            if (m_vehicleSprite and type == IconType::Ship) m_vehicleSprite->setPositionY(-6.f);
        };

        return index;
    }
    $override bool init(int p0, int p1, GJBaseGameLayer* p2, cocos2d::CCLayer* p3, bool p4) {
        if (!PlayerObject::init(p0, p1, p2, p3, p4)) return false;
        PlayerObject::updatePlayerFrame(m_maybeSavedPlayerFrame);
        return true;
    }
    $override void updatePlayerFrame(int p0) {
        PlayerObject::updatePlayerFrame(p0);
        this->m_maybeSavedPlayerFrame = customFramesUpateFor(p0, IconType::Cube);
    }
    $override void updatePlayerShipFrame(int p0) {
        PlayerObject::updatePlayerShipFrame(p0);
        this->m_maybeSavedPlayerFrame = customFramesUpateFor(m_maybeSavedPlayerFrame, IconType::Cube);
        this->m_iconRequestID = customFramesUpateFor(p0, IconType::Ship, true);
    };
    $override void updatePlayerRollFrame(int p0) {
        PlayerObject::updatePlayerRollFrame(p0);
        this->m_iconRequestID = customFramesUpateFor(p0, IconType::Ball);
    };
    $override void updatePlayerBirdFrame(int p0) {
        PlayerObject::updatePlayerBirdFrame(p0);
        this->m_maybeSavedPlayerFrame = customFramesUpateFor(m_maybeSavedPlayerFrame, IconType::Cube);
        this->m_iconRequestID = customFramesUpateFor(p0, IconType::Ufo, true);
    };
    $override void updatePlayerDartFrame(int p0) {
        PlayerObject::updatePlayerDartFrame(p0);
        this->m_iconRequestID = customFramesUpateFor(p0, IconType::Wave);
    };
    $override void updatePlayerSwingFrame(int p0) {
        PlayerObject::updatePlayerSwingFrame(p0);
        this->m_iconRequestID = customFramesUpateFor(p0, IconType::Swing);
    };
    $override void updatePlayerJetpackFrame(int p0) {
        PlayerObject::updatePlayerJetpackFrame(p0);
        this->m_maybeSavedPlayerFrame = customFramesUpateFor(m_maybeSavedPlayerFrame, IconType::Cube);
        this->m_iconRequestID = customFramesUpateFor(p0, IconType::Jetpack, true);
    };
    $override void createRobot(int frame) {
        PlayerObject::createRobot(frame);
        this->m_iconRequestID = frame;
        //still idk. todo :D
    }
    $override void createSpider(int frame) {
        PlayerObject::createSpider(frame);
        this->m_iconRequestID = frame;
        //still idk. todo :D
    }
};

#include <Geode/modify/SimplePlayer.hpp>
class $modify(SimplePlayerIconsExt, SimplePlayer) {
    $override void updatePlayerFrame(int p0, IconType p1) {
        auto index = p0;
        auto type = p1;
        SimplePlayer::updatePlayerFrame(index, type);
        //not simple ones
        if (type == IconType::Robot) {
            //SimplePlayer::updatePlayerFrame(index, type);
            return;
        }
        if (type == IconType::Spider) {
            //SimplePlayer::updatePlayerFrame(index, type);
            return;
        }
        //update frames
        auto names = frameNamesInVec(index, type);
        setFrames(names[0], names[1], names[2], names[3], names[4]);
    }
};

#include <Geode/modify/GJGarageLayer.hpp>
class $modify(GJGarageLayerIconsExt, GJGarageLayer) {
    $override void setupPage(int p0, IconType p1) {
        if (GameManager::sharedState()->countForType(p1) <= 36) p0 = 0;
        GJGarageLayer::setupPage(p0, p1);
        if (m_playerObject) m_playerObject->updatePlayerFrame(
            GameManager::get()->activeIconForType(
                GameManager::get()->m_playerIconType
            ),
            GameManager::get()->m_playerIconType
        );
    }
};