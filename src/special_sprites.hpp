#pragma once
#include <_main.hpp>

#include <Geode/modify/CCSprite.hpp>
class $modify(SpecialSprites, CCSprite) {
    void pulseOpacitySch(float) {
        if (auto sprite = typeinfo_cast<CCSprite*>(this)) {
            auto fmod = FMODAudioEngine::sharedEngine();
            if (not fmod->m_metering) fmod->enableMetering();
            auto pulse = (fmod->m_pulse1 + fmod->m_pulse2 + fmod->m_pulse3) / 3;
            if (fmod->getBackgroundMusicVolume() > 0) sprite->setOpacity(pulse * 120.f);
        }
    }
    void gradientColorSch(float) {
        if (auto sprite = typeinfo_cast<CCSprite*>(this)) {
            srand(time(0));
            auto colorID = rand() % 23;
            auto color = reinterpret_cast<LevelSelectLayer*>(sprite)->colorForPage(colorID);
            sprite->runAction(CCTintTo::create(3.0f, color.r, color.g, color.b));
            sprite->setTag(colorID);
        }
    }
    void trySetupGradientColorSch(float) {
        if (auto base_layer = typeinfo_cast<CCSprite*>(this)) {
            if (ccc3BEqual(base_layer->getColor(), {0, 102, 255})) {
                //color
                srand(time(0));
                auto colorID = rand() % 23;
                auto color = reinterpret_cast<LevelSelectLayer*>(base_layer)->colorForPage(colorID);
                //color layer
                CCSprite* color_layer = CCSprite::create("GJ_gradientBG.png");
                color_layer->setAnchorPoint(CCPointZero);
                color_layer->setBlendFunc({ GL_ONE, GL_ONE });
                color_layer->setColor(color);
                color_layer->setOpacity(255);
                color_layer->schedule(schedule_selector(SpecialSprites::gradientColorSch), 3.0f);
                base_layer->addChild(color_layer, 0, colorID);
            };
        }
    };
    void sfx_on_init(std::string name) {
        auto sfx_path = fmt::format("{}/{}.sfx_on_init", getMod()->getResourcesDir(), name);
        if (fs::exists(sfx_path)) FMODAudioEngine::get()->playEffect(sfx_path);
    }
    $override bool initWithSpriteFrameName(const char* pszSpriteFrameName) {
        sfx_on_init(pszSpriteFrameName);
        return CCSprite::initWithSpriteFrameName(pszSpriteFrameName);
    }
    $override bool initWithFile(const char* pszFilename) {
        sfx_on_init(pszFilename);
        return CCSprite::initWithFile(pszFilename);
    }
    $override static CCSprite* create(const char* pszFileName) {
        if (string::contains(pszFileName, "GJ_gradientBG")) {
            auto base_layer = CCSprite::create(pszFileName);
            base_layer->scheduleOnce(schedule_selector(SpecialSprites::trySetupGradientColorSch), 0.f);
            return base_layer;
        }
        return CCSprite::create(pszFileName);
    }
    $override static CCSprite* createWithSpriteFrameName(const char* pszSpriteFrameName) {
        if (string::contains(pszSpriteFrameName, "robtoplogo_small.png")) {
            //label
            CCLabelBMFont* label = CCLabelBMFont::create("   user95401's   \noriginal", "chatFont.fnt");
            label->setAlignment(kCCTextAlignmentCenter);
            label->setAnchorPoint(CCPoint());
            //blankSprite
            CCSprite* blankSprite = CCSprite::create();
            blankSprite->addChild(label);
            blankSprite->setContentSize(label->getContentSize());
            //return
            return blankSprite;
        }
        if (string::contains(pszSpriteFrameName, "RobTopLogoBig_001.png")) {
            //label
            CCLabelBMFont* label = CCLabelBMFont::create("user95401's original", "gjFont06.fnt");
            label->setAlignment(kCCTextAlignmentCenter);
            label->setAnchorPoint(CCPointZero);
            //blankSprite
            CCSprite* blankSprite = CCSprite::create();
            blankSprite->addChild(label);
            blankSprite->setContentSize(label->getContentSize());
            //return
            return blankSprite;
        }
        if (string::contains(pszSpriteFrameName, "GJ_logo_001.png")) {
            //base layer (container)
            CCSprite* base_layer = CCSprite::create("GTPS_logo_001.png");
            //color
            srand(time(0));
            auto colorID = rand() % 23;
            auto color = reinterpret_cast<LevelSelectLayer*>(base_layer)
                ->colorForPage(colorID);
            //color layer
            CCSprite* color_layer = CCSprite::create("GTPS_logo_002.png");
            color_layer->setAnchorPoint(CCPointZero);
            color_layer->setBlendFunc({ GL_ONE, GL_ONE });
            color_layer->setColor(color);
            base_layer->addChild(color_layer, 0, colorID);
            //pulse layer
            CCSprite* pulse_layer = CCSprite::create("GTPS_logo_002.png");
            pulse_layer->setAnchorPoint(CCPointZero);
            pulse_layer->setBlendFunc({ GL_ONE, GL_ONE });
            pulse_layer->setOpacity(0);
            pulse_layer->schedule(schedule_selector(SpecialSprites::pulseOpacitySch));
            base_layer->addChild(pulse_layer);
            //rtn base_layer
            return base_layer;
        }
        return CCSprite::createWithSpriteFrameName(pszSpriteFrameName);
    }
};