
#include <_main.hpp>
//LOG_THIS_FILE;

#include <Geode/modify/CCSprite.hpp>
class $modify(SpecialSprites, CCSprite) {
    static void onModify(auto & self) {
        auto names = { 
            "cocos2d::CCSprite::initWithSpriteFrameName",
            "cocos2d::CCSprite::initWithFile",
            "cocos2d::CCSprite::create",
            "cocos2d::CCSprite::createWithSpriteFrameName",
        };
        for (auto name : names) if (!self.setHookPriorityPost(name, Priority::Last)) {
            log::error("Failed to set hook priority for {}.", name);
        }
    }
    void pulseOpacitySch(float) {
        if (auto sprite = typeinfo_cast<CCSprite*>(this)) {
            auto fmod = FMODAudioEngine::sharedEngine();
            if (not fmod->m_metering) fmod->enableMetering();
            auto pulse = (fmod->m_pulse1 + fmod->m_pulse2 + fmod->m_pulse3) / 3;
            if (fmod->getBackgroundMusicVolume() > 0) sprite->setOpacity(pulse * 120.f);
        }
    }
    void sfx_on_init(std::string name) {
        auto sfx_path = fmt::format("{}.sfx_on_init", name);
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
            Ref base_layer = CCSprite::create(pszFileName);
            queueInMainThread(
                [base_layer] {
                    if (!ccc3BEqual(base_layer->getColor(), { 0, 102, 255 })) return;
                    auto custombg = CCSprite::create(CCFileUtils::get()->fullPathForFilename("bg.gif", 0).c_str());
                    base_layer->addChild(custombg);
                    custombg->setScaleX(base_layer->getContentSize().width / custombg->getContentSize().width);
					custombg->setScaleY(base_layer->getContentSize().height / custombg->getContentSize().height);
					custombg->setPosition(base_layer->getContentSize() / 2);
                }
            );
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