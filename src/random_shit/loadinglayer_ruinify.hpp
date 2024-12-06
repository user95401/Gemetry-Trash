#pragma once
#include <_main.hpp>

#include <Geode/modify/LoadingLayer.hpp>
class $modify(LoadingLayerExt, LoadingLayer) {
    $override bool init(bool p0) {
        srand(time(0)); //bool(rand() % 2)

        auto rtn = LoadingLayer::init(p0);

        auto bg = typeinfo_cast<CCSprite*>(this->getChildByID("bg-texture"));
        if (bg) {
            auto background = rand() % 59;
            auto new_bg = CCSprite::create(fmt::format(
                "game_bg_{:02d}_001.png", background >= 1 ? background : 1
            ).c_str());
            if (new_bg) bg->setDisplayFrame(new_bg->displayFrame());
            auto colorID = rand() % 23;
            auto color = reinterpret_cast<LevelSelectLayer*>(bg)->colorForPage(colorID);
            bg->setColor(color);
        }

        if(SETTING(bool, "play loading theme at resource loading")) 
            GameManager::get()->fadeInMusic("loading_theme.mp3");

        {
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
            verLabel->limitLabelWidth(92.f, 0.5f, 0.1f);
            verLabel->setPositionY(this->getContentHeight());
            verLabel->setAnchorPoint(CCPoint(-0.05f, 1.1f));
            this->addChild(verLabel);
        };

        return rtn;
    }
};