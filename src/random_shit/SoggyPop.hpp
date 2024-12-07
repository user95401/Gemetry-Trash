#pragma once
#include <_main.hpp>

#include <Geode/modify/CCMenuItem.hpp>
class $modify(PopupSoggyCat, CCMenuItem) {
    inline static bool can_asd = false;
    inline static std::function<void()> onClick = []() {};
    static auto createPopup() {
        auto popup = createQuickPopup("", "", "", "", [](auto, auto) { 
            PopupSoggyCat::onClick(); 
            }, false);
        popup->m_mainLayer->removeAllChildrenWithCleanup(0);
        popup->m_mainLayer->addChild(popup->m_buttonMenu);
        popup->m_button1->getParent()->setContentSize(CCSize(1, 1) * 7777);
        popup->m_button1->setVisible(0);
        popup->m_button2->setVisible(0);
        auto sprite = CCSprite::create("Screenshot_64.png");
        popup->m_mainLayer->addChild(sprite);
        sprite->setPosition(popup->getContentSize() / 2);
        sprite->setScaleX((popup->getContentSize().width) / sprite->getContentSize().width);
        sprite->setScaleY((popup->getContentSize().height) / sprite->getContentSize().height);
        return popup;
    };
    $override void activate() {
        if (SETTING(bool, "soggy cat?")) {
            if (string::containsAny(this->getID(), { "play-button", "level-button" })) {
                srand(time(0));
                if (rndb(6)) {
                    auto pop = createPopup();
                    pop->show();
                    PopupSoggyCat::onClick = [this]()
                        {
                            GameManager::get()->playMenuMusic();
                            return CCMenuItem::activate();
                        };
                    GameManager::get()->fadeInMusic("SogLoop.mp3");
                    return;
                }
            };
        };
        return CCMenuItem::activate();
    }
};